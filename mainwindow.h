#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QFileInfo>
#include <QTextStream>
#include <QFileDialog>
#include <QDockWidget>
#include <QTextEdit>
#include <QPlainTextEdit>

#include <container.h>
#include <filterwidget.h>
#include <imageedit.h>
#include <imgsourceswidget.h>
#include <imgstatswidget.h>
#include <matview.h>
#include <mlwidget.h>
#include "savestreamdialog.h"

#include <sstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
    
private slots:
    void on_actionFiles_from_disk_triggered();

    void on_actionFolder_from_disk_triggered();

    void on_actionCamera_triggered();

    void on_actionCamera_Stream_triggered();

    void on_actionToggle_Source_View_triggered();

    void handleImgLabel(cv::Mat img, int label);

    void handleSourceChange(container* cont);

    void on_actionLabel_Images_triggered();

    void on_actionFilter_Images_triggered();

    void displayMat(cv::Mat features, bool isImg);

    void on_actionApply_filters_to_all_iamges_triggered();

    void viewMatrix(cv::Mat img);

    void on_actionExtract_Stats_for_all_images_triggered();

    void on_actionSave_Camera_Stream_triggered();
	void loadLabels(QString file);
signals:

    void consoleOutput(QString msg);
private:
    Ui::MainWindow *ui;
    QGridLayout*        layout;

    filterWidget*       filter;
    imgSourcesWidget*   sources;
    imgStatsWidget*     stats;
    imageEdit*          sourceImg;
    imageEdit*          filteredImg;
    mlWidget*           machineLearning;

    QDockWidget*        sourceDock;
    QDockWidget*        filterDock;
    QDockWidget*        statDock;
    QDockWidget*        mlDock;
    QPlainTextEdit*     console;

    container*          currentSourceImg;
    matView*            matViewer;
    QDialog*            matViewerDlg;
    QSpinBox*           scrollY;
    QSpinBox*           scrollX;
    QSpinBox*           scrollZ;

    saveStreamDialog* saveDialog;
};

#endif // MAINWINDOW_H
