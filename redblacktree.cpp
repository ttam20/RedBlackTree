#include "redblacktree.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDomDocument>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QQueue>

RedBlackTree::RedBlackTree() :
    root(NIL), height(0), nodeCount(0), enableRBTValidations(true)
{}

void RedBlackTree::SetTreeDataType(DataType dataType)
{
    this->dataType = dataType;
}

bool RedBlackTree::SetTreeDataType(char type)
{
    switch (std::tolower(type))
    {
    case 'n':
        dataType = DataType::NUMBER;
        return true;
    case 'c':
        dataType = DataType::CHAR;
        return true;
    case 't':
        dataType = DataType::TEXT;
        return true;
    default:
        return false;
    }
}

QChar RedBlackTree::GetTreeDataTypeChar() const
{
    switch (dataType)
    {
    case DataType::NUMBER:
        return 'n';
    case DataType::CHAR:
        return 'c';
    case DataType::TEXT:
        return 't';
    default:
        return QChar();
    }
}

bool RedBlackTree::TryGetColorFromChar(const QChar &colorChar, Color& color)
{
    switch (colorChar.toLatin1())
    {
    case 'R':
        color = Color::RED;
        return true;
    case 'B':
        color = Color::BLACK;
        return true;
    default:
        emit ErrorMessageSignal("Invalid color!");
        root = NIL;
        return false;
    }
}


quint16 RedBlackTree::CalculateHeight(std::shared_ptr<Node> node)
{
    if (node == NIL)
        return 0;

    return 1 + std::max(CalculateHeight(node->left), CalculateHeight(node->right));
}


void RedBlackTree::UpdateHeight()
{
    height = CalculateHeight(root);
    emit UpdateHeightSignal();
}

quint16 RedBlackTree::CalculateNodeCount(std::shared_ptr<Node> node)
{
    if (node == NIL)
        return 0;

    return 1 + CalculateNodeCount(node->left) + CalculateNodeCount(node->right);
}


void RedBlackTree::UpdateNodeCount()
{
    nodeCount = CalculateNodeCount(root);
    emit UpdateNodeCountSignal();
}


quint16 RedBlackTree::GetNewNodeHeight(const QString &key)
{
    bool ok = true;
    auto data = ConvertValue(dataType, key, ok);
    if (!ok)
        return 0;

    auto z = std::make_shared<Node>(data, Color::RED);
    auto x = root;

    int nodeHeight = 0;
    while (x != NIL)
    {
        if (z->CompareData(x->data))
            x = x->left;
        else
            x = x->right;

        ++nodeHeight;
    }
    return nodeHeight + 1;
}

bool RedBlackTree::IsBST(std::shared_ptr<Node> node) const
{
    std::shared_ptr<Node> prev = NIL;
    return IsValidBST(node, prev);
}

bool RedBlackTree::IsValidBST(std::shared_ptr<Node> node, std::shared_ptr<Node> &prev) const
{
    if (node == NIL)
        return true;

    bool left = IsValidBST(node->left, prev);

    if (prev != NIL && node->CompareData(prev->data))
        return false;

    prev = node;

    bool right = IsValidBST(node->right, prev);

    return left && right;
}

bool RedBlackTree::IsBlackBalanced(std::shared_ptr<Node> root) const
{
    return root->GetBlackHeight() != -1;
}

bool RedBlackTree::ColorValidation(std::shared_ptr<Node> node) const
{
    if (node == NIL)
        return true;

    if (node->left->color       == Color::RED &&
        node->left->left->color == Color::RED)
        return false;

    if (node->left->color        == Color::RED &&
        node->left->right->color == Color::RED)
        return false;

    if (node->right->color        == Color::RED &&
        node->right->right->color == Color::RED)
        return false;

    if (node->right->color       == Color::RED &&
        node->right->left->color == Color::RED)
        return false;

    return ColorValidation(node->left) && ColorValidation(node->right);
}

