#ifndef IMGSTATSWIDGET_H
#define IMGSTATSWIDGET_H

#include <QWidget>
//#include <imgFilter.h>
//#include <container.h>
#include <FCVMLT.h>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTreeWidget>
#include <QGridLayout>
#include <QList>
#include <qdialog.h>
//#include <statsettingwidget.h>
#include <QCheckBox>
#include <imgsourceswidget.h>
namespace Ui {
class imgStatsWidget;
}

class imgStatsWidget : public QWidget
{
    Q_OBJECT

public:
	explicit imgStatsWidget(QWidget *parent = 0, imgSourcesWidget* sourceList_ = 0);
    ~imgStatsWidget();
    //QTreeWidget* statList;
    QList<featureWindowPtr>				windows;
    QList<featureExtractorPtr>			extractor;
    bool eventFilter(QObject *obj, QEvent *ev);

public slots:
    void handleExtractorChange(int idx);
    void handleWindowChange(int idx);
    void handleParamChange();
	void handleImgChange(containerPtr cont);
    void paramChange(QString val);
    void paramChange(bool val);
	// Triggered on button press
    void handleSaveStat();
	void handleSaveStat(QString name);
    void handleCalcStat();
	void handleViewStat();
	// Used to cache a stat after calculation into the database
	void saveStat(QString name, statPtr parent);

	void handleSrcChange(containerPtr I);
	void handleInputChange(containerPtr I);
	void handleMaskChange(containerPtr I);

	void handleFeatures(featurePtr f);
	void handleDisplayImg(cv::Mat img);
	void handleLog(QString line, int level);
	void findDispImg();
signals:
    void saveFeatures(cv::Mat features, QString name);
    void extractedFeatures(cv::Mat img, bool isImg);
    void consoleOutput(QString msg);
	void log(QString line, int level);
	void viewFeatures(cv::Mat f);
private:
    Ui::imgStatsWidget*			ui;
	QWidget*					statSettings;
	QWidget*					extractorSettings;
    //featureExtractorPtr			curExtractor;
	//featureWindowPtr			curWindow;
	featurePtr				curFeatures;
	imgPtr						curInput;
	// Name from current input image
	QString						curInputName;
	imgPtr						curSource;
	// BaseName from current source image
	QString						curSourceName;
	imgPtr						curMask;
	cv::Mat						mask;
	// Name from current mask image
	QString						curMaskName;
    QString						nameOfSourceImg;
    cv::Mat						features;
    bool						isImg;
	cv::Mat						displayImg;
	imgSourcesWidget*			sourceList;
	QList<statPtr>				savedStats;
	bool						applied;
	// True if this stat is saved to the image database
	bool						cached;
	// True if this stat is saved to disk
	bool						saved;
	QString						curStatName;
	statPtr						curStat;
};

#endif // IMGSTATSWIDGET_H
