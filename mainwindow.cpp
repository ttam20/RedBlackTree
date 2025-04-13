#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "nilnode.h"
#include "treenode.h"
#include <QQueue>
#include <QTimer>
#include <QStandardPaths>
#include <QTextEdit>
#include <QParallelAnimationGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr), root(nullptr)
{
    ui->setupUi(this);
    seqGroup = new QSequentialAnimationGroup(this);

    setWindowTitle("Red-black tree");
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    setFixedSize(1024, 768);

    CreateMenus();

    connect(ui->dataSetButton, SIGNAL(clicked()), this, SLOT(On_DataSetButtonClicked()));
    connect(ui->insertButton, SIGNAL(clicked()), this, SLOT(On_InsertButtonClicked()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(On_DeleteButtonClicked()));
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(On_FindButtonClicked()));

    connect(ui->cbShowNil, SIGNAL(stateChanged(int)), this, SLOT(On_NilCbStateChanged(int)));
    connect(ui->cbShowBlackHeight, SIGNAL(stateChanged(int)), this, SLOT(On_BlackHeightStateChanged(int)));
    connect(ui->cbEnableRBTValidations, SIGNAL(stateChanged(int)), this, SLOT(On_EnableRBTValidationsChanged(int)));

    connect(newTreeAction, &QAction::triggered, this, &MainWindow::On_NewTree);
    connect(importAction, &QAction::triggered, this, &MainWindow::On_Import);
    connect(exportAction, &QAction::triggered, this, &MainWindow::On_Export);
    connect(clearAction,  &QAction::triggered, this, &MainWindow::On_Clear);
    connect(propertiesAction,  &QAction::triggered, this, &MainWindow::On_Properties);

    connect(&redBlackTree, &RedBlackTree::UpdateHeightSignal, this, &MainWindow::On_UpdateHeight);
    connect(&redBlackTree, &RedBlackTree::UpdateNodeCountSignal, this, &MainWindow::On_UpdateNodeCount);
    connect(&redBlackTree, SIGNAL(ErrorMessageSignal(QString)), this, SLOT(On_ErrorMessage(QString)));
    connect(this, SIGNAL(EnableRBTValidations(bool)), &redBlackTree, SLOT(On_EnableRBTValidations(bool)));

    connect(ui->backwardButton, SIGNAL(clicked()), this, SLOT(On_BackwardButtonClicked()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(On_PlayButtonClicked()));
    connect(ui->forwardButton, SIGNAL(clicked()), this, SLOT(On_ForwardButtonClicked()));

    connect(ui->insertLineEdit, &QLineEdit::returnPressed, ui->insertButton, &QPushButton::click);
    connect(ui->deleteLineEdit, &QLineEdit::returnPressed, ui->deleteButton, &QPushButton::click);
    connect(ui->findLineEdit, &QLineEdit::returnPressed, ui->findButton, &QPushButton::click);

    connect(&redBlackTree, SIGNAL(HighlightNodeSignal(std::shared_ptr<Node>,QColor,bool,QString)), this, SLOT(On_HighlightNode(std::shared_ptr<Node>,QColor,bool,QString)));
    connect(&redBlackTree, SIGNAL(ChangeColorSignal(std::shared_ptr<Node>,Color)), this, SLOT(On_ChangeColor(std::shared_ptr<Node>,Color)));
    connect(&redBlackTree, SIGNAL(CreateNodeSignal(std::shared_ptr<Node>)), this, SLOT(On_CreateNode(std::shared_ptr<Node>)));
    connect(&redBlackTree, SIGNAL(MoveNodeSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool)), this, SLOT(On_MoveNode(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool)));

    connect(&redBlackTree, SIGNAL(MoveYSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool,bool)), this, SLOT(On_MoveY(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool,bool)));
    connect(&redBlackTree, SIGNAL(MoveXSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool)), this, SLOT(On_MoveX(std::shared_ptr<Node>,std::shared_ptr<Node>,bool)));
    connect(&redBlackTree, SIGNAL(MoveStartSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool)), this, SLOT(On_MoveStart(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool)));
    connect(&redBlackTree, SIGNAL(ChangeParentSignal(std::shared_ptr<Node>,std::shared_ptr<Node>)), this, SLOT(On_ChangeParent(std::shared_ptr<Node>,std::shared_ptr<Node>)));
    connect(&redBlackTree, SIGNAL(LeftRotateSignal(std::shared_ptr<Node>)), this, SLOT(On_LeftRotate(std::shared_ptr<Node>)));
    connect(&redBlackTree, SIGNAL(RightRotateSignal(std::shared_ptr<Node>)), this, SLOT(On_RightRotate(std::shared_ptr<Node>)));

    connect(&redBlackTree, SIGNAL(TransplantSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool)), this, SLOT(On_Transplant(std::shared_ptr<Node>,std::shared_ptr<Node>,bool,bool)));
    connect(&redBlackTree, SIGNAL(DeleteSignal(std::shared_ptr<Node>)), this, SLOT(On_Delete(std::shared_ptr<Node>)));
    connect(&redBlackTree, SIGNAL(ChangeSiblingSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool)), this, SLOT(On_ChangeSiblingSignal(std::shared_ptr<Node>,std::shared_ptr<Node>,bool)));
}


