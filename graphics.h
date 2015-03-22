#ifndef GRAPHICS
#define GRAPHICS
#include <cassert>
#include <vector>
#include <QImage>
#include <QColor>
#include <QPaintDevice>
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include "model.h"
class RealInterpolator{
    int step,m_t,delta,dt;
    double  du,m_u,m,u_0;
  public:
    RealInterpolator(){}
    RealInterpolator(int t0,double u0,int t1,double u1){
        dt=t1-t0;
        delta=0;
        m_u=u0;m_t=t0;
        u_0=u0;
        du=0;step=0;m=0;
        if(dt!=0){
            du=u1-u0;
            m=du/double(dt);
            step=dt>0?1:-1;
        }
    }
    int next(){
        if(delta!=dt){
            m_t+=step;delta+=step;
            m_u=u_0+delta*m;
            return 1;
        }
        return 0;
    }

    const int& t() const {
        return m_t;
    }

    const double& u() const {
        return m_u;
    }
};

class Bresenham{
    int m_t,m_u,m_t_end,m_u_end;
    int error,dt,du,step_u,step_t;
    int (Bresenham::*m_next)(void);
  public:
    Bresenham(int t0,int u0,int t1,int u1){
        if(abs(u1-u0)>abs(t1-t0)){
            m_next=&Bresenham::next_swapped;
        } else {
            m_next=&Bresenham::next_normal;
        }
        m_t=t0;m_u=u0;m_t_end=t1;m_u_end=u1;
        dt=t1-t0;du=u1-u0;
        step_u=du>0?1:du==0?0:-1;
        step_t=dt>0?1:dt==0?0:-1;
        if(dt<0){
            dt=-dt;
        }

        if(du<0)
            du=-du;
        error=0;
    }
    int next_normal(){
        while(m_t!=m_t_end){
            error+=du;
            if(2*error>=dt){
                m_u+=step_u;
                error-=dt;
            }
            m_t+=step_t;
            return 1;
        }
        return 0;
    }
    int next_swapped(){
        while(m_u!=m_u_end){
            do{
                m_u+=step_u;
                error+=dt;
                if(2*error>=-dt&&2*error<dt){
                    return 1;
                }
            }while(2*error<du);
            m_t+=step_t;
            error-=du;
            if(2*error>=-dt&&2*error<dt){
                return 1;
            }
        }
        return 0;
    }
    inline int next(){
        return (this->*m_next)();
    }

    inline const int& t()const{
        return m_t;
    }
    inline const int& u()const{
        return m_u;
    }
};

struct Point{
    int x,y;
    double z;
    Point(){}
    Point(const Point3& arg){
        x=int(arg.x()+0.5);
        y=int(arg.y()+0.5);
        z=arg.z();
    }
};

template<class TX=Bresenham,class TZ=RealInterpolator>
class LineInterpolator{
    TX interpol_x;
    TZ interpol_z;
    Point res;
  public:
    LineInterpolator(const Point& p0,const Point& p1)
         :interpol_x(p0.y,p0.x,p1.y,p1.x),
          interpol_z(p0.y,p0.z,p1.y,p1.z),
          res(p0)
    {
        assert(p1.y!=p0.y);
    }

    inline const Point& current() const {
        return res;
    }

    int next(){
        if (interpol_x.next()&&interpol_z.next()){
            assert(interpol_x.t()==interpol_z.t());
            res.y=interpol_x.t();
            res.x=interpol_x.u();
            res.z=interpol_z.u();
            return 1;
        }
        return 0;
    }
};

template<class TZ=RealInterpolator>
class ScanlineInterpolator{
    TZ interpol_z;
    Point res;
  public:
    ScanlineInterpolator(const Point& p0,const Point& p1)
         :interpol_z(p0.x,p0.z,p1.x,p1.z),
          res(p0)
    {
        assert(p1.y==p0.y);
    }

    inline const Point& current() const {
        return res;
    }

    int next(){
        if (interpol_z.next()){
            res.x=interpol_z.t();
            res.z=interpol_z.u();
            return 1;
        }
        return 0;
    }
};

class LightSource {
  public:
    virtual double intensity(Point3&)=0;
    virtual Point3 vector(Point3&)=0;
};

template <typename Color>
class Shader{
  public:
    LightSource *lightsource;
    virtual void swap(size_t,size_t){}
    virtual bool processFace(Model&,const Face&)=0;
    virtual void processLongLine(){}
    virtual void stepLongLine(){}
    virtual void processShortLine1(){}
    virtual void processShortLine2(){}
    virtual void stepShortLine(){}
    virtual void processScanLine(const Point&,const Point&){}
    virtual void stepScanLine(){}
    virtual Color color()=0;
};

