#ifndef IMGSTATSWIDGET_H
#define IMGSTATSWIDGET_H

#include <QWidget>
#include <imgFilter.h>
#include <container.h>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTreeWidget>
#include <QGridLayout>
#include <QList>
#include <statsettingwidget.h>
#include <QCheckBox>
namespace Ui {
class imgStatsWidget;
}

class imgStatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit imgStatsWidget(QWidget *parent = 0);
    ~imgStatsWidget();
    //QTreeWidget* statList;
    QList<featureWindow*> stats;
    QList<featureExtractor*> extractor;
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void handleExtractorChange(int idx);
    void handleWindowChange(int idx);
    void handleParamChange();
    void handleImgChange(container* cont);
    void paramChange(QString val);
    void paramChange(bool val);
    void handleSaveStat();
    void handleCalcStat();

signals:
    void saveFeatures(cv::Mat features, QString name);
    void extractedFeatures(cv::Mat img, bool isImg);
    void consoleOutput(QString msg);
private:
    QComboBox * extractorComboBox;
    QComboBox * windowComboBox;
    QTreeWidget* statHist;
    Ui::imgStatsWidget *ui;
    QGridLayout* layout;
    QGridLayout* statSelect;
    statSettingWidget* statSettings;
    featureExtractor* curExtractor;
    featureWindow* curWindow;
    container* curCont;
    QString nameOfSourceImg;
    cv::Mat features;
    QWidget* extractorSettings;
    QGridLayout* extractorLayout;
    bool isImg;

};

#endif // IMGSTATSWIDGET_H
