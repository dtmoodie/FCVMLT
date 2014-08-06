#ifndef MLDIALOG_H
#define MLDIALOG_H

#include <QDialog>
//#include <container.h>
#include <FCVMLT.h>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <qcheckbox.h>
#include "imgsourceswidget.h"
namespace Ui {
class mlDialog;
}


// Wizard to guide you through processing feature data
// Upon opening the dialog, it will examin the first element in the source list and find all feature and label files
// It will then present a wizard that will walk you through selecting the features you want to use and the 
// algorithm you want to use.  After the algorithm is selected it will process all of the data in the source list and 
// give you some nice results as to the performance of the algorithm
class mlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mlDialog(QWidget *parent = 0, imgSourcesWidget* sources_ = 0);
    ~mlDialog();

public slots:
	// Finds relavent features and populates the featureSelection widget
	void updateFeatureSelection();
	void on_next_clicked();
	void on_back_clicked();
	void on_cancel_clicked();
	void updateSettingPage();
	

signals :
	void log(QString line, int level);


private:
    Ui::mlDialog*		ui;
	imgSourcesWidget*	sources;
	QList<mlPtr>		algorithms;
	QList<QCheckBox*>	algorithmSelectBoxes;
	QList<QWidget*>		currentControlWidgets;
};

#endif // MLDIALOG_H