void MainWindow::CreateMenus()
{
    menu = new QMenu("Tree");
    consoleMenu = new QMenu("Console");

    newTreeAction = new QAction("New tree");
    importAction = new QAction("Import tree");
    exportAction = new QAction("Export tree");
    clearAction = new QAction("Clear console");
    propertiesAction = new QAction("Properties");

    exportAction->setEnabled(false);

    menuBar()->addMenu(menu);
    menuBar()->addAction(propertiesAction);
    menuBar()->addMenu(consoleMenu);

    consoleMenu->addAction(clearAction);
    menu->addAction(newTreeAction);
    menu->addAction(importAction);
    menu->addAction(exportAction);
}

void MainWindow::Draw(quint16 height)
{
    delete scene;
    scene = new QGraphicsScene(ui->graphicsView);
    root = nullptr;

    nodeMap.clear();
    MakeTree(root, redBlackTree.GetRoot());
    DrawTree(scene, root, height);

    QRectF itemsRect = scene->itemsBoundingRect();
    itemsRect.adjust(-scenePadding, -scenePadding, scenePadding, scenePadding);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);

    ui->graphicsView->show();
}

void MainWindow::DrawTree(QGraphicsScene* scene, TreeNode *treeNode, int treeHeight)
{
    if (treeNode == nullptr)
        return;

    QQueue<TreeNode*> queue;
    queue.enqueue(treeNode);
    qreal padding = paddingX * qPow(2, ((--treeHeight) - 1)) / 2;

    while (!queue.empty())
    {
        int levelSize = queue.size();

        for (int i = 0; i < levelSize; ++i)
        {
            TreeNode* curr = queue.dequeue();

            if (curr->scene() == nullptr)
                scene->addItem(curr);

            if (curr->left)
            {
                curr->left->SetPos(curr->left->parent->GetX() - padding, curr->left->parent->GetY() + paddingY);
                curr->left->parent->SetLeftLine(QPointF(curr->left->rect().x() - padding, curr->left->rect().top()));
                queue.enqueue(curr->left);
            }
            else
            {
                NilNode* nilNode = new NilNode();
                qreal padding = paddingX * qPow(2, treeHeight) / 4;

                nilNode->SetPos(curr->GetX() - padding, curr->GetY() + paddingY);
                nilNode->SetLine(QPointF(curr->rect().center().x() + padding, curr->rect().y()));

                nilNodes.push_back(nilNode);
            }
            if (curr->right)
            {
                curr->right->SetPos(curr->right->parent->GetX() + padding, curr->right->parent->GetY() + paddingY);
                curr->right->parent->SetRightLine(QPointF(curr->right->rect().x() + padding, curr->right->rect().top()));
                queue.enqueue(curr->right);
            }
            else
            {
                NilNode* nilNode = new NilNode();
                qreal padding = paddingX * qPow(2, treeHeight) / 4;

                nilNode->SetPos(curr->GetX() + padding, curr->GetY() + paddingY);
                nilNode->SetLine(QPointF(curr->rect().center().x() - padding, curr->rect().y()));

                nilNodes.push_back(nilNode);
            }

        }
        //qDebug() << padding;
        padding = paddingX * qPow(2, ((--treeHeight) - 1)) / 2;
    }

    QTimer::singleShot(0, this, [scene, this]() {
        QRectF itemsRect = scene->itemsBoundingRect();
        itemsRect.adjust(-scenePadding, -scenePadding, scenePadding, scenePadding);

        scene->setSceneRect(itemsRect);
        ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);
    });
}


void MainWindow::MakeTree(TreeNode*& treeNode, std::shared_ptr<Node> node)
{
    if (node == NIL)
        return;

    treeNode = new TreeNode(node->GetDataString(), node->GetBlackHeight(), node->color);
    connect(this, SIGNAL(ShowBlackHeightSignal(bool)), treeNode, SLOT(On_ShowBlackHeight(bool)));

    nodeMap.insert(node, treeNode);

    MakeTree(treeNode->left, node->left);
    if (node->left != NIL)
        treeNode->left->parent = treeNode;

    MakeTree(treeNode->right, node->right);
    if (node->right != NIL)
        treeNode->right->parent = treeNode;
}

void MainWindow::On_NewTree()
{
    auto reply = QMessageBox::warning(this, "Warning", "Creating a new tree will erase your currently edited one.\nDo you want to proceed?",
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    ui->cbShowBlackHeight->setCheckState(Qt::Unchecked);
    ui->cbShowNil->setCheckState(Qt::Unchecked);
    nilNodes.clear();

    delete scene;
    scene = nullptr;
    root = nullptr;
    redBlackTree = RedBlackTree();
    On_UpdateNodeCount();
    On_UpdateHeight();

    ui->dataSetGroupBox->setEnabled(true);
    ui->operationsGroupBox->setEnabled(false);

    On_Clear();
}

void MainWindow::On_Import()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file for import",
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    "Text files (*.txt);;JSON files (*.json);;XML files (*.xml);;Binary files (*.bin *.dat)");

    if (!redBlackTree.ImportTree(fileName))
        return;

    QMessageBox::information(this, "Info", "File was successfully imported!");
    ConfigureLineEdits();
    ui->operationsGroupBox->setEnabled(ui->cbEnableRBTValidations->isChecked());
    ui->dataSetComboBox->setCurrentIndex((int)redBlackTree.GetDataType());
    ui->dataSetGroupBox->setEnabled(false);

    exportAction->setEnabled(true);

    ui->consoleWidget->clear();
    ui->cbShowBlackHeight->setCheckState(Qt::Unchecked);
    ui->cbShowNil->setCheckState(Qt::Unchecked);
    nilNodes.clear();

    On_Clear();

    Draw(redBlackTree.GetHeight());
}

