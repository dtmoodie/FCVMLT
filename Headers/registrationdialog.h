#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>
#include "FCVMLT.h"
#include "imageedit.h"
#include "qpoint.h"


namespace Ui {
class registrationDialog;
}

class registrationDialog : public QDialog
{
    Q_OBJECT
	enum overlayMethod
	{
		Edges = 0,
		Overlay
	};



public:
    explicit registrationDialog(QWidget *parent = 0);
    ~registrationDialog();

public slots:
	void handleImageSelect(container* cont);

private slots:
    void on_btnSelectSource_clicked(bool val);

	void on_btnSelectInput_clicked(bool val);

    void on_btnSaveRegistration_clicked();

    void on_btnCancel_clicked();

    void on_technique_currentIndexChanged(int index);

    void on_displayMethod_currentIndexChanged(int index);

    void on_boarderMethod_currentIndexChanged(int index);

	void handleKeyPress(int key);

	void handleDragStart(QPoint pt);

	void handleDrag(QPoint pt);

	bool eventFilter(QObject* o, QEvent* e);

	void applyTransform();

	void updateRegistered(cv::Mat img);

signals:
	void log(QString msg, int lvl);

private:
    Ui::registrationDialog *ui;
	bool selectSource;
	bool selectInput;
	imgContainer*	currentSource;
	imgContainer*	currentInput;
	cv::Mat			overlayBase;
	cv::Mat			transformedInput;

	cv::Mat			currentTransform;
	overlayMethod	currentOverlayMethod;
	QPoint			affineCenter;
};

#endif // REGISTRATIONDIALOG_H