void RedBlackTree::LeftRotate(std::shared_ptr<Node> x)
{
    auto y = x->right;

    emit MoveStartSignal(x, y, false, true);
    x->right = y->left;

    if (y->left != NIL)
    {
        emit ChangeParentSignal(y->left, x);
        y->left->parent = x;
    }

    emit ChangeParentSignal(y, x->parent.lock());
    y->parent = x->parent;

    auto xp = x->parent.lock();
    if (xp == NIL)
    {
        emit MoveYSignal(y, root, true, true, true);
        root = y;
    }
    else if (x == xp->left)
    {
        emit MoveYSignal(y, xp, true, true);
        xp->left = y;
    }
    else
    {
        emit MoveYSignal(y, xp, false, true);
        xp->right = y;
    }

    emit MoveXSignal(x, y, true);
    emit ChangeParentSignal(x, y);

    y->left = x;
    x->parent = y;

    emit LeftRotateSignal(x);
}

void RedBlackTree::RightRotate(std::shared_ptr<Node> x)
{
    auto y = x->left;

    emit MoveStartSignal(x, y, true, false);
    x->left = y->right;

    if (y->right != NIL)
    {
        emit ChangeParentSignal(y->right, x);
        y->right->parent = x;
    }

    emit ChangeParentSignal(y, x->parent.lock());
    y->parent = x->parent;

    auto xp = x->parent.lock();
    if (xp == NIL)
    {
        emit MoveYSignal(y, root, true, false, true);
        root = y;
    }
    else if (x == xp->left)
    {
        emit MoveYSignal(y, xp, true, false);
        xp->left = y;
    }
    else
    {
        emit MoveYSignal(y, xp, false, false);
        xp->right = y;
    }

    emit MoveXSignal(x, y, false);
    emit ChangeParentSignal(x, y);

    y->right = x;
    x->parent = y;

    emit RightRotateSignal(x);
}

void RedBlackTree::Insert(const QString& key)
{
    bool ok = true;
    auto data = ConvertValue(dataType, key, ok);
    if (!ok)
        return;

    auto z = std::make_shared<Node>(data, Color::RED);
    emit CreateNodeSignal(z);

    auto x = root;
    auto y = NIL;

    while (x != NIL)
    {
        y = x;

        if (z->CompareData(x->data))
        {
            emit HighlightNodeSignal(x, Qt::blue, true, key);
            x = x->left;
        }
        else
        {
            emit HighlightNodeSignal(x, Qt::blue, false, key);
            x = x->right;
        }
    }

    z->parent = y;

    if (y == NIL)
    {
        emit MoveNodeSignal(z, root, true, true);
        root = z;
    }
    else if (z->CompareData(y->data))
    {
        emit MoveNodeSignal(z, y, true);
        y->left = z;
    }
    else
    {
        emit MoveNodeSignal(z, y, false);
        y->right = z;
    }

    z->left = NIL;
    z->right = NIL;

    InsertFixup(z);

    UpdateHeight();
    UpdateNodeCount();
}


void RedBlackTree::InsertFixup(std::shared_ptr<Node> z)
{
    std::shared_ptr<Node> y;

    while (z->parent.lock()->color == Color::RED)
    {
        auto zp = z->parent.lock();
        auto zpp = zp->parent.lock();

        if (zp == zpp->left)
        {
            y = zpp->right;
            if (y->color == Color::RED)
            {
                emit ChangeColorSignal(zp, Color::BLACK);
                emit ChangeColorSignal(y, Color::BLACK);
                emit ChangeColorSignal(zpp, Color::RED);

                zp->color = Color::BLACK;
                y->color = Color::BLACK;
                zpp->color = Color::RED;
                z = zpp;
            }
            else
            {
                if (z == zp->right)
                {
                    z = zp;
                    LeftRotate(z);
                }

                auto zp = z->parent.lock();
                auto zpp = zp->parent.lock();

                emit ChangeColorSignal(zp, Color::BLACK);
                emit ChangeColorSignal(zpp, Color::RED);

                zp->color = Color::BLACK;
                zpp->color = Color::RED;
                RightRotate(zpp);
            }
        }
        else
        {
            y = zpp->left;
            if (y->color == Color::RED)
            {
                emit ChangeColorSignal(zp, Color::BLACK);
                emit ChangeColorSignal(y, Color::BLACK);
                emit ChangeColorSignal(zpp, Color::RED);

                zp->color = Color::BLACK;
                y->color = Color::BLACK;
                zpp->color = Color::RED;
                z = zpp;
            }
            else
            {
                if (z == zp->left)
                {
                    z = zp;
                    RightRotate(z);
                }

                auto zp = z->parent.lock();
                auto zpp = zp->parent.lock();

                emit ChangeColorSignal(zp, Color::BLACK);
                emit ChangeColorSignal(zpp, Color::RED);

                zp->color = Color::BLACK;
                zpp->color = Color::RED;
                LeftRotate(zpp);
            }
        }
    }

    emit ChangeColorSignal(root, Color::BLACK);
    root->color = Color::BLACK;
}

