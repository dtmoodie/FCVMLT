#include "mldialog.h"
#include "ui_mldialog.h"

mlDialog::mlDialog(QWidget *parent, QTreeWidget *tree_) :
    QDialog(parent),
    selectedTree(tree_),
    ui(new Ui::mlDialog)
{
    informationTree = new QTreeWidget(this);

}

mlDialog::~mlDialog()
{
    delete ui;
}

void mlDialog::handleSourceSelectionChange()
{
    QList<QTreeWidgetItem*> items = selectedTree->selectedItems();
    for(unsigned int i = 0; i < items.size(); ++i)
    {

    }
}
