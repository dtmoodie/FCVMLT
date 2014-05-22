#include "imgsourceswidget.h"
#include "ui_imgsourceswidget.h"

imgSourcesWidget::imgSourcesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imgSourcesWidget)
{
    QGridLayout* layout = new QGridLayout(this);
    QLabel* lblSourceList = new QLabel(this);
    lblSourceList->setText("Sources");
    sourceList = new QTreeWidget(this);
    sourceList->setMinimumHeight(100);
    sourceList->setMinimumWidth(100);
    sourceList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sourceList->setColumnCount(2);
    sourceList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(lblSourceList,0,0);
    layout->addWidget(sourceList,1,0);
    setLayout(layout);
    setWindowTitle("Sources");

    connect(sourceList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(handleItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(sourceList, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(handleItemActivated(QTreeWidgetItem*,int)));
    sourceList->installEventFilter(this);
	_sourceAction = new QAction("Save image", this);
	connect(_sourceAction, SIGNAL(triggered()), this, SLOT(handleSaveAction()));
	sourceList->addAction(_sourceAction);
	sourceList->setContextMenuPolicy(Qt::ActionsContextMenu);
	

}

imgSourcesWidget::~imgSourcesWidget()
{
    delete ui;
}


void imgSourcesWidget::handleItemChange(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(previous != NULL)
    {
        container* prev = reinterpret_cast<container*>(previous);
        if(!prev->img.empty() && prev->isTop)
        {
            prev->img.release();
        }
    }
    container* cur = dynamic_cast<container*>(current);
    if(cur == NULL) return;
    if(cur->img.empty())
    {
        try
        {
            if(cur->filePath.size() > 0)
                cur->img = cv::imread(cur->filePath.toStdString());
        }catch(cv::Exception &e)
        {
            std::cout << e.what();
            return;
        }
    }
    if(cur->img.empty()) return;
    if(cur->isImg)
        emit sourcePreview(cur->img);
    emit sourcePreview(cur);
}

void imgSourcesWidget::handleItemActivated(QTreeWidgetItem *item, int col)
{
    if(item == NULL) return;
    curCont = dynamic_cast<container*>(item);
    try
    {
        if(curCont->img.empty() && curCont->filePath.size() > 0) curCont->img = cv::imread(curCont->filePath.toStdString());
    }catch(cv::Exception &e)
    {
        std::cout << e.what();
        return;
    }
    if(curCont->img.empty()) return;
    if(curCont->isImg)
    {
        emit sourcePreview(curCont->img);
        emit sourceChange(curCont->img);
    }
    emit sourceChange(curCont);
}

void imgSourcesWidget::handleSaveImage(cv::Mat img, QString name)
{
    container* child = new container(sourceList);
    child->isTop = false;
    img.copyTo(child->img);
    child->setText(0,name);
    sourceList->currentItem()->addChild(child);
}

void imgSourcesWidget::handleSaveAction()
{
	QTreeWidgetItem* orig = sourceList->currentItem();
	container* item = dynamic_cast<container*>(orig);
	if (item->isTop) return;
	if (item->img.empty()) return;
	QString dir = dynamic_cast<container*>(orig->parent())->dirName;
	QString baseName = dynamic_cast<container*>(orig->parent())->baseName;
	cv::imwrite((dir + "/" + baseName + "_" + item->text(0) + ".jpg").toStdString(), item->img);
	item->savedToDisk = true;
	item->parentFilter->savedToDisk = true;

}

void imgSourcesWidget::handleSaveToDisk(container* item)
{
	if (item->isTop) return;
	if (item->img.empty()) return;
	container* parent = dynamic_cast<container*>(dynamic_cast<QTreeWidgetItem*>(item)->parent());
	parent->parentFilter->savedToDisk = true;
	QString dir = parent->dirName;
	QString baseName = parent->baseName;
	cv::imwrite((dir + "/" + baseName + "_" + item->text(0) + ".jpg").toStdString(), item->img);
	item->savedToDisk = true;
	item->parentFilter->savedToDisk = true;
}

void imgSourcesWidget::handleSaveFeatures(cv::Mat features, QString name)
{
    container* child = new container(sourceList);
    child->isTop = false;
    child->isFeatures = true;
    child->isImg = false;
    features.copyTo(child->img);
    child->setText(0,name);
    container* cols = new container(sourceList);
    cols->setText(0,"Columns");
    cols->setText(1,QString::number(features.cols));
    child->addChild(cols);
    container* rows = new container(sourceList);
    rows->setText(0,"Rows");
    rows->setText(1, QString::number(features.rows));
    child->addChild(rows);

    sourceList->currentItem()->addChild(child);
}

bool imgSourcesWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj != (QObject*)sourceList) return false;
    if(ev->type() != QEvent::KeyPress) return false;
    QKeyEvent* event = reinterpret_cast<QKeyEvent*>(ev);
    if(event->key() == Qt::Key_Delete)
    {
        QList<QTreeWidgetItem*> items = sourceList->selectedItems();
        for(int i = 0; i < items.size(); ++i)
        {
            delete items[i];
        }
    }
    if(event->key() == Qt::Key_Up)
    {
        sourceList->setCurrentItem(sourceList->itemAbove(sourceList->currentItem()));
    }
    if(event->key() == Qt::Key_Down)
    {
        sourceList->setCurrentItem(sourceList->itemBelow(sourceList->currentItem()));
    }
    if(event->key() == Qt::Key_Right)
    {
        sourceList->expandItem(sourceList->currentItem());
    }

}

void imgSourcesWidget::cameraUpdated()
{
    if(sourceList->selectedItems().size() > 0)
    {
        if(sender() == curCont)
        {
#ifdef KEITH_BUILD
			container* cur = curCont;
#else
            container* cur = dynamic_cast<container*>(sourceList->currentItem());
#endif
			if (cur == NULL) return;
            if(!cur->img.empty())
            {
                emit sourcePreview(cur);
                emit sourcePreview(cur->img);
                emit sourceChange(cur);
                emit sourceChange(cur->img);
            }
        }
    }
}
