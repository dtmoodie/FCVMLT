#ifndef MEASUREMENTDIALOG_H
#define MEASUREMENTDIALOG_H

#include <QDialog>
#include <imgsourceswidget.h>
#include "FCVMLT.h"

namespace Ui {
class measurementDialog;
}

class measurementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit measurementDialog(QWidget *parent = 0, imgSourcesWidget* sourceList_ = 0);
    ~measurementDialog();
	void calculate();

public slots:
	void onImgDrop(containerPtr cont);



signals:
	void log(QString line, int level);
private:
    Ui::measurementDialog *ui;

	// Source image in which everything is compared
	imgPtr reference;
	// Mask for removing spurious line entries
	imgPtr mask;
	// Extracted line
	imgPtr line;
};

#endif // MEASUREMENTDIALOG_H
