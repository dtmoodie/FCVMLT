#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QDialog>
#include <qtimer.h>
#include "FCVMLT.h"
#include "imageedit.h"
#include "qpoint.h"
#include <Qt>

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
	enum registrationTechnique
	{
		Manual = 0,
		Line,
		Points
	};



public:
	explicit registrationDialog(QWidget *parent = 0, imgSourcesWidget* sourceList = 0);
    ~registrationDialog();

public slots:
	void handleImageSelect(containerPtr cont);

private slots:
    void on_btnSelectSource_clicked(bool val);

	void on_btnSelectInput_clicked(bool val);

    void on_btnSaveRegistration_clicked();

    void on_btnCancel_clicked();

	void on_technique_currentIndexChanged(int index);

    void on_displayMethod_currentIndexChanged(int index);

    void on_boarderMethod_currentIndexChanged(int index);

	void checkKeyRelease();

	void handleKeyPress(int key);

	void handleDragStart(QPoint pt);

	void handleDrag(QPoint pt);
	
	void handleDragEnd(QPoint pt);

	void handleImageChanged(imgPtr img);

	void handleLineSelect(cv::Point start, cv::Point end);

	bool eventFilter(QObject* o, QEvent* e);

	void applyTransform();

	void updateRegistered(cv::Mat img);

signals:
	void log(QString msg, int lvl);

private:
    Ui::registrationDialog* ui;
	bool					selectSource;
	bool					selectInput;
	imgPtr					currentSource;
	imgPtr					currentInput;
	cv::Mat					overlayBase;
	cv::Mat					transformedInput;

	cv::Mat					currentTransform;
	overlayMethod			currentOverlayMethod;
	registrationTechnique	currentRegistrationTechnique;
	QPoint					affineCenter;
	std::vector<cv::Mat>	_transformHistory;
	// Used to determine if the key has been released
	QTimer*					_keyPressTimer;

	bool					_keyPressed;
	bool					_keyPreviouslyPressed;
	cv::Vec2f				_sourceVec;
	cv::Vec2f				_destVec;
	cv::Vec2f				_sourcePt;
	cv::Vec2f				_destPt;
	imgSourcesWidget*		_sourceList;
};

#endif // REGISTRATIONDIALOG_H