void RedBlackTree::Transplant(std::shared_ptr<Node> u, std::shared_ptr<Node> v)
{
    auto up = u->parent.lock();
    if (up == NIL)
    {
        emit TransplantSignal(v, root, true, true);
        root = v;
    }
    else if (u == up->left)
    {
        emit TransplantSignal(v, up, true);
        up->left = v;
    }
    else
    {
        emit TransplantSignal(v, up, false);
        up->right = v;
    }

    emit ChangeParentSignal(v, up);
    v->parent = up;
}


std::shared_ptr<Node> RedBlackTree::Minimum(std::shared_ptr<Node> node)
{
    auto x = node;
    while (node->left != NIL)
    {
        emit HighlightNodeSignal(x, Qt::yellow, false, "min");
        node = node->left;
    }
    emit HighlightNodeSignal(node, Qt::yellow, false);
    return node;
}


bool RedBlackTree::Delete(const QString &key)
{
    auto z = NIL;
    auto node = root;

    bool ok = true;
    auto data = ConvertValue(dataType, key, ok);
    if (!ok)
        return false;

    while (node != NIL)
    {
        if (node->data == data)
            z = node;

        if (node->CompareData(data) || node->data == data)
        {
            emit HighlightNodeSignal(node, Qt::blue, false, key);
            node = node->right;
        }
        else
        {
            emit HighlightNodeSignal(node, Qt::blue, true, key);
            node = node->left;
        }
    }

    if (z == NIL)
    {
        emit ErrorMessageSignal("Key was not found in the tree!");
        return false;
    }
    else
        emit HighlightNodeSignal(z, QColor(Qt::magenta));

    std::shared_ptr<Node> x, y;

    y = z;
    Color y_original_color = y->color;

    if (z->left == NIL)
    {
        x = z->right;
        Transplant(z, z->right);
    }
    else if (z->right == NIL)
    {
        x = z->left;
        Transplant(z, z->left);
    }
    else
    {
        y = Minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y != z->right)
        {
            Transplant(y, y->right);

            emit ChangeSiblingSignal(y, z->right, false);
            emit ChangeParentSignal(y->right, y);

            y->right = z->right;
            y->right->parent = y;
        }
        else
        {
            emit ChangeParentSignal(x, y);
            x->parent = y;    
        }

        Transplant(z, y);

        emit ChangeSiblingSignal(y, z->left, true);
        emit ChangeParentSignal(y->left, y);
        emit ChangeColorSignal(y, z->color);

        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    emit DeleteSignal(z);

    if (y_original_color == Color::BLACK)
        DeleteFixup(x);

    UpdateHeight();
    UpdateNodeCount();

    return true;
}