void MainWindow::On_Export()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export tree to file",
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                    "Text files (*.txt);;JSON files (*.json);;XML files (*.xml);;Binary files (*.bin *.dat)");

    if (!redBlackTree.ExportTree(fileName))
        return;

    QMessageBox::information(this, "Info", "File was successfully exported!");
}

void MainWindow::On_Clear()
{
    ui->consoleWidget->clear();
    ui->currentStep->clear();
}

void MainWindow::On_Properties()
{
    ShowPropertiesDialog();
}

void MainWindow::On_UpdateHeight()
{
    ui->heightLabel->setText(QString::number(redBlackTree.GetHeight()));
}

void MainWindow::On_UpdateNodeCount()
{
    ui->nodesLabel->setText(QString::number(redBlackTree.GetNodeCount()));
}

void MainWindow::On_ErrorMessage(const QString &msg)
{
    QMessageBox::critical(this, "Error", msg);
}

void MainWindow::On_DataSetButtonClicked()
{
    redBlackTree.SetTreeDataType((DataType)ui->dataSetComboBox->currentIndex());
    ui->dataSetGroupBox->setEnabled(false);
    ui->operationsGroupBox->setEnabled(true);

    ConfigureLineEdits();
}

void MainWindow::On_InsertButtonClicked()
{
    auto key = ui->insertLineEdit->text();
    if (key.isEmpty())
        return;

    seqGroup->clear();

    ui->cbShowBlackHeight->setCheckState(Qt::Unchecked);
    ui->cbShowNil->setCheckState(Qt::Unchecked);
    nilNodes.clear();

    newNodeHeight = redBlackTree.GetNewNodeHeight(key);
    if (newNodeHeight > redBlackTree.GetHeight())
    {
        Draw(newNodeHeight);
        QTimer::singleShot(0, this, [this]() {
            QRectF itemsRect = scene->itemsBoundingRect();
            itemsRect.adjust(-scenePadding * 3.5, -scenePadding * 3.5, scenePadding * 3.5, scenePadding * 3.5);

            scene->setSceneRect(itemsRect);
            ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);
        });
    }
    redBlackTree.Insert(key);

    ui->playButton->setIcon(QIcon(":/resources/img/pause_button.svg"));
    seqGroup->start();
    ui->operationsGroupBox->setEnabled(false);
    ui->propertiesGroupBox->setEnabled(false);

    ui->insertLineEdit->clear();

    connect(seqGroup, &QSequentialAnimationGroup::finished, this, [this, key] {
        ui->consoleWidget->append(QString("Node <b>%1</b> has been added to the tree.").arg(key));
        exportAction->setEnabled(true);

        QTimer::singleShot(0, this, [this]() {
            QRectF itemsRect = scene->itemsBoundingRect();
            itemsRect.adjust(-scenePadding, -scenePadding, scenePadding, scenePadding);

            scene->setSceneRect(itemsRect);
            ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);
        });

        nilNodes.clear();
        Draw(redBlackTree.GetHeight());

        ui->operationsGroupBox->setEnabled(true);
        ui->propertiesGroupBox->setEnabled(true);

        seqGroup->disconnect();
    });
}

void MainWindow::On_DeleteButtonClicked()
{
    auto key = ui->deleteLineEdit->text();
    if (key.isEmpty())
        return;

    seqGroup->clear();

    ui->cbShowBlackHeight->setCheckState(Qt::Unchecked);
    ui->cbShowNil->setCheckState(Qt::Unchecked);
    nilNodes.clear();

    if (!redBlackTree.Delete(key))
        return;

    ui->playButton->setIcon(QIcon(":/resources/img/pause_button.svg"));
    seqGroup->start();
    ui->operationsGroupBox->setEnabled(false);
    ui->propertiesGroupBox->setEnabled(false);

    ui->deleteLineEdit->clear();

    connect(seqGroup, &QSequentialAnimationGroup::finished, this, [this, key] {
        ui->consoleWidget->append(QString("Node <b>%1</b> has been removed from the tree.").arg(key));
        exportAction->setEnabled(redBlackTree.GetNodeCount() != 0);

        QTimer::singleShot(0, this, [this]() {
            QRectF itemsRect = scene->itemsBoundingRect();
            itemsRect.adjust(-scenePadding, -scenePadding, scenePadding, scenePadding);

            scene->setSceneRect(itemsRect);
            ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);
        });

        nilNodes.clear();
        Draw(redBlackTree.GetHeight());

        ui->operationsGroupBox->setEnabled(true);
        ui->propertiesGroupBox->setEnabled(true);

        seqGroup->disconnect();
    });
}

void MainWindow::On_FindButtonClicked()
{
    auto key = ui->findLineEdit->text();
    if (key.isEmpty())
        return;

    seqGroup->clear();

    ui->cbShowBlackHeight->setCheckState(Qt::Unchecked);
    ui->cbShowNil->setCheckState(Qt::Unchecked);

    bool result = redBlackTree.Find(key);

    ui->playButton->setIcon(QIcon(":/resources/img/pause_button.svg"));
    seqGroup->start();
    ui->operationsGroupBox->setEnabled(false);
    ui->propertiesGroupBox->setEnabled(false);

    ui->findLineEdit->clear();
    connect(seqGroup, &QSequentialAnimationGroup::finished, this, [this, key, result] {
        if (result)
           ui->consoleWidget->append(QString("Node <b>%1</b> was found in the tree.").arg(key));
        else
            ui->consoleWidget->append(QString("Node <b>%1</b> was not found in the tree.").arg(key));

        ui->operationsGroupBox->setEnabled(true);
        ui->propertiesGroupBox->setEnabled(true);

        seqGroup->disconnect();
    });
}


