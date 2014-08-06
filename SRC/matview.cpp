#include "matview.h"
#include "ui_matview.h"

matView::matView(QWidget *parent, cv::Mat matrix, int rows_, int cols_) :
    //itemView(new QTableWidget(rows_, cols_, this)),
    QWidget(parent),
    cols(cols_),
    rows(rows_),
	ui(new Ui::matView()),
	RED(50, 255 / 25, true),
	GREEN(25 / 3, 255 / 25, true),
	BLUE(0, 255 / 25, true)
{
	b_histPlot = false;
	b_linePlot = false;
	b_scatterPlot = false;
	offsetX = 0;
	offsetY = 0;
	ui->setupUi(this);
	ui->minRowSlice->setValue(0);
	ui->maxRowSlice->setMaximum(matrix.rows);
	ui->maxRowSlice->setValue(matrix.rows);
	ui->minColSlice->setValue(0);
	ui->maxColSlice->setMaximum(matrix.cols);
	ui->maxColSlice->setValue(matrix.cols);
	ui->minChanSlice->setValue(0);
	ui->maxChanSlice->setValue(matrix.channels());
	ui->maxChanSlice->setValue(1);
	changeMat(matrix);
	updateLabel();
	updateDisplay(0, 0);
	ui->maxRowSlice->hide();
	ui->minRowSlice->hide();
	ui->maxColSlice->hide();
	ui->minColSlice->hide();
	ui->maxChanSlice->hide();
	ui->minChanSlice->hide();
	ui->label_10->hide();
	ui->label_9->hide();
	ui->label_8->hide();
	ui->label_7->hide();
	ui->label_2->hide();
	ui->label_16->hide();
	ui->histButton->hide();
	ui->scatterButton->hide();
	ui->lineButton->hide();
	ui->plot->hide();
	ui->paramLbl1->hide();
	ui->paramLbl2->hide();
	ui->paramLbl3->hide();
	ui->paramLbl4->hide();
	ui->param1->hide();
	ui->param2->hide();
	ui->param3->hide();
	ui->param4->hide();
	ui->slicingButton->setCheckable(true);
	ui->histButton->setCheckable(true);
	ui->plotButton->setCheckable(true);
	

	connect(ui->slicingButton, SIGNAL(clicked(bool)), this, SLOT(onSliceClicked(bool)));
	connect(ui->plotButton, SIGNAL(clicked(bool)), this, SLOT(onGraphClicked(bool)));

	connect(ui->histButton, SIGNAL(clicked(bool)), this, SLOT(onHistClicked(bool)));
	connect(ui->lineButton, SIGNAL(clicked(bool)), this, SLOT(onLineClicked(bool)));

	connect(ui->horizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeX(int)));
	connect(ui->verticalSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeY(int)));

	connect(ui->minColSlice, SIGNAL(editingFinished()), this, SLOT(updateSlice()));
	connect(ui->maxColSlice, SIGNAL(editingFinished()), this, SLOT(updateSlice()));
	connect(ui->minRowSlice, SIGNAL(editingFinished()), this, SLOT(updateSlice()));
	connect(ui->maxRowSlice, SIGNAL(editingFinished()), this, SLOT(updateSlice()));

	
	ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));


}
matView::~matView()
{
	delete ui;
}
void
matView::changeX(int x)
{
	updateDisplay(x, offsetY);
}
void
matView::changeY(int y)
{
	updateDisplay(offsetX, y);
}
void
matView::changeMat(cv::Mat M_)
{
	if (M_.size() != m.size())
	{
		ui->minRowSlice->setValue(0);
		ui->maxRowSlice->setMaximum(M_.rows);
		ui->maxRowSlice->setValue(M_.rows);
		ui->minColSlice->setValue(0);
		ui->maxColSlice->setMaximum(M_.cols);
		ui->maxColSlice->setValue(M_.cols);
		ui->minChanSlice->setValue(0);
		ui->maxChanSlice->setValue(M_.channels());
		ui->maxChanSlice->setValue(1);
		prevMinRowSlice = 0;
		prevMaxRowSlice = M_.rows-1;
		prevMinColSlice = 0;
		prevMaxColSlice = M_.cols-1;
	}
	updateLabel();
	m = M_;
	updateSlice();
	//updatePlot();
}
void
matView::updateLabel()
{
	double minVal, maxVal;
	cv::minMaxIdx(ROI, &minVal, &maxVal);
	ui->rowLbl->setText("Rows: " + QString::number(ROI.rows));
	ui->colLbl->setText("Cols: " + QString::number(ROI.cols));
	ui->chLbl->setText("Channels: " + QString::number(ROI.channels()));
	ui->maxLbl->setText("Max Val: " + QString::number(maxVal));
	ui->minLbl->setText("Min Val: " + QString::number(minVal));
	ui->horizontalSlider->setMaximum(ROI.cols);
	ui->verticalSlider->setMaximum(ROI.rows);
}
void 
matView::updateDisplay(int x, int y)
{
    offsetX = x;
    offsetY = y;
    for(int i = 0; i < itemList.size(); ++i)
    {
        delete itemList[i];
    }
    itemList.clear();
    unsigned int posX = 0;
    unsigned int posY = 0;
    for(int i = offsetX; i < cols + offsetX && i < ROI.cols; ++i, ++posX)
    {
		for (int j = offsetY; j < rows + offsetY && j < ROI.rows; ++j, ++posY)
        {
            QTableWidgetItem* item;
			if (ROI.type() == CV_8U)
            {
				item = new QTableWidgetItem(QString::number(ROI.at<uchar>(j, i)));
            }
			if (ROI.type() == CV_8UC3)
            {
				cv::Vec3b tmp = ROI.at<cv::Vec3b>(j, i);
                item = new QTableWidgetItem("[" + QString::number(tmp[0]) + "," + QString::number(tmp[1]) + "," + QString::number(tmp[2]) + "]");
            }
			if (ROI.type() == CV_32F)
            {
				item = new QTableWidgetItem(QString::number(ROI.at<float>(j, i)));
            }
			if (ROI.type() == CV_32FC3)
            {
				cv::Vec3f tmp = ROI.at<cv::Vec3f>(j, i);
                item = new QTableWidgetItem("[" + QString::number(tmp[0]) + "," + QString::number(tmp[1]) + "," + QString::number(tmp[2]) + "]");
            }
            ui->itemView->setItem(posY,posX,item);
            itemList.append(item);
        }
        posY = 0;
    }
	emit positionUpdated(cv::Range(y + ui->minRowSlice->value(), y + ui->minRowSlice->value() + ui->itemView->rowCount()), 
						 cv::Range(x + ui->minColSlice->value(), x + ui->minColSlice->value() + ui->itemView->columnCount()));
}
void 
matView::updateSlice()
{
	// Triggered after a slice parameter is updated
	if (m.empty()) return;
	// If the values are all the same as last time, skip this.  Prevents accessive updates due to editingFinished 
	// accidentally firing too often
	if (prevMinRowSlice == ui->minRowSlice->value() && prevMaxRowSlice == ui->maxRowSlice->value() &&
		prevMinColSlice == ui->minColSlice->value() && prevMaxColSlice == ui->maxColSlice->value()) return;
	prevMinRowSlice = ui->minRowSlice->value();
	prevMaxRowSlice = ui->maxRowSlice->value();
	prevMinColSlice = ui->minColSlice->value();
	prevMaxColSlice = ui->maxColSlice->value();
	ROI = m(cv::Range(ui->minRowSlice->value(), ui->maxRowSlice->value()), 
			cv::Range(ui->minColSlice->value(), ui->maxColSlice->value()));
	updateDisplay(offsetX, offsetY);
	updateLabel();
	updatePlot();
}
void
matView::updatePlot()
{
	if (b_histPlot)
	{
		// Create a histogram plot of ROI
		std::vector<cv::Mat> planes;
		cv::split(ROI, planes);
		ui->plot->clearGraphs();
		penColors.resize(ROI.channels());
		double location = 0;
		double step = 100 / (double)penColors.size();
		for (int i = 0; i < penColors.size(); ++i, location += step)
		{
			penColors[i] = QColor(RED(location), GREEN(location), BLUE(location));
		}
		int count = 0;
		float maxVal = 0;
		for (int i = 0; i < ROI.channels(); ++i)
		{
			if (i >= ui->minChanSlice->value() && i < ui->maxChanSlice->value())
			{
				cv::Mat hist;
				
				cv::vector<cv::Mat> input;
				std::vector<int> channels;
				std::vector<int> histSize;
				std::vector<float> ranges;
				channels.push_back(0);
				histSize.push_back(ui->param3->value());
				ranges.push_back(ui->param1->value());
				ranges.push_back(ui->param2->value());
				input.push_back(planes[i]);
				try
				{
					cv::calcHist(input, channels, cv::Mat(), hist, histSize, ranges, false);
				}
				catch (cv::Exception &e)
				{
					return;
				}
				QVector<double> x(hist.rows), y(hist.rows);
				float xPos = ui->param1->value();
				float step = (ui->param2->value() - ui->param1->value()) / ((float)ui->param3->value());
				for (int i = 0; i < hist.rows; ++i, xPos += step)
				{
					x[i] = xPos;
					y[i] = hist.at<float>(i, 0);
					if (y[i] > maxVal) maxVal = y[i];
				}
				ui->plot->addGraph();
				ui->plot->graph(count)->setData(x, y);
				ui->plot->graph(count)->setPen(QPen(penColors[count]));
				ui->plot->xAxis->setLabel("Intensity Value");
				ui->plot->yAxis->setLabel("Count");
				ui->plot->xAxis->setRange(ui->param1->value(), ui->param2->value());
				ui->plot->yAxis->setRange(0, maxVal);
				++count;
			}		
		}
		ui->plot->replot();
		return;
	}
	if (b_scatterPlot)
	{
		// Create a scatter plot of all data in ROI
		
		return;
	}
	if (b_linePlot)
	{
		// Create a profile of roi accross a row or column
		QVector<QVector<double>> x, y;
		QString xAxis;
		QString yAxis = "Value";;
		if (ui->param1->value() == 0)
		{
			// Plot for each row of this ROI
			xAxis = "Row index";
			
			x.resize(std::min(ROI.cols,10));
			y.resize(std::min(ROI.cols,10));
			for (int i = 0; i < ROI.cols && i < 10 /*Hard coded max number of plots*/; ++i)
			{
				x[i].resize(ROI.rows);
				y[i].resize(ROI.rows);
				for (int j = 0; j < ROI.rows; ++j)
				{
					float val;
					if (ROI.type() == CV_32F)
					{
						val = ROI.at<float>(j, i);
					}else
					{
						if (ROI.type() == CV_8U)
						{
							val = (float)ROI.at<uchar>(j, i);
						}else
						{
							if (ROI.type() == CV_8UC3)
							{
								val = (float)ROI.at<cv::Vec3b>(j, i)[ui->minChanSlice->value()];
							}
						}
					}
					y[i][j] = val;
					x[i][j] = j;
				}
			}
			
		}
		else
		{
			if (ui->param1->value() == 1)
			{
				// Plot each column of this ROI

			}
			else
			{
				if (ui->param1->value() == 2)
				{

				}
			}

		}
		ui->plot->clearGraphs();
		double location = 0;
		double step = 100 / (double)x.size();
		for (int i = 0; i < x.size(); ++i, location += step)
		{
			ui->plot->addGraph();
			ui->plot->graph(i)->setData(x[i], y[i]);
			ui->plot->graph(i)->setPen(QPen(QColor(RED(location), GREEN(location), BLUE(location))));
			ui->plot->xAxis->setLabel(xAxis);
			ui->plot->yAxis->setLabel(yAxis);
		}
		ui->plot->replot();
		return;
	}
}
void 
matView::onSliceClicked(bool val)
{
	// Open up the slicing window to allow changes to the ROI of the image used
	//ui->slicingButton->clicked(val);
	if (val)
	{
		ui->maxRowSlice->show();
		ui->minRowSlice->show();
		ui->maxColSlice->show();
		ui->minColSlice->show();
		ui->maxChanSlice->show();
		ui->minChanSlice->show();
		ui->label_10->show();
		ui->label_9->show();
		ui->label_8->show();
		ui->label_7->show();
		ui->label_2->show();
		ui->label_16->show();
	}
	else
	{
		ui->maxRowSlice->hide();
		ui->minRowSlice->hide();
		ui->maxColSlice->hide();
		ui->minColSlice->hide();
		ui->maxChanSlice->hide();
		ui->minChanSlice->hide();
		ui->label_10->hide();
		ui->label_9->hide();
		ui->label_8->hide();
		ui->label_7->hide();
		ui->label_2->hide();
		ui->label_16->hide();
	}
}
void 
matView::onGraphClicked(bool val)
{
	// Open up the graphing window to allow changing of graphing properties
	if (val)
	{
		ui->plot->show();
		ui->histButton->show();
		ui->scatterButton->show();
		ui->lineButton->show();
		ui->paramLbl1->setText("Hist Min Value: ");
		ui->paramLbl2->setText("Hist Max Value: ");
		ui->paramLbl3->setText("Number of bins: ");
		ui->paramLbl1->show();
		ui->paramLbl2->show();
		ui->paramLbl3->show();
		ui->param1->show();
		ui->param2->show();
		ui->param3->show();
		ui->param1->setValue(0);
		ui->param2->setValue(256);
		ui->param3->setValue(256);
		b_histPlot = true;
	}
	else
	{
		ui->plot->hide();
		ui->histButton->hide();
		ui->scatterButton->hide();
		ui->lineButton->hide();
	}
}
void
matView::onHistClicked(bool val)
{
	b_histPlot = val;
	b_linePlot = !val;
	b_scatterPlot = !val;
	if (val)
	{
		ui->scatterButton->setChecked(false);
		ui->lineButton->setChecked(false);
		ui->paramLbl1->setText("Hist Min Value: ");
		ui->paramLbl2->setText("Hist Max Value: ");
		ui->paramLbl3->setText("Number of bins: ");
		ui->paramLbl1->show();
		ui->paramLbl2->show();
		ui->paramLbl3->show();
		ui->param1->show();
		ui->param2->show();
		ui->param3->show();
		updatePlot();
	}else
	{
		ui->paramLbl1->hide();
		ui->paramLbl2->hide();
		ui->paramLbl3->hide();
		ui->param1->hide();
		ui->param2->hide();
		ui->param3->hide();
	}
}
void
matView::onScatterClicked(bool val)
{
	b_histPlot = !val;
	b_linePlot = !val;
	b_scatterPlot = val;
	if (val)
	{

		
	}else
	{
		
	}
}
void
matView::onLineClicked(bool val)
{
	b_histPlot = !val;
	b_linePlot = val;
	b_scatterPlot = !val;
	if (val)
	{
		ui->scatterButton->setChecked(false);
		ui->histButton->setChecked(false);
		ui->histButton->setChecked(false);
		ui->lineButton->setChecked(false);
		ui->paramLbl1->setText("Select dimension");
		ui->paramLbl2->setText("Set step");
		ui->paramLbl1->show();
		ui->paramLbl2->show();
		ui->param1->show();
		ui->param2->show();
		updatePlot();
	}else
	{

	}
}