void RedBlackTree::DeleteFixup(std::shared_ptr<Node> x)
{
    std::shared_ptr<Node> w;
    while (x != root && x->color == Color::BLACK)
    {
        auto xp = x->parent.lock();

        if (x == xp->left)
        {
            w = xp->right;
            if (w->color == Color::RED)
            {
                emit ChangeColorSignal(w, Color::BLACK);
                emit ChangeColorSignal(xp, Color::RED);

                w->color = Color::BLACK;
                xp->color = Color::RED;
                LeftRotate(xp);
                w = xp->right;
            }
            if (w->left->color == Color::BLACK &&
                w->right->color == Color::BLACK)
            {
                emit ChangeColorSignal(w, Color::RED);

                w->color = Color::RED;
                x = xp;
            }
            else
            {
                if (w->right->color == Color::BLACK)
                {
                    emit ChangeColorSignal(w->left, Color::BLACK);
                    emit ChangeColorSignal(w, Color::RED);

                    w->left->color = Color::BLACK;
                    w->color = Color::RED;
                    RightRotate(w);
                    w = xp->right;
                }
                emit ChangeColorSignal(w, xp->color);
                emit ChangeColorSignal(xp, Color::BLACK);
                emit ChangeColorSignal(w->right, Color::BLACK);

                w->color = xp->color;
                xp->color = Color::BLACK;
                w->right->color = Color::BLACK;
                LeftRotate(xp);
                x = root;
            }
        }
        else
        {
            w = xp->left;
            if (w->color == Color::RED)
            {
                emit ChangeColorSignal(w, Color::BLACK);
                emit ChangeColorSignal(xp, Color::RED);

                w->color = Color::BLACK;
                xp->color = Color::RED;
                RightRotate(xp);
                w = xp->left;
            }
            if (w->right->color == Color::BLACK &&
                w->left->color == Color::BLACK)
            {
                emit ChangeColorSignal(w, Color::RED);

                w->color = Color::RED;
                x = xp;
            }
            else
            {
                if (w->left->color == Color::BLACK)
                {
                    emit ChangeColorSignal(w->right, Color::BLACK);
                    emit ChangeColorSignal(w, Color::RED);

                    w->right->color = Color::BLACK;
                    w->color = Color::RED;
                    LeftRotate(w);
                    w = xp->left;
                }
                emit ChangeColorSignal(w, xp->color);
                emit ChangeColorSignal(xp, Color::BLACK);
                emit ChangeColorSignal(w->left, Color::BLACK);

                w->color = xp->color;
                xp->color = Color::BLACK;
                w->left->color = Color::BLACK;
                RightRotate(xp);
                x = root;
            }
        }
    }

    emit ChangeColorSignal(x, Color::BLACK);
    x->color = Color::BLACK;
}

bool RedBlackTree::Find(const QString &key)
{
    auto node = root;

    bool ok = true;
    auto data = ConvertValue(dataType, key, ok);
    if (!ok)
        return false;

    while (node != NIL)
    {
        if (node->data == data)
        {
            emit HighlightNodeSignal(node, QColor(Qt::green));
            return true;
        }

        if (node->CompareData(data))
        {
            emit HighlightNodeSignal(node, Qt::blue, false, key);
            node = node->right;
        }
        else
        {
            emit HighlightNodeSignal(node, Qt::blue, true, key);
            node = node->left;
        }
    }

    return false;
}

void RedBlackTree::On_EnableRBTValidations(bool state)
{
    enableRBTValidations = state;
}

std::variant<qint16, QString, QChar> RedBlackTree::ConvertValue(DataType dataType, const QString& valueStr, bool& ok)
{
    switch(dataType)
    {
    case DataType::NUMBER:
    {
        qint16 value = valueStr.toShort(&ok);
        if (!ok || !(value > LOWER_BOUND && value < UPPER_BOUND))
        {
            emit ErrorMessageSignal(QString("Invalid node data!\nNumber must be between %1 and %2.")
                                    .arg(LOWER_BOUND).arg(UPPER_BOUND));
            ok = false;
            return quint16{};
        }
        return value;
    }
    case DataType::TEXT:
        if (valueStr.length() > MAX_LENGTH)
        {
            emit ErrorMessageSignal(QString("Invalid node data!\nText must be less than %1 characters.")
                                        .arg(MAX_LENGTH + 1));
            ok = false;
            return QString{};
        }
        ok = true;
        return valueStr;
    case DataType::CHAR:
        if (valueStr.length() == 1)
        {
            ok = true;
            return QChar(valueStr[0]);
        }
        else
        {
            emit ErrorMessageSignal("Invalid node data!\nCharacter length must be 1.");
            ok = false;
            return QChar{};
        }
    default:
        return std::variant<qint16, QString, QChar>{};
    }
}

template<class T>
void RedBlackTree::ReadTree(T& in, std::shared_ptr<Node>& node, DataType dataType, bool& ok)
{
    QString valueStr;
    QChar colorChar;

    in >> valueStr;
    if (valueStr == QStringLiteral("NIL") || in.status())
    {
        node = NIL;
        return;
    }

    auto value = ConvertValue(dataType, valueStr, ok);

    if (!ok)
    {
        root = NIL;
        return;
    }

    if constexpr (std::is_same_v<T, QTextStream>)
        in.skipWhiteSpace();

    in >> colorChar;

    Color color;
    if (!TryGetColorFromChar(colorChar, color))
    {
        ok = false;
        return;
    }

    node = std::make_shared<Node>(value, color);

    ReadTree(in, node->left, dataType, ok);
    if (node->left != NIL)
        node->left->parent = node;

    ReadTree(in, node->right, dataType, ok);
    if (node->right != NIL)
        node->right->parent = node;
}


