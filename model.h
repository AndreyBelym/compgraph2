#ifndef MODEL
#define MODEL
#include <cmath>
#include <vector>
#include <iostream>
#include <cassert>
template<size_t N,typename T>
class Vector{
    T m_data[N];
  public:
    Vector(){}
    Vector(const Vector& arg){
        (*this)=arg;
    }

    const Vector& operator=(const Vector& arg){
        for(size_t i=0;i<N;i++)
            m_data[i]=arg.m_data[i];
        return *this;
    }

    const T& at(size_t i)const{
        assert(i<N);
        return m_data[i];
    }

    const T& operator[](size_t i)const{
        assert(i<N);
        return m_data[i];
    }
    T& operator[](size_t i){
        assert(i<N);
        return m_data[i];
    }

    void set(size_t i,const T& value){
        assert(i<N);
        m_data[i]=value;
    }
    T operator *(const Vector& arg)const{
        T res=T();
        for(size_t i=0;i<N;i++){
            res+=m_data[i]*arg.m_data[i];
        }
        return res;
    }
    Vector operator -(const Vector& arg)const{
        Vector res;
        for(size_t i=0;i<N;i++){
            res.set(i,m_data[i]-arg.m_data[i]);
        }
        return res;
    }
    void normalize(){
        double temp=sqrt((*this)*(*this));
        for(size_t i=0;i<N;i++){
            m_data[i]/=temp;
        }
    }
};

template <size_t N,size_t M,typename T>
class Matrix{
  private:
    T m_data[N][M];

    template <size_t,size_t,typename>
    friend class Matrix;

  public:
    Matrix(){
        for(size_t i=0;i<N;i++)
            for(size_t j=0;j<M;j++)
                m_data[i][j]=T();
    }

    Matrix(const Matrix& arg){
        copy(arg);
    }

    Matrix(const T (&arg)[N][M]){
        for(size_t i=0;i<N;i++){
            for(size_t j=0;j<M;j++)
                m_data[i][j]=arg[i][j];
        }
    }

    const Matrix& operator=(const T (&arg)[N][M]){
        for(size_t i=0;i<N;i++){
            for(size_t j=0;j<M;j++)
                m_data[i][j]=arg[i][j];
        }
        return (*this);
    }

    const Matrix& operator=(const Matrix& arg){
        copy(arg);
        return (*this);
    }

    void copy(const Matrix& arg){
        for(size_t i=0;i<N;i++){
            for(size_t j=0;j<M;j++){
                m_data[i][j]=arg.m_data[i][j];
            }
        }
    }
    T& operator()(size_t i,size_t j){
        assert(i<N&&j<M);
        return m_data[i][j];
    }
    const T& operator()(size_t i,size_t j) const {
        assert(i<N&&j<M);
        return m_data[i][j];
    }
    T& at(size_t i,size_t j){
        return m_data[i][j];
    }
    const T& at(size_t i,size_t j) const {
        return m_data[i][j];
    }

    template <size_t L>
    Matrix<N,L,T>operator*(const Matrix<M,L,T>& arg)const
    {
        Matrix<N,L,T> res;
        for(size_t i=0;i<N;i++){
            for(size_t j=0;j<L;j++){
                res.m_data[i][j]=0;
                for(size_t k=0;k<M;k++){
                    res.m_data[i][j]+=m_data[i][k]*arg.m_data[k][j];

                }
            }
        }
        return res;
    }
    Vector<N,T> operator*(const Vector<M,T>& arg)const{
        Vector<N,T> res;
        for(size_t i=0;i<N;i++){
            res.set(i,0);
            for(size_t k=0;k<M;k++){
                res[i]+=m_data[i][k]*arg[k];

            }
        }
        return res;
    }
    friend Vector<M,T> operator*(const Vector<N,T>& arg,const Matrix& obj){
        Vector<M,T> res;
        for(size_t j=0;j<M;j++){
            res.m_data[j]=0;
            for(size_t k=0;k<M;k++){
                res.m_data[j]+=arg[k]*obj.m_data[k][j];
            }
        }

        return res;
    }
    operator Matrix<M,N,T>(){
        Matrix<M,N,T> res;
        for(size_t i=0;i<N;i++)
            for(size_t j=0;j<M;j++)
                res.m_data[j][i]=m_data[i][j];
        return res;
    }
};

namespace Transformations{
    typedef double arr[4][4];
    typedef Matrix<4,4,double> mat;
    inline double rad2deg(double a){
        return 180*a/M_PI;
    }

    inline double deg2rad(double a){
        return M_PI*a/180;
    }
    inline Matrix<4,4,double> Identity(){
        return mat({{  1  ,  0  ,  0  ,  0  },
                    {  0  ,  1  ,  0  ,  0  },
                    {  0  ,  0  ,  1  ,  0  },
                    {  0  ,  0  ,  0  ,  1  }});
    }

