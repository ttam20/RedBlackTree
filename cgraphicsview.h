#ifndef CGRAPHICSVIEW_H
#define CGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class CGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    CGraphicsView(QWidget *parent = nullptr) : QGraphicsView(parent)
    {
        setRenderHint(QPainter::Antialiasing);
    }

protected:
    // Source: https://stackoverflow.com/questions/19113532/qgraphicsview-zooming-in-and-out-under-mouse-position-using-mouse-wheel
    void wheelEvent(QWheelEvent* pWheelEvent)
    {
        if (pWheelEvent->modifiers() & Qt::ControlModifier)
        {
            double angle = pWheelEvent->angleDelta().y();
            double factor = qPow(1.0015, angle);

            auto targetViewportPos = pWheelEvent->position();
            auto targetScenePos = mapToScene(pWheelEvent->position().toPoint());

            scale(factor, factor);
            centerOn(targetScenePos);
            QPointF deltaViewportPos = targetViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
            QPointF viewportCenter = mapFromScene(targetScenePos) - deltaViewportPos;
            centerOn(mapToScene(viewportCenter.toPoint()));
        }
    }
};


#endif // CGRAPHICSVIEW_H
