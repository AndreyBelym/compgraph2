#ifndef COMPGRAPHVIEW_H
#define COMPGRAPHVIEW_H
#include <QWidget>
#include "graphics.h"
#include "model.h"
class CompGraphView : public QWidget
{
    Q_OBJECT
public:
    explicit CompGraphView(QWidget *parent = 0);
    inline Renderer<Context> *renderer(){return &m_renderer;}
private:
    //Отрисовка
    Context ctx;

    Renderer<Context> m_renderer;

    GoraudShaderTextured shader;

    PointLight light;

    void paintEvent(QPaintEvent *event);    
};

#endif // COMPGRAPHVIEW_H
