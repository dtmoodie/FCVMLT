#ifndef IMGSOURCESWIDGET_H
#define IMGSOURCESWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QGridLayout>
#include <container.h>
#include <QKeyEvent>
#include <QTreeWidgetItem>
namespace Ui {
class imgSourcesWidget;
}

class imgSourcesWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit imgSourcesWidget(QWidget *parent = 0);
    ~imgSourcesWidget();
    QTreeWidget* sourceList;
    QLabel* sourceView;
	container* curCont;
public slots:
	
    void handleItemChange(QTreeWidgetItem* current,QTreeWidgetItem* previous);
    void handleItemActivated(QTreeWidgetItem* item, int col);
    void handleSaveImage(cv::Mat img, QString name);
    void handleSaveFeatures(cv::Mat features, QString name);
	void handleSaveAction();
	void handleSaveToDisk(container* item);
    bool eventFilter(QObject *obj, QEvent *ev);

    void cameraUpdated();

signals:
    void sourcePreview(cv::Mat img);
    void sourcePreview(container* cont);
    void sourceChange(cv::Mat img);
    void sourceChange(container* cont);

private:
    Ui::imgSourcesWidget *ui;
	QAction * _sourceAction;
    

};

#endif // IMGSOURCESWIDGET_H
