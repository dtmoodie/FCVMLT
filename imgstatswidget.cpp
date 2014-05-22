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

    featureExtractor* sumExt = new featureExtractor(sum);
    extractor.push_back(sumExt);
    featureExtractor* avgExt = new featureExtractor(avg);
    extractor.push_back(avgExt);
    featureExtractor* medianExt = new featureExtractor(median);
    extractor.push_back(medianExt);
    featureExtractor* stdevExt = new featureExtractor(stdev);
    extractor.push_back(stdevExt);
    featureExtractor* histExt = new featureExtractor(hist);
    extractor.push_back(histExt);
    featureExtractor* siftExt = new featureExtractor(sift);
    extractor.push_back(siftExt);
    featureExtractor* hogExt = new featureExtractor(HoG);
    extractor.push_back(hogExt);
    featureExtractor* orbExt = new featureExtractor(orb);
    extractor.push_back(orbExt);

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

    featureWindow* wholeImg = new featureWindow(wholeImage, &curExtractor);
    stats.push_back(wholeImg);
    featureWindow* roi = new featureWindow(ROI, &curExtractor);
    stats.push_back(roi);
    featureWindow* supPix = new featureWindow(superPixel, &curExtractor);
    stats.push_back(supPix);
    featureWindow* perPix = new featureWindow(perPixel, &curExtractor);
    stats.push_back(perPix);
    featureWindow* keyPoint_ = new featureWindow(keyPoint, &curExtractor);
    stats.push_back(keyPoint_);

    windowComboBox = new QComboBox(this);
    for(int i = 0; i < stats.size(); ++i)
    {
        windowComboBox->addItem(stats[i]->statName);
    }
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

void imgStatsWidget::handleExtractorChange(int idx)
{
    curExtractor = extractor[idx];
    if(curExtractor->parameters.size() != 0)
    {
        if(extractorSettings != NULL)
        {
            delete extractorSettings;
        }
        if(extractorLayout != NULL)
        {
            delete extractorLayout;
        }
        extractorSettings = new QWidget(this);
        extractorLayout = new QGridLayout();
        for(unsigned int i = 0; i < curExtractor->parameters.size(); ++i)
        {
            QLabel* label = new QLabel(curExtractor->parameters[i].name);
            extractorLayout->addWidget(label,i,0);
            if(curExtractor->parameters[i].type == t_char)
            {
                QSpinBox* box = new QSpinBox(extractorSettings);
                box->setMinimum(0);
                box->setMaximum(255);
                box->setObjectName(curExtractor->parameters[i].name);
                box->setValue(curExtractor->parameters[i].value);
                curExtractor->parameters[i].box = box;
                extractorLayout->addWidget(box,i,1);
                connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));
            }
            if(curExtractor->parameters[i].type == t_float)
            {
                QDoubleSpinBox* box = new QDoubleSpinBox(extractorSettings);
                box->setObjectName(curExtractor->parameters[i].name);
                box->setValue(curExtractor->parameters[i].value);
                box->setMinimum(-10000.0);
                box->setMaximum(10000.0);
                box->setSingleStep(0.1);
                box->setValue(curExtractor->parameters[i].value);
                curExtractor->parameters[i].box = box;
                extractorLayout->addWidget(box,i,1);
                connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));
            }
            if(curExtractor->parameters[i].type == t_double)
            {
                QDoubleSpinBox* box = new QDoubleSpinBox(extractorSettings);
                box->setObjectName(curExtractor->parameters[i].name);
                box->setValue(curExtractor->parameters[i].value);
                box->setMinimum(-10000.0);
                box->setMaximum(10000.0);
                box->setSingleStep(0.1);
                box->setValue(curExtractor->parameters[i].value);
                curExtractor->parameters[i].box = box;
                extractorLayout->addWidget(box,i,1);
                connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));

            }
            if(curExtractor->parameters[i].type == t_int)
            {
                QSpinBox* box = new QSpinBox(extractorSettings);
                box->setObjectName(curExtractor->parameters[i].name);
                box->setValue(curExtractor->parameters[i].value);
                extractorLayout->addWidget(box,i,1);
                box->setMinimum(-10000);
                box->setMaximum(10000);
                box->setValue(curExtractor->parameters[i].value);
                curExtractor->parameters[i].box = box;
                connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));

            }
            if(curExtractor->parameters[i].type == t_pullDown)
            {
                QComboBox* box = new QComboBox(extractorSettings);
                box->setObjectName(curExtractor->parameters[i].name);
                box->setToolTip(curExtractor->parameters[i].toolTip);
                extractorLayout->addWidget(box,i,1);
                box->addItems(curExtractor->parameters[i].pullDownItems);
                curExtractor->parameters[i].box = box;
                connect(box, SIGNAL(currentIndexChanged(QString)),this,SLOT(paramChange(QString)));
            }
            if(curExtractor->parameters[i].type == t_bool)
            {
                QCheckBox* box = new QCheckBox(extractorSettings);
                box->setObjectName(curExtractor->parameters[i].name);
                box->setCheckable(true);
                box->setChecked(curExtractor->parameters[i].value == 1);
                curExtractor->parameters[i].box = box;
                connect(box, SIGNAL(toggled(bool)), this, SLOT(paramChange(bool)));
                extractorLayout->addWidget(box,i,1);
            }
        }
        layout->addLayout(extractorLayout,3,1);
    }
}