void MainWindow::ConfigureLineEdits()
{
    QRegularExpression regExp;

    switch(redBlackTree.GetDataType())
    {
    case DataType::NUMBER:
        regExp.setPattern("^-?\\d{0,4}$");
        break;
    case DataType::TEXT:
        regExp.setPattern(".{0,4}");
        break;
    case DataType::CHAR:
        regExp.setPattern(".");
        break;
    }

    ui->insertLineEdit->setValidator(new QRegularExpressionValidator(regExp));
    ui->deleteLineEdit->setValidator(new QRegularExpressionValidator(regExp));
    ui->findLineEdit->setValidator(new QRegularExpressionValidator(regExp));
}

void MainWindow::On_NilCbStateChanged(int state)
{
    if (state == Qt::Checked)
    {
        for (auto& x : nilNodes)
            scene->addItem(x);
    }
    else
    {
        for (auto& x : nilNodes)
            scene->removeItem(x);
    }

    if (scene)
    {
        QTimer::singleShot(0, this, [this]() {
            QRectF itemsRect = scene->itemsBoundingRect();
            itemsRect.adjust(-scenePadding, -scenePadding, scenePadding, scenePadding);

            scene->setSceneRect(itemsRect);
            ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);
        });
    }
}

void MainWindow::On_BlackHeightStateChanged(int state)
{
    if (scene)
    {
        emit ShowBlackHeightSignal(state == Qt::Checked ? true : false);
        scene->update();
        QTimer::singleShot(0, this, [this]() {
            QRectF itemsRect = scene->itemsBoundingRect();
            itemsRect.adjust(-scenePadding, -scenePadding, scenePadding, scenePadding);

            scene->setSceneRect(itemsRect);
            ui->graphicsView->fitInView(itemsRect, Qt::KeepAspectRatio);
        });
    }
}

void MainWindow::On_EnableRBTValidationsChanged(int state)
{
    emit EnableRBTValidations(state == Qt::Checked ? true : false);
}

// todo change for other anims
// add new nodes to map
void MainWindow::On_BackwardButtonClicked()
{
    if (seqGroup->state() == QAbstractAnimation::Running && seqGroup->indexOfAnimation(seqGroup->currentAnimation()) > 0)
    {
        auto currentAnim = seqGroup->currentAnimation();
        if (currentAnim->inherits("QParallelAnimationGroup"))
        {
            int prevDur = currentAnim->duration() * (seqGroup->indexOfAnimation(currentAnim) - 1);
            QParallelAnimationGroup* parallelGroup = qobject_cast<QParallelAnimationGroup*>(currentAnim);
            if (parallelGroup)
            {
                for (int i = 0; i < parallelGroup->animationCount(); ++i)
                {
                    QPropertyAnimation* anim = qobject_cast<QPropertyAnimation*>(parallelGroup->animationAt(i));
                    if (anim)
                    {
                        if (anim->propertyName() == "PenColor" ||
                            anim->propertyName() == "PenWidth")
                            anim->setCurrentTime(anim->duration());
                        else
                        {
                            prevDur = 0;
                            for (int i = 0; i < seqGroup->indexOfAnimation(currentAnim) - 1; ++i)
                                prevDur += seqGroup->animationAt(i)->duration();

                            anim->setCurrentTime(0);
                        }

                    }
                }
            }
            seqGroup->stop();
            seqGroup->start();
            seqGroup->setCurrentTime(prevDur);
        }
        else
        {
            int prevDur = 0;
            for (int i = 0; i < seqGroup->indexOfAnimation(currentAnim) - 1; ++i)
                prevDur += seqGroup->animationAt(i)->duration();
            qDebug() << prevDur;
            seqGroup->currentAnimation()->setCurrentTime(0);
            seqGroup->stop();
            seqGroup->start();
            seqGroup->setCurrentTime(prevDur);
        }

    }
}

void MainWindow::On_PlayButtonClicked()
{
    if (!seqGroup->currentAnimation())
        return;

    if (seqGroup->state() == QAbstractAnimation::Running)
    {
        ui->playButton->setIcon(QIcon(":/resources/img/play_button.svg"));
        seqGroup->pause();
    }
    else if (seqGroup->state() == QAbstractAnimation::Paused)
    {
        ui->playButton->setIcon(QIcon(":/resources/img/pause_button.svg"));
        seqGroup->resume();
    }
}

