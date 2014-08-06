#ifndef IMGSOURCESWIDGET_H
#define IMGSOURCESWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QGridLayout>
//#include <container.h>
#include <FCVMLT.h>
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
	QList<imgPtr> sources;
public slots:
	
    void handleItemChange(QTreeWidgetItem* current,QTreeWidgetItem* previous);
    void handleItemActivated(QTreeWidgetItem* item, int col);
    void handleSaveImage(cv::Mat img, QString name);
    void handleSaveFeatures(cv::Mat features, QString name);
	void handleSaveAction();
	void handleSaveToDisk(container* item);
    bool eventFilter(QObject *obj, QEvent *ev);

    void cameraUpdated();
	void cameraUpdated(cv::Mat img);
	// Searches sources based on baseName and source children based on name
	containerPtr getContainer(QString name);
	containerPtr getContainer(container* ptr);
	void handleViewMatrix();

signals:
	// Emitted on single click
    void sourcePreview(cv::Mat img);
    void sourcePreview(container* cont);
	void sourcePreview(containerPtr cont);
	// Emitted on double click
    void sourceChange(cv::Mat img);
    void sourceChange(container* cont);
	void sourceChange(containerPtr cont);

	void viewMatrix(cv::Mat M);

private:
    Ui::imgSourcesWidget *ui;
	QAction *	_sourceSaveAction;
	QAction*	_matrixViewAction;

};

#endif // IMGSOURCESWIDGET_H
