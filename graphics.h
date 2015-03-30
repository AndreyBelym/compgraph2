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
#include <QImageReader>
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
        } else {
            m_u=(u0+u1)/2;
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
    Bresenham(){}
    Bresenham(int t0,int u0,int t1,int u1){
        m_t=t0;m_u=u0;m_t_end=t1;m_u_end=u1;
        dt=t1-t0;du=u1-u0;
        step_u=du>0?1:du==0?0:-1;
        step_t=dt>0?1:dt==0?0:-1;
        if(dt<0)
            dt=-dt;
        if(du<0)
            du=-du;

        if(dt==0){
            m_u=(u0+u1)/2;
            m_u_end = m_u;
        }

        if(du>dt){
            m_next=&Bresenham::next_swapped;
        } else {
            m_next=&Bresenham::next_normal;
        }
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

template <typename Context>
class LightSource {
  protected:
    Context *m_ctx = 0;
    Model *m_model = 0;
  public:

    virtual double intensity(const Point3&, const Point3&)=0;
    virtual Point3 vector(const Point3&)=0;
    void setContext(Context* ctx){
        m_ctx = ctx;
    }
    void setModel(Model* model_){
        m_model = model_;
    }
};

template <typename Context>
class Shader{
  protected:
    Context *m_ctx = 0;
    LightSource<Context> *lightsource = 0;
    Model *m_model = 0;

  public:

    virtual void swap(size_t,size_t){}
    virtual bool processFace(const Face&){return false;}
    virtual void processLongLine(){}
    virtual void stepLongLine(){}
    virtual void processShortLine1(){}
    virtual void processShortLine2(){}
    virtual void stepShortLine(){}
    virtual void processScanLine(const Point&,const Point&){}
    virtual void stepScanLine(){}
    virtual typename Context::Color color(){return 0;}
    virtual double intensity(){return 0;}

    virtual void setModel(Model* model_){
        m_model = model_;
        if(lightsource)
            lightsource->setModel(m_model);
    }

    virtual void setContext(Context* ctx){
        m_ctx = ctx;
        if(lightsource)
            lightsource->setContext(m_ctx);
    }
    virtual void setLightSource(LightSource<Context>* lsource){
        lightsource = lsource;
        if(m_ctx)
            lightsource->setContext(m_ctx);
        if(m_model)
            lightsource->setModel(m_model);
    }
    virtual LightSource<Context>* lightSource(){
        return lightsource;
    }
};

template <typename Context>
class Renderer{
    Model m_model,m_temp_model;
    Matrix<4,4,double> m_transform, m_inverse_transform;

    Context* m_ctx;
    typename Context::Shader *m_shader;

  public:
    Renderer():m_ctx(0),m_shader(0){
        using namespace Transformations ;

        m_transform = Move(400,400)*Scale(-400,-400,400);
        m_inverse_transform = inverse(m_transform);

    }

    void setContext(Context *ctx){
        m_ctx = ctx;
        if(m_shader)
            m_shader->setContext(m_ctx);

    }

    void setShader(typename Context::Shader *shader){
        m_shader = shader;

        if(m_ctx)
            shader->setContext(m_ctx);

        m_shader->setModel(&m_temp_model);
    }

    void setModel(const Model& m){
        m_model = m;
        m_temp_model = m_model;
        if(m_shader)
            m_shader->setModel(&m_temp_model);
    }

    void setTransform(const Matrix<4,4,double>& transform){
        using namespace Transformations ;

        m_transform=Move(400,400)*Scale(-400,-400,400)*transform;
        m_inverse_transform = inverse(m_transform);
    }

    inline void drawPoint(const Point& p){
        if(p.x>=0&&p.x<int(m_ctx->width())&&
            p.y>=0&&p.y<int(m_ctx->height())&&
             p.z>=m_ctx->zBuffer(p.x,p.y)){
            m_ctx->setZBuffer(p.x,p.y,p.z);
            m_ctx->setColorBuffer(p.x,p.y,m_shader->color());
        }
    }

    template <typename TX, typename TZ,typename TZ2>
    inline void drawHalfTriangle(LineInterpolator<TX,TZ> &line1,LineInterpolator<TX,TZ> &line2){
        do{
            ScanlineInterpolator<TZ2> sline(line1.current(),line2.current());

            m_shader->processScanLine(line1.current(),line2.current());

            do{
                drawPoint(sline.current());
                m_shader->stepScanLine();
            }while(sline.next());

            m_shader->stepLongLine(); m_shader->stepShortLine();
        }while(line2.next()&&line1.next());
    }

    template <typename TX=Bresenham, typename TZ=RealInterpolator, typename TZ2=RealInterpolator>
    inline void drawTriangle(const Point& p1,const Point& p2,const Point& p3){

        LineInterpolator<TX,TZ> line1(p1,p3);

        m_shader->processLongLine();

        if(p1.y!=p2.y){
            LineInterpolator<TX,TZ> line2(p1,p2);
            m_shader->processShortLine1();
            drawHalfTriangle<TX,TZ,TZ2>(line1,line2);
        }
        if(p2.y!=p3.y){
            LineInterpolator<TX,TZ> line2(p2,p3);
            m_shader->processShortLine2();
            drawHalfTriangle<TX,TZ,TZ2>(line1,line2);
        }
    }

    template <typename TX=Bresenham, typename TZ=RealInterpolator, typename TZ2=RealInterpolator>
    void rasterize(){
        if(!m_shader||!m_model.faces.size())
            return;

        m_ctx->prepare();

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

        for (const Face& f:m_temp_model.faces){

            if(f.v[0]!=f.vn[0]||f.v[1]!=f.vn[1]||f.v[2]!=f.vn[2])
                qDebug()<<f.v[0]<<f.vn[0]<<f.v[1]<<f.vn[1]<<f.v[2]<<f.vn[2];
            if(m_shader->processFace(f)){
                Point p1,p2,p3;
                p1=m_temp_model.vertices[f.v[0]];
                p2=m_temp_model.vertices[f.v[1]];
                p3=m_temp_model.vertices[f.v[2]];

                if (p1.y>p2.y){
                    std::swap(p1,p2);
                    m_shader->swap(0,1);
                }
                if(p2.y>p3.y){
                    std::swap(p2,p3);
                    m_shader->swap(1,2);
                    if(p1.y>p2.y){
                        std::swap(p1,p2);
                        m_shader->swap(0,1);
                    }
                }
                if(p1.y==p3.y||(p1.x==p2.x&&p1.x==p3.x)) continue;
                drawTriangle(p1,p2,p3);
            }

        }
        m_ctx->done();
    }
};


struct Context{
  protected:
    Context(const Context&);
    void operator=(const Context&);
  public:
    typedef QRgb Color;
    typedef ::Shader<Context> Shader;

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
    Point3 camera(){
        return Point3(m_width/2, m_height/2, m_width/2);
    }
};
class TextureShader: public Shader<Context> {
  private:
    struct TexCoords{
        int x,y;
    };
    QRgb c;
    QImage m_tex;
    Point3 points[3];
    TexCoords m_tex_coords[3];
    Bresenham long_inter_x,short_inter_x,scan_inter_x,
              long_inter_y,short_inter_y,scan_inter_y;
  public:
    TextureShader()
        :m_tex(1,1,QImage::Format_RGB32){
        m_tex.fill(Qt::white);
    }
    void loadTexture(const char *filename){
        if(m_tex.load(filename))
            m_tex = m_tex.mirrored(false,true);
        else {
            m_tex = QImage(1,1,QImage::Format_RGB32);
            m_tex.fill(Qt::white);
        }
    }

    bool processFace(const Face& face){
        for(size_t i=0;i<3;i++){
            points[i] = m_model->vertices[face.v[i]];
            m_tex_coords[i].x = int(m_model->texture_coords[face.vt[i]][0] * m_tex.width());
            m_tex_coords[i].y = int(m_model->texture_coords[face.vt[i]][1] * m_tex.height());
        }
        return true;
    }

    void swap(size_t i, size_t j){
        std::swap(points[i],points[j]);
        std::swap(m_tex_coords[i],m_tex_coords[j]);
    }

    virtual QRgb color(){
        return m_tex.pixel(scan_inter_x.u(),scan_inter_y.u());
    }

    virtual void processLongLine(){
        long_inter_x = Bresenham(points[0].y(),m_tex_coords[0].x,points[2].y(),m_tex_coords[2].x);
        long_inter_y = Bresenham(points[0].y(),m_tex_coords[0].y,points[2].y(),m_tex_coords[2].y);
    }
    virtual void stepLongLine(){
        long_inter_x.next();
        long_inter_y.next();
    }
    virtual void processShortLine1(){
        short_inter_x = Bresenham(points[0].y(),m_tex_coords[0].x,points[1].y(),m_tex_coords[1].x);
        short_inter_y = Bresenham(points[0].y(),m_tex_coords[0].y,points[1].y(),m_tex_coords[1].y);
    }
    virtual void processShortLine2(){
        short_inter_x = Bresenham(points[1].y(),m_tex_coords[1].x,points[2].y(),m_tex_coords[2].x);
        short_inter_y = Bresenham(points[1].y(),m_tex_coords[1].y,points[2].y(),m_tex_coords[2].y);
    }

    virtual void stepShortLine(){
        short_inter_x.next();
        short_inter_y.next();
    }
    virtual void processScanLine(const Point& p1,const Point& p2){
        scan_inter_x = Bresenham(p1.x,long_inter_x.u(),p2.x,short_inter_x.u());
        scan_inter_y = Bresenham(p1.x,long_inter_y.u(),p2.x,short_inter_y.u());
    }
    virtual void stepScanLine(){
        scan_inter_x.next();
        scan_inter_y.next();
    }
};

class FlatShader: public Shader<Context> {
  private:
    QRgb c;
    double i;
  public:
    bool processFace(const Face& face){
        Point3 p0 = m_model->vertices[face.v[0]],
               p1 = m_model->vertices[face.v[1]],
               p2 = m_model->vertices[face.v[2]];

        Point3 culling_dir={0,0,1},vn,barycenter = (p0+p1+p2)/3;
        vn=(p1-p0).cross(p2-p0);
        vn.normalize();
//        vn = (m_model->vertex_normals[face.vn[0]] +
//                m_model->vertex_normals[face.vn[1]] +
//                m_model->vertex_normals[face.vn[2]])/3;
        double culling=vn*culling_dir;

        i=lightsource->intensity(barycenter, vn);

        i = i>=0?(i>1?1:i):0;
        if(culling >= 0){
            c=qRgb(255*i,255*i,255*i);
            return true;
        } else {
            return false;
        }
    }

    virtual QRgb color(){
        return c;
    }

    virtual double intensity(){
        return i;
    }
};

class GoraudShader: public Shader<Context> {
  private:
    QRgb c;
    double m_intensity[3];
    Point3 points[3];
    RealInterpolator long_inter,short_inter,scan_inter;
  public:
    bool processFace(const Face& face){
        for(size_t i=0;i<3;i++){
            points[i] = m_model->vertices[face.v[i]];
        }

        Point3 culling_dir={0,0,-1},vn;
        vn=(points[2]-points[0]).cross(points[1]-points[0]);
        vn.normalize();

        double culling=vn*culling_dir;


        if(culling > 0){
            for(size_t i=0;i<3;i++){
                const Point3& vn = m_model->vertex_normals[face.vn[i]];
                m_intensity[i]=lightsource->intensity(points[i],vn);
            }
            return true;
        } else {
            return false;
        }
    }

    void swap(size_t i, size_t j){
        std::swap(points[i],points[j]);
        std::swap(m_intensity[i],m_intensity[j]);
    }

    virtual QRgb color(){
        double i = intensity();
        c=qRgb(255*i,255*i,255*i);
        return c;
    }

    virtual double intensity(){
        double i = scan_inter.u()>=0?scan_inter.u():0;
        i = i>1?1:i;
        return i;
    }

    virtual void processLongLine(){
        long_inter = RealInterpolator(points[0].y(),m_intensity[0],points[2].y(),m_intensity[2]);
    }
    virtual void stepLongLine(){
        long_inter.next();
    }
    virtual void processShortLine1(){
        short_inter = RealInterpolator(points[0].y(),m_intensity[0],points[1].y(),m_intensity[1]);
    }
    virtual void processShortLine2(){
        short_inter = RealInterpolator(points[1].y(),m_intensity[1],points[2].y(),m_intensity[2]);
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

class DirectionalLight:public LightSource<Context> {
  public:
    Point3 direction;
    double ambient, diffuse, specular, shiness;
    double intensity(const Point3& p,const Point3& v_N){
        Point3 v_V = m_ctx->camera()  - p, v_R = v_N*2*(direction*v_N)-direction;

        v_V.normalize(); v_R.normalize();
        double i_D = diffuse*(direction*v_N), i_S = 1;
        if(i_D>0){
            double t = v_R*v_V;
            if(t>0){
                i_S = specular*pow(t,shiness);
                i_S = i_S>=0?i_S:0;
            } else {
                i_S = 0;
            }

        } else {
            i_D = i_S = 0;
        }
        return ambient + i_D + i_S;
    }

    Point3 vector(const Point3&){return direction;}
};

class PointLight:public LightSource<Context> {
  public:
    Point3 position;
    double constant, linear, quadratic;
    double ambient, diffuse, specular, shiness;

    double intensity(const Point3& p,const Point3& v_N){
        Point3 v_L=position - p;
        double d = sqrt(v_L*v_L),i_D = 1/(constant + d*linear + d*d*quadratic), i_S = i_D;

        v_L.normalize();
        Point3 v_V = m_ctx->camera()  - p, v_R = v_N*2*(v_L*v_N)-v_L;

        v_V.normalize(); v_R.normalize();
        i_D *= diffuse*(v_L*v_N);
        if(i_D>0){
            double t = v_R*v_V;
            if(t>0){
                i_S *= specular*pow(t,shiness);
                i_S = i_S>=0?i_S:0;
            } else {
                i_S = 0;
            }

        } else {
            i_D = i_S = 0;
        }
        return ambient + i_D + i_S;
    }
    Point3 vector(const Point3& p){
        Point3 v=(position - p);
        v.normalize();
        return v;
    }
};

class GoraudShaderTextured: public GoraudShader{
  private:
    TextureShader m_texshader;
  public:
    virtual void swap(size_t a,size_t b){
        GoraudShader::swap(a,b);
        m_texshader.swap(a,b);
    }
    virtual bool processFace(const Face& face){
        m_texshader.processFace(face);
        return GoraudShader::processFace(face);
    }
    virtual void processLongLine(){
        m_texshader.processLongLine();
        GoraudShader::processLongLine();
    }
    virtual void stepLongLine(){
        m_texshader.stepLongLine();
        GoraudShader::stepLongLine();
    }
    virtual void processShortLine1(){
        m_texshader.processShortLine1();
        GoraudShader::processShortLine1();
    }
    virtual void processShortLine2(){
        m_texshader.processShortLine2();
        GoraudShader::processShortLine2();
    }
    virtual void stepShortLine(){
        m_texshader.stepShortLine();
        GoraudShader::stepShortLine();
    }
    virtual void processScanLine(const Point& p1,const Point& p2){
        m_texshader.processScanLine(p1,p2);
        GoraudShader::processScanLine(p1,p2);
    }
    virtual void stepScanLine(){
        m_texshader.stepScanLine();
        GoraudShader::stepScanLine();
    }
    virtual Context::Color color(){
        QColor c(m_texshader.color());
        double i = GoraudShader::intensity();
        return qRgb(c.red()*i,c.green()*i,c.blue()*i);
    }
    virtual double intensity(){
        return GoraudShader::intensity();
    }

    virtual void setModel(Model* model_){
        m_texshader.setModel(model_);
        GoraudShader::setModel(model_);
    }

    virtual void setContext(Context* ctx){
        m_texshader.setContext(ctx);
        GoraudShader::setContext(ctx);
    }
    void loadTexture(const char *filename){
        m_texshader.loadTexture(filename);
    }

    virtual void setLightSource(LightSource<Context>* lsource){
        m_texshader.setLightSource(lsource);
        GoraudShader::setLightSource(lsource);
    }
    virtual LightSource<Context>* lightSource(){
        return GoraudShader::lightSource();
    }
};

#endif // GRAPHICS

