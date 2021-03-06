#include "filterwidget.h"
#include "ui_filterwidget.h"

filterWidget::filterWidget(QWidget *parent, cv::Mat curImg_) :
    QWidget(parent),
    filterSettings(NULL),
    currentFilterIdx(-1),
    ui(new Ui::filterWidget)
{
    cropping = false;
    curImg = curImg_;
    layout = new QGridLayout(this);
	
	QPushButton* saveMacro = new QPushButton(this);
	saveMacro->setText("Save Macro");
	connect(saveMacro, SIGNAL(clicked()), this, SLOT(handleSaveMacro()));
	layout->addWidget(saveMacro, 0, 0);

	QPushButton* loadMacro = new QPushButton(this);
	loadMacro->setText("Load Macro");
	connect(loadMacro, SIGNAL(clicked()), this, SLOT(handleLoadMacro()));
	layout->addWidget(loadMacro, 0, 1);

	QPushButton* clearMacro = new QPushButton(this);
	clearMacro->setText("Clear Macro");
	connect(clearMacro, SIGNAL(clicked()), this, SLOT(handleClearMacro()));
	layout->addWidget(clearMacro, 0, 2);

    QLabel* lblFilterHist = new QLabel(this);
    lblFilterHist->setText("Filter History");
    lblFilterHist->setMaximumHeight(20);
	layout->addWidget(lblFilterHist, 1, 0, 1, 3);

    filterHistory = new QTreeWidget(this);
    filterHistory->setMinimumHeight(100);
    filterHistory->setMinimumWidth(50);
    filterHistory->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    connect(filterHistory, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(handleFilterHistSelect(QTreeWidgetItem*,int)));
    connect(filterHistory, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(handleFilterHistActivate(QTreeWidgetItem*,int)));
    filterHistory->installEventFilter(this);
	filterHistory->setColumnCount(2);
	filterHistory->setDragEnabled(true);
	filterHistory->setDragDropMode(QAbstractItemView::InternalMove);
	layout->addWidget(filterHistory,2,0,2,3);
    

    QPushButton* deleteFilter = new QPushButton(this);
    deleteFilter->setText("Delete Filter");
    layout->addWidget(deleteFilter,4,0);
    connect(deleteFilter,SIGNAL(clicked()), this, SLOT(handleDeleteFilter()));

    QPushButton* saveImg = new QPushButton(this);
    saveImg->setText("Save Image");
    connect(saveImg, SIGNAL(clicked()), this, SLOT(handleSaveImage()));
	layout->addWidget(saveImg, 4, 1);

	QPushButton* acceptFilter = new QPushButton(this);
	acceptFilter->setText("Accept Filter");
	layout->addWidget(acceptFilter, 4, 2);
	connect(acceptFilter, SIGNAL(clicked()), this, SLOT(handleFilterAccepted()));

    // Filter selection
    filterSelect = new QGridLayout(this);

	for (int i = 0; i < FILTER_TYPES.size(); ++i)
	{
		filterPtr tmp(new filterContainer(FILTER_TYPES[i]));
		filters.push_back(tmp);
	}

    for(int i = 0; i < filters.size(); ++i)
    {
        QPushButton* button = new QPushButton;
        button->installEventFilter(this);
        button->setText(filters[i]->name);
		connect(button, SIGNAL(clicked()), this, SLOT(handleFilterSelect()));
        filters[i]->buttonPtr = button;
        filterSelect->addWidget(button);
		connect(filters[i].get(), SIGNAL(accepted()), this, SLOT(handleFilterAccepted()));
		//connect(filters[i], SIGNAL(processingFinished(container*)), this, SLOT(handleFilterUpdated(container*)));
		connect(filters[i].get(), SIGNAL(parameterUpdated()), this, SLOT(handleFilterUpdated()));
		connect(filters[i].get(), SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
    }
    layout->addLayout(filterSelect,0,3,4,1);
    setLayout(layout);
    setWindowTitle("Filter");
	
}
filterWidget::~filterWidget()
{
	savedFilters.clear();
    delete ui;
}
void 
filterWidget::handleFilterSelect()
{
	for (int i = 0; i < filters.size(); ++i)
	{
		if (sender() == (QObject*)filters[i]->buttonPtr)
		{
			currentFilter = filters[i];
			if (filterSettings != NULL) delete filterSettings;
			filterSettings = currentFilter->getParamControlWidget(this);
			for (int i = 0; i < currentFilter->parameters.size(); ++i)
			{
				if (currentFilter->parameters[i].btn != NULL)
				{
					//connect(currentFilter->parameters[i].btn, SIGNAL(clicked()), this, SLOT(handlePathSelect()));
				}
			}
			layout->addWidget(filterSettings, 4, 3);
			containerPtr output = filters[i]->process(currentBase);
			if (output == NULL) return;
			if (output->type != container::Img) return;
			emit filterImgChanged(output);
			return;
		}
	}
}
bool 
filterWidget::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == (QObject*)filterHistory)
	{
		if (ev->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyev = dynamic_cast<QKeyEvent*>(ev);
			if (keyev->key() == Qt::Key_Delete)
			{
				handleDeleteFilter();
				return true;
			}
		}
		if (ev->type() == QEvent::Drop)
		{
			// there has been a rearrangement of filters, reprocess everything
			updateFilterImages(-1);
		}
	}
    return false;
}
// 
void 
filterWidget::receiveImgChange(cv::Mat img_)
{
    //curImg = img_;
    //orgImg = img_;
    // Recalculate all the filters for this image
    //updateFilterImages(-1); // Recalculate all filter images
}
// Depricated
void 
filterWidget::handleImgChange(container* cont)
{
    if(cont == NULL) return;
	switch (cont->type)
	{
		case container::Img:
		{
			imgContainer* tmp = (imgContainer*)cont;
			curImg = tmp->M();
			orgImg = tmp->M();
			break;
		}
		case container::Display:
			return;
	}
	// This may de-allocate ownership incorrectly
	//sourceImg = dynamic_cast<imgContainer*>(cont);
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    updateFilterImages(-1);
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log("Total filtering pipeline took " + QString::number(delta.total_milliseconds()) + " milliseconds", 1);
}
void
filterWidget::handleImgChange(containerPtr cont)
{
	if (cont == NULL) return;
	switch (cont->type)
	{
	case container::Img:
	{
		imgPtr tmp = boost::dynamic_pointer_cast<imgContainer, container>(cont);
		curImg = tmp->M();
		orgImg = tmp->M();
		break;
	}
	case container::Display:
		return;
	case container::Features:
		return;
	}
	// This may de-allocate ownership incorrectly
	sourceImg = boost::dynamic_pointer_cast<imgContainer, container>(cont);
	currentBase = sourceImg;
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	updateFilterImages(-1);
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log("Total filtering pipeline took " + QString::number(delta.total_milliseconds()) + " milliseconds", 1);
}
void 
filterWidget::handleLog(QString line, int level)
{
	emit log(line, level);
}
void 
filterWidget::handleParamChange(int idx)
{
    /*currentFilterIdx = idx;
    //tmpOutput = filters[idx]->applyFilter(curImg);
    if(!tmpOutput.empty())
    {
        emit filterImgChanged(tmpOutput);
    }*/
}
void 
filterWidget::handleFilterAccepted()
{
    // Save filter stuff to the filter history window
	// Make a copy of the filter parameters memory so it can be re-applied
	
	//filterContainer* tmpContainer = dynamic_cast<filterContainer*>(sender());
	filterPtr tmp = currentFilter->getDisplayCopy();
	connect(tmp.get(), SIGNAL(parameterUpdated()), this, SLOT(handleFilterUpdated()));
	filterHistory->addTopLevelItem(tmp.get());
	savedFilters.push_back(tmp);
	currentBase = currentFilter->output;
}
void 
filterWidget::handlePathSelect()
{
	for (int i = 0; i < currentFilter->parameters.size(); ++i)
	{
		if (sender() == currentFilter->parameters[i].btn)
		{
			currentFilter->parameters[i].pathText = QFileDialog::getOpenFileName(this, "Select file");
			return;
		}
	}
}
void 
filterWidget::handleFilterParamsDestroyed()
{
    filterSettings = NULL;
}
void 
filterWidget::handleFilterHistSelect(QTreeWidgetItem *item, int column)
{
	// Validate that this is a valid object
    if(column != 0) return;
	if (item == NULL) return;
	container* cont = dynamic_cast<container*>(item);
	if (cont->type != container::Filter) return;
	filterContainer* filterCont = dynamic_cast<filterContainer*>(cont);
	for (int i = 0; i < savedFilters.size(); ++i)
	{
		if (filterCont == savedFilters[i].get())
		{
			currentFilter = savedFilters[i];
			break;
		}
	}
	if (currentFilter == NULL) return;
	// Update the parameter control area to reflect this filter
	if (filterSettings) delete filterSettings;
	filterSettings = currentFilter->getParamControlWidget(this);
	for (int i = 0; i < currentFilter->parameters.size(); ++i)
	{
		if (currentFilter->parameters[i].btn != NULL)
		{
			connect(currentFilter->parameters[i].btn, SIGNAL(clicked()), this, SLOT(handlePathSelect()));
		}
	}
	layout->addWidget(filterSettings, 4, 3);

}
void 
filterWidget::handleFilterHistActivate(QTreeWidgetItem* item, int column)
{
	// Double clicking sets this filter as the filter which updates the filter preview display
	// This allows selection of a different filter for output than what is currently being modified
	if (column != 0) return;
	if (item == NULL) return;
	container* cont = dynamic_cast<container*>(item);
	if (cont->type != container::Filter) return;
	filterContainer* filterCont = dynamic_cast<filterContainer*>(cont);
	//currentDisplayFilter = filterPtr((filterContainer*)item);
	for (int i = 0; i < savedFilters.size(); ++i)
	{
		if (filterCont == savedFilters[i].get())
		{
			currentDisplayFilter = savedFilters[i];
			break;
		}
	}
	if (currentDisplayFilter == NULL) return;
	if (currentDisplayFilter->output == NULL) return;
		emit filterImgChanged(currentDisplayFilter->output);
}
void 
filterWidget::handleRectSelect(cv::Rect rectangle)
{
	if (currentFilter == NULL) return;
	if (currentFilter->fType != crop) return;
	currentFilter->parameters[0].value = rectangle.x;
	currentFilter->parameters[1].value = rectangle.y;
	currentFilter->parameters[2].value = rectangle.width;
	currentFilter->parameters[3].value = rectangle.height;
	imgPtr img = boost::dynamic_pointer_cast<imgContainer,container>(currentFilter->process(currentBase));
	handleProcessingComplete(img, currentFilter.get());
	
}
void 
filterWidget::handleFilterUpdated(containerPtr filteredImg_)
{
	if (filteredImg_ == NULL) return;
	if (filteredImg_->type != container::Img) return;
	imgContainer* tmp = dynamic_cast<imgContainer*>(filteredImg_.get());
	if (sender() == (QObject*)currentFilter.get())
	{
		// Update display widget for previewing filter parameters
		emit filterImgChanged(filteredImg_);
	}
	currentBase = boost::dynamic_pointer_cast<imgContainer,container>(filteredImg_);
	int idx = filterHistory->indexOfTopLevelItem((QTreeWidgetItem*)sender());
	if (idx == -1)
	{
	}else
	{
		updateFilterImages(idx);
	}
	
}
void
filterWidget::handleFilterUpdated()
{
	// First make sure this signal is from a valid filter object
	container* srcCont = dynamic_cast<container*>(sender());
	if (srcCont->type != container::Filter) return;
	// Now get that filter object

	// This might cause an early de-allocation since this instance of a shared pointer is not the original instance of this shared pointer.
	filterContainer* srcFilter = dynamic_cast<filterContainer*>(srcCont);
	int idx = filterHistory->indexOfTopLevelItem(srcCont);
	imgPtr img;
	if (idx == -1)
	{
		img = boost::dynamic_pointer_cast<imgContainer, container>(srcFilter->process(currentBase));
	}
	else
	{
		if (idx == 0)
		{
			img = boost::dynamic_pointer_cast<imgContainer, container>(srcFilter->process(sourceImg));
		}
		else
		{
			filterContainer* prevFilter = dynamic_cast<filterContainer*>(filterHistory->topLevelItem(idx - 1));
			img = boost::dynamic_pointer_cast<imgContainer, container>(srcFilter->process(prevFilter->output));
		}
	}
	handleProcessingComplete(img, srcFilter);
}
void
filterWidget::handleProcessingComplete(imgPtr img, filterContainer* srcFilter)
{
	if (img == NULL)
	{
		log("Processing failed, NULL image returned!\nDid you forget to put a case in for this filter?", 2);
		return;
	}
	cv::Mat tmp = img->_M;
	//currentBase = img;
	// Determine the location of this filter in the tree
	// TODO: set currentBase to the output of this filter and then re-process
	
	int idx = filterHistory->indexOfTopLevelItem((QTreeWidgetItem*)srcFilter);
	if (idx == -1)
	{
		emit filterImgChanged(img);
	}
	else
	{
		// Process everything after this filter
		currentBase = img;
		updateFilterImages(idx);
	}
}
void 
filterWidget::handleDragStart(QPoint pos)
{

}
void 
filterWidget::handleDragMove(QPoint pos)
{

}
void 
filterWidget::handleDragStop(QPoint pos)
{

}
void 
filterWidget::handleManualCrop()
{
    //tmpOutput = filters[currentFilterIdx]->process(curImg);
    /*QString output = "Cropped values: " + QString::number(filters[currentFilterIdx]->parameters[0].value) +
            " " + QString::number(filters[currentFilterIdx]->parameters[1].value) +
            " " + QString::number(filters[currentFilterIdx]->parameters[2].value) +
            " " + QString::number(filters[currentFilterIdx]->parameters[3].value);
    emit consoleOutput(output);

    if(!tmpOutput.empty())
    {
        emit filterImgChanged(tmpOutput);
    }*/
}
void 
filterWidget::handleDeleteFilter()
{
    if(currentFilter == NULL) return;
	// If this an unsaved filter, cannot delete
	for (int i = 0; i < filters.size(); ++i)
	{
		if (currentFilter == filters[i]) return;
	}
    int idx = filterHistory->indexOfTopLevelItem(currentFilter.get());
	if (currentDisplayFilter == currentFilter)
		currentDisplayFilter.reset();
	// Set the current base to the output of the previous filter
	if (idx > 1)
		currentBase = dynamic_cast<filterContainer*>(filterHistory->topLevelItem(idx - 1))->output;
	// Deleted the first filter, set processing image to the source image
	if (idx == 0)
		currentBase = sourceImg;
	// Recalculate all filters after this one
    updateFilterImages(idx-1); 
}
void 
filterWidget::handleSaveImage()
{
    if(filterHistory->selectedItems().size() == 1)
    {
        filterContainer* cont = dynamic_cast<filterContainer*>(filterHistory->currentItem());
		cont->cached = true;
		// Create a dialog to ask the desired name of the filtered image
		QDialog tmpDialog;
		QGridLayout tmpLayout(&tmpDialog);
		tmpDialog.setLayout(&tmpLayout);

		QLabel tmpLabel(&tmpDialog);
		tmpLabel.setText("Save name");
		tmpLayout.addWidget(&tmpLabel);

		QLineEdit tmpEdit(&tmpDialog);
		tmpEdit.setText(cont->saveName);
		tmpLayout.addWidget(&tmpEdit);
		QPushButton ok(&tmpDialog);
		tmpLayout.addWidget(&ok);
		ok.setText("Ok");
		connect(&ok, SIGNAL(clicked()), &tmpDialog, SLOT(accept()));

		tmpDialog.exec();
		cont->saveName = tmpEdit.text();
        saveImage(cont);
    }
}
void
filterWidget::handleSaveMacro()
{
	if (currentMacro != NULL)
	{
		// TODO: verify if this causes a memory leak or not
		currentMacro->filters.clear();
		currentMacro.reset();
	}
	currentMacro.reset(new filterMacro);
	QString saveName = QFileDialog::getSaveFileName(this, "Save Macro As","","*.xml");

	currentMacro->filters.reserve(filterHistory->topLevelItemCount());

	for (int i = 0; i < filterHistory->topLevelItemCount(); ++i)
	{
		filterContainer* ptr = dynamic_cast<filterContainer*>(filterHistory->topLevelItem(i));
		for (int j = 0; j < savedFilters.size(); ++j)
		{
			if (ptr == savedFilters[j].get())
			{
				currentMacro->filters.push_back(savedFilters[j]);
				break;
			}
		}
		
	}
	currentMacro->save(saveName);
}
void
filterWidget::handleLoadMacro()
{
	QString file = QFileDialog::getOpenFileName(this, "Select macro file", "", "*.xml");
	if (file.size() == 0) return;
	//TODO: Make sure to handle cleaning up children when deleting a macro, or this could be a pretty big leak
//	if (currentMacro != NULL)  currentMacro->;
	currentMacro.reset(new filterMacro(file));
	filterHistory->clear();
	for (int i = 0; i < currentMacro->filters.size(); ++i)
	{
		filterHistory->addTopLevelItem(currentMacro->filters[i].get());
		savedFilters.push_back(currentMacro->filters[i]);
		connect(currentMacro->filters[i].get(), SIGNAL(parameterUpdated()), this, SLOT(handleFilterUpdated()));
		connect(currentMacro->filters[i].get(), SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
	}
	
}

void 
filterWidget::handleClearMacro()
{
	filterHistory->clear();
	currentFilter.reset();
	if (currentMacro != NULL) currentMacro.reset();;
}
void 
filterWidget::saveImage(container* cont)
{
	if (cont == NULL) return;
	if (sourceImg == NULL) return;
    //if(!currentSource->isTop) return;
	if (cont->type != container::Filter) return;
	
	if (!cont->cached) return;

	filterContainer* tmp = dynamic_cast<filterContainer*>(cont);
	QString name = tmp->saveName;
    // Check to see if this exists already, if so, update the image
	for (int i = 0; i < sourceImg->getParent()->childCount(); ++i)
    {
		if (sourceImg->getParent()->child(i)->text(0) == name)
		{
			imgContainer* child = dynamic_cast<imgContainer*>(sourceImg->getParent()->child(i));
			child->M() = tmp->M();
			return;
		}
    }
	// Image doesn't already exist in the sources list, make a new object to hold it
	// Upcast sourceImg to a QTreeWidgetItem* so that it isn't accidentally copied
	imgPtr child(new imgContainer((QTreeWidgetItem*)sourceImg->getParent()));
	child->parentContainer = cont;
	child->setText(0,name);
	tmp->M().copyTo(child->M());
	tmp->cached			= true;
	child->name			= name;
	child->dirName		= sourceImg->dirName + "/Processed";
	child->baseName		= sourceImg->baseName;
	child->cached		= true;
	child->isTop		= false;
	child->saved		= cont->saved;
	if (child->saved) child->save();
	// Keep a pointer to the filter that created this image, so that if the image is saved to disk, we can tell update the filter accordingly.
	//child->parentContainer = tmp;
	dynamic_cast<imgContainer*>(sourceImg->getParent())->addChild(child.get());
	sourceImg->getParent()->childContainers.push_back(child);
}
// Idx is the index of the filter that needs to be updated, if -1, then update all filters for a new image
void 
filterWidget::updateFilterImages(int idx)
{
	filterContainer* cont = NULL;
	// Update all filters AFTER the current filter, current filter is updated elsewhere
	++idx;
	// Flag if the filter preview has been updated or not
	bool updated = false;
	for (int i = idx; i < filterHistory->topLevelItemCount(); ++i)
	{
		filterContainer* tmpFilter = dynamic_cast<filterContainer*>(filterHistory->topLevelItem(i));
		currentBase = boost::dynamic_pointer_cast<imgContainer, container>(tmpFilter->process(boost::dynamic_pointer_cast<container, imgContainer>(currentBase)));
		if (currentBase == NULL)
		{
			emit log("Processing of " + tmpFilter->name + " failed", 1);
			return;
		}
		cv::Mat img = boost::dynamic_pointer_cast<imgContainer,container>(currentBase)->_M;
		saveImage(tmpFilter);
		if (tmpFilter == currentFilter.get())
		{
			emit filterImgChanged(currentBase);
			updated = true;
		}
	}
	// If the filter preview hasn't been updated, it's because the current filter is not from the filter 
	// history window.  Thus apply the current filter and update the display
	if (updated == false && currentFilter != NULL && currentDisplayFilter == NULL)
	{
		imgPtr tmpImg = boost::dynamic_pointer_cast<imgContainer, container>(currentFilter->process(currentBase));
		emit filterImgChanged(tmpImg);
		return;
	}
	//currentFilter = filterPtr((filterContainer*)(filterHistory->topLevelItem(filterHistory->topLevelItemCount() - 1)));
	filterContainer* ptr = dynamic_cast<filterContainer*>(filterHistory->topLevelItem(filterHistory->topLevelItemCount() - 1));
	for (int j = 0; j < savedFilters.size(); ++j)
	{
		if (ptr == savedFilters[j].get())
		{
			currentFilter = savedFilters[j];
			break;
		}
	}
	
	
	if (currentDisplayFilter == NULL)
	{
		emit filterImgChanged(currentBase);
	}else
	{
		emit filterImgChanged(currentDisplayFilter->output);
	}

}
