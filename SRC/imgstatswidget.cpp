#include "imgstatswidget.h"
#include "ui_imgstatswidget.h"

imgStatsWidget::imgStatsWidget(QWidget *parent, imgSourcesWidget* sourceList_) :
    QWidget(parent),
    statSettings(NULL),
    extractorSettings(NULL),
    ui(new Ui::imgStatsWidget),
	sourceList(sourceList_),
	curStat(new statContainer())
{
	
	ui->setupUi(this);
	ui->mask->setSourceList(sourceList_);
	ui->src->setSourceList(sourceList_);
	ui->input->setSourceList(sourceList_);
	connect(ui->mask, SIGNAL(imgReceived(containerPtr)), this, SLOT(handleMaskChange(containerPtr)));
	connect(ui->src, SIGNAL(imgReceived(containerPtr)), this, SLOT(handleSrcChange(containerPtr)));
	connect(ui->input, SIGNAL(imgReceived(containerPtr)), this, SLOT(handleInputChange(containerPtr)));
	connect(ui->windowSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(handleWindowChange(int)));
	connect(ui->extractorSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(handleExtractorChange(int)));
	connect(ui->calc, SIGNAL(clicked()), this, SLOT(handleCalcStat()));
	connect(ui->view, SIGNAL(clicked()), this, SLOT(handleViewStat()));
	connect(ui->save, SIGNAL(clicked()), this, SLOT(handleSaveStat()));
	applied = false;
	for (int i = 0; i < STAT_TYPES.size(); ++i)
	{
		featureExtractorPtr tmp(new featureExtractorContainer(this, STAT_TYPES[i]));
		connect(tmp.get(), SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
		extractor.push_back(tmp);
		ui->extractorSelect->addItem(tmp->name);
	}
	for (int i = 0; i < STAT_METHODS.size(); ++i)
	{
		featureWindowPtr tmp(new featureWindowContainer(STAT_METHODS[i]));
		connect(tmp.get(), SIGNAL(displayImage(cv::Mat)), this, SLOT(handleDisplayImg(cv::Mat)));
		connect(tmp.get(), SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
		windows.push_back(tmp);
		ui->windowSelect->addItem(tmp->name);
	}
	curStat->extractor = extractor[0];
	curStat->window = windows[0];
}


imgStatsWidget::~imgStatsWidget()
{
    delete ui;
}
void
imgStatsWidget::handleSrcChange(containerPtr I)
{
	if (I == NULL) return;
	curSource = boost::dynamic_pointer_cast<imgContainer,container>(I);
	curSourceName = I->baseName;
	ui->src->setText(I->baseName);
	// Find the mask
	handleMaskChange(boost::dynamic_pointer_cast<imgContainer, container>(curSource->getChild(curMaskName)));
	// Since the source image has changed, we need to update the input (unless the source image is the input)
	if (curSource->isTop)
	{
		handleInputChange(curSource);
	}
	else
	{
		handleInputChange(boost::dynamic_pointer_cast<imgContainer, container>(curSource->getChild(curInputName)));
	}
	
	
	emit log("Source changed", 2);
}
void
imgStatsWidget::handleInputChange(containerPtr I)
{
	if (I == NULL) 
	{
		emit log(curInputName + " not found", 2);
		return;
	};
	ui->input->setText(I->name);
	curInput = boost::dynamic_pointer_cast<imgContainer, container>(I);
	curInputName = I->name;
	// This is a top level image, thus set the source to this image as well
	if (I->isTop)
	{
		curSourceName = I->baseName;
		curSource = boost::dynamic_pointer_cast<imgContainer, container>(I);
	}else
	{
		// Input is not a top level image, thus check if this is from a different source image
		container* parent = dynamic_cast<container*>(I->parent());
		if (parent->baseName != curSourceName)
		{
			// This is from a different source image, so set the source name
			curSourceName = dynamic_cast<container*>(I->parent())->baseName;
			// Set the source
			curSource = boost::dynamic_pointer_cast<imgContainer, container>(sourceList->getContainer(curSourceName));
		}
	}
	ui->src->setText(curSourceName);
	// If a mask has been selected and we've determined the source image, find the mask
	if (curMaskName.size() > 0 && curSource != NULL)
	{
		curMask = boost::dynamic_pointer_cast<imgContainer, container>(curSource->getChild(curMaskName));
		if (curMask == NULL)
		{
			curMask.reset();
			mask = cv::Mat();
			emit log("Unable to find associated mask for new image", 2);
			return;
		}
	}
	emit log("Input Changed to " + I->name, 2);
}
void
imgStatsWidget::handleMaskChange(containerPtr I)
{
	if (I == NULL)
	{
		emit log("Unabled to find matching mask", 2);
		curMask.reset();
		mask = cv::Mat();
		return;
	}
	ui->mask->setText(I->name);
	curMask = boost::dynamic_pointer_cast<imgContainer, container>(I);
	curMaskName = I->name;
	mask = curMask->M();
	// Check to see if this is from a new source
	if (curMask->baseName != curSource->baseName)
	{
		// This is from a different source image, so set the source name
		curSourceName = dynamic_cast<container*>(I->parent())->baseName;
		// Set the source
		handleSrcChange(sourceList->getContainer(curSourceName)); 
		// Since this is a new source, we need to now see if the current input exists in the new source
		handleInputChange(curSource->getChild(curInputName));

	}


	if (mask.channels() > 1) cv::cvtColor(mask,mask,CV_BGR2GRAY);
	emit log("Mask changed to " + I->name, 2);
}
void 
imgStatsWidget::handleExtractorChange(int idx)
{
	curStat->extractor = extractor[idx];
	if (curStat->window == NULL) return;
	if (extractorSettings != NULL) delete extractorSettings;
	extractorSettings = curStat->extractor->getParamControlWidget(this);
	ui->gridLayout->addWidget(extractorSettings, 8, 2);
	
	findDispImg();
	emit log("Extractor changed to " + curStat->extractor->name, 2);
	//curWindow->extractFeatures(curInput,mask, displayImg, curExtractor);
	//emit extractedFeatures(displayImg, true);
}
void 
imgStatsWidget::findDispImg()
{
	cv::Mat disp; // Display image
	if (curInput == NULL) return;
	if (!curInput->isTop)
	{
		// Get the parent of this container and copy it to the display image, so that when extracting features from
		// processed images, you can see the visualized results overlaid on the original image
		container* par = dynamic_cast<container*>(curInput->parent());
		if (par->type == container::Img)
		{
			imgContainer* img = dynamic_cast<imgContainer*>(par);
			if (img->load())
			{
				img->M().copyTo(disp);
			}
		}
	}
	displayImg = disp;
}
void 
imgStatsWidget::handleLog(QString line, int level)
{
	emit log(line, level);
}
void 
imgStatsWidget::handleWindowChange(int idx)
{
	curStat->window = windows[idx];
	if (curStat->window->parameters.size() == 0) return;
    if(statSettings != NULL)
        delete statSettings;
	statSettings = windows[idx]->getParamControlWidget(this); 
	ui->gridLayout->addWidget(statSettings, 8, 0);
}
void 
imgStatsWidget::handleFeatures(featurePtr f)
{

}
void 
imgStatsWidget::handleDisplayImg(cv::Mat img)
{
	emit extractedFeatures(img, true);
}
void 
imgStatsWidget::paramChange(QString val)
{
	for (unsigned int i = 0; i < curStat->extractor->parameters.size(); ++i)
    {
		if (sender() == (QObject*)curStat->extractor->parameters[i].box)
        {
			curStat->extractor->parameters[i].value = val.toDouble();
        }
    }
}
void 
imgStatsWidget::paramChange(bool val)
{
	for (unsigned int i = 0; i < curStat->extractor->parameters.size(); ++i)
    {
		if (sender() == (QObject*)curStat->extractor->parameters[i].box)
        {
			curStat->extractor->parameters[i].value = val;
        }
    }
}
bool 
imgStatsWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if(ev->type() == QEvent::MouseButtonPress)
    {
		for (int i = 0; i < windows.size(); ++i)
        {

                //connect(statSettings, SIGNAL(paramChange()), this, SLOT(handleParamChange()));

        }
        return false;
    }
    return false;
}
void 
imgStatsWidget::handleParamChange()
{
    /*if(curWindow == NULL) return;
    if(curExtractor == NULL) return;
    if(curCont == NULL) return;
    if(curCont->M.empty()) return;
    curExtractor->recalculateNumParams();
	cv::Mat img;
    features = curWindow->extractFeatures(curCont->M, img)->M;
	if (img.empty()) return;
	emit extractedFeatures(img, true);*/
}
void 
imgStatsWidget::handleImgChange(containerPtr cont)
{
	if (cont == NULL) return;
	if (cont->type != container::Img) return;

	curFeatures.reset();
	handleSrcChange(cont);
	
	for(int i = 0; i < savedStats.size(); ++i)
	{
		curStat = savedStats[i];
		// For each of the saved features
		// Set the mask name for this feature
		curMaskName = savedStats[i]->mask;
		handleMaskChange(curSource->getChild(curMaskName));
		// Set the source image for this feature
		curInputName = savedStats[i]->inputName;
		handleInputChange(curSource->getChild(curInputName));
		// Set the extraction window technique
		curStat->window = savedStats[i]->window;
		// Set the extractor
		curStat->extractor	= savedStats[i]->extractor;
		curStatName			= savedStats[i]->name;
		// handleSrcChange should set the source image, find the input and masks and then extract the features
		handleCalcStat();
		saveStat(curStat->name, curStat);
		if (curStat->saved)
		{
			curFeatures->save();
		}
    }
}
void 
imgStatsWidget::handleCalcStat()
{
	if (curStat->window == NULL || curInput == NULL || curStat->extractor == NULL) return;
	curFeatures = curStat->window->extractFeatures(curInput, mask, displayImg, curStat->extractor);
	// Features have been extracted, need to display somehow.....
	// Matrix view would be nice as well as print to the first line to the console
	cv::Mat f = curFeatures->_M;
	if (curMask != NULL)
	{
		if (curMask->type == container::Label)
		{
			labelPtr lbl = boost::dynamic_pointer_cast<labelContainer, imgContainer>(curMask);
			curFeatures->label = cv::Mat::ones(f.rows, 1, CV_32F)*lbl->label;
		}
	}
	emit log(curStat->extractor->name + " extraction complete with " + QString::number(curFeatures->_M.rows) + " rows, first row: ", 2);
	QString line = "[ ";
	for (int i = 0; i < curFeatures->_M.cols; ++i)
	{
		if (i != 0) line += ", ";
		if (curFeatures->_M.channels() == 1)
		{
			line += QString::number(curFeatures->_M.at<float>(0, i));
		}else
		{
			line += "(" + QString::number(curFeatures->_M.at<cv::Vec3f>(0, i)[0]) + "," +
				QString::number(curFeatures->_M.at<cv::Vec3f>(0, i)[1]) + "," +
				QString::number(curFeatures->_M.at<cv::Vec3f>(0, i)[2]) + ")";
		}
	}
	line += " ]";
	applied = true;
	emit log(line, 2);
}
void 
imgStatsWidget::handleSaveStat()
{
	// TODO save name dialog
	if (curFeatures == NULL) return;
	QDialog tmpDialog;
	QGridLayout tmpLayout(&tmpDialog);
	tmpDialog.setLayout(&tmpLayout);

	QLabel tmpLabel(&tmpDialog);
	tmpLabel.setText("Save name");
	tmpLayout.addWidget(&tmpLabel);

	QLineEdit tmpEdit(&tmpDialog);
	tmpEdit.setText(curStat->extractor->name + " extracted from " + curInput->name);
	tmpLayout.addWidget(&tmpEdit);
	QPushButton ok(&tmpDialog);
	tmpLayout.addWidget(&ok);
	ok.setText("Ok");
	connect(&ok, SIGNAL(clicked()), &tmpDialog, SLOT(accept()));
	tmpDialog.exec();
	handleSaveStat(tmpEdit.text());
}
void
imgStatsWidget::handleSaveStat(QString name)
{
	
	// TODO check if this stat already exists, if so, replace it
	statPtr tmpStat(new statContainer(ui->statHist));
	tmpStat->name = name;
	tmpStat->cached = true;
	tmpStat->mask = curMaskName;
	tmpStat->inputName = curInputName;
	tmpStat->setText(0, name);
	tmpStat->setText(1, "Mask: " + curMaskName);
	tmpStat->setText(2, "Input: " + curInputName);
	tmpStat->extractor.reset(new featureExtractorContainer(curStat->extractor.get()));
	tmpStat->window.reset(new featureWindowContainer(curStat->window.get()));
	savedStats.push_back(tmpStat);
	curStatName = name;
	saveStat(name, tmpStat);
	emit log("Stats saved as " + name, 2);
}
void 
imgStatsWidget::handleViewStat()
{
	emit viewFeatures(curFeatures->M());
}
void
imgStatsWidget::saveStat(QString name, statPtr parent)
{
	saved = true;
	curFeatures->baseName = curSource->baseName;
	curFeatures->dirName = curSource->dirName;
	curFeatures->setText(0, name);
	curFeatures->name = name;
	curSource->deleteChild(name);
	curFeatures->parentContainer = parent.get();
	// Add to QTreeWidget
	curSource->addChild(curFeatures.get());
	// Add smart pointer to child list
	curSource->childContainers.push_back(curFeatures);
}