void MainWindow::On_ForwardButtonClicked()
{
    if (seqGroup->state() == QAbstractAnimation::Running && seqGroup->indexOfAnimation(seqGroup->currentAnimation()) < seqGroup->animationCount() - 1)
    {
        auto currentAnim = seqGroup->currentAnimation();
        if (currentAnim->inherits("QParallelAnimationGroup"))
        {
            int nextDur = currentAnim->duration() * (seqGroup->indexOfAnimation(currentAnim) + 1);
            QParallelAnimationGroup* parallelGroup = qobject_cast<QParallelAnimationGroup*>(currentAnim);
            if (parallelGroup)
            {
                for (int i = 0; i < parallelGroup->animationCount(); ++i)
                {
                    QPropertyAnimation* anim = qobject_cast<QPropertyAnimation*>(parallelGroup->animationAt(i));
                    if (anim)
                    {
                        if (anim->propertyName() == "PenColor" ||
                            anim->propertyName() == "PenWidth")
                            anim->setCurrentTime(anim->duration());
                        else
                        {
                            nextDur = 0;
                            for (int i = 0; i < seqGroup->indexOfAnimation(currentAnim) + 1; ++i)
                                nextDur += seqGroup->animationAt(i)->duration();

                            anim->setCurrentTime(anim->duration());
                        }
                    }
                }
            }
            seqGroup->stop();
            seqGroup->start();
            seqGroup->setCurrentTime(nextDur);
        }
        else
        {
            int nextDur = 0;
            for (int i = 0; i < seqGroup->indexOfAnimation(currentAnim) + 1; ++i)
                nextDur += seqGroup->animationAt(i)->duration();
            seqGroup->currentAnimation()->setCurrentTime(seqGroup->currentAnimation()->duration());
            seqGroup->stop();
            seqGroup->start();
            seqGroup->setCurrentTime(nextDur);
        }

    }
}

void MainWindow::On_HighlightNode(std::shared_ptr<Node> node, QColor color, bool isLeft, const QString& key)
{
    if (node == NIL)
        return;

    int duration = 2000;
    TreeNode* treeNode = nodeMap[node];

    QPropertyAnimation* colorAnimation = new QPropertyAnimation(treeNode, "PenColor");
    colorAnimation->setDuration(duration);
    colorAnimation->setStartValue(color);
    colorAnimation->setEndValue(QColor(Qt::black));
    colorAnimation->setEasingCurve(QEasingCurve::InOutElastic);

    if (!key.isEmpty())
    {
        if (key == "min")
        {
            colorAnimation->setProperty("desc", QString("Finding minimum in <b>%1</b>'s subtree.").arg(node->GetDataString()));
        }
        else
        {
            if (isLeft)
                colorAnimation->setProperty("desc", QString("<b>%1</b> \u003E <b>%2</b> (look to left subtree).")
                                                        .arg(node->GetDataString(), key));
            else
                colorAnimation->setProperty("desc", QString("<b>%1</b> \u2264 <b>%2</b> (look to right subtree).")
                                                        .arg(node->GetDataString(), key));
        }

        connect(colorAnimation, &QPropertyAnimation::stateChanged, this, [this, colorAnimation] {
            if (colorAnimation->state() == QPropertyAnimation::Running)
                ui->currentStep->setText(colorAnimation->property("desc").toString());
        });
    }

    QPropertyAnimation* widthAnimation = new QPropertyAnimation(treeNode, "PenWidth");
    widthAnimation->setDuration(duration);
    widthAnimation->setStartValue(6);
    widthAnimation->setEndValue(2);
    widthAnimation->setEasingCurve(QEasingCurve::InOutElastic);

    QParallelAnimationGroup* group = new QParallelAnimationGroup;
    group->addAnimation(colorAnimation);
    group->addAnimation(widthAnimation);

    seqGroup->addAnimation(group);
}

void MainWindow::On_ChangeColor(std::shared_ptr<Node> node, Color color)
{
    int duration = 2000;
    //TreeNode* treeNode = nodeMap[node];

    if (!nodeMap[node])
        return;

    QPropertyAnimation* colorAnimation = new QPropertyAnimation(nodeMap[node], "BrushColor");
    colorAnimation->setDuration(duration);
    colorAnimation->setStartValue(nodeMap[node]->color == Color::RED ? QBrush(Qt::red) : QBrush(Qt::black));
    colorAnimation->setEndValue(color == Color::RED ? QBrush(Qt::red) : QBrush(Qt::black));
    colorAnimation->setEasingCurve(QEasingCurve::InOutElastic);
    colorAnimation->setProperty("desc", QString("Change node <b>%1</b> to <i><b>%2</b></i>.")
                                            .arg(node->GetDataString(), color == Color::RED ? "<font color='#ff0000'>Red</font>" : "Black"));

    connect(colorAnimation, &QPropertyAnimation::stateChanged, this, [this, colorAnimation] {
        if (colorAnimation->state() == QPropertyAnimation::Running)
            ui->currentStep->setText(colorAnimation->property("desc").toString());
    });
    nodeMap[node]->color = color;
    seqGroup->addAnimation(colorAnimation);
}

void MainWindow::On_CreateNode(std::shared_ptr<Node> node)
{
    TreeNode* newNode = new TreeNode(node->GetDataString(), 1, Color::RED);
    connect(this, SIGNAL(ShowBlackHeightSignal(bool)), newNode, SLOT(On_ShowBlackHeight(bool)));
    newNode->SetPos(-50, 0);
    nodeMap[node] = newNode;

    scene->addItem(newNode);
    scene->update();
}

