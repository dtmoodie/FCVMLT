#include "savestreamdialog.h"
#include "ui_savestreamdialog.h"
#include <QFileDialog>
saveStreamDialog::saveStreamDialog(QWidget *parent) :
    QDialog(parent),
    writer(NULL),
    first(true),
    opened(false),
    ui(new Ui::saveStreamDialog)
{
    ui->setupUi(this);
	//fourCC = CV_FOURCC('H', '2', '6', '4');
	fourCC = CV_FOURCC('M', 'J', 'P', 'G');
    ui->resolution->addItem("1920x1080");
    ui->resolution->addItem("1280x720");
    ui->resolution->addItem("640x480");
    counter = 0;

}

saveStreamDialog::~saveStreamDialog()
{
    delete ui;
}

void saveStreamDialog::on_browse_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this,"Select base directory");
	baseFileName = "Video-" + QDate::currentDate().toString() + "-";
    QString fileName =  baseFileName + QString::number(counter) +  ".avi";
    ui->fileName->setText(fileName);
    ui->path->setText(filePath + "/");
}

void saveStreamDialog::on_OK_clicked()
{
	if (opened && writer != NULL)
	{
		writer->release();
		delete writer;
		writer = NULL;
		opened = false;
        ui->checkBox->setChecked(false);
        ++counter;
		QString fileName = baseFileName + QString::number(counter) + ".avi";
		ui->fileName->setText(fileName);
	}
    this->hide();
}

void saveStreamDialog::on_cancel_clicked()
{
    ui->checkBox->setChecked(false);
    this->hide();
}

void saveStreamDialog::on_checkBox_clicked()
{

}
// This handles new images on the image stream and saves it
void saveStreamDialog::handleNewImage(container *cont)
{
    if(!ui->checkBox->isChecked()) return;
    if(cont == NULL) return;
	streamContainer* tmp = dynamic_cast<streamContainer*>(cont);
	if (tmp->M().empty()) return;
	if (ui->path->text().size() == 0) return;
	if (ui->fileName->text().size() == 0) return;
    if(writer == NULL)
    {
		writer = new cv::VideoWriter;// ((ui->path->text() + ui->fileName->text()).toStdString(), fourCC, 30, cont->img.size());
		opened = writer->open((ui->path->text() + ui->fileName->text()).toStdString(), fourCC, tmp->frameRate, tmp->M().size());
		if (!opened)
		{
			std::cout << "Codec not found" << std::endl;
			opened = writer->open((ui->path->text() + ui->fileName->text()).toStdString(), -1, tmp->frameRate, tmp->M().size());
		}
    }else
    {
        if(!opened)
        {
			opened = writer->open((ui->path->text() + ui->fileName->text()).toStdString(), fourCC, tmp->frameRate, tmp->M().size());
			if (!opened)
			{
				std::cout << "Codec not found" << std::endl;
				opened = writer->open((ui->path->text() + ui->fileName->text()).toStdString(), -1, tmp->frameRate, tmp->M().size());
			}
        }
    }
	writer->operator <<(tmp->M());
}

void saveStreamDialog::on_path_editingFinished()
{

}

void saveStreamDialog::on_checkBox_clicked(bool checked)
{
    if(checked == false)
    {
        if(writer != NULL)
        {
			writer->release();
            delete writer;
            opened = false;
            writer = NULL;
			++counter;
			QString fileName = baseFileName + QString::number(counter) + ".avi";
			ui->fileName->setText(fileName);
        }
    }
}