void RedBlackTree::WriteTree(QTextStream& out, const Node* node, bool isFirst) const
{
    if (node == NIL.get())
    {
        out << " NIL";
        return;
    }

    if (!isFirst)
        out << " ";
    out << node->GetDataString() << " " << node->GetColorChar();

    WriteTree(out, node->left.get(), false);
    WriteTree(out, node->right.get(), false);
}

void RedBlackTree::WriteTree(QDataStream &out, const Node *node) const
{
    if (node == NIL.get())
    {
        out << QStringLiteral("NIL");
        return;
    }

    out << node->GetDataString() << node->GetColorChar();

    WriteTree(out, node->left.get());
    WriteTree(out, node->right.get());
}

template<class T>
void RedBlackTree::ReadFromStream(T &in, RedBlackTree &newRedBlackTree, bool &ok)
{
    QChar type;
    in >> type;

    if (!newRedBlackTree.SetTreeDataType(type.toLatin1()))
    {
        emit ErrorMessageSignal("Invalid tree data type!\nPossible characters: N, T, C");
        ok = false;
        return;
    }

    ReadTree(in, newRedBlackTree.root, newRedBlackTree.dataType, ok);
}

void RedBlackTree::ReadJSON(const QString& fileData, RedBlackTree &redBlackTree, bool &ok)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData.toUtf8());
    if (jsonDoc.isNull())
    {
        emit ErrorMessageSignal("Invalid JSON file!");
        ok = false;
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    if (!CheckJsonKey(jsonObj, "dataType", ok))
        return;

    QString dataType = jsonObj["dataType"].toString();

    if (dataType.length() != 1 || !redBlackTree.SetTreeDataType(dataType.at(0).toLatin1()))
    {
        emit ErrorMessageSignal("Invalid tree data type!\nPossible characters: N, T, C");
        ok = false;
        return;
    }

    if (!jsonObj.contains("tree") || !jsonObj["tree"].isArray())
    {
        emit ErrorMessageSignal("\"tree\" key is missing or is not an array!");
        ok = false;
        return;
    }

    QJsonArray tree = jsonObj["tree"].toArray();
    QMap<int, std::shared_ptr<Node>> nodeMap;
    std::vector<int> indexes;
    int i = 0;

    for (const QJsonValue& nodeValue : tree)
    {
        QJsonObject nodeObj = nodeValue.toObject();

        if (!CheckJsonKey(nodeObj, "color", ok) || !CheckJsonKey(nodeObj, "value", ok))
            return;

        auto value = ConvertValue(redBlackTree.GetDataType(),
            nodeObj["value"].isDouble() ? QString::number(nodeObj["value"].toInt()) : nodeObj["value"].toString(), ok);

        if (!ok)
            return;

        QString colorStr = nodeObj["color"].toString();
        Color color;
        if (colorStr.length() != 1 || !TryGetColorFromChar(colorStr.at(0), color))
        {
            ok = false;
            return;
        }

        QJsonValue leftValue = nodeObj["left"];
        QJsonValue rightValue = nodeObj["right"];

        if (leftValue.isDouble())
            indexes.push_back(leftValue.toInt());

        if (rightValue.isDouble())
            indexes.push_back(rightValue.toInt());

        std::shared_ptr<Node> newNode = std::make_shared<Node>(value, color);
        nodeMap.insert(i++, newNode);
    }

    auto it = std::adjacent_find(indexes.begin(), indexes.end(), std::greater<int>());

    if (it != indexes.end() ||
       !std::all_of(indexes.begin(), indexes.end(), [nodeMap](int x) { return x > 0 && x < nodeMap.count(); }))
    {
        emit ErrorMessageSignal("Invalid indexes!");
        ok = false;
        return;
    }

    i = 0;
    for (const QJsonValue& nodeValue : tree)
    {
        QJsonObject nodeObj = nodeValue.toObject();

        auto currentNode = nodeMap.value(i++);

        if (!CheckJsonKey(nodeObj, "left", ok) || !CheckJsonKey(nodeObj, "right", ok))
            return;

        QJsonValue leftValue = nodeObj["left"];
        QJsonValue rightValue = nodeObj["right"];

        if (!leftValue.isNull())
        {
            if  (!leftValue.isDouble())
            {
                emit ErrorMessageSignal("Invalid left node index!");
                ok = false;
                return;
            }

            int leftIndex = leftValue.toInt();
            currentNode->left = nodeMap.value(leftIndex);
            nodeMap.value(leftIndex)->parent = currentNode;
        }
        else
            currentNode->left = NIL;

        if (!rightValue.isNull())
        {
            if  (!rightValue.isDouble())
            {
                emit ErrorMessageSignal("Invalid right node index!");
                ok = false;
                return;
            }

            int rightIndex = rightValue.toInt();
            currentNode->right = nodeMap.value(rightIndex);
            nodeMap.value(rightIndex)->parent = currentNode;
        }
        else
            currentNode->right = NIL;

    }

    if (!nodeMap.isEmpty())
        redBlackTree.root = nodeMap.first();
}