void MainWindow::On_MoveNode(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isRoot)
{
    int duration = 5000;

    TreeNode* child = nodeMap[node];

    QPropertyAnimation* moveAnimation = new QPropertyAnimation(child, "Position");

    connect(moveAnimation, &QPropertyAnimation::stateChanged, this, [this, moveAnimation, to, leftChild](QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running)
        {
            ui->currentStep->setText(moveAnimation->property("desc").toString());
            if (nodeMap[to])
            {
                if (leftChild)
                    nodeMap[to]->ClearLeftLine();
                else
                    nodeMap[to]->ClearRightLine();
            }
        }
    });

    if (!isRoot)
    {
        moveAnimation->setDuration(duration);
        moveAnimation->setStartValue(child->Position());

        TreeNode* parent = nodeMap[to];
        nodeMap[node]->parent = parent;

        qreal padding = paddingX / 2;
        if (leftChild)
        {
            moveAnimation->setProperty("desc", QString("Make node <b>%1</b> the left child of <b>%2</b>.")
                                           .arg(node->GetDataString(), to->GetDataString()));
            moveAnimation->setEndValue(QPointF(parent->GetX() - padding, parent->GetY() + paddingY));
            nodeMap[node]->position = QPointF(parent->GetX() - padding, parent->GetY() + paddingY);
            nodeMap[to]->left = nodeMap[node];
            connect(moveAnimation, &QPropertyAnimation::finished, this, [this, child, parent, node, padding] {
                parent->SetLeftLine(QPointF(child->rect().x() - padding, child->rect().top()));
            });
        }
        else
        {
            moveAnimation->setProperty("desc", QString("Make node <b>%1</b> the right child of <b>%2</b>.")
                                           .arg(node->GetDataString(), to->GetDataString()));
            moveAnimation->setEndValue(QPointF(parent->GetX() + padding, parent->GetY() + paddingY));
            nodeMap[node]->position = QPointF(parent->GetX() + padding, parent->GetY() + paddingY);
            nodeMap[to]->right = nodeMap[node];
            connect(moveAnimation, &QPropertyAnimation::finished, this, [this, child, parent, node, padding] {
                parent->SetRightLine(QPointF(child->rect().x() + padding, child->rect().top()));
            });

        }
    }
    else
    {
        moveAnimation->setDuration(duration);
        moveAnimation->setStartValue(QPointF(0, 0));
        moveAnimation->setProperty("desc", QString("Make node <b>%1</b> the root of the tree.")
                                       .arg(node->GetDataString()));
        moveAnimation->setEndValue(QPointF(0, 0));
        nodeMap[node]->position = QPointF(0, 0);
    }


    seqGroup->addAnimation(moveAnimation);
}

void MainWindow::On_MoveY(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isLeftRotate, bool isRoot)
{
    int duration = 5000;

    TreeNode* y = nodeMap[node];
    TreeNode* xp = nodeMap[to];
    TreeNode* o = nullptr;

    if (isLeftRotate)
        o = nodeMap[node]->right;
    else
        o = nodeMap[node]->left;

    QPropertyAnimation* moveAnimation = new QPropertyAnimation(y, "Position");
    QPropertyAnimation* moveToYPosAnim = new QPropertyAnimation(o, "Position");

    connect(moveAnimation, &QPropertyAnimation::stateChanged, this, [this, node](QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running)
            nodeMap[node]->ClearLines();
    });

    connect(moveToYPosAnim, &QPropertyAnimation::stateChanged, this, [this, o](QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running && o)
            o->ClearLines();
    });

    moveAnimation->setDuration(duration);
    moveAnimation->setStartValue(nodeMap[node]->Position());

    if (o)
    {
        moveToYPosAnim->setDuration(duration);
        moveToYPosAnim->setStartValue(o->Position());
        moveToYPosAnim->setEndValue(nodeMap[node]->Position());
    }

    if (!isRoot)
    {
        if (leftChild)
        {
            if (nodeMap[to]->left == nullptr)
            {
                moveAnimation->setEndValue(QPointF(nodeMap[to]->Position().x() - (nodeMap[to]->diameter), nodeMap[to]->Position().y() + paddingY));
                nodeMap[node]->position = QPointF(nodeMap[to]->Position().x() - (nodeMap[to]->diameter), nodeMap[to]->Position().y() + paddingY);
            }
            else
            {
                moveAnimation->setEndValue(nodeMap[to]->left->Position());
                nodeMap[node]->position = nodeMap[to]->left->Position();
            }
            nodeMap[to]->left = nodeMap[node];
        }
        else
        {
            if (nodeMap[to]->right == nullptr)
            {
                moveAnimation->setEndValue(QPointF(nodeMap[to]->Position().x() + (nodeMap[to]->diameter), nodeMap[to]->Position().y() + paddingY));
                nodeMap[node]->position = QPointF(nodeMap[to]->Position().x() + (nodeMap[to]->diameter), nodeMap[to]->Position().y() + paddingY);
            }
            else
            {
                moveAnimation->setEndValue(nodeMap[to]->right->Position());
                nodeMap[node]->position = nodeMap[to]->right->Position();
            }

            nodeMap[to]->right = nodeMap[node];
        }
    }
    else
    {
        moveAnimation->setEndValue(QPointF(0, 0));
        nodeMap[node]->position = QPointF(0, 0);
        root = nodeMap[node];
    }

    if (isLeftRotate)
    {
        leftRotateAnims.append(moveAnimation);
        if (o)
            leftRotateAnims.append(moveToYPosAnim);
    }
    else
    {
        rightRotateAnims.append(moveAnimation);
        if (o)
            rightRotateAnims.append(moveToYPosAnim);
    }
}