    inline Matrix<4,4,double> ScaleMovePerspective(double p=0,     double q=0,     double r=0,
                                                   double scl_x=1, double scl_y=1, double scl_z=1, double scl_w=1,
                                                   double l=0,     double m=0,     double n=0){
        return mat({{scl_x,  0  ,  0  ,  p  },
                    {  0  ,scl_y,  0  ,  q  },
                    {  0  ,  0  ,scl_z,  r  },
                    {  l  ,  m  ,  n  ,scl_w}});
    }

    inline Matrix<4,4,double> Scale(double scl_x=1, double scl_y=1, double scl_z=1, double scl_w=1){
        return mat({{scl_x,  0  ,  0  ,  0  },
                    {  0  ,scl_y,  0  ,  0  },
                    {  0  ,  0  ,scl_z,  0  },
                    {  0  ,  0  ,  0  ,scl_w}});
    }

    inline Matrix<4,4,double> Move(double p=0,     double q=0,     double r=0){
        return mat({{  1  ,  0  ,  0  ,  p  },
                    {  0  ,  1  ,  0  ,  q  },
                    {  0  ,  0  ,  1  ,  r  },
                    {  0  ,  0  ,  0  ,  1  }});
    }

    inline Matrix<4,4,double> Perspective(double l=0,     double m=0,     double n=0){
        return mat({{  1  ,  0  ,  0  ,  0  },
                    {  0  ,  1  ,  0  ,  0  },
                    {  0  ,  0  ,  1  ,  0  },
                    {  l  ,  m  ,  n  ,  1  }});
    }

    inline Matrix<4,4,double> RotateOX(double a=0){
        return mat({{  1  ,   0   ,   0   ,  0  },
                    {  0  , cos(a), sin(a),  0  },
                    {  0  ,-sin(a), cos(a),  0  },
                    {  0  ,   0   ,   0   ,  1  }});
    }

    inline Matrix<4,4,double> RotateOY(double a=0){
        return mat({{ cos(a),  0  , sin(a),  0  },
                    {   0   ,  1  ,   0   ,  0  },
                    {-sin(a),  0  , cos(a),  0  },
                    {   0   ,  0  ,   0   ,  1  }});
    }

    inline Matrix<4,4,double> RotateOZ(double a=0){
        return mat({{ cos(a), sin(a),  0  ,  0  },
                    {-sin(a), cos(a),  0  ,  0  },
                    {   0   ,   0   ,  1  ,  0  },
                    {   0   ,   0   ,  0  ,  1  }});
    }

//    Matrix<4,4,double> RotateVector(double a=0,const Point3& v){
//        double cos_a=cos(a),sin_a=sin(a);
//        return {{ cos(a), sin(a),  0  ,  0  },
//                {-sin(a), cos(a),  0  ,  0  },
//                {   0   ,   0   ,  0  ,  0  },
//                {   0   ,   0   ,  0  ,  1  }};
//    }
}
class Point2: public Vector<2,double>{
  public:
    Point2(){}
    Point2(const double& x,const double& y){
        set(0,x);set(1,y);
    }
    Point2(const double (&arg)[2]){
        set(0,arg[0]);set(1,arg[1]);
    }
    Point2(const Point2& arg):Vector(arg)
    {}

    const Point2& operator =(const Point2& arg){
        (Vector&)(*this)=(Vector&)arg;
        return *this;
    }

    const double& x()const{return at(0);}
    const double& y()const{return at(1);}

    void setX(const double& value){set(0,value);}
    void setY(const double& value){set(1,value);}

    const double& at(size_t i)const{
        return Vector::at(i);
    }

    void set(size_t i,const double& value){
        Vector::set(i,value);
    }
};

class Point3: public Vector<3,double>{
  public:
    Point3(){}
    Point3(const double& x,const double& y,const double& z){
        set(0,x);set(1,y);set(2,z);
    }
    Point3(const double (&arg)[3]){
        set(0,arg[0]);set(1,arg[1]);set(2,arg[2]);
    }
    Point3(const Point3& arg):Vector(arg)
    {}
    Point3(const Vector& arg):Vector(arg)
    {}

    const Point3& operator =(const Point3& arg){
        (Vector&)(*this)=(Vector&)arg;
        return *this;
    }

    const double& x()const{return at(0);}
    const double& y()const{return at(1);}
    const double& z()const{return at(2);}

    void setX(const double& value){set(0,value);}
    void setY(const double& value){set(1,value);}
    void setZ(const double& value){set(2,value);}