bool RedBlackTree::CheckJsonKey(const QJsonObject &obj, const QString &key, bool &ok)
{
    if (!obj.contains(key))
    {
        emit ErrorMessageSignal(QString("\"%1\" key is missing!").arg(key));
        ok = false;
        return false;
    }
    return true;
}

void RedBlackTree::WriteJSON(QFile& file) const
{
    QJsonDocument jsonDoc;
    QJsonObject jsonObj;

    jsonObj.insert("dataType", QString(GetTreeDataTypeChar().toLatin1()));

    QJsonArray treeArray;
    QMap<std::shared_ptr<Node>, int> nodeMap;

    std::function<void(const std::shared_ptr<Node>&)> serializeNode = [this, &treeArray, &serializeNode, &nodeMap](const std::shared_ptr<Node>& node)
    {
        if (node == NIL)
            return;

        int currentIndex = treeArray.size();
        QJsonObject nodeObj;
        if (dataType == DataType::NUMBER)
            nodeObj.insert("value", node->GetDataString().toInt());
        else
            nodeObj.insert("value", node->GetDataString());

        nodeObj.insert("color", QString(node->GetColorChar()));

        treeArray.append(nodeObj);

        nodeMap[node] = currentIndex;

        serializeNode(node->left);
        serializeNode(node->right);
    };

    if (root)
        serializeNode(root);


    for (auto it = nodeMap.constBegin(); it != nodeMap.constEnd(); ++it)
    {
        const std::shared_ptr<Node>& node = it.key();
        int index = it.value();

        QJsonObject nodeObj = treeArray[index].toObject();

        if (node->left != NIL)
            nodeObj.insert("left", nodeMap[node->left]);
        else
            nodeObj.insert("left", QJsonValue::Null);

        if (node->right != NIL)
            nodeObj.insert("right", nodeMap[node->right]);
        else
            nodeObj.insert("right", QJsonValue::Null);

        treeArray[index] = nodeObj;
    }

    jsonObj.insert("tree", treeArray);

    jsonDoc.setObject(jsonObj);
    file.write(jsonDoc.toJson());
}

