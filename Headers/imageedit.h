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
#include <qlineedit.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
//#include <container.h>
#include <FCVMLT.h>
#include <iostream>
#define COLORMAP_RESOLUTION 50000
//#define OPENGL
#define MULTI_THREAD
#ifdef OPENGL
#include <QGLWidget>
#endif // OPENGL

namespace Ui {
    class imageEdit;
}
// Defines how a specific color will scale with values
struct colorScale
{
	colorScale(double start_ = 0, double slope_ = 1, bool symmetric_ = false)
	{
		start = start_;
		slope = slope_;
		symmetric = symmetric_;
		flipped = false;
		inverted = false;
	}
	// Defines where this color starts to take effect, between zero and 1000
	double start;
	// Defines the slope of increase / decrease for this color between 1 and 255
	double slope;
	// Defines if the slope decreases after it peaks
	bool	symmetric;
	// Defines if this color starts high then goes low
	bool	inverted;
	uchar operator ()(double location)
	{
		return getValue(location);
	}
	uchar getValue(double location_)
	{
		double value = 0;
		if (location_ > start)
		{
			value = (location_ - start)*slope;
		}else
		{
			value = 0;
		}
		if (value > 255)
		{
			if (symmetric) value = 512 - value;
			else value = 255;
		}
		if (value < 0) value = 0;
		if (inverted) value = 255 - value;
		return (uchar)value;
	}
	bool flipped;
};
#ifdef OPENGL
class GLimageEdit : public QGLWidget
{
	Q_OBJECT
public:
	explicit GLimageEdit(QWidget *parent = 0);
signals:
	void    imageSizeChanged(int outW, int outH);		/// Used to resize the image outside the widget
	void	log(QString text, int level);
public slots:
	bool    showImage(cv::Mat image);					/// Used to set the image to be viewed
	bool	eventFilter(QObject* o, QEvent* e);
protected:
	void 	initializeGL();								/// OpenGL initialization
	void 	paintGL();									/// OpenGL Rendering
	void 	resizeGL(int width, int height);			/// Widget Resize Event

	void        updateScene();
	void        renderImage();

private:
	bool        mSceneChanged;          /// Indicates when OpenGL view is to be redrawn

	QImage      mRenderQtImg;           /// Qt image to be rendered
	cv::Mat     mOrigImage;             /// original OpenCV image to be shown

	QColor      mBgColor;				/// Background color

	int         mOutH;                  /// Resized Image height
	int         mOutW;                  /// Resized Image width
	float       mImgRatio;				/// height/width ratio

	int         mPosX;                  /// Top left X position to render image in the center of widget
	int         mPosY;                  /// Top left Y position to render image in the center of widget

	bool		labeling;

	QPoint		pressStartPos;
	float		transZ;
};
#endif // OPENGL

class imageEdit : public QWidget
{
    Q_OBJECT
    
public:
    explicit imageEdit(QWidget *parent = 0);
    ~imageEdit();
    bool eventFilter(QObject *obj, QEvent *ev);
    cv::Rect calcRect(QPoint start_, QPoint end_);

	QComboBox* classSelect;
	std::vector<std::pair<QString, int>> classes;

public slots:
	void addClasses(QStringList classes);
	void addClass(std::pair<QString, int> class_);
	
	void changeImg(cv::Mat img);
	void changeImg(cv::Mat img, bool update);
    void changeImg(container* cont);

	void updateROI(cv::Rect ROI, bool source = true);
	void adjustROI(cv::Mat img);
	void drawImg(cv::Mat img);
    void labelImg();
    void stopLabelImg();
    void displaySelection();
    void handleAcceptLabel();
    void handleClearLabel();
    int  handleAddClass();
	void handleROIUpdate(cv::Rect ROI);
    void toggleColormap(bool value);
	// Used for merging images between two views. Such as displaying a canny edge detect on the original image
	void receivePairedImage(cv::Mat img);

	void buildLUT(int maxVal);
	void buildLUThelper(int threadNum, int maxVal);

	void handleLog(QString text, int level);
signals:
    void dragStart(QPoint pos);
    void dragPos(QPoint pos);
    void dragEnd(QPoint pos);

    void rectSelect(cv::Rect);
    void log(QString text, int level);
    void label(container* lbl);
    void displayMat(cv::Mat img);
	void newROI(cv::Rect ROI);
	void sendPairedImage(cv::Mat img);
	void sendPairedImage(imgContainer* img);
	void keyPress(int key);

private:
    Ui::imageEdit *ui;
	// Original un-altered image
    cv::Mat orgImg;
	// same as orgImg unless an overlay has been applied
	cv::Mat dispImg;
	// Used for region of interest and panning
    cv::Mat roiImg;
#ifndef OPENGL
	// Object for displaying the image
    QLabel* imgDisp;
#else
	GLimageEdit* imgDisp;
#endif
	
    QPixmap pixmap;
    QPixmap scaled;
    bool dragging;
    QPoint start;
    QPoint end;
    QPoint lastPos;
    bool blockUpdate;
    float zoomFactor;
    cv::Rect currentROI;
	bool useColormap;
	bool merge;
	//cv::Vec3b mergeColor;
    // These are used when labeling images
	imgContainer* curCont;
    QPlainTextEdit* className;
    QPushButton* classAdd;
    QPushButton* acceptLabel;
    QPushButton* clearLabel;
    bool labeling;
    cv::Point firstClickInOriginal;
    std::vector<cv::Point> polygonInOriginal;
    QStringList labelNames;
	QLabel* aspectRatioLabel;
	QSpinBox* labelWidth;
	QSpinBox* labelHeight;
    QGridLayout* layout;


	// Colormapping values
	double minVal;
	double maxVal;
	std::vector<cv::Vec3b> LUT;
	bool	updateLUT;
	QLineEdit*			lineEdit;
	int numThreads;
};
#endif // IMAGEEDIT_H
