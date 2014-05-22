#include "filterwidget.h"
#include "ui_filterwidget.h"

filterWidget::filterWidget(QWidget *parent, cv::Mat curImg_) :
    QWidget(parent),
    filterSettings(NULL),
    currentFilter(NULL),
    currentFilterIdx(-1),
    currentSource(NULL),
    ui(new Ui::filterWidget)
{

    cropping = false;
    curImg = curImg_;
    layout = new QGridLayout(this);
    QLabel* lblFilterHist = new QLabel(this);
    lblFilterHist->setText("Filter History");
    lblFilterHist->setMaximumHeight(20);
    filterHistory = new QTreeWidget(this);
    filterHistory->setMinimumHeight(100);
    filterHistory->setMinimumWidth(50);
    filterHistory->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    connect(filterHistory, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(handleFilterHistSelect(QTreeWidgetItem*,int)));
    connect(filterHistory, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(handleFilterHistActivate(QTreeWidgetItem*,int)));
    filterHistory->installEventFilter(this);
    layout->addWidget(lblFilterHist,0,0);
    layout->addWidget(filterHistory,1,0,2,1);
    filterHistory->setColumnCount(2);

    QPushButton* deleteFilter = new QPushButton(this);
    deleteFilter->setText("Delete Filter");
    layout->addWidget(deleteFilter,3,0);
    connect(deleteFilter,SIGNAL(clicked()), this, SLOT(handleDeleteFilter()));

    QPushButton* saveImg = new QPushButton(this);
    saveImg->setText("Save Image");
    layout->addWidget(saveImg,3,1);
    connect(saveImg, SIGNAL(clicked()), this, SLOT(handleSaveImage()));


    // Filter selection
    filterSelect = new QGridLayout(this);

    imgFilter* res = new imgFilter(filterType::resize);
    filters.push_back(res);

    imgFilter* thresh = new imgFilter(threshold);
    filters.push_back(thresh);

    imgFilter* sobelFilter = new imgFilter(sobel);
    filters.push_back(sobelFilter);

    imgFilter* cannyFilter = new imgFilter(canny);
    filters.push_back(cannyFilter);

    imgFilter* blur = new imgFilter(smooth);
    filters.push_back(blur);

    imgFilter* gab = new imgFilter(gabor);
    filters.push_back(gab);

    imgFilter* crp = new imgFilter(crop);
    filters.push_back(crp);

    imgFilter* cvtGray = new imgFilter(grey);
    filters.push_back(cvtGray);

    imgFilter* log = new imgFilter(laplacian);
    filters.push_back(log);

    imgFilter* orient = new imgFilter(gradientOrientation);
    filters.push_back(orient);

    for(int i = 0; i < filters.size(); ++i)
    {
        QPushButton* button = new QPushButton;
        button->installEventFilter(this);
        button->setText(QString::fromStdString(filters[i]->filterName));
        filters[i]->buttonPtr = button;
        filterSelect->addWidget(button);
    }

    layout->addLayout(filterSelect,1,1);


    setLayout(layout);
    setWindowTitle("Filter");
}

filterWidget::~filterWidget()
{
    delete ui;
}

bool filterWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        for(int i = 0; i < filters.size(); ++i)
        {
            if(obj == (QObject*)filters[i]->buttonPtr && !curImg.empty())
            {
                currentFilterIdx = i;
                tmpOutput = filters[i]->applyFilter(curImg);
                if(!tmpOutput.empty())
                {
                    emit filterImgChanged(tmpOutput);
                }
                if(filterSettings != NULL)
                {
                    layout->removeWidget(filterSettings);
                    filterSettings->~filterSettingWidget();
                }
                if(filters[i]->type == crop)
                {
                    cropping = true;
                }
                filterSettings = new filterSettingWidget(this,filters[i],i);
                connect(filterSettings, SIGNAL(paramChanged(int)), this, SLOT(handleParamChange(int)));
                connect(filterSettings, SIGNAL(accepted(int)), this, SLOT(handleFilterAccepted(int)));
                connect(filterSettings, SIGNAL(destroyed()), this, SLOT(handleFilterParamsDestroyed()));
                layout->addWidget(filterSettings,2,1);

                return true;
            }
        }
        return false;
    }
    if(event->type() == QEvent::KeyPress)
    {
        if(obj == (QObject*)filterHistory)
        {
            QKeyEvent* ev = dynamic_cast<QKeyEvent*>(event);
            if(ev->key() == Qt::Key_Delete)
            {
                QList<QTreeWidgetItem*> items = filterHistory->selectedItems();
                for(int i = 0; i < items.size(); ++i)
                {
                    delete items[i];
                }
                handleImgChange(currentSource);
                return true;
            }
        }
    }
    return false;
}

