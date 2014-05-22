#include "imgstats.h"
#include "ui_imgstats.h"

imgStats::imgStats(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imgStats)
{
    ui->setupUi(this);
}

imgStats::~imgStats()
{
    delete ui;
}
