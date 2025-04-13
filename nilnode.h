#ifndef NILNODE_H
#define NILNODE_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPainter>

class NilNode : public QGraphicsRectItem
{
public:
    NilNode(qreal x = 0, qreal y = 0, qreal w = 25, qreal h = 20);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void SetPos(qreal x, qreal y);
    void SetLine(const QPointF &point);

    qreal GetW() const { return w; }
private:
    QGraphicsTextItem* textItem;
    QGraphicsLineItem* line;

    qreal x, y, w, h;
};


#endif // NILNODE_H
