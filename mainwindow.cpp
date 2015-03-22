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

    ui->widget->readModel("african_head.obj");
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
