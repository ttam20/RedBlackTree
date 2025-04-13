#include "node.h"

const std::shared_ptr<Node> NIL = std::make_shared<Node>(std::variant<qint16, QString, QChar>{}, Color::BLACK);

qint16 Node::CalculateBlackHeight(const Node* node) const
{
    if (node == NIL.get())
        return 0;

    qint16 leftBlackHeight = CalculateBlackHeight(node->left.get()) + (node->left->color == Color::BLACK ? 1 : 0);
    qint16 rightBlackHeight = CalculateBlackHeight(node->right.get()) + (node->right->color == Color::BLACK ? 1 : 0);

    if (leftBlackHeight != rightBlackHeight)
        return -1;

    return leftBlackHeight;
}

qint16 Node::GetBlackHeight() const
{
    return CalculateBlackHeight(this);
}