template <typename Context>
class Renderer{
    Model m_model,m_temp_model;
    Matrix<4,4,double> m_transform, m_inverse_transform;
  public:
    Renderer(){
        using namespace Transformations ;

        m_transform = Move(400,400)*Scale(-400,-400,400);
        m_inverse_transform = inverse(m_transform);


    }

    typename Context::Shader *shader;

    void setModel(const Model& m){
        m_model = m;
        m_temp_model = m_model;
    }

    void setTransform(const Matrix<4,4,double>& transform){
        using namespace Transformations ;

        m_transform=Move(400,400)*Scale(-400,-400,400)*transform;
        m_inverse_transform = inverse(m_transform);


    }

    inline void drawPoint(Context& ctx,const Point& p){
        if(p.x>=0&&p.x<int(ctx.width())&&
            p.y>=0&&p.y<int(ctx.height())&&
             p.z>=ctx.zBuffer(p.x,p.y)){
            ctx.setZBuffer(p.x,p.y,p.z);
            ctx.setColorBuffer(p.x,p.y,shader->color());
        }
    }

    template <typename TX, typename TZ,typename TZ2>
    inline void drawHalfTriangle(Context& ctx,LineInterpolator<TX,TZ> &line1,LineInterpolator<TX,TZ> &line2){
        do{
            ScanlineInterpolator<TZ2> sline(line1.current(),line2.current());
            shader->processScanLine(line1.current(),line2.current());

            do{
                drawPoint(ctx,sline.current());
                shader->stepScanLine();
            }while(sline.next());

            shader->stepLongLine(); shader->stepShortLine();
        }while(line2.next()&&line1.next());
    }

    template <typename TX=Bresenham, typename TZ=RealInterpolator, typename TZ2=RealInterpolator>
    inline void drawTriangle(Context& ctx,const Point& p1,const Point& p2,const Point& p3){

        LineInterpolator<TX,TZ> line1(p1,p3);
        shader->processLongLine();

        if(p1.y!=p2.y){
            LineInterpolator<TX,TZ> line2(p1,p2);
            shader->processShortLine1();
            drawHalfTriangle<TX,TZ,TZ2>(ctx,line1,line2);
        }
        if(p2.y!=p3.y){
            LineInterpolator<TX,TZ> line2(p2,p3);
            shader->processShortLine2();
            drawHalfTriangle<TX,TZ,TZ2>(ctx,line1,line2);
        }
    }

    template <typename TX=Bresenham, typename TZ=RealInterpolator, typename TZ2=RealInterpolator>
    void rasterize(Context& ctx){
        ctx.prepare();

        m_temp_model.vertices.clear();
        for(const auto& v:m_model.vertices){
            Point4 t(v);
            m_temp_model.vertices.push_back(m_transform*t);
        }

        m_temp_model.vertex_normals.clear();
        for(const auto& v:m_model.vertex_normals){
            Point3 t((Point4::fromVector3(v)*m_inverse_transform).toVector3());//t.setW(0);
            t.normalize();
            m_temp_model.vertex_normals.push_back(t);
        }
        int i=0;
        for (const Face& f:m_temp_model.faces){

            i++;


            //
            if(f.v[0]!=f.vn[0]||f.v[1]!=f.vn[1]||f.v[2]!=f.vn[2])
                qDebug()<<f.v[0]<<f.vn[0]<<f.v[1]<<f.vn[1]<<f.v[2]<<f.vn[2];
            if(shader->processFace(m_temp_model,f)){
                Point p1,p2,p3;
                p1=m_temp_model.vertices[f.v[0]];
                p2=m_temp_model.vertices[f.v[1]];
                p3=m_temp_model.vertices[f.v[2]];

                if (p1.y>p2.y){
                    std::swap(p1,p2);
                    shader->swap(0,1);
                }
                if(p2.y>p3.y){
                    std::swap(p2,p3);
                    shader->swap(1,2);
                    if(p1.y>p2.y){
                        std::swap(p1,p2);
                        shader->swap(0,1);
                    }
                }
                if(p1.y==p3.y||(p1.x==p2.x&&p1.x==p3.x)) continue;
                drawTriangle(ctx,p1,p2,p3);
            }

        }
        ctx.done();
    }
};


struct Context{
  private:
    Context(const Context&);
    void operator=(const Context&);
  public:
    typedef Shader<QRgb> Shader;
    size_t m_width,m_height;
    QImage image;
    std::vector<double> zbuf;
    void *userData;
    Context(size_t width,size_t  height)
        :m_width(width),m_height(height),
          image(width,height,QImage::Format_RGB32),
          zbuf(width*height),
          userData(0)
    {
        for(size_t i=0;i<zbuf.size();i++)
                zbuf[i]=-INFINITY;
    }
    void prepare(){
        image.fill("black");
        for(size_t i=0;i<zbuf.size();i++)
            zbuf[i]=-INFINITY;
    }

