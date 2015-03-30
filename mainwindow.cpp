#include "graphics.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QDebug>
#include <vector>
#include <fstream>
#include <QTime>
#include <QFileDialog>
using namespace Transformations;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rotationOX(Identity()),
    rotationOY(Identity()),
    rotationOZ(Identity()),
    shaders({&flat,&goraud,&goraudTex}),
    lights({&dirLight, &pointLight})
{
    ui->setupUi(this);
    setShader(&goraudTex);
    setLightSource(&pointLight);

    dirLight.direction = {0,0,-1};
    dirLight.ambient = 0.1; dirLight.diffuse = 0.8; dirLight.specular = 0.1; dirLight.shiness = 0.11;

    pointLight.position = {400,400,800};
    pointLight.constant = 0; pointLight.linear = 0.001; pointLight.quadratic = 0.000001;
    pointLight.ambient = 0.1; pointLight.diffuse = 0.8; pointLight.specular = 0.1; pointLight.shiness = 0.11;

    ui->frameGoraudTex->setVisible(true);
    ui->frameDirectLight->setVisible(false);
    QMenu *instr = this->createPopupMenu();
    instr->setTitle("Instruments");
    ui->menuView->addMenu(instr);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setTransformationMatrix()
{
    ui->widget->renderer()->setTransform(rotationOX*rotationOY*rotationOZ);
    ui->widget->repaint();
}

void MainWindow::setLightSource(LightSource<Context> *lightSource)
{
    currLightSource = lightSource;
    if(currShader)
        currShader->setLightSource(lightSource);
}

void MainWindow::setShader(Shader<Context> *shader)
{
    currShader = shader;

    if(currLightSource)
        currShader->setLightSource(currLightSource);

    ui->widget->renderer()->setShader(currShader);
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

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename;
    if(!(filename = QFileDialog::getOpenFileName(this,"Open Model","","Wavefront OBJ (*.obj);; All filed (*.*)")).isNull()){
        readModel(qPrintable(filename));
        ui->widget->repaint();
    }
}
void MainWindow::readModel(const char *path)
{
    Model m_model;
    std::ifstream input(path);
    m_model.parse(input);
    ui->widget->renderer()->setModel(m_model);
}

void MainWindow::on_stackedShading_currentChanged(int arg1)
{
}

void MainWindow::on_comboShading_currentIndexChanged(int index)
{
    if(index == 2)
        ui->frameGoraudTex->setVisible(true);
    else
        ui->frameGoraudTex->setVisible(false);

    setShader(shaders[index]);
    ui->widget->repaint();
}

void MainWindow::on_comboLight_currentIndexChanged(int index)
{
    if(index == 0){
        ui->frameDirectLight->setVisible(true);
        ui->framePointLight->setVisible(false);
    } else {
        ui->frameDirectLight->setVisible(false);
        ui->framePointLight->setVisible(true);
    }

    setLightSource(lights[index]);
    ui->widget->repaint();
}

void MainWindow::on_toolTexture_clicked()
{
    QString filename;
    if(!(filename = QFileDialog::getOpenFileName(this,"Open Model","","Texture (*.png);; All filed (*.*)")).isNull()){
        goraudTex.loadTexture(qPrintable(filename));
        ui->lineTexture->setText(filename);
        ui->widget->repaint();
    }
}

void MainWindow::on_actionClear_triggered()
{
    Model m;
    ui->widget->renderer()->setModel(m);
}

void MainWindow::on_ambientDoubleSpinBox_valueChanged(double arg1)
{
    pointLight.ambient = arg1;
    dirLight.ambient = arg1;
    ui->widget->repaint();
}

void MainWindow::on_diffuseDoubleSpinBox_valueChanged(double arg1)
{
    pointLight.diffuse = arg1;
    dirLight.diffuse = arg1;
    ui->widget->repaint();
}

void MainWindow::on_specularDoubleSpinBox_valueChanged(double arg1)
{
    pointLight.specular = arg1;
    dirLight.specular = arg1;
    ui->widget->repaint();
}

void MainWindow::on_shinessDoubleSpinBox_valueChanged(double arg1)
{
    pointLight.shiness = arg1;
    dirLight.shiness = arg1;
    ui->widget->repaint();
}

void MainWindow::on_directionX_valueChanged(double arg1)
{
    dirLight.direction.setX(arg1);
    dirLight.direction.normalize();
    ui->widget->repaint();
}

void MainWindow::on_directionY_valueChanged(double arg1)
{
    dirLight.direction.setY(arg1);
    dirLight.direction.normalize();
    ui->widget->repaint();
}

void MainWindow::on_directionZ_valueChanged(double arg1)
{
    dirLight.direction.setZ(arg1);
    dirLight.direction.normalize();
    ui->widget->repaint();
}

void MainWindow::on_postitionX_valueChanged(double arg1)
{
    pointLight.position.setX(arg1);
    ui->widget->repaint();
}

void MainWindow::on_positionY_valueChanged(double arg1)
{
    pointLight.position.setY(arg1);
    ui->widget->repaint();
}

void MainWindow::on_positionZ_valueChanged(double arg1)
{
    pointLight.position.setZ(arg1);
    ui->widget->repaint();
}

void MainWindow::on_constantDoubleSpinBox_4_valueChanged(double arg1)
{
    pointLight.constant = arg1;
    ui->widget->repaint();
}

void MainWindow::on_linearDoubleSpinBox_4_valueChanged(double arg1)
{
    pointLight.linear = arg1;
    ui->widget->repaint();
}

void MainWindow::on_quadraticDoubleSpinBox_4_valueChanged(double arg1)
{
    pointLight.quadratic = arg1;
    ui->widget->repaint();
}
