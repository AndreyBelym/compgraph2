#include "graphics.h"
#include "compgraphview.h"
#include <fstream>
#include <QDebug>
#include <QTimer>
#include <QPainter>
#include <climits>


using namespace std;

const int main_size=300,arrow_size=130,tail_wdth=50,wdth=85,zwdth=20;
const double deg=cos(45*M_PI/180);



CompGraphView::CompGraphView(QWidget *parent) :
    QWidget(parent),
    ctx(800,800)
{



    m_renderer.setContext(&ctx);

}

void CompGraphView::paintEvent(QPaintEvent*){

    m_renderer.rasterize();

    QPainter painter(this);
    painter.drawImage(0,0,ctx.image);
}

//Описание точек фигуры
//    figure_points<<Vector(0,0,zwdth); //0
//    figure_points<<Vector(0,main_size,zwdth); //1
//    figure_points<<Vector(main_size/2-deg*tail_wdth,main_size/2+deg*tail_wdth,zwdth); //2
//    figure_points<<Vector(1./2*(-arrow_size + 2*main_size - 2*deg*tail_wdth),1./2*(-arrow_size + 2*main_size + 2*deg*tail_wdth),zwdth); //3
//    figure_points<<Vector(main_size-arrow_size,+main_size,zwdth); //4
//    figure_points<<Vector(main_size,main_size,zwdth); //5
//    figure_points<<Vector(main_size,main_size-arrow_size,zwdth); //6
//    figure_points<<Vector(1./2*(-arrow_size + 2*main_size + 2*deg*tail_wdth),1./2*(-arrow_size + 2*main_size - 2*deg*tail_wdth),zwdth); //7
//    figure_points<<Vector(main_size/2+deg*tail_wdth,main_size/2-deg*tail_wdth,zwdth); //8
//    figure_points<<Vector(main_size,0,zwdth); //9
//    figure_points<<Vector(main_size-2*deg*wdth,0,zwdth); //10
//    figure_points<<Vector(wdth,-wdth-2*deg*wdth + main_size,zwdth); //11
//    figure_points<<Vector(wdth,0,zwdth); //12
//    figure_points<<Vector(0,0,-zwdth); //13
//    figure_points<<Vector(0,main_size,-zwdth); //14
//    figure_points<<Vector(main_size/2-deg*tail_wdth,main_size/2+deg*tail_wdth,-zwdth); //15
//    figure_points<<Vector(1./2*(-arrow_size + 2*main_size - 2*deg*tail_wdth),1./2*(-arrow_size + 2*main_size + 2*deg*tail_wdth),-zwdth); //16
//    figure_points<<Vector(main_size-arrow_size,+main_size,-zwdth); //17
//    figure_points<<Vector(main_size,main_size,-zwdth); //18
//    figure_points<<Vector(main_size,main_size-arrow_size,-zwdth); //19
//    figure_points<<Vector(1./2*(-arrow_size + 2*main_size + 2*deg*tail_wdth),1./2*(-arrow_size + 2*main_size - 2*deg*tail_wdth),-zwdth); //20
//    figure_points<<Vector(main_size/2+deg*tail_wdth,main_size/2-deg*tail_wdth,-zwdth); //21
//    figure_points<<Vector(main_size,0,-zwdth); //22
//    figure_points<<Vector(main_size-2*deg*wdth,0,-zwdth); //23
//    figure_points<<Vector(wdth,-wdth-2*deg*wdth + main_size,-zwdth); //24
//    figure_points<<Vector(wdth,0,-zwdth); //25

//    figure_t figure;

//    figure<<0<<1<<2<<3<<4<<5<<6<<7<<
//            8<<9<<10<<11<<12;
//    figures<<figure;
//    figure.clear();

//    figure<<13<<14<<15<<16<<17<<18<<19<<
//            20<<21<<22<<23<<24<<25;
//    figures<<figure;
//    figure.clear();


//    figure<<0<<1<<14<<13;
//    figures<<figure;
//    figure.clear();

//    figure<<1<<2<<15<<14;
//    figures<<figure;
//    figure.clear();

//    figure<<2<<3<<16<<15;
//    figures<<figure;
//    figure.clear();

//    figure<<3<<4<<17<<16;
//    figures<<figure;
//    figure.clear();

//    figure<<4<<5<<18<<17;
//    figures<<figure;
//    figure.clear();

//    figure<<5<<6<<19<<18;
//    figures<<figure;
//    figure.clear();

//    figure<<6<<7<<20<<19;
//    figures<<figure;
//    figure.clear();

//    figure<<7<<8<<21<<20;
//    figures<<figure;
//    figure.clear();

//    figure<<8<<9<<22<<21;
//    figures<<figure;
//    figure.clear();

//    figure<<9<<10<<23<<22;
//    figures<<figure;
//    figure.clear();

//    figure<<10<<11<<24<<23;
//    figures<<figure;
//    figure.clear();

//    figure<<11<<12<<25<<24;
//    figures<<figure;
//    figure.clear();

//    figure<<12<<0<<13<<25;
//    figures<<figure;
//    figure.clear();


