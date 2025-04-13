#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include <QTextStream>
#include <QFileInfo>
#include <QColor>
#include "node.h"

enum class DataType { NUMBER, TEXT, CHAR };

class RedBlackTree : public QObject
{
    Q_OBJECT
private:
    std::shared_ptr<Node> root;
    quint16 height, nodeCount;

    DataType dataType;

    bool enableRBTValidations;
public:
    RedBlackTree();

    std::shared_ptr<Node> GetRoot() const { return root; }
    quint16 GetHeight() const { return height; }
    quint16 GetNodeCount() const { return nodeCount; }
    DataType GetDataType() const { return dataType; }

    quint16 GetNewNodeHeight(const QString &key);

    bool ImportTree(const QString& fileName);
    bool ExportTree(const QString& fileName);

    //void PrintTree(const Node* node, QListWidget* list, int depth = 0);

    void SetTreeDataType(DataType dataType);

    RedBlackTree& operator=(RedBlackTree&& other) noexcept;

    void Insert(const QString &key);
    bool Delete(const QString& key);
    bool Find(const QString& key);
private:
    std::variant<qint16, QString, QChar> ConvertValue(DataType dataType, const QString& valueStr, bool& ok);
    bool SetTreeDataType(char type);
    QChar GetTreeDataTypeChar() const;
    bool TryGetColorFromChar(const QChar& colorChar, Color& color);

    template<class T>
    void ReadTree(T& in, std::shared_ptr<Node>& node, DataType dataType, bool& ok);

    template<class T>
    void ReadFromStream(T& in, RedBlackTree& newRedBlackTree, bool& ok);

    void WriteTree(QTextStream& out, const Node* node, bool isFirst = true) const;
    void WriteTree(QDataStream& out, const Node* node) const;

    void ReadJSON(const QString& fileData, RedBlackTree& redBlackTree, bool& ok);
    void WriteJSON(QFile& file) const;

    bool CheckJsonKey(const QJsonObject &obj, const QString &key, bool &ok);

    void ReadXML(QFile &file, RedBlackTree& redBlackTree, bool& ok);
    void WriteXML(QFile& file) const;

    quint16 CalculateHeight(std::shared_ptr<Node> node);
    quint16 CalculateNodeCount(std::shared_ptr<Node> node);

    void UpdateHeight();
    void UpdateNodeCount();

    bool IsBST(std::shared_ptr<Node> node) const;
    bool IsValidBST(std::shared_ptr<Node> node, std::shared_ptr<Node>& prev) const;

    bool IsBlackBalanced(std::shared_ptr<Node> root) const;

    bool ColorValidation(std::shared_ptr<Node> node) const;

    void LeftRotate(std::shared_ptr<Node> x);
    void RightRotate(std::shared_ptr<Node> x);

    void InsertFixup(std::shared_ptr<Node> z);

    void Transplant(std::shared_ptr<Node> u, std::shared_ptr<Node> v);
    void DeleteFixup(std::shared_ptr<Node> x);
    std::shared_ptr<Node> Minimum(std::shared_ptr<Node> node);

signals:
    void UpdateHeightSignal();
    void UpdateNodeCountSignal();

    void ErrorMessageSignal(QString);

    void HighlightNodeSignal(std::shared_ptr<Node> x, QColor color, bool isLeft = true, const QString& key = "");
    void ChangeColorSignal(std::shared_ptr<Node> node, Color color);
    void CreateNodeSignal(std::shared_ptr<Node> node);
    void MoveNodeSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isRoot = false);


    void MoveYSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isLeftRotate, bool isRoot = false);
    void MoveXSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild);
    void MoveStartSignal(std::shared_ptr<Node> x, std::shared_ptr<Node> y, bool x_leftChild, bool y_leftChild);
    void ChangeParentSignal(std::shared_ptr<Node> x, std::shared_ptr<Node> y);

    void LeftRotateSignal(std::shared_ptr<Node> x);
    void RightRotateSignal(std::shared_ptr<Node> x);


    void TransplantSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isRoot = false);
    void ChangeSiblingSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild);
    void DeleteSignal(std::shared_ptr<Node> node);

private slots:
    void On_EnableRBTValidations(bool state);
};


#endif // REDBLACKTREE_H
