#include "redblacktree.h"
#include <QTest>
#include <QDir>

class TestRedBlackTree : public QObject
{
    Q_OBJECT
private:
    RedBlackTree redBlackTree;
private slots:
    void TestJsonFileHandling();
    void TestTxtFileHandling();
    void TestXmlFileHandling();
    void TestInsert();
    void TestDelete();
};


void TestRedBlackTree::TestJsonFileHandling()
{
    QVERIFY(redBlackTree.ImportTree(QDir::currentPath() + "/rbtree.json"));
    QVERIFY(redBlackTree.ExportTree(QDir::currentPath() + "/rbtree2.json"));
}

void TestRedBlackTree::TestTxtFileHandling()
{
    QVERIFY(redBlackTree.ImportTree(QDir::currentPath() + "/rbtree.txt"));
    QVERIFY(redBlackTree.ExportTree(QDir::currentPath() + "/rbtree2.txt"));
}

void TestRedBlackTree::TestXmlFileHandling()
{
    QVERIFY(redBlackTree.ImportTree(QDir::currentPath() + "/rbtree.xml"));
    QVERIFY(redBlackTree.ExportTree(QDir::currentPath() + "/rbtree2.xml"));
}

void TestRedBlackTree::TestInsert()
{
    redBlackTree.ImportTree(QDir::currentPath() + "/rbtree.json");
    redBlackTree.Insert("100");
    QVERIFY(redBlackTree.Find("100"));
}

void TestRedBlackTree::TestDelete()
{
    redBlackTree.ImportTree(QDir::currentPath() + "/rbtree.json");
    QVERIFY(redBlackTree.Delete("5"));
}

QTEST_MAIN(TestRedBlackTree)
#include "testredblacktree.moc"
