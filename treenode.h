#ifndef TREENODE_H
#define TREENODE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QGraphicsLineItem>
#include "node.h"

class TreeNode : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF Position READ Position WRITE SetPosition)

    Q_PROPERTY(QColor PenColor READ PenColor WRITE SetPenColor NOTIFY PenChanged)
    Q_PROPERTY(qreal PenWidth READ PenWidth WRITE SetPenWidth NOTIFY PenWidthChanged)
    Q_PROPERTY(QColor BrushColor READ BrushColor WRITE SetBrushColor NOTIFY BrushChanged)
    Q_PROPERTY(qreal Opacity READ Opacity WRITE SetOpacity)
public:
    TreeNode(QString text, qint16 blackHeight, Color color, qreal diameter = 50);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void SetLeftLine(const QPointF& point);
    void SetRightLine(const QPointF& point);
    void SetPos(qreal x, qreal y);
    void ClearLines();
    void ClearLeftLine();
    void ClearRightLine();

    TreeNode* left, *right, *parent;
    Color color;
    qreal diameter;
    QPointF position;

    qreal GetX() const { return x; }
    qreal GetY() const { return y; }

    QColor PenColor() const { return pen.color(); }
    void SetPenColor(const QColor &color)
    {
        if (pen.color() != color)
        {
            pen.setColor(color);
            update();
            emit PenChanged();
        }
    }

    qreal PenWidth() const { return penWidth; }
    void SetPenWidth(qreal width)
    {
        if (penWidth != width)
        {
            penWidth = width;
            pen.setWidthF(width);
            update();
            emit PenWidthChanged();
        }
    }

    qreal Opacity() const { return opacity; }
    void SetOpacity(qreal value)
    {
        opacity = value;
        setOpacity(opacity);
        update();
    }

    QColor BrushColor() const { return brush.color(); }
    void SetBrushColor(const QColor& color)
    {
        if (brush.color() != color)
        {
            brush.setColor(color);
            update();
            emit BrushChanged();
        }
    }

    QPointF Position() const { return position; }
    void SetPosition(const QPointF& pos)
    {
        position = pos;
        SetPos(position.x(), position.y());
    }

private:
    QGraphicsTextItem* textItem, *blackHeightText;
    QGraphicsLineItem* leftLine, *rightLine;


    QPen pen;
    QBrush brush;

    bool showBlackHeight;
    qreal x, y;
    qreal penWidth, opacity;
private slots:
    void On_ShowBlackHeight(bool state);

signals:
    void PenChanged();
    void PenWidthChanged();
    void BrushChanged();
};

#endif // TREENODE_H
