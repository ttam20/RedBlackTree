#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSequentialAnimationGroup>
#include <QGraphicsEllipseItem>
#include <QPropertyAnimation>
#include "nilnode.h"
#include "redblacktree.h"
#include "treenode.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    RedBlackTree redBlackTree;
    QSequentialAnimationGroup* seqGroup;

    QMenu* menu;
    QMenu* consoleMenu;
    QAction* newTreeAction;
    QAction* importAction;
    QAction* exportAction;
    QAction* clearAction;
    QAction* propertiesAction;

    TreeNode* root;

    QGraphicsScene *scene;

    qreal paddingX = 50;
    qreal paddingY = 50;
    qreal scenePadding = 20;

    QList<NilNode*> nilNodes;

    quint16 newNodeHeight = 0;
    QMap<std::shared_ptr<Node>, TreeNode*> nodeMap;
    QList<QPropertyAnimation*> leftRotateAnims, rightRotateAnims, transplantAnims;
private:
    void CreateMenus();
    void ConfigureLineEdits();
    void ShowPropertiesDialog();

    void Draw(quint16 height);
    void DrawTree(QGraphicsScene* scene, TreeNode* treeNode, int treeHeight);

    void MakeTree(TreeNode *&treeNode, std::shared_ptr<Node> node);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void On_NewTree();
    void On_Import();
    void On_Export();
    void On_Clear();
    void On_Properties();

    void On_UpdateHeight();
    void On_UpdateNodeCount();
    void On_ErrorMessage(const QString& msg);

    void On_DataSetButtonClicked();
    void On_InsertButtonClicked();
    void On_DeleteButtonClicked();
    void On_FindButtonClicked();

    void On_NilCbStateChanged(int state);
    void On_BlackHeightStateChanged(int state);
    void On_EnableRBTValidationsChanged(int state);

    void On_BackwardButtonClicked();
    void On_PlayButtonClicked();
    void On_ForwardButtonClicked();

    void On_HighlightNode(std::shared_ptr<Node> node, QColor color, bool isLeft, const QString& key);
    void On_ChangeColor(std::shared_ptr<Node> node, Color color);
    void On_CreateNode(std::shared_ptr<Node> node);
    void On_MoveNode(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isRoot = false);

    void On_MoveY(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isLeftRotate, bool isRoot = false);
    void On_MoveX(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild);
    void On_MoveStart(std::shared_ptr<Node> x, std::shared_ptr<Node> y, bool x_leftChild, bool y_leftChild);
    void On_ChangeParent(std::shared_ptr<Node> x, std::shared_ptr<Node> y);
    void On_LeftRotate(std::shared_ptr<Node> x);
    void On_RightRotate(std::shared_ptr<Node> x);

    void On_Transplant(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isRoot = false);
    void On_Delete(std::shared_ptr<Node> node);
    void On_ChangeSiblingSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild);

signals:
    void ShowBlackHeightSignal(bool);
    void EnableRBTValidations(bool state);
};
#endif // MAINWINDOW_H
