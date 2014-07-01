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
void 
imgSourcesWidget::handleItemChange(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(previous != NULL)
    {
		container* prev = dynamic_cast<container*>(previous);
		switch (prev->type)
		{
			case container::Img:
				imgContainer* tmp = dynamic_cast<imgContainer*>(prev);
				//if (!tmp->M.empty() && tmp->isTop)
					//tmp->M.release();
				break;
		}
    }
    container* cur = dynamic_cast<container*>(current);
	curCont = cur;
    if(cur == NULL) return;
	switch (cur->type)
	{
	case container::Img:
	{
		imgContainer* tmp = dynamic_cast<imgContainer*>(cur);
		emit sourcePreview(tmp->M());
		break;
	}
	case container::Label:
	{
		labelContainer* tmp = dynamic_cast<labelContainer*>(cur);
		emit sourcePreview(tmp->M());
		break;
	}
	}
    emit sourcePreview(cur);
}
void 
imgSourcesWidget::handleItemActivated(QTreeWidgetItem *item, int col)
{
    if(item == NULL) return;
    curCont = dynamic_cast<container*>(item);
	switch (curCont->type)
	{
		case(container::Img) :
		{
			imgContainer* tmp = dynamic_cast<imgContainer*>(curCont);
			try
			{
				//if (tmp->M().empty() && tmp->filePath.size() > 0)
				//	tmp->M() = cv::imread(tmp->filePath.toStdString());
				if (tmp->M().empty()) return;
				emit sourcePreview(tmp->M());
				emit sourceChange(tmp->M());
			}
			catch (cv::Exception &e)
			{
				std::cout << e.what();
				return;
			}
			break;
		}
		case(container::Matrix):
		{
		}
	}
    emit sourceChange(curCont);
}
void 
imgSourcesWidget::handleSaveImage(cv::Mat img, QString name)
{
	imgContainer* child = new imgContainer(sourceList);
    child->isTop = false;
    img.copyTo(child->M());
    child->setText(0,name);
    sourceList->currentItem()->addChild(child);
}
void 
imgSourcesWidget::handleSaveAction()
{
	QTreeWidgetItem* orig = sourceList->currentItem();
	container* cont = dynamic_cast<container*>(orig);
	// No need to save a source image loaded from disk
	if (cont->isTop) return; 
	switch (cont->type)
	{
	case container::Img:
		imgContainer* tmp = dynamic_cast<imgContainer*>(cont);
		if (tmp->M().empty()) return;
		tmp->save();
		break;
	}
}
void 
imgSourcesWidget::handleSaveToDisk(container* item)
{

}
void 
imgSourcesWidget::handleSaveFeatures(cv::Mat features, QString name)
{
	featureContainer* child = new featureContainer(sourceList);
    child->isTop = false;
    features.copyTo(child->M());
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
bool 
imgSourcesWidget::eventFilter(QObject *obj, QEvent *ev)
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
void 
imgSourcesWidget::cameraUpdated()
{
    if(sourceList->selectedItems().size() > 0)
    {
		QObject* src = sender();
        if(sender() == (QObject*)curCont)
        {
			#ifdef KEITH_BUILD
			imgContainer* cur = curCont;
			#else
			imgContainer* cur = dynamic_cast<imgContainer*>(sourceList->currentItem());
			#endif
			if (cur == NULL) return;
            if(!cur->M().empty())
            {
                emit sourcePreview(cur);
                emit sourcePreview(cur->M());
                emit sourceChange(cur);
                emit sourceChange(cur->M());
            }
        }
    }
}
void
imgSourcesWidget::cameraUpdated(cv::Mat img)
{
	emit sourcePreview(img);
	emit sourceChange(img);
}