void filterWidget::receiveImgChange(cv::Mat img_)
{
    //curImg = img_;
    //orgImg = img_;
    // Recalculate all the filters for this image
    //updateFilterImages(-1); // Recalculate all filter images
}

void filterWidget::handleImgChange(container* cont)
{
    if(cont == NULL) return;
    curImg = cont->img;
    orgImg = cont->img;
    currentSource = cont;
    if(cont->isTop)
        updateFilterImages(-1);
}

void filterWidget::handleParamChange(int idx)
{
    currentFilterIdx = idx;
    tmpOutput = filters[idx]->applyFilter(curImg);
    if(!tmpOutput.empty())
    {
        emit filterImgChanged(tmpOutput);
    }
}

void filterWidget::handleFilterAccepted(int idx)
{
    // Save filter stuff to the filter history window
    container* tmpContainer = new container(filterHistory);
    tmpContainer->setText(0, QString::fromStdString(filters[idx]->filterName));
    tmpContainer->isTop = true;
    tmpContainer->idx = idx; // Store which filter index this filter belongs to

    // Make a copy of the filter in memory so it can be re-applied
    tmpContainer->filter = *(filters[idx]);
    for(unsigned int i = 0; i < filters[idx]->parameters.size(); ++i)
    {
        container* child = new container();
        child->isTop = false;
        child->setText(0,filters[idx]->parameters[i].name);
        child->setText(1,QString::number(filters[idx]->parameters[i].value));
        tmpContainer->addChild(child);
    }
    tmpOutput.copyTo(tmpContainer->img);
    filterHistory->addTopLevelItem(tmpContainer);
    curImg = tmpOutput;
}

void filterWidget::handleFilterParamsDestroyed()
{
    filterSettings = NULL;
}

void filterWidget::handleFilterHistSelect(QTreeWidgetItem *item, int column)
{
    if(column != 0) return;
    currentFilter = dynamic_cast<container*>(item);
    if(currentFilter->img.empty()) return;
    emit filterImgChanged(currentFilter->img);

}

// Reload this filter's parameters and allow editing
void filterWidget::handleFilterHistActivate(QTreeWidgetItem* item, int column)
{
    if(column != 0) return;
    container* it = dynamic_cast<container*>(item);
    if(!it->isTop) return;
    if(filterSettings != NULL)
    {
        layout->removeWidget(filterSettings);
        filterSettings->~filterSettingWidget();
    }
    filterSettings = new filterSettingWidget(this,filters[it->idx],it->idx);
    connect(filterSettings, SIGNAL(paramChanged(int)),	this, SLOT(handleParamChange(int)));
    connect(filterSettings, SIGNAL(accepted(int)),		this, SLOT(handleFilterAccepted(int)));
    connect(filterSettings, SIGNAL(destroyed()),		this, SLOT(handleFilterParamsDestroyed()));
    layout->addWidget(filterSettings,2,1);
}

void filterWidget::handleRectSelect(cv::Rect rectangle)
{
    if(cropping)
    {
        QSpinBox* box = dynamic_cast<QSpinBox*>(filters[currentFilterIdx]->parameters[0].box);
        box->setValue(rectangle.x);
        box = dynamic_cast<QSpinBox*>(filters[currentFilterIdx]->parameters[1].box);
        box->setValue(rectangle.y);
        box = dynamic_cast<QSpinBox*>(filters[currentFilterIdx]->parameters[2].box);
        box->setValue(rectangle.width);
        box = dynamic_cast<QSpinBox*>(filters[currentFilterIdx]->parameters[3].box);
        box->setValue(rectangle.height);
        handleManualCrop();
    }
}