    const double& at(size_t i)const{
        return Vector::at(i);
    }
    Point3 operator  -(const Point3& arg)const{
        return (Point3)(*((Vector<3,double>*)this)-arg);
    }
    void set(size_t i,const double& value){
        Vector::set(i,value);
    }
    Point3 cross(const Point3& arg){
        Point3 res;
        res.setX(y()*arg.z()-z()*arg.y());
        res.setY(x()*arg.z()-z()*arg.x());
        res.setZ(x()*arg.y()-y()*arg.x());
        return res;
    }
};
class Point4: public Vector<4,double>{
  public:
    Point4(){}
    Point4(const double& x,const double& y,const double& z,const double &w){
        set(0,x);set(1,y);set(2,z);set(3,w);
    }
    Point4(const double (&arg)[4]){
        set(0,arg[0]);set(1,arg[1]);set(2,arg[2]);set(3,arg[3]);
    }
    Point4(const Point4& arg):Vector(arg)
    {}
    Point4(const Point3& arg)
    {
         set(0,arg[0]);set(1,arg[1]);set(2,arg[2]);set(3,1);
    }
    Point4(const Vector& arg):Vector(arg)
    {}

    const Point4& operator =(const Point4& arg){
        (Vector&)(*this)=(Vector&)arg;
        return *this;
    }

    friend Point4 operator *(const Matrix<4,4,double>& mat,const Point4& arg){
        return mat*(Vector<4,double>&)arg;
    }

    const double& x()const{return at(0);}
    const double& y()const{return at(1);}
    const double& z()const{return at(2);}
    const double& w()const{return at(3);}

    void setX(const double& value){set(0,value);}
    void setY(const double& value){set(1,value);}
    void setZ(const double& value){set(2,value);}
    void setW(const double& value){set(3,value);}

    const double& at(size_t i)const{
        return Vector::at(i);
    }

    void set(size_t i,const double& value){
        Vector::set(i,value);
    }
    operator Point3(){
        return Point3(x()/w(),y()/w(),z()/w());
    }
};
class Face{
  public:
    Vector<3,size_t> v;
    Vector<3,size_t> vn;
    Vector<3,size_t> vt;
  public:
    Face(){}
    Face(const Face& arg):v(arg.v),vn(arg.vn),vt(arg.vt){}

    Face(const Vector<3,size_t> &v_,
         const Vector<3,size_t> &vn_,
         const Vector<3,size_t> &vt_)
        :v(v_),vn(vn_),vt(vt_)
    {

    }

    const Face& operator=(const Face& arg){
        v=arg.v; vt=arg.vt; vn=arg.vn;
        return *this;
    }
};

class Model{
  public:
    std::vector<Point3> vertices;
    std::vector<Point2> texture_coords;
    std::vector<Point3> vertex_normals;
    std::vector<Face> faces;
    enum Symbol{
        symVertex,
        symNormal,
        symTexCoord,
        symFace,
        symEOF,
        symError
    };
    Symbol gettype(std::istream& input){
        char c; Symbol s=symEOF; std::ios_base::fmtflags flags=input.flags();
        input.flags(flags&(~std::ios_base::skipws));
        while(input.good()){
            input>>c;
            if(isspace(c)){
                continue;
            } else if(c=='f'){
                s=symFace;
                break;
            } else if(c=='v'){
                input>>c;
                if(isspace(c)){
                    s=symVertex;
                    break;
                } else if(c=='t'){
                    s=symTexCoord;
                    break;
                } else if(c=='n'){
                    s=symNormal;
                    break;
                }
            }
        }
        input.flags(flags);
        return s;
    }

  public:


    std::istream& parse(std::istream& input){
        while(input.good()){
            switch(gettype(input)){
                case symVertex: {
                    Point3 v; double d;
                    input>>d; v.setX(d);
                    input>>d; v.setY(d);
                    input>>d; v.setZ(d);
                    vertices.push_back(v);
                    break;
                }
                case symNormal:{
                    Point3 v; double d;
                    input>>d; v.setX(d);
                    input>>d; v.setY(d);
                    input>>d; v.setZ(d);
                    vertex_normals.push_back(v);
                    break;
                }
                case symTexCoord: {
                    Point2 v; double d;
                    input>>d; v.setX(d);
                    input>>d; v.setY(d);
                    texture_coords.push_back(v);
                    break;
                }
                case symFace:{
                    Vector<3,size_t> v,vn,vt; size_t n; char c;
                    for(size_t i=0;i<3;i++){
                        input>>n;
                        v.set(i,n-1);
                        std::ios_base::fmtflags flags=input.flags();
                        input.flags(flags&(~std::ios_base::skipws));
                        input>>c;
                        input.flags(flags);
                        if(c=='/'){
                            input>>n;
                            vt.set(i,n-1);
                            if(input.fail()) input.clear();
                            flags=input.flags();
                            input.flags(flags&(~std::ios_base::skipws));
                            input>>c;
                            input.flags(flags);
                            if(c=='/'){
                                input>>n;
                                vn.set(i,n-1);
                            }
                        }
                    }
                    Face f(v,vn,vt);
                    faces.push_back(f);
                    break;
                }
                case symEOF:{
                    return input;
                }
                default:
                    assert(false);
            }
        }
        return input;
    }

};

#endif // MODEL

