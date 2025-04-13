#include "treenode.h"

TreeNode::TreeNode(QString text, qint16 blackHeight, Color color, qreal diameter) :
    QGraphicsEllipseItem(0, 0, diameter, diameter), x(0), y(0), color(color),
    diameter(diameter), left(nullptr), right(nullptr), parent(nullptr), leftLine(nullptr), rightLine(nullptr),
    showBlackHeight(false), penWidth(2),
    position()
{
    textItem = new QGraphicsTextItem(text);
    textItem->setPos(x + (diameter - textItem->boundingRect().width()) / 2,
                     y + (diameter - textItem->boundingRect().height()) / 2);

    blackHeightText = new QGraphicsTextItem(QString::number(blackHeight));
    pen = QPen(Qt::black, penWidth);
    brush = color == Color::RED ? QBrush(Qt::red) : QBrush(Qt::black);
}

void TreeNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawEllipse(boundingRect());

    if (leftLine)
    {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(leftLine->line());
    }

    if (rightLine)
    {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(rightLine->line());
    }

    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("Segoe UI", 12, QFont::Bold));
    painter->drawText(boundingRect(), Qt::AlignCenter, textItem->toPlainText());

    if (showBlackHeight)
    {
        painter->setPen(QPen(Qt::black));
        painter->setFont(QFont("Segoe UI", 10, QFont::Bold));
        painter->drawText(boundingRect().adjusted(-diameter * 1.5, 0, 0, 0) , Qt::AlignCenter, blackHeightText->toPlainText());
    }

    setPos(QPointF(x, y));
    //qDebug() << x << " " << y;
}



void TreeNode::SetLeftLine(const QPointF &point)
{
    delete leftLine;
    //qDebug() <<  point.x() + (diameter / 2) << " " << point.y() + diameter;
    leftLine = new QGraphicsLineItem(x + (diameter / 2),
                                 y + diameter + penWidth,
                                 point.x() + (diameter / 2),
                                 point.y() + diameter, this);
    leftLine->setPen(QPen(Qt::black, penWidth));
}

void TreeNode::SetRightLine(const QPointF &point)
{
    delete rightLine;
    //qDebug() <<  point.x() + (diameter / 2) << " " << point.y() + diameter;
    rightLine = new QGraphicsLineItem(x + (diameter / 2),
                                 y + diameter + penWidth,
                                 point.x() + (diameter / 2),
                                 point.y() + diameter, this);
    rightLine->setPen(QPen(Qt::black, penWidth));
}

void TreeNode::SetPos(qreal x, qreal y)
{
    this->x = x;
    this->y = y;
    position = QPointF(x, y);

    setRect(x, y, diameter, diameter);
    update();
}

void TreeNode::ClearLines()
{
    delete rightLine;
    delete leftLine;
    rightLine = leftLine = nullptr;
    update();
}

void TreeNode::ClearLeftLine()
{
    delete leftLine;
    leftLine = nullptr;
    update();
}

void TreeNode::ClearRightLine()
{
    delete rightLine;
    rightLine = nullptr;
    update();
}

void TreeNode::On_ShowBlackHeight(bool state)
{
    showBlackHeight = state;
}

