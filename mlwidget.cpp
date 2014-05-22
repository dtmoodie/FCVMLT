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
    for(int i = 0; i < curItems.size(); ++i)
    {
        delete curItems[i];
    }
    curItems.clear();
    QList<QTreeWidgetItem*> items = selectionTree->selectedItems();
    for(int i = 0; i < items.size(); ++i)
    {
        container* cont = dynamic_cast<container*>(items[i]);
        if(cont == NULL) continue;
        if(!cont->isFeatures) continue;

        container* newCont = new container(selectionDetailsTree);
        newCont->setText(0,cont->text(0));
        curItems.append(newCont);

        container* rows = new container();
        rows->setText(0,"Rows: ");
        rows->setText(1, QString::number(cont->img.rows));
        newCont->addChild(rows);

        container* cols = new container();
        cols->setText(0,"Cols: ");
        cols->setText(1, QString::number(cont->img.cols));
        newCont->addChild(cols);

        container* type = new container();
        type->setText(0,"Type:");
        type->setText(1, QString::number(cont->img.type()));
        newCont->addChild(type);

        double minVal;
        double maxVal;
        int* minLoc = new int [cont->img.channels()];
        int* maxLoc = new int [cont->img.channels()];
        std::cout << cont->img.channels() << std::endl;
        if(cont->img.channels() == 1)
        {
            try
            {
                cv::minMaxIdx(cont->img, &minVal, &maxVal, minLoc, maxLoc, cv::Mat());
            }catch(cv::Exception &e)
            {
                std::cout << e.what();
            }
        }

        container* min = new container();
        min->setText(0, "Min val / location:");
        min->setText(1, QString::number(minVal));
        QString location;
        for(int i = 0; i < cont->img.dims; ++i)
        {
            location += QString::number(minLoc[i]) + " ";
        }
        min->setText(2, location);

        newCont->addChild(min);

        container* max = new container();
        max->setText(0, "Max val / location: ");
        max->setText(1, QString::number(maxVal));
        location = "";
        for(int i = 0; i < cont->img.dims; ++i)
        {
            location += QString::number(maxLoc[i]) + " ";
        }
        max->setText(2, location);
        newCont->addChild(max);
    }
}