void RedBlackTree::ReadXML(QFile &file, RedBlackTree &redBlackTree, bool &ok)
{
    QXmlStreamReader xml(&file);

    QList<QString> leftInd, rightInd;
    QMap<unsigned int, std::shared_ptr<Node>> nodeMap;
    int i = 0;

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == QStringLiteral("dataType"))
            {
                xml.readNext();
                QString dataType = xml.text().toString();

                if (dataType.length() != 1 || !redBlackTree.SetTreeDataType(dataType.at(0).toLatin1()))
                {
                    emit ErrorMessageSignal("Invalid tree data type!\nPossible characters: N, T, C");
                    ok = false;
                    return;
                }
            }
            else if (xml.name() == QStringLiteral("node"))
            {
                std::variant<qint16, QString, QChar> value;
                Color color;
                char checkAssigns = 0b0000;

                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QStringLiteral("node")))
                {
                    if (xml.tokenType() == QXmlStreamReader::StartElement)
                    {
                        if (xml.name() == QStringLiteral("value"))
                        {
                            xml.readNext();
                            value = ConvertValue(redBlackTree.GetDataType(), xml.text().toString(), ok);

                            if (!ok)
                                return;

                            checkAssigns |= 0b0001;
                        }
                        else if (xml.name() == QStringLiteral("color"))
                        {
                            xml.readNext();
                            QString colorStr = xml.text().toString();

                            if (colorStr.length() != 1 || !TryGetColorFromChar(colorStr.at(0), color))
                            {
                                ok = false;
                                return;
                            }

                            checkAssigns |= 0b0010;
                        }
                        else if (xml.name() == QStringLiteral("left"))
                        {
                            xml.readNext();
                            leftInd.append(xml.text().toString());

                            checkAssigns |= 0b0100;
                        }
                        else if (xml.name() == QStringLiteral("right"))
                        {
                            xml.readNext();
                            rightInd.append(xml.text().toString());

                            checkAssigns |= 0b1000;
                        }
                    }

                    if (xml.hasError())
                    {
                        emit ErrorMessageSignal(xml.errorString());
                        ok = false;
                        return;
                    }

                    xml.readNext();
                }

                if (checkAssigns != 0b1111)
                {
                    emit ErrorMessageSignal("Invalid or missing xml node!\nAllowed nodes are: value, color, left, right.");
                    ok = false;
                    return;
                }

                std::shared_ptr<Node> newNode = std::make_shared<Node>(value, color);
                nodeMap.insert(i++, newNode);
            }
        }
    }

    if (xml.hasError())
    {
        emit ErrorMessageSignal(xml.errorString());
        ok = false;
        return;
    }

    std::vector<unsigned int> indexes;
    i = 0;
    for (; i < nodeMap.size(); ++i)
    {
        auto currentNode = nodeMap.value(i);
        QString leftValue = leftInd[i];
        QString rightValue = rightInd[i];

        if (!leftValue.isEmpty() && leftValue.toUpper() != QStringLiteral("NIL"))
        {
            unsigned int leftIndex = leftValue.toUInt(&ok);

            if  (!ok || leftIndex == 0 || leftIndex >= nodeMap.count())
            {
                emit ErrorMessageSignal("Invalid left node index!");
                ok = false;
                return;
            }

            indexes.push_back(leftIndex);

            currentNode->left = nodeMap.value(leftIndex);
            nodeMap.value(leftIndex)->parent = currentNode;
        }
        else
            currentNode->left = NIL;

        if (!rightValue.isEmpty() && rightValue.toUpper() != QStringLiteral("NIL"))
        {
            unsigned int rightIndex = rightValue.toUInt(&ok);

            if  (!ok || rightIndex == 0 || rightIndex >= nodeMap.count())
            {
                emit ErrorMessageSignal("Invalid right node index!");
                ok = false;
                return;
            }

            indexes.push_back(rightIndex);

            currentNode->right = nodeMap.value(rightIndex);
            nodeMap.value(rightIndex)->parent = currentNode;
        }
        else
            currentNode->right = NIL;
    }

    auto it = std::adjacent_find(indexes.begin(), indexes.end(), std::greater<int>());

    if (it != indexes.end())
    {
        emit ErrorMessageSignal("Invalid indexes!");
        ok = false;
        return;
    }

    if (!nodeMap.isEmpty())
        redBlackTree.root = nodeMap.first();
}

void RedBlackTree::WriteXML(QFile &file) const
{
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("treeData");
    stream.writeTextElement("dataType", GetTreeDataTypeChar());
    stream.writeStartElement("nodes");

    QMap<std::shared_ptr<Node>, int> nodeMap;
    QList<std::shared_ptr<Node>> nodeList;

    QQueue<std::shared_ptr<Node>> queue;

    if (root)
        queue.enqueue(root);

    while (!queue.isEmpty())
    {
        auto node = queue.dequeue();
        if (node == NIL)
            continue;

        nodeMap[node] = nodeList.size();
        nodeList.append(node);

        if (node->left != NIL)
            queue.enqueue(node->left);
        if (node->right != NIL)
            queue.enqueue(node->right);
    }

    for (const auto& node : nodeList)
    {
        stream.writeStartElement("node");

        stream.writeTextElement("value", node->GetDataString());
        stream.writeTextElement("color", node->GetColorChar());

        stream.writeTextElement("left", (node->left != NIL ? QString::number(nodeMap[node->left]) : ""));
        stream.writeTextElement("right", (node->right != NIL ? QString::number(nodeMap[node->right]) : ""));

        stream.writeEndElement();
    }

    stream.writeEndElement();
    stream.writeEndElement();

    stream.writeEndDocument();
}

