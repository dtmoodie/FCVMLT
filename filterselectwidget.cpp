#include "filterselectwidget.h"
#include "ui_filterselectwidget.h"

filterSelectWidget::filterSelectWidget(QWidget *parent, QTreeWidget* tree_) :
    QWidget(parent), currentSettingWidget(NULL), tree(tree_),
    ui(new Ui::filterSelectWidget)
{
    imgFilter* res = new imgFilter(filterType::resize);
    filters.push_back(res);

    imgFilter* thresh = new imgFilter(threshold);
    filters.push_back(thresh);

    imgFilter* sobelFilter = new imgFilter(sobel);
    filters.push_back(sobelFilter);

    imgFilter* blur = new imgFilter(smooth);
    filters.push_back(blur);

    imgFilter* gab = new imgFilter(gabor);
    filters.push_back(gab);

    imgFilter* crp = new imgFilter(crop);
    filters.push_back(crp);


    layout = new QGridLayout();
    for(unsigned int i = 0; i < filters.size(); ++i)
    {
        QPushButton* button = new QPushButton;
        button->installEventFilter(this);
        button->setText(QString::fromStdString(filters[i]->filterName));
        filters[i]->buttonPtr = button;
        layout->addWidget(button);
    }
    setLayout(layout);
}

filterSelectWidget::~filterSelectWidget()
{
    delete ui;
}

bool filterSelectWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        for(unsigned int i = 0; i < filters.size(); ++i)
        {
            if(obj == (QObject*)filters[i]->buttonPtr && !curImg.empty())
            {
                tmpOutput = filters[i]->applyFilter(curImg);
                if(!tmpOutput.empty())
                {
                    cv::Mat resized;
                    cv::resize(tmpOutput,resized,cv::Size(640,480));
                    cv::imshow("Filter temp output", resized);
                }
                if(filters[i]->type == crop)
                {
//                    currentDialogIsImageEdit = true;
//                    curDialog = new cvImageEditDialog(this,filters[i],i,curImg);
//                    connect(dynamic_cast<cvImageEditDialog*>(curDialog), SIGNAL(paramChanged(int)),
//                            this, SLOT(filterParamChanged(int)));
//                    connect(dynamic_cast<cvImageEditDialog*>(curDialog), SIGNAL(accepted(int)),
//                            this, SLOT(filterAccepted(int)));
//                    curDialog->show();
                }else
                {
                    if(currentSettingWidget != NULL)
                    {
                        layout->removeWidget(currentSettingWidget);
                    }
                    currentSettingWidget = new filterSettingWidget(this,filters[i],i);
                    layout->addWidget(currentSettingWidget,0,1);
                }
            }
        }
        return false;
    }
    return false;
}
