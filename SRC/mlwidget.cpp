#include "mlwidget.h"
#include "ui_mlwidget.h"

mlWidget::mlWidget(QWidget *parent, QTreeWidget* tree_) :
    QWidget(parent),
    selectionTree(tree_),
    selectionDetailsTree(new QTreeWidget),
    layout(new QGridLayout(this)),
    ui(new Ui::mlWidget)
{
    layout->addWidget(selectionDetailsTree);
    selectionDetailsTree->setColumnCount(3);
    selectionDetailsTree->setColumnWidth(0, 100);
    selectionDetailsTree->setColumnWidth(1, 100);
    selectionDetailsTree->setColumnWidth(2, 100);
    setLayout(layout);
}

mlWidget::~mlWidget()
{
    delete ui;
}

void mlWidget::handleSelectionChange()
{

}
