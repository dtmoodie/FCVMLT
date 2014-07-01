#include "imgstatswidget.h"
#include "ui_imgstatswidget.h"

imgStatsWidget::imgStatsWidget(QWidget *parent) :
    QWidget(parent),
    statSettings(NULL),
    curWindow(NULL),
    curExtractor(NULL),
    curCont(NULL),
    extractorSettings(NULL),
    extractorLayout(NULL),
    ui(new Ui::imgStatsWidget)
{
    layout = new QGridLayout(this);
    setLayout(layout);

    statHist = new QTreeWidget(this);
    statHist->setColumnCount(2);
    layout->addWidget(statHist,0,0,2,1);

	for (int i = 0; i < STAT_TYPES.size(); ++i)
	{
		featureExtractorContainer* tmp = new featureExtractorContainer(this, STAT_TYPES[i]);
		connect(tmp, SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
		extractor.push_back(tmp);
	}

    extractorComboBox = new QComboBox(this);
    for(int i = 0; i < extractor.size(); ++i)
    {
        extractorComboBox->addItem(extractor[i]->name);
    }

    connect(extractorComboBox, SIGNAL(activated(int)), this, SLOT(handleExtractorChange(int idx)));
    connect(extractorComboBox,SIGNAL(currentIndexChanged(int)), this, SLOT(handleExtractorChange(int)));
    
	extractorComboBox->setMaximumHeight(20);
    layout->addWidget(extractorComboBox,0,1);
    curExtractor = extractor[0];

    statSelect = new QGridLayout(this);
    layout->addLayout(statSelect,1,1);

	for (int i = 0; i < STAT_METHODS.size(); ++i)
	{
		featureWindowContainer* tmp = new featureWindowContainer(STAT_METHODS[i]);
		connect(tmp, SIGNAL(displayImage(cv::Mat)), this, SLOT(handleDisplayImg(cv::Mat)));
		connect(tmp, SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
		stats.push_back(tmp);
	}
    windowComboBox = new QComboBox(this);
    for(int i = 0; i < stats.size(); ++i)
    {
		windowComboBox->addItem(stats[i]->name);
    }
	curWindow = stats[0];
    connect(windowComboBox, SIGNAL(activated(int)), this, SLOT(handleWindowChange(int)));
    connect(windowComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleWindowChange(int)));
    layout->addWidget(windowComboBox,1,1);


    QGridLayout* buttons = new QGridLayout(this);
    layout->addLayout(buttons,2,0);

    QPushButton* calcStat = new QPushButton(this);
    calcStat->setText("Calculate Stat");
    buttons->addWidget(calcStat,0,0);
    connect(calcStat, SIGNAL(clicked()), this, SLOT(handleCalcStat()));

    QPushButton* saveStat = new QPushButton(this);
    saveStat->setText("Save stat");
    buttons->addWidget(saveStat,0,1);
    connect(saveStat, SIGNAL(clicked()), this, SLOT(handleSaveStat()));
}


imgStatsWidget::~imgStatsWidget()
{
    delete ui;
}
void 
imgStatsWidget::handleExtractorChange(int idx)
{
	curExtractor = extractor[idx];
	if (curWindow == NULL) return;
	if (extractorSettings != NULL) delete extractorSettings;
	extractorSettings = curExtractor->getParamControlWidget(this);
	layout->addWidget(extractorSettings, 3, 1);
	cv::Mat disp; // Display image
	if (!curCont->isTop)
	{
		container* par = dynamic_cast<container*>(curCont->parent());
		if (par->type == container::Img)
		{
			imgContainer* img = dynamic_cast<imgContainer*>(par);
			if (img->load())
			{
				img->M().copyTo(disp);
			}
		}
	}
	curWindow->extractFeatures(curCont, disp, curExtractor);
	emit extractedFeatures(disp, true);
}
void 
imgStatsWidget::handleLog(QString line, int level)
{
	emit log(line, level);
}
void 
imgStatsWidget::handleWindowChange(int idx)
{
    curWindow = stats[idx];
    if(statSettings != NULL)
    {
        layout->removeWidget(statSettings);
        delete statSettings;
    }
	statSettings = stats[idx]->getParamControlWidget(this); // = new statSettingWidget(this,stats[idx]);
    layout->addWidget(statSettings,2,1);
}
void 
imgStatsWidget::handleFeatures(featureContainer* f)
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
    for(unsigned int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        if(sender() == (QObject*)curExtractor->parameters[i].box)
        {
            curExtractor->parameters[i].value = val.toDouble();
        }
    }
}
void 
imgStatsWidget::paramChange(bool val)
{
    for(unsigned int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        if(sender() == (QObject*)curExtractor->parameters[i].box)
        {
            curExtractor->parameters[i].value = val;
        }
    }
}
bool 
imgStatsWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if(ev->type() == QEvent::MouseButtonPress)
    {
        for(int i = 0; i < stats.size(); ++i)
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
imgStatsWidget::handleImgChange(container *cont)
{
	if (cont == NULL) return;
	if (cont->type != container::Img) return;
	curCont = dynamic_cast<imgContainer*>(cont);
    if(cont->isTop)
    {
        for(int i = 0; i < statHist->topLevelItemCount(); ++i)
        {
			statContainer* statCont = dynamic_cast<statContainer*>(statHist->topLevelItem(i));
            if(statCont == NULL) return;
            bool exit = false;
            for(int j = 0; j < curCont->childCount(); ++j)
            {
                if(curCont->child(j)->text(0) == statCont->text(0))
                {
                    exit = true;
                    break;
                }
            }
            if(exit) break;
			featureExtractorContainer* extractorPtr = statCont->extractor;
            if(statCont->sourceIdx == -1)// && cont->idx == -1)
            {
				cv::Mat img;
				features = statCont->window->extractFeatures(curCont, img, extractorPtr)->M();
                handleSaveStat();
            }else
            {
               if(true)//cont->idx == -1)
               {
				   // Search for child image to extract stats from
                   container* tmp = NULL;
                   for(int k = 0; k < cont->childCount(); ++k)
                   {
                       if(cont->child(k)->text(0) == statCont->name)
                       {
                           tmp = dynamic_cast<container*>(cont->child(k));
                       }
                   }
                   if(tmp == NULL) return;
				   switch (tmp->type)
				   {
				   case container::Img:
					   imgContainer* img = dynamic_cast<imgContainer*>(tmp);
					   if (img->M().empty()) return;
					   features = statCont->window->extractFeatures(img, cv::Mat(), curExtractor)->M();
				   }
                   handleSaveStat();
                   emit extractedFeatures(features,isImg);
               }
            }
        }
    }
}
void 
imgStatsWidget::handleCalcStat()
{
    /*handleParamChange();
	if (curCont == NULL) return;
    nameOfSourceImg = curCont->text(0);
    for(int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        if(curExtractor->parameters[i].type == t_double)
        {
            dynamic_cast<QDoubleSpinBox*>(curExtractor->parameters[i].box)->setValue(curExtractor->parameters[i].value);
        }
    }*/
}
void 
imgStatsWidget::handleSaveStat()
{
    /*QString name = curExtractor->name + " of " + curWindow->name + " from " + nameOfSourceImg;
	statContainer* parent = new statContainer(statHist);
	parent->setText(0, name);

	parent->isImg = isImg;
	parent->extractor = new featureExtractorContainer(curExtractor);
    if(curCont->isTop)
    {
		//parent->sourceIdx = -1;
    }else
    {
        container* parent = dynamic_cast<container*>(((QTreeWidgetItem*)curCont)->parent());
		if (parent == NULL) return;
		if (parent->type != container::Stat) return;
		statContainer* tmp = dynamic_cast<statContainer*>(parent);
		//tmp->sourceIdx = tmp->indexOfChild(curCont);
    }
	//parent->sourceName = nameOfSourceImg;

    container* technique = new container();
    technique->setText(0,curWindow->name);
    for(int i = 0; i < curWindow->parameters.size(); ++i)
    {
        container* techChild = new container();
        techChild->setText(0,curWindow->parameters[i].name);
        techChild->setText(1,QString::number(curWindow->parameters[i].value));
        technique->addChild(techChild);
    }
	parent->addChild(technique);

    container* extractor = new container();
    extractor->setText(0,curExtractor->name);
    for(int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        container* extractorChild = new container();
        extractorChild->setText(0, curExtractor->parameters[i].name);
        extractorChild->setText(1, QString::number(curExtractor->parameters[i].value));
        extractor->addChild(extractorChild);
    }

	parent->addChild(extractor);


	// Display of the source image name
    container* source = new container();
    source->setText(0,"Source Img:");
    source->setText(1,nameOfSourceImg);
	parent->addChild(source);

	// feature matrix holding the feature info
	featureContainer* sourceCont = new featureContainer(dynamic_cast<QTreeWidgetItem*>(parent));
    sourceCont->setText(0, name);
    features.copyTo(sourceCont->M);
    sourceCont->isTop = false;
    sourceCont->isImg = isImg;
    if(curCont->isTop)
        curCont->addChild(sourceCont);
    else
    {
        container* parent = dynamic_cast<container*>(((QTreeWidgetItem*)curCont)->parent());
        if(parent->isTop)
        {
            parent->addChild(sourceCont);
        }
    }*/
}