void MainWindow::On_MoveX(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild)
{
    int duration = 5000;

    TreeNode* xNode = nodeMap[node];
    //TreeNode* yNode = nodeMap[to];

    QPropertyAnimation* moveAnimation = new QPropertyAnimation(nodeMap[node], "Position");
    connect(moveAnimation, &QPropertyAnimation::stateChanged, this, [this, moveAnimation, node](QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running)
            nodeMap[node]->ClearLines();
    });

    moveAnimation->setDuration(duration);
    moveAnimation->setStartValue(nodeMap[node]->Position());

    if (leftChild)
    {
        if (nodeMap[node]->left == nullptr)
        {
            moveAnimation->setEndValue(QPointF(nodeMap[node]->Position().x() - (nodeMap[node]->diameter), nodeMap[node]->Position().y() + paddingY));
            nodeMap[node]->position = QPointF(nodeMap[node]->Position().x() - (nodeMap[node]->diameter), nodeMap[node]->Position().y() + paddingY);
        }
        else
        {
            moveAnimation->setEndValue(nodeMap[node]->left->Position());
            QPropertyAnimation* moveLeftChildAnimation = new QPropertyAnimation(nodeMap[node]->left, "Position");
            moveLeftChildAnimation->setEndValue(QPointF(nodeMap[node]->left->Position().x() - (nodeMap[node]->diameter), nodeMap[node]->left->Position().y() + paddingY));

            nodeMap[node]->left->position = QPointF(nodeMap[node]->left->Position().x() - (nodeMap[node]->diameter), nodeMap[node]->left->Position().y() + paddingY);
            nodeMap[node]->position = nodeMap[node]->left->Position();
            leftRotateAnims.append(moveLeftChildAnimation);
        }
        nodeMap[to]->left = nodeMap[node];

        leftRotateAnims.append(moveAnimation);
    }
    else
    {
        if (nodeMap[node]->right == nullptr)
        {
            moveAnimation->setEndValue(QPointF(nodeMap[node]->Position().x() + (nodeMap[node]->diameter), nodeMap[node]->Position().y() + paddingY));
            nodeMap[node]->position = QPointF(nodeMap[node]->Position().x() + (nodeMap[node]->diameter), nodeMap[node]->Position().y() + paddingY);
        }
        else
        {
            moveAnimation->setEndValue(nodeMap[node]->right->Position());
            QPropertyAnimation* moveRightChildAnimation = new QPropertyAnimation(nodeMap[node]->right, "Position");
            moveRightChildAnimation->setEndValue(QPointF(nodeMap[node]->right->Position().x() + (nodeMap[node]->diameter), nodeMap[node]->right->Position().y() + paddingY));

            nodeMap[node]->right->position = QPointF(nodeMap[node]->right->Position().x() + (nodeMap[node]->diameter), nodeMap[node]->right->Position().y() + paddingY);
            nodeMap[node]->position = nodeMap[node]->right->Position();
            rightRotateAnims.append(moveRightChildAnimation);
        }

        nodeMap[to]->right = nodeMap[node];

        rightRotateAnims.append(moveAnimation);
    }

}

void MainWindow::On_MoveStart(std::shared_ptr<Node> x, std::shared_ptr<Node> y, bool x_leftChild, bool y_leftChild)
{
    int duration = 5000;

    if ((y_leftChild && nodeMap[y]->left == nullptr) || (!y_leftChild && nodeMap[y]->right == nullptr))
        return;

    QPropertyAnimation* moveAnimation = nullptr;

    if (!x_leftChild && y_leftChild)
    {
        moveAnimation = new QPropertyAnimation(nodeMap[y]->left, "Position");
        moveAnimation->setDuration(duration);
        moveAnimation->setStartValue(nodeMap[y]->left->Position());

        if (nodeMap[x]->left == nullptr)
        {
            moveAnimation->setEndValue(QPointF(nodeMap[x]->Position().x() - (nodeMap[x]->diameter) / 2, nodeMap[y]->left->Position().y()));
            nodeMap[y]->left->position = QPointF(nodeMap[x]->Position().x() - (nodeMap[x]->diameter) / 2, nodeMap[y]->left->Position().y());
        }
        else
        {
            moveAnimation->setEndValue(QPointF(nodeMap[x]->left->Position().x() + (nodeMap[x]->diameter), nodeMap[x]->left->Position().y() + paddingY));
            nodeMap[y]->left->position = QPointF(nodeMap[x]->left->Position().x() + (nodeMap[x]->diameter), nodeMap[x]->left->Position().y() + paddingY);
        }

        nodeMap[x]->right = nodeMap[y]->left;

        leftRotateAnims.append(moveAnimation);
    }
    else if (x_leftChild && !y_leftChild)
    {
        moveAnimation = new QPropertyAnimation(nodeMap[y]->right, "Position");
        moveAnimation->setDuration(duration);
        moveAnimation->setStartValue(nodeMap[y]->right->Position());


        if (nodeMap[x]->right == nullptr)
        {
            moveAnimation->setEndValue(QPointF(nodeMap[x]->Position().x() + (nodeMap[x]->diameter) / 2, nodeMap[y]->right->Position().y()));
            nodeMap[y]->right->position = QPointF(nodeMap[x]->Position().x() + (nodeMap[x]->diameter) / 2, nodeMap[y]->right->Position().y());
        }
        else
        {
            moveAnimation->setEndValue(QPointF(nodeMap[x]->right->Position().x() - (nodeMap[x]->diameter), nodeMap[x]->right->Position().y() + paddingY));
            nodeMap[y]->right->position = QPointF(nodeMap[x]->right->Position().x() - (nodeMap[x]->diameter), nodeMap[x]->right->Position().y() + paddingY);
        }

        nodeMap[x]->left = nodeMap[y]->right;

        rightRotateAnims.append(moveAnimation);
    }

    connect(moveAnimation, &QPropertyAnimation::stateChanged, this, [this, moveAnimation, y](QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running)
            nodeMap[y]->ClearLines();
    });

}

