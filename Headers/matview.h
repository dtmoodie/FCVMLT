#ifndef MATVIEW_H
#define MATVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSpinBox>
#include <QGridLayout>
#include <FCVMLT.h>
namespace Ui {
class matView;
}

class matView : public QWidget
{
    Q_OBJECT

public:
explicit matView(QWidget *parent = 0, cv::Mat matrix = cv::Mat(), int rows_ = 20, int cols_ = 9);
    ~matView();
public slots:
    void updateDisplay(int x, int y);
    void changeX(int x);
    void changeY(int y);
	void changeMat(cv::Mat M_);
	void updateSlice();
	void updateLabel();
	void updatePlot();
	void onSliceClicked(bool val);
	void onGraphClicked(bool val);
	void onHistClicked(bool val);
	void onScatterClicked(bool val);
	void onLineClicked(bool val);
signals:
	void positionUpdated(cv::Range rowRange, cv::Range colRange);
	void ROIupdated(cv::Range rowRange, cv::Range colRange);

private:
    Ui::matView *ui;
    cv::Mat m;
	cv::Mat ROI;
    int offsetX;
    int offsetY;
    int cols;
    int rows;
    QList<QTableWidgetItem*>	itemList;
	bool b_histPlot;
	bool b_scatterPlot;
	bool b_linePlot;
	std::vector<QColor> penColors;
	colorScale RED;// (50, 255 / 25, true);
	colorScale GREEN;// (25 / 3, 255 / 25, true);
	colorScale BLUE;// (0, 255 / 25, true);
	int prevMinRowSlice;
	int prevMaxRowSlice;
	int prevMinColSlice;
	int prevMaxColSlice;

};

#endif // MATVIEW_H