bool RedBlackTree::ImportTree(const QString& fileName)
{
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);

    RedBlackTree newRedBlackTree;
    bool ok = true;

    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix();
    bool isBinary = suffix == QStringLiteral("bin") || suffix == QStringLiteral("dat");

    if (!file.open((isBinary ? QIODevice::ReadOnly : QIODevice::ReadOnly | QIODevice::Text)))
    {
        emit ErrorMessageSignal("File could not be opened for reading!");
        return false;
    }

    if (suffix == QStringLiteral("txt"))
    {
        QTextStream in(&file);

        ReadFromStream(in, newRedBlackTree, ok);
    }
    else if (isBinary)
    {
        QDataStream in(&file);

        ReadFromStream(in, newRedBlackTree, ok);
    }
    else if (suffix == QStringLiteral("json"))
    {
        ReadJSON(file.readAll(), newRedBlackTree, ok);
    }
    else if (suffix == QStringLiteral("xml"))
    {
        ReadXML(file, newRedBlackTree, ok);
    }

    QString errMsg = "";

    if (enableRBTValidations)
    {
        if (newRedBlackTree.root->color != Color::BLACK)
            errMsg += "The root of the tree must be black!";

        if (!IsBST(newRedBlackTree.root))
            errMsg += QString(!errMsg.isEmpty() ? "\n" : "") + "The tree is not a valid binary search tree!";

        if (!ColorValidation(newRedBlackTree.root))
            errMsg += QString(!errMsg.isEmpty() ? "\n" : "") + "The colors of the tree nodes do not follow Red-black tree rules!\n"
                                                               "Check the 4th requirement under 'Properties' menu.";

        if (!IsBlackBalanced(newRedBlackTree.root))
            errMsg += QString(!errMsg.isEmpty() ? "\n" : "") + "The tree is not black-balanced!\n"
                                                               "Check the 5th requirement under 'Properties' menu.";
    }

    if (ok && errMsg.isEmpty())
    {
        newRedBlackTree.root->parent = NIL;
        *this = std::move(newRedBlackTree);
        UpdateHeight();
        UpdateNodeCount();
    }
    else
    {
        if (!errMsg.isEmpty())
            emit ErrorMessageSignal(errMsg);
        ok = false;
    }

    return ok;
}


bool RedBlackTree::ExportTree(const QString& fileName)
{ 
    if (root == NIL || fileName.isEmpty())
        return false;

    QFile file(fileName);

    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix();
    bool isBinary = suffix == QStringLiteral("bin") || suffix == QStringLiteral("dat");

    if (!file.open((isBinary ? QIODevice::WriteOnly : QIODevice::WriteOnly | QIODevice::Text)))
    {
        emit ErrorMessageSignal("File could not be opened for writing!");
        return false;
    }

    if (suffix == QStringLiteral("txt"))
    {
        QTextStream out(&file);

        out << GetTreeDataTypeChar() << "\n";

        WriteTree(out, root.get());
    }
    else if (isBinary)
    {
        QDataStream out(&file);

        out << GetTreeDataTypeChar();

        WriteTree(out, root.get());
    }
    else if (suffix == QStringLiteral("json"))
    {
        WriteJSON(file);
    }
    else if (suffix == QStringLiteral("xml"))
    {
        WriteXML(file);
    }

    return true;
}


// void RedBlackTree::PrintTree(const Node* node, QListWidget* list, int depth)
// {
//     if (node == NIL.get())
//     {
//         list->addItem(QString(depth * 2, ' ') + "NIL");
//         return;
//     }

//     list->addItem(QString(depth * 2, ' ') + node->GetDataString() + " (" + node->GetColorChar() + ")\n");
//     PrintTree(node->left.get(), list, depth + 1);
//     PrintTree(node->right.get(), list, depth + 1);
// }


RedBlackTree& RedBlackTree::operator=(RedBlackTree&& other) noexcept
{
    if (this != &other)
    {
        root = std::move(other.root);
        dataType = other.dataType;
        height = other.height;
        nodeCount = other.nodeCount;
    }
    return *this;
}

