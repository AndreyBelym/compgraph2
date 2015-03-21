#include "graphics.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QDebug>
#include <vector>
#include <fstream>
#include <QTime>

using namespace Transformations;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rotationOX(Identity()),
    rotationOY(Identity()),
    rotationOZ(Identity())
{


    ui->setupUi(this);
//    ui->label->setEnabled(true);
//    ui->label->hSlider=ui->scrollArea->horizontalScrollBar();
//    ui->label->vSlider=ui->scrollArea->verticalScrollBar();

//    QPixmap pixmap(800,600);
//    PainterContext ctx(&pixmap);

//    Model m;
//    std::ifstream input("african_head.obj");
//    m.parse(input);
    ui->widget->readModel("african_head.obj");
//    QTime t;
//    t.start();
    //Renderer<PainterContext>::rasterize(ctx,m);
    //qDebug("Time elapsed: %d ms", t.elapsed());
//    Point p1={400,100,0},p2={300,250,10},p3={200,300,20},p4={400,300,30},p5={400,300,20},p6={400,100,20};

//    drawTriangle(ctx,p1,p2,p3);
//    drawTriangle(ctx,p2,p3,p4);

//    drawTriangle(ctx,p1,p2,p4);
//    //drawTriangle<Bresenham,Bresenham,Bresenham>(picture,p1,p2,p4,c4);
//    drawTriangle(ctx,p3,p5,p6);
//    ui->label->setMyPixmap(pixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setTransformationMatrix()
{
    ui->widget->setUserTransformation(rotationOX*rotationOY*rotationOZ);
    ui->widget->repaint();
}

void MainWindow::on_spinBox_OX_valueChanged(int arg1)
{
    rotationOX=RotateOX(deg2rad(arg1));
    setTransformationMatrix();
    ui->dial_OX->setValue(arg1);
}


void MainWindow::on_spinBox_OY_valueChanged(int arg1)
{
    rotationOY=RotateOY(deg2rad(arg1));
    setTransformationMatrix();
    ui->dial_OY->setValue(arg1);
}

void MainWindow::on_spinBox_OZ_valueChanged(int arg1)
{
    rotationOZ=RotateOZ(deg2rad(arg1));
    setTransformationMatrix();
    ui->dial_OZ->setValue(arg1);
}

void MainWindow::on_dial_OX_valueChanged(int value)
{
    ui->spinBox_OX->setValue(value);
}

void MainWindow::on_dial_OY_valueChanged(int value)
{
    ui->spinBox_OY->setValue(value);
}

void MainWindow::on_dial_OZ_valueChanged(int value)
{
    ui->spinBox_OZ->setValue(value);
}
