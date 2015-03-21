#ifndef GRAPHICS
#define GRAPHICS
#include <cassert>
#include <vector>
#include <QImage>
#include <QColor>
#include <QPaintDevice>
#include <QPainter>
#include <QDebug>
#include "model.h"
class RealInterpolator{
    int step,m_t,delta,dt;
    double  du,m_u,m,u_0;
  public:
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

struct Context{
  private:
    Context(const Context&);
    void operator=(const Context&);
  public:
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

struct PainterContext{
    size_t m_width,m_height;
    QPainter painter;
    std::vector<double> zbuf;
    void *userData;
    PainterContext(QPaintDevice *parent)
        :m_width(parent->width()),m_height(parent->height()),
          painter(parent),
          zbuf(m_width*m_height),
          userData(0)
    {
        for(size_t i=0;i<m_width;i++)
            for(size_t j=0;j<m_height;j++)
                zbuf[j*m_height+i]=-INFINITY;
    }

    const double& zBuffer(size_t x,size_t y)const{
        assert(x<m_width&&y<m_height);
        return zbuf[m_height*y+x];
    }

    void setZBuffer(size_t x,size_t y,double value){
            assert(x<m_width&&y<m_height);
            zbuf[m_height*y+x]=value;
    }

//    QRgb colorBuffer(size_t x,size_t y)const{
//        return image.pixel(x,y);
//    }

    void setColorBuffer(size_t x,size_t y,const QRgb& value){
        painter.setPen(QColor(value));
        painter.drawPoint(x,y);
    }

    const size_t& width() const{
        return m_width;
    }
    const size_t& height() const {
        return m_height;
    }
};

template <typename Context>
class Renderer{
  public:

    inline static void drawPoint(Context& ctx,const Point& p){
        if(p.x>=0&&p.x<int(ctx.width())&&
            p.y>=0&&p.y<int(ctx.height())&&
             p.z>=ctx.zBuffer(p.x,p.y)){
            ctx.setZBuffer(p.x,p.y,p.z);
            ctx.setColorBuffer(p.x,p.y,*((QRgb*)ctx.userData));
        }
    }

    template <typename TX, typename TZ,typename TZ2>
    inline static void drawHalfTriangle(Context& ctx,LineInterpolator<TX,TZ> &line1,LineInterpolator<TX,TZ> &line2){
        do{
            ScanlineInterpolator<TZ2> sline(line1.current(),line2.current());

            do{
                drawPoint(ctx,sline.current());
            }while(sline.next());

        }while(line2.next()&&line1.next());
    }

    template <typename TX=Bresenham, typename TZ=RealInterpolator, typename TZ2=RealInterpolator>
    inline static void drawTriangle(Context& ctx,const Point& p1,const Point& p2,const Point& p3){



        LineInterpolator<TX,TZ> line1(p1,p3);

        if(p1.y!=p2.y){
            LineInterpolator<TX,TZ> line2(p1,p2);
            drawHalfTriangle<TX,TZ,TZ2>(ctx,line1,line2);
        }
        if(p2.y!=p3.y){
            LineInterpolator<TX,TZ> line2(p2,p3);
            drawHalfTriangle<TX,TZ,TZ2>(ctx,line1,line2);
        }
    }

    template <typename TX=Bresenham, typename TZ=RealInterpolator, typename TZ2=RealInterpolator>
    static void rasterize(Context& ctx,const Model& m,const Matrix<4,4,double>& trans){
        ctx.prepare();
        Point3 light={0,0,-1},vn;
        std::vector<Point3> vertices;
        for(const auto& v:m.vertices){
            Point4 t(v);
            vertices.push_back(trans*t);
        }
        for (const Face& f:m.faces){

            Point p1,p2,p3;
            p1=vertices[f.v[0]];
            p2=vertices[f.v[1]];
            p3=vertices[f.v[2]];

            if (p1.y>p2.y){
                std::swap(p1,p2);
            }
            if(p2.y>p3.y){
                std::swap(p2,p3);
                if(p1.y>p2.y){
                    std::swap(p1,p2);
                }
            }

            if(p1.y==p3.y||(p1.x==p2.x&&p1.x==p3.x)) continue;

            vn=(vertices[f.v[1]]-vertices[f.v[0]]).cross(vertices[f.v[2]]-vertices[f.v[0]]);
            vn.normalize();

            double intensity=vn*light;
            if(intensity>0){
                QRgb c=qRgb(255*intensity,255*intensity,255*intensity);

                ctx.userData=&c;

                drawTriangle(ctx,p1,p2,p3);

                ctx.userData=0;
            }


        }
        ctx.done();
    }
};








#endif // GRAPHICS

