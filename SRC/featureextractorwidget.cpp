#include "featureextractorwidget.h"
#include "ui_featureextractorwidget.h"

featureExtractorWidget::featureExtractorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::featureExtractorWidget)
{
    ui->setupUi(this);
}

featureExtractorWidget::~featureExtractorWidget()
{
    delete ui;
}