void MainWindow::On_ChangeParent(std::shared_ptr<Node> x, std::shared_ptr<Node> y)
{
    if (nodeMap[x])
        nodeMap[x]->parent = nodeMap[y];
}

void MainWindow::On_LeftRotate(std::shared_ptr<Node> x)
{
    QParallelAnimationGroup* group = new QParallelAnimationGroup;
    connect(group, &QParallelAnimationGroup::stateChanged, this, [this, group] {
        if (group->state() == QPropertyAnimation::Running)
            ui->currentStep->setText(group->property("desc").toString());
    });
    group->setProperty("desc", QString("Left rotation on node <b>%1</b>.")
                                           .arg(x->GetDataString()));

    for (auto& x : leftRotateAnims)
        group->addAnimation(x);
    leftRotateAnims.clear();

    seqGroup->addAnimation(group);
}

void MainWindow::On_RightRotate(std::shared_ptr<Node> x)
{
    QParallelAnimationGroup* group = new QParallelAnimationGroup;
    connect(group, &QParallelAnimationGroup::stateChanged, this, [this, group] {
        if (group->state() == QPropertyAnimation::Running)
            ui->currentStep->setText(group->property("desc").toString());
    });
    group->setProperty("desc", QString("Right rotation on node <b>%1</b>.")
                                   .arg(x->GetDataString()));

    for (auto& x : rightRotateAnims)
        group->addAnimation(x);
    rightRotateAnims.clear();

    seqGroup->addAnimation(group);
}

void MainWindow::On_Transplant(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild, bool isRoot)
{
    int duration = 5000;

    TreeNode* y = nodeMap[node];
    TreeNode* xp = nodeMap[to];
    TreeNode* o = nullptr;

    if (!y)
    {
        if (leftChild)
            nodeMap[to]->left = nullptr;
        else
            nodeMap[to]->right = nullptr;
        return;
    }


    QPropertyAnimation* moveAnimation = new QPropertyAnimation(y, "Position");
    //QPropertyAnimation* moveToYPosAnim = new QPropertyAnimation(o, "Position");

    connect(moveAnimation, &QPropertyAnimation::stateChanged, this, [this, node](QAbstractAnimation::State newState, QAbstractAnimation::State oldState) {
        if (newState == QAbstractAnimation::Running && oldState != QAbstractAnimation::Running)
            nodeMap[node]->ClearLines();
    });

    moveAnimation->setDuration(duration);
    moveAnimation->setStartValue(nodeMap[node]->Position());


    if (!isRoot)
    {
        if (leftChild)
        {
            moveAnimation->setEndValue(nodeMap[to]->left->Position());
            nodeMap[node]->position = nodeMap[to]->left->Position();
            nodeMap[to]->left = nodeMap[node];
        }
        else
        {
            moveAnimation->setEndValue(nodeMap[to]->right->Position());
            nodeMap[node]->position = nodeMap[to]->right->Position();
            nodeMap[to]->right = nodeMap[node];
        }
    }
    else
    {
        moveAnimation->setEndValue(QPointF(0, 0));
        nodeMap[node]->position = QPointF(0, 0);
        root = nodeMap[node];
    }

    transplantAnims.append(moveAnimation);
}

void MainWindow::On_Delete(std::shared_ptr<Node> node)
{
    QParallelAnimationGroup* group = new QParallelAnimationGroup;

    int duration = 5000;

    TreeNode* treeNode = nodeMap[node];
    nodeMap[node] = nullptr;

    QPropertyAnimation* animation = new QPropertyAnimation(treeNode, "Opacity");
    animation->setDuration(duration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    animation->setProperty("desc", QString("Delete node <b>%1</b> from tree.")
                                            .arg(node->GetDataString()));

    connect(animation, &QPropertyAnimation::stateChanged, this, [this, animation] {
        if (animation->state() == QPropertyAnimation::Running)
            ui->currentStep->setText(animation->property("desc").toString());
    });

    group->addAnimation(animation);

    for (auto& x : transplantAnims)
        group->addAnimation(x);
    transplantAnims.clear();

    seqGroup->addAnimation(group);
}

void MainWindow::On_ChangeSiblingSignal(std::shared_ptr<Node> node, std::shared_ptr<Node> to, bool leftChild)
{
    if (nodeMap[node])
    {
        if (leftChild)
            nodeMap[node]->left = nodeMap[to];
        else
            nodeMap[node]->right = nodeMap[to];
    }
}

void MainWindow::ShowPropertiesDialog() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Red-black tree properties");

    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);
    textEdit->setText(tr("A red-black tree is a binary search tree that satisfies the following red-black properties:\n"
                         "1. Every node is either red or black.\n"
                         "2. The root is black.\n"
                         "3. Every leaf (NIL) is black.\n"
                         "4. If a node is red, then both its children are black.\n"
                         "5. For each node, all simple paths from the node to descendant leaves contain the same number of black nodes."));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(textEdit);

    QPushButton *closeButton = new QPushButton("Close", dialog);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    dialog->setLayout(layout);
    dialog->resize(400, 300);
    dialog->exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Space:
        ui->playButton->click();
        break;
    case Qt::Key_Right:
        ui->forwardButton->click();
        break;
    case Qt::Key_Left:
        ui->backwardButton->click();
        break;
    default:
        break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
