#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <vector>
#include "graphics.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_spinBox_OX_valueChanged(int arg1);

    void on_spinBox_OY_valueChanged(int arg1);

    void on_spinBox_OZ_valueChanged(int arg1);

    void on_dial_OX_valueChanged(int value);

    void on_dial_OY_valueChanged(int value);

    void on_dial_OZ_valueChanged(int value);

    void on_actionQuit_triggered();

    void on_actionOpen_triggered();

    void on_stackedShading_currentChanged(int arg1);

    void on_comboShading_currentIndexChanged(int index);

    void on_comboLight_currentIndexChanged(int index);

    void on_toolTexture_clicked();

    void on_actionClear_triggered();

    void on_ambientDoubleSpinBox_valueChanged(double arg1);

    void on_diffuseDoubleSpinBox_valueChanged(double arg1);

    void on_specularDoubleSpinBox_valueChanged(double arg1);

    void on_shinessDoubleSpinBox_valueChanged(double arg1);

    void on_directionX_valueChanged(double arg1);

    void on_directionY_valueChanged(double arg1);

    void on_directionZ_valueChanged(double arg1);

    void on_postitionX_valueChanged(double arg1);

    void on_positionY_valueChanged(double arg1);

    void on_positionZ_valueChanged(double arg1);

    void on_constantDoubleSpinBox_4_valueChanged(double arg1);

    void on_linearDoubleSpinBox_4_valueChanged(double arg1);

    void on_quadraticDoubleSpinBox_4_valueChanged(double arg1);

private:

    Ui::MainWindow *ui;


    Matrix<4,4,double> rotationOX,rotationOY,rotationOZ;

    Shader<Context> *currShader = 0;
    LightSource<Context> *currLightSource = 0;

    FlatShader flat;
    GoraudShader goraud;
    GoraudShaderTextured goraudTex;
    std::vector<Shader<Context>*> shaders;

    DirectionalLight dirLight;
    PointLight pointLight;
    std::vector<LightSource<Context>*> lights;

    void setTransformationMatrix();
    void setLightSource(LightSource<Context> *currLightSource);
    void setShader(Shader<Context> *currShader);
    void readModel(const char *path);
};

#endif // MAINWINDOW_H
