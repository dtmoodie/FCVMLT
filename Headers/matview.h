#ifndef MATVIEW_H
#define MATVIEW_H

#include <QTableWidget>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSpinBox>
#include <QGridLayout>

class matView : public QTableWidget
{
    Q_OBJECT
public:
    explicit matView(QWidget *parent = 0, cv::Mat matrix = cv::Mat(), int rows_ = 20, int cols_ = 20);

signals:

public slots:
    void updateOffset(int x, int y, int z);
    void changeX(int x);
    void changeY(int y);
    void changeZ(int z);
private:
    cv::Mat m;
    int offsetX;
    int offsetY;
    int offsetZ;
    int cols;
    int rows;
    QList<QTableWidgetItem*> itemList;
};

#endif // MATVIEW_H
