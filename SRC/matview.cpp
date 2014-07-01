#include "matview.h"

matView::matView(QWidget *parent, cv::Mat matrix, int rows_, int cols_) :
    QTableWidget(rows_,cols_,parent),
    cols(cols_),
    rows(rows_),
    m(matrix)
{
    updateOffset(0,0,0);
}

void matView::updateOffset(int x, int y, int z)
{
    offsetX = x;
    offsetY = y;
    offsetZ = z;
    if(offsetZ >= m.channels()) offsetZ = m.channels() - 1;
    for(int i = 0; i < itemList.size(); ++i)
    {
        delete itemList[i];
    }
    itemList.clear();
    unsigned int posX = 0;
    unsigned int posY = 0;
    for(int i = offsetX; i < cols + offsetX && i < m.cols; ++i, ++posX)
    {
        for(int j = offsetY; j < rows + offsetY && j < m.rows; ++j, ++posY)
        {
            QTableWidgetItem* item;
            if(m.type() == CV_8U)
            {
                item = new QTableWidgetItem(QString::number(m.at<uchar>(j,i)));
            }
            if(m.type() == CV_8UC3)
            {
                item = new QTableWidgetItem(QString::number(m.at<uchar>(j,i,offsetZ)));
            }
            if(m.type() == CV_32F)
            {
                item = new QTableWidgetItem(QString::number(m.at<float>(j,i)));
            }
            if(m.type() == CV_32FC3)
            {
                cv::Vec3f tmp = m.at<cv::Vec3f>(j,i);
                item = new QTableWidgetItem(QString::number(tmp[offsetZ]));
            }
            setItem(posY,posX,item);
            itemList.append(item);
        }
        posY = 0;
    }
}

void matView::changeX(int x)
{
    updateOffset(x, offsetY, offsetZ);
}

void matView::changeY(int y)
{
    updateOffset(offsetX, y, offsetZ);
}

void matView::changeZ(int z)
{
    updateOffset(offsetX, offsetY,z);
}
