#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include <qdialog.h>
#include <QTreeWidget>
#include <QGridLayout>
#include <qlineedit.h>
#include <QLabel>
#include <QKeyEvent>
#include <qfiledialog.h>
#include <FCVMLT.h>
//#include <imgFilter.h>
//#include <filtersettingwidget.h>
//#include <container.h>
namespace Ui {
class filterWidget;
}

class filterWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit filterWidget(QWidget *parent = 0, cv::Mat curImg_ = cv::Mat());
    ~filterWidget();
    QList<filterPtr> filters;
    bool eventFilter(QObject *obj, QEvent *ev);
public slots:
    void receiveImgChange(cv::Mat img_);

    void handleImgChange(container* cont);
	void handleImgChange(containerPtr cont);
    void handleParamChange(int idx);
    void handleFilterAccepted();
	void handleFilterSelect();
	void handleFilterParamsDestroyed();
	// Triggered on single click
    void handleFilterHistSelect(QTreeWidgetItem* item, int column);
	// Triggered on double click
    void handleFilterHistActivate(QTreeWidgetItem* item, int column);
	void handleFilterUpdated(containerPtr filteredImg_); 
	void handleFilterUpdated();
	void handleProcessingComplete(imgPtr img, filterContainer* srcFilter);
    void handleRectSelect(cv::Rect rectangle);
    void handleDragStart(QPoint pos);
    void handleDragMove(QPoint pos);
    void handleDragStop(QPoint pos);
	// Macro specific slots
	void handleSaveMacro();
	void handleLoadMacro();
	void handleClearMacro();

    void handleManualCrop();
    void handleDeleteFilter();
    void handleSaveImage();
    void saveImage(container* cont);
	//
    void updateFilterImages(int idx);
	void handleLog(QString line, int level);

	void handlePathSelect();

signals:
    void filterImgChanged(cv::Mat img);
	void filterImgChanged(containerPtr img);
    void consoleOutput(QString text);
    void saveImage(cv::Mat img, QString name);
	void log(QString line, int level);
private:
    Ui::filterWidget*	ui;
    QTreeWidget*		filterHistory;
    QGridLayout*		filterSelect;
    QGridLayout*		layout;
	filterPtr			currentFilter;
	filterPtr			currentDisplayFilter;
	macroPtr			currentMacro;
	imgPtr				currentBase;
	imgPtr				sourceImg;
    cv::Mat				curImg;
    cv::Mat				orgImg; // Only used for resetting everything when all the filters have been deleted
    cv::Mat				tmpOutput;
    QWidget*			filterSettings;
    bool				cropping;
    QPoint				cropStart;
    QPoint				cropEnd;
    int					currentFilterIdx;
	QList<filterMacro*> macros;
	QList<filterPtr>	savedFilters;
};


#endif // FILTERWIDGET_H