    void done(){

    }

    const double& zBuffer(size_t x,size_t y)const{
        assert(x<m_width&&y<m_height);
        return zbuf[m_height*y+x];
    }

    void setZBuffer(size_t x,size_t y,double value){
            assert(x<m_width&&y<m_height);
            zbuf[m_height*y+x]=value;
    }

    QRgb colorBuffer(size_t x,size_t y)const{
        return image.pixel(x,y);
    }

    void setColorBuffer(size_t x,size_t y,const QRgb& value){
        image.setPixel(x,y,value);
    }
    const size_t& width() const{
        return m_width;
    }
    const size_t& height() const {
        return m_height;
    }
};

class FlatShader: public Shader<QRgb> {
  private:
    QRgb c;
  public:
    bool processFace(const Model& model,const Face& face){
        //const Face& face = model.faces[faceNum];
        Point3 p0 = model.vertices[face.v[0]],
               p1 = model.vertices[face.v[1]],
               p2 = model.vertices[face.v[2]];

        Point3 culling_dir={0,0,1},vn,barycenter = (p0+p1+p2)/3;
        vn=(p1-p0).cross(p2-p0);
        vn.normalize();
        //vn = (model.vertex_normals[face.vn[0]] + model.vertex_normals[face.vn[1]] + model.vertex_normals[face.vn[2]])/double(-3);
        double culling=vn*culling_dir,
               intensity=lightsource->intensity(barycenter)*(lightsource->vector(barycenter)*vn);

        intensity = intensity>=0?intensity:0;
        if(culling >= 0){
            c=qRgb(255*intensity,255*intensity,255*intensity);
            return true;
        } else {
            return false;
        }
    }

    virtual QRgb color(){
        return c;
    }
};

class GoraudShader: public Shader<QRgb> {
  private:
    QRgb c;
    double intensity[3];
    Point3 points[3];
    RealInterpolator long_inter,short_inter,scan_inter;
  public:
    bool processFace(Model& model,const Face& face){
        //const Face& face = model.faces[faceNum];
        for(size_t i=0;i<3;i++){
            points[i] = model.vertices[face.v[i]];
        }

        Point3 culling_dir={0,0,-1},vn;
        vn=(points[2]-points[0]).cross(points[1]-points[0]);
        vn.normalize();

        double culling=vn*culling_dir;


        if(culling > 0){
            for(size_t i=0;i<3;i++){
                intensity[i]=-lightsource->intensity(points[i])
                                *(lightsource->vector(points[i])*model.vertex_normals[face.vn[i]]);
            }
            //assert(intensity[0]==intensity[1]&&intensity[1]==intensity[2]);

            return true;
        } else {
            return false;
        }
    }

    void swap(size_t i, size_t j){
        std::swap(points[i],points[j]);
        std::swap(intensity[i],intensity[j]);
    }

    virtual QRgb color(){
        double intensity = scan_inter.u()>=0?scan_inter.u():0;
        intensity = intensity>1?1:intensity;
        c=qRgb(255*intensity,255*intensity,255*intensity);
        return c;
    }

    virtual void processLongLine(){
        long_inter = RealInterpolator(points[0].y(),intensity[0],points[2].y(),intensity[2]);
    }
    virtual void stepLongLine(){
        long_inter.next();
    }
    virtual void processShortLine1(){
        short_inter = RealInterpolator(points[0].y(),intensity[0],points[1].y(),intensity[1]);
    }
    virtual void processShortLine2(){
        short_inter = RealInterpolator(points[1].y(),intensity[1],points[2].y(),intensity[2]);
    }
    virtual void stepShortLine(){
        short_inter.next();
    }
    virtual void processScanLine(const Point& p1,const Point& p2){
        scan_inter = RealInterpolator(p1.x,long_inter.u(),p2.x,short_inter.u());
    }
    virtual void stepScanLine(){
        scan_inter.next();
    }

};

class DirectionalLight:public LightSource {
  public:
    Point3 direction;
    double intensity(Point3&){return 1;}
    Point3 vector(Point3&){return direction;}
};

class SpotLight:public LightSource {
  public:
    Point3 position;
    double constant, linear, quadratic;
    double intensity(Point3& p){
        Point3 v=p-position;
        double d = sqrt(v*v),falloff = 1/(constant + d*linear + d*d*quadratic);
        return falloff;
    }
    Point3 vector(Point3& p){
        Point3 v=(p-position);
        v.normalize();
        return v;
    }
};

#endif // GRAPHICS

