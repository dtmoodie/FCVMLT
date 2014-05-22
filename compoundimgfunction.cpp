#include "compoundimgfunction.h"
#include "ui_compoundimgfunction.h"

compoundImgFunction::compoundImgFunction(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::compoundImgFunction)
{
    ui->setupUi(this);
}

compoundImgFunction::~compoundImgFunction()
{
    delete ui;
}
