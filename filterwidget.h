#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QGridLayout>
#include <QLabel>
#include <QKeyEvent>

#include <imgFilter.h>
#include <filtersettingwidget.h>
#include <container.h>
namespace Ui {
class filterWidget;
}

class filterWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit filterWidget(QWidget *parent = 0, cv::Mat curImg_ = cv::Mat());
    ~filterWidget();
    QList<imgFilter*> filters;
    bool eventFilter(QObject *obj, QEvent *event);
public slots:
    void receiveImgChange(cv::Mat img_);

    void handleImgChange(container* cont);
    void handleParamChange(int idx);
    void handleFilterAccepted(int idx);
    void handleFilterParamsDestroyed();
    void handleFilterHistSelect(QTreeWidgetItem* item, int column);
    void handleFilterHistActivate(QTreeWidgetItem* item, int column);
    void handleRectSelect(cv::Rect rectangle);
    void handleDragStart(QPoint pos);
    void handleDragMove(QPoint pos);
    void handleDragStop(QPoint pos);
    void handleManualCrop();
    void handleDeleteFilter();
    void handleSaveImage();
    void saveImage(container* cont);
    void updateFilterImages(int idx);

signals:
    void filterImgChanged(cv::Mat img);
    void consoleOutput(QString text);
    void saveImage(cv::Mat img, QString name);
private:
    Ui::filterWidget *ui;
    QTreeWidget* filterHistory;
    QGridLayout* filterSelect;
    QGridLayout* layout;
    container* currentFilter;
    container* currentSource;
    cv::Mat curImg;
    cv::Mat orgImg; // Only used for resetting everything when all the filters have been deleted
    cv::Mat tmpOutput;
    filterSettingWidget* filterSettings;
    bool cropping;
    QPoint cropStart;
    QPoint cropEnd;
    int currentFilterIdx;

};


#endif // FILTERWIDGET_H
