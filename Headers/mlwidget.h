#ifndef MLWIDGET_H
#define MLWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QGridLayout>


#include <FCVMLT.h>
//#include <container.h>
//#include <imgFilter.h>

namespace Ui {
class mlWidget;
}

class mlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mlWidget(QWidget *parent = 0, QTreeWidget* tree_ = 0);
    ~mlWidget();


public slots:

    void handleSelectionChange();

private:
    Ui::mlWidget *ui;
	// Pointer to the source list tree
    QTreeWidget* selectionTree;
    QTreeWidget* selectionDetailsTree;

    QGridLayout* layout;
    QList<container*> curItems;

};

#endif // MLWIDGET_H