void filterWidget::handleDragStart(QPoint pos)
{

}

void filterWidget::handleDragMove(QPoint pos)
{

}

void filterWidget::handleDragStop(QPoint pos)
{

}

void filterWidget::handleManualCrop()
{
    tmpOutput = filters[currentFilterIdx]->applyFilter(curImg);
    QString output = "Cropped values: " + QString::number(filters[currentFilterIdx]->parameters[0].value) +
            " " + QString::number(filters[currentFilterIdx]->parameters[1].value) +
            " " + QString::number(filters[currentFilterIdx]->parameters[2].value) +
            " " + QString::number(filters[currentFilterIdx]->parameters[3].value);
    emit consoleOutput(output);

    if(!tmpOutput.empty())
    {
        emit filterImgChanged(tmpOutput);
    }
}

void filterWidget::handleDeleteFilter()
{
    if(currentFilter == NULL) return;
    int idx = filterHistory->indexOfTopLevelItem(currentFilter);
    int children = currentFilter->childCount();
    for(int i = 0; i < children; ++i)
    {
        currentFilter->removeChild(currentFilter->child(0));
    }
    delete currentFilter;
    currentFilter = NULL;
    updateFilterImages(idx); // Recalculate all filters after this one
}

void filterWidget::handleSaveImage()
{
    if(filterHistory->selectedItems().size() == 1)
    {
        container* cont = dynamic_cast<container*>(filterHistory->currentItem());
        saveImage(cont);
    }
}

void filterWidget::saveImage(container* cont)
{
    if(cont != NULL)
    {
        if(currentSource == NULL) return;
        if(!currentSource->isTop) return;
        QString name = QString::fromStdString(cont->filter.filterName);
        // Check to see if this exists already
        for(int i = 0; i < currentSource->childCount(); ++i)
        {
            if(currentSource->child(i)->text(0) == name) return;
        }
        cont->saved = true;
        container* child = new container();
        cont->img.copyTo(child->img);
        child->isTop = false;
        child->setText(0,name);
        child->baseName = name;
		child->parentFilter = cont;
		if (cont->savedToDisk) child->savedToDisk = true;
        currentSource->addChild(child);
        int idx = currentSource->indexOfChild(child);
        child->idx = idx;
    }
}

void filterWidget::updateFilterImages(int idx)
{
    container* cont = NULL;
    if(idx != -1)
    {
        cont = dynamic_cast<container*>(filterHistory->topLevelItem(idx));
        if(cont != NULL)
            cont->img.copyTo(curImg);
        ++idx;
    }else
    {
        ++idx;
    }
    for(int i = idx; i < filterHistory->topLevelItemCount(); ++i)
    {
        container* cont = dynamic_cast<container*>(filterHistory->topLevelItem(i));
        if(cont == NULL) return;
        curImg = cont->filter.applyFilter(curImg);
        curImg.copyTo(cont->img);
        if(cont->saved)
            saveImage(cont);
    }
    if(cont == NULL)
    {
        curImg = orgImg;
    }
    if(filterHistory->selectedItems().size() == 1)
    {
        container* cont = dynamic_cast<container*>(filterHistory->itemWidget(filterHistory->selectedItems()[0],0));
        if(cont != NULL)
        {
            cv::Mat img = cont->img;
            if(!img.empty())
                curImg = img;
        }
    }
    // Lastly, update the current image based on currently selected filter parameters
    if(currentFilterIdx != -1)
    tmpOutput = filters[currentFilterIdx]->applyFilter(curImg);
    if(!tmpOutput.empty())
    {
        emit filterImgChanged(tmpOutput);
    }else
    {
        emit filterImgChanged(curImg);
    }

}
