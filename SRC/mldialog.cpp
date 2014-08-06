#include "mldialog.h"
#include "ui_mldialog.h"

mlDialog::mlDialog(QWidget *parent, imgSourcesWidget *sources_) :
    QDialog(parent),
	sources(sources_),
    ui(new Ui::mlDialog)
{

}

mlDialog::~mlDialog()
{
    delete ui;
}
