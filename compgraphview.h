#ifndef COMPGRAPHVIEW_H
#define COMPGRAPHVIEW_H
#include <QWidget>
#include "graphics.h"
#include "model.h"
class CompGraphView : public QWidget
{
    Q_OBJECT
public:
    void setUserTransformation(const Matrix<4,4,double>& t);
    explicit CompGraphView(QWidget *parent = 0);
    void readModel(const char* path);
private:
    //Отрисовка
    Model m_model;
    Context ctx;
    Matrix<4,4,double> m_transformation;
    void paintEvent(QPaintEvent *event);    
};

#endif // COMPGRAPHVIEW_H
