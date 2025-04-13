#include "nilnode.h"

NilNode::NilNode(qreal x, qreal y, qreal w, qreal h)
    : QGraphicsRectItem(x, y, w, h), x(x), y(y), w(w), h(h), line(nullptr)
{
    textItem = new QGraphicsTextItem("NIL");
    textItem->setPos(x + (w - textItem->boundingRect().width()) / 2,
                     y + (h - textItem->boundingRect().height()) / 2);
}

void NilNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setBrush(QBrush(Qt::black));
    painter->setPen(QPen(Qt::black, 2));

    painter->drawRect(boundingRect());

    if (line)
        painter->drawLine(line->line());

    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("Segoe UI", 10, QFont::Bold));
    painter->drawText(boundingRect(), Qt::AlignCenter, textItem->toPlainText());

    setPos(QPointF(x, y));
}


void NilNode::SetPos(qreal x, qreal y)
{
    this->x = x;
    this->y = y;

    setRect(x + w / 2, y, w, h);
}

void NilNode::SetLine(const QPointF &point)
{
    line = new QGraphicsLineItem(x + w,
                                 y,
                                 point.x(),
                                 point.y() + 1, this);
    line->setPen(QPen(Qt::black, 2));
}

