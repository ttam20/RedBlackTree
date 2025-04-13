#ifndef NODE_H
#define NODE_H

#include <QString>

static constexpr qint16 UPPER_BOUND = 10000;
static constexpr qint16 LOWER_BOUND = -10000;
static constexpr qsizetype MAX_LENGTH = 4;

enum class Color { RED, BLACK };

template<typename T>
QString ConvertToString(const T& value);

template<>
inline QString ConvertToString<qint16>(const qint16& value)
{
    return QString::number(value);
}

template<>
inline QString ConvertToString<QString>(const QString& value)
{
    return value;
}

template<>
inline QString ConvertToString<QChar>(const QChar& value)
{
    return QString(value);
}

struct DataComparer
{
    bool operator()(const QString& s1, const QString& s2) const
    {
        return QString::localeAwareCompare(s1, s2) < 0;
    }

    bool operator()(const QChar& c1, const QChar& c2) const
    {
        return QString::localeAwareCompare(c1, c2) < 0;
    }

    template <class T, class U>
    bool operator()(const T& a, const U& b) const
    {
        return a < b;
    }
};

struct Node
{
    std::variant<qint16, QString, QChar> data;
    Color color;
    std::shared_ptr<Node> left, right;
    std::weak_ptr<Node> parent;

    Node(const std::variant<qint16, QString, QChar>& data, Color color)
        : data(data), color(color), left(nullptr), right(nullptr)
    {}

    QChar GetColorChar() const { return color == Color::BLACK ? 'B' : 'R'; }

    QString GetDataString() const
    {
        return std::visit([](auto&& arg) {
            return ConvertToString(arg);
        }, data);
    }

    qint16 CalculateBlackHeight(const Node* node) const;
    qint16 GetBlackHeight() const;

    bool CompareData(const std::variant<qint16, QString, QChar> &data) const
    {
        return std::visit(DataComparer{}, this->data, data);
    }

};

extern const std::shared_ptr<Node> NIL;

#endif // NODE_H
