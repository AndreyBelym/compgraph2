#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>
#include <QTimer>

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
    void on_dial_valueChanged(int value);

    void on_checkBox_4_stateChanged(int arg1);

    void timer_timeout();

    void on_spinBox_2_valueChanged(int value);

    void on_spinBox_valueChanged(int value);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void on_doubleSpinBox_4_valueChanged(double arg1);

    void on_doubleSpinBox_5_valueChanged(double arg1);

    void on_pushButton_clicked();

    void on_checkBox_5_stateChanged(int arg1);

    void on_checkBox_6_stateChanged(int arg1);

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_horizontalSlider_5_valueChanged(int value);

    void on_dial_2_valueChanged(int value);

    void on_dial_3_valueChanged(int value);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_doubleSpinBox_6_valueChanged(double arg1);

    void on_doubleSpinBox_8_valueChanged(double arg1);

    void on_horizontalSlider_7_valueChanged(int value);

    void on_horizontalSlider_6_valueChanged(int value);

    void on_checkBox_7_stateChanged(int arg1);

    void on_checkBox_10_stateChanged(int arg1);

    void on_checkBox_9_stateChanged(int arg1);

    void on_checkBox_8_stateChanged(int arg1);

    void on_doubleSpinBox_9_valueChanged(double arg1);

    void on_doubleSpinBox_7_valueChanged(double arg1);

    void on_doubleSpinBox_10_valueChanged(double arg1);

    void on_horizontalSlider_9_valueChanged(int value);

    void on_horizontalSlider_8_valueChanged(int value);

    void on_horizontalSlider_10_valueChanged(int value);

private:
    bool update;
    Ui::MainWindow *ui;
    QTimer timer;
    int start_angle_OX,angle_OX;
    int start_angle_OY,angle_OY;
    int start_angle_OZ,angle_OZ;
    double start_x,start_y,start_z,new_x,new_y,new_z;
    double start_scl_x,start_scl_y,start_scl_z,scl_x,scl_y,scl_z;
    double scl_gen,start_scl_gen;
    int hreflect,vreflect,freflect;
    double l,m,n;
};

#endif // MAINWINDOW_H
