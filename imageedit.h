#ifndef IMAGEEDIT_H
#define IMAGEEDIT_H
// This widget allows simple interaction with images.
// Resizing, panning, zooming, clicking points, dragging, lasso'ing, etc.

#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QPixmap>
#include <QSpinBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QComboBox>
#include <QDir>
#include <QTextStream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <container.h>
#include <iostream>
namespace Ui {
    class imageEdit;
}

class imageEdit : public QWidget
{
    Q_OBJECT
    
public:
    explicit imageEdit(QWidget *parent = 0);
    ~imageEdit();
    bool eventFilter(QObject *obj, QEvent *ev);
    cv::Rect calcRect(QPoint start_, QPoint end_);

    QComboBox* classSelect;

public slots:
	void addClasses(QStringList classes);
    void changeImg(cv::Mat img, bool update = true);
    void changeImg(container* cont);
    void updateROI(cv::Rect ROI);
    void labelImg();
    void stopLabelImg();
    void displaySelection();
    void handleAcceptLabel();
    void handleClearLabel();
    void handleAddClass();

signals:
    void dragStart(QPoint pos);
    void dragPos(QPoint pos);
    void dragEnd(QPoint pos);
    void rectSelect(cv::Rect);
    void consoleOutput(QString text);
    void label(cv::Mat mask, int labelNum);
    void displayMat(cv::Mat img);

private:
    Ui::imageEdit *ui;
    cv::Mat orgImg;
    cv::Mat roiImg;
    QLabel* imgDisp;
    QPixmap pixmap;
    QPixmap scaled;
    bool dragging;
    QPoint start;
    QPoint end;
    QPoint lastPos;
    bool blockUpdate;
    float zoomFactor;
    cv::Rect currentROI;
    // These are used when labeling images

    container* curCont;
    QPlainTextEdit* className;
    QPushButton* classAdd;
    QPushButton* acceptLabel;
    QPushButton* clearLabel;
    bool labeling;
    cv::Point firstClickInOriginal;
    std::vector<cv::Point> polygonInOriginal;
    QStringList labelNames;
    QGridLayout* layout;
};

#endif // IMAGEEDIT_H
