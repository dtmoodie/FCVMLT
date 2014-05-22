#include "statsettingwidget.h"
#include "ui_statsettingwidget.h"

statSettingWidget::statSettingWidget(QWidget *parent, featureWindow* stats_) :
    stats(stats_),
    QWidget(parent),
    ui(new Ui::statSettingWidget)
{
    layout = new QGridLayout(this);

    for(unsigned int i = 0; i < stats->parameters.size(); ++i)
    {
        QLabel* label = new QLabel(stats->parameters[i].name);
        layout->addWidget(label,i,0);
        if(stats->parameters[i].type == t_char)
        {
            QSpinBox* box = new QSpinBox;
            box->setMinimum(0);
            box->setMaximum(255);
            box->setObjectName(stats->parameters[i].name);
            box->setValue(stats->parameters[i].value);
            stats->parameters[i].box = box;
            layout->addWidget(box,i,1);
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));
        }
        if(stats->parameters[i].type == t_float)
        {
            QDoubleSpinBox* box = new QDoubleSpinBox;
            box->setObjectName(stats->parameters[i].name);
            box->setValue(stats->parameters[i].value);
            box->setMinimum(-10000.0);
            box->setMaximum(10000.0);
            box->setSingleStep(0.1);
            box->setValue(stats->parameters[i].value);
            stats->parameters[i].box = box;
            layout->addWidget(box,i,1);
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));
        }
        if(stats->parameters[i].type == t_double)
        {
            QDoubleSpinBox* box = new QDoubleSpinBox;
            box->setObjectName(stats->parameters[i].name);
            box->setValue(stats->parameters[i].value);
            box->setMinimum(-10000.0);
            box->setMaximum(10000.0);
            box->setSingleStep(0.1);
            box->setValue(stats->parameters[i].value);
            stats->parameters[i].box = box;
            layout->addWidget(box,i,1);
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));

        }
        if(stats->parameters[i].type == t_int)
        {
            QSpinBox* box = new QSpinBox;
            box->setObjectName(stats->parameters[i].name);
            box->setValue(stats->parameters[i].value);
            layout->addWidget(box,i,1);
            box->setMinimum(-10000);
            box->setMaximum(10000);
            box->setValue(stats->parameters[i].value);
            stats->parameters[i].box = box;
            connect(box,SIGNAL(valueChanged(QString)), this, SLOT(paramChange(QString)));

        }
        if(stats->parameters[i].type == t_pullDown)
        {
            QComboBox* box = new QComboBox;
            box->setObjectName(stats->parameters[i].name);
            box->setToolTip(stats->parameters[i].toolTip);
            layout->addWidget(box,i,1);
            box->addItems(stats->parameters[i].pullDownItems);
            stats->parameters[i].box = box;
            connect(box, SIGNAL(currentIndexChanged(QString)),this,SLOT(paramChange(QString)));
        }
    }
    setLayout(layout);
}

statSettingWidget::~statSettingWidget()
{
    delete ui;
}

void statSettingWidget::paramChange(QString val)
{
    for(unsigned int i = 0; i < stats->parameters.size(); ++i)
    {
        if(sender() == (QObject*)stats->parameters[i].box)
        {
            stats->parameters[i].value = val.toDouble();
            emit paramChange();
        }
    }
}
