#include "filtersettingwidget.h"
#include "ui_filtersettingwidget.h"

filterSettingWidget::filterSettingWidget(QWidget *parent, imgFilter *filter_, int idx_) :
    QWidget(parent), filter(filter_), idx(idx_),
    ui(new Ui::filterSettingWidget)
{
    QGridLayout *layout = new QGridLayout;

    for(unsigned int i = 0; i < filter->parameters.size(); ++i)
    {
        QLabel* label = new QLabel(filter->parameters[i].name);
        layout->addWidget(label,i,0);
        if(filter->parameters[i].type == t_char)
        {
            QSpinBox* box = new QSpinBox;
            box->setMinimum(0);
            box->setMaximum(255);
            box->setObjectName(filter->parameters[i].name);
            box->setValue(filter->parameters[i].value);
            filter->parameters[i].box = box;
            layout->addWidget(box,i,1);
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));
        }
        if(filter->parameters[i].type == t_float)
        {
            QDoubleSpinBox* box = new QDoubleSpinBox;
            box->setObjectName(filter->parameters[i].name);
            box->setValue(filter->parameters[i].value);
            box->setMinimum(-10000.0);
            box->setMaximum(10000.0);
            box->setSingleStep(0.1);
            box->setValue(filter->parameters[i].value);
            filter->parameters[i].box = box;
            layout->addWidget(box,i,1);
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));
        }
        if(filter->parameters[i].type == t_double)
        {
            QDoubleSpinBox* box = new QDoubleSpinBox;
            box->setObjectName(filter->parameters[i].name);
            box->setValue(filter->parameters[i].value);
            box->setMinimum(-10000.0);
            box->setMaximum(10000.0);
            box->setSingleStep(0.1);
            box->setValue(filter->parameters[i].value);
            filter->parameters[i].box = box;
            layout->addWidget(box,i,1);
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));

        }
        if(filter->parameters[i].type == t_int)
        {
            QSpinBox* box = new QSpinBox;
            box->setObjectName(filter->parameters[i].name);
            box->setValue(filter->parameters[i].value);
            layout->addWidget(box,i,1);
            box->setMinimum(-10000);
            box->setMaximum(10000);
            box->setValue(filter->parameters[i].value);
            filter->parameters[i].box = box;
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));

        }
        if(filter->parameters[i].type == t_pullDown)
        {
            QComboBox* box = new QComboBox;
            box->setObjectName(filter->parameters[i].name);
            box->setToolTip(filter->parameters[i].toolTip);
            layout->addWidget(box,i,1);
            box->addItems(filter->parameters[i].pullDownItems);
            filter->parameters[i].box = box;
            connect(box, SIGNAL(currentIndexChanged(QString)),this,SLOT(paramChange(QString)));
        }
    }
    QPushButton* accept = new QPushButton;
    QPushButton* cancel = new QPushButton;
    accept->setText("Accept Changes");
    cancel->setText("Cancel Changes");
    layout->addWidget(accept, filter->parameters.size(), 0);
    layout->addWidget(cancel, filter->parameters.size(), 1);
    connect(accept,SIGNAL(clicked()), this, SLOT(acceptClicked()));
    setLayout(layout);
    setWindowTitle(QString::fromStdString(filter->filterName) + " Settings");
}

filterSettingWidget::~filterSettingWidget()
{
    delete ui;
}


void filterSettingWidget::paramChange(QString value)
{
    for(unsigned int i = 0; i < filter->parameters.size(); ++i)
    {
        if(sender() == (QObject*)filter->parameters[i].box)
        {
            if(filter->parameters[i].type == t_pullDown)
            {
                for(unsigned int j = 0; j < filter->parameters[i].pullDownItems.size(); ++j)
                {
                    if(filter->parameters[i].pullDownItems[j] == value)
                    {
                        filter->parameters[i].value = j;
                        emit paramChanged(idx);
                    }
                }
            }else
            {
                filter->parameters[i].value = value.toDouble();
                emit paramChanged(idx);
            }
        }
    }
}

void filterSettingWidget::acceptClicked()
{
    emit accepted(idx);
    this->~filterSettingWidget();
}