void imgStatsWidget::handleWindowChange(int idx)
{
    curWindow = stats[idx];
    if(statSettings != NULL)
    {
        layout->removeWidget(statSettings);
        delete statSettings;
    }
    statSettings = new statSettingWidget(this,stats[idx]);
    layout->addWidget(statSettings,2,1);
}


void imgStatsWidget::paramChange(QString val)
{
    for(unsigned int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        if(sender() == (QObject*)curExtractor->parameters[i].box)
        {
            curExtractor->parameters[i].value = val.toDouble();
        }
    }
}
void imgStatsWidget::paramChange(bool val)
{
    for(unsigned int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        if(sender() == (QObject*)curExtractor->parameters[i].box)
        {
            curExtractor->parameters[i].value = val;
        }
    }
}

bool imgStatsWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        for(int i = 0; i < stats.size(); ++i)
        {

                //connect(statSettings, SIGNAL(paramChange()), this, SLOT(handleParamChange()));

        }
        return false;
    }
    return false;
}

void imgStatsWidget::handleParamChange()
{
    if(curWindow == NULL) return;
    if(curExtractor == NULL) return;
    if(curCont == NULL) return;
    if(curCont->img.empty()) return;
    curExtractor->recalculateNumParams();
    isImg = false;
    features = curWindow->extractFeatures(curCont->img, isImg);
    if(!features.empty())
        emit extractedFeatures(features, isImg);
}

void imgStatsWidget::handleImgChange(container *cont)
{
    curCont = cont;
    if(cont->isTop)
    {
        for(int i = 0; i < statHist->topLevelItemCount(); ++i)
        {
            container* statCont = dynamic_cast<container*>(statHist->topLevelItem(i));
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
            featureExtractor* extractorPtr= &statCont->statExtractor;
            if(statCont->sourceIdx == -1 && cont->idx == -1)
            {
                features = statCont->statWindow.extractFeatures(curCont->img,isImg, &extractorPtr);
                handleSaveStat();
            }else
            {
               if(cont->idx == -1)
               {
                   container* tmp = NULL;
                   for(int k = 0; k < cont->childCount(); ++k)
                   {
                       if(cont->child(k)->text(0) == statCont->sourceName)
                       {
                           tmp = dynamic_cast<container*>(cont->child(k));
                       }
                   }
                   if(tmp == NULL) return;
                   if(tmp->img.empty()) return;
                   features = statCont->statWindow.extractFeatures(tmp->img
                                                               ,isImg, &extractorPtr);
                   handleSaveStat();
                   emit extractedFeatures(features,isImg);
               }
            }
        }
    }
}

void imgStatsWidget::handleCalcStat()
{
    handleParamChange();
    nameOfSourceImg = curCont->text(0);
    for(int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        if(curExtractor->parameters[i].type == t_double)
        {
            dynamic_cast<QDoubleSpinBox*>(curExtractor->parameters[i].box)->setValue(curExtractor->parameters[i].value);
        }
    }
}

void imgStatsWidget::handleSaveStat()
{
    QString name = curExtractor->name + " of " + curWindow->statName + " from " + nameOfSourceImg;
    container* child = new container(statHist);
    child->setText(0,name);
    features.copyTo(child->img);
    child->isFeatures = true;
    child->isImg = isImg;
    child->statExtractor = *curExtractor;
    child->statWindow = *curWindow;
    if(curCont->isTop)
    {
        child->sourceIdx = -1;
    }else
    {
        container* parent = dynamic_cast<container*>(((QTreeWidgetItem*)curCont)->parent());
        if(parent == NULL) return;
        child->sourceIdx = parent->indexOfChild(curCont);
    }
    child->sourceName = nameOfSourceImg;

    container* technique = new container();
    technique->setText(0,curWindow->statName);
    for(int i = 0; i < curWindow->parameters.size(); ++i)
    {
        container* techChild = new container();
        techChild->setText(0,curWindow->parameters[i].name);
        techChild->setText(1,QString::number(curWindow->parameters[i].value));
        technique->addChild(techChild);
    }
    child->addChild(technique);

    container* extractor = new container();
    extractor->setText(0,curExtractor->name);
    for(int i = 0; i < curExtractor->parameters.size(); ++i)
    {
        container* extractorChild = new container();
        extractorChild->setText(0, curExtractor->parameters[i].name);
        extractorChild->setText(1, QString::number(curExtractor->parameters[i].value));
        extractor->addChild(extractorChild);
    }

    child->addChild(extractor);



    container* source = new container();
    source->setText(0,"Source Img:");
    source->setText(1,nameOfSourceImg);
    child->addChild(source);

    container* sourceCont = new container();
    sourceCont->setText(0, name);
    features.copyTo(sourceCont->img);
    sourceCont->isTop = false;
    sourceCont->isFeatures = true;
    sourceCont->isImg = isImg;
    //sourceCont->extractor = curExtractor;
    //sourceCont->window = curWindow;
    if(curCont->isTop)
        curCont->addChild(sourceCont);
    else
    {
        container* parent = dynamic_cast<container*>(((QTreeWidgetItem*)curCont)->parent());
        if(parent->isTop)
        {
            parent->addChild(sourceCont);
        }
    }
}
