#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include "model.h"
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

private:

    Ui::MainWindow *ui;
    Matrix<4,4,double> rotationOX,rotationOY,rotationOZ;

    void setTransformationMatrix();
};

#endif // MAINWINDOW_H
