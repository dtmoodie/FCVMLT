#ifndef IMAGEEDIT_H
#define IMAGEEDIT_H
// This widget allows simple interaction with images.
// Resizing, panning, zooming, clicking points, dragging, lasso'ing, etc.
#define COLORMAP_RESOLUTION 50000
#define MULTI_THREAD
//#define OPENGL

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
#include <qmimedata.h>
#include <QMenu>
#include "imgsourceswidget.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#if CV_VERSION_MAJOR == 2
#include <opencv2/contrib/contrib.hpp>
#endif

//#include <container.h>
#include <FCVMLT.h>
#include <iostream>

#ifdef OPENGL
#include <QGLWidget>
#endif // OPENGL

namespace Ui {
    class imageEdit;
}

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

class imgDropLabel :public QLabel
{
	Q_OBJECT
public:
	explicit imgDropLabel(QWidget* parent = 0, imgSourcesWidget* sourceList_ = 0);
	void setSourceList(imgSourcesWidget* sourceList_);
private:

	imgSourcesWidget* sourceList;
signals:
	// Can be a label,mask,etc
	void imgReceived(containerPtr img);
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
};
class imageEdit : public QWidget
{
    Q_OBJECT
    
public:
	explicit imageEdit(QWidget *parent = 0, imgSourcesWidget* sourceList_ = 0);
    ~imageEdit();
    bool eventFilter(QObject *obj, QEvent *ev);
    cv::Rect calcRect(QPoint start_, QPoint end_);

	QComboBox* classSelect;
    std::vector<std::pair<QString, int> > classes;
	static void colorMapHelper(cv::Mat src, cv::Mat dest, int minVal, std::vector<cv::Vec3b>& LUT, int threadNum, int numThreads);
	imgSourcesWidget* sourceList;
public slots:
	void addClasses(QStringList classes);
	void addClass(std::pair<QString, int> class_);
	
	void changeImg(cv::Mat img);
	void changeImg(cv::Mat img, bool update);
    void changeImg(container* cont);
	void changeImg(containerPtr cont);
	cv::Mat colorMapImg(cv::Mat img);

	void updateROI(cv::Rect ROI, bool source = true);
	void adjustROI(cv::Mat img);

	// Enables or disables editing of images in this window
	void handleImageEditing(bool enable);

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
	void receivePairedImage(cv::Mat img, cv::Rect ROI);
	void mirrorDraw(cv::Rect rect, cv::Mat tool);
	void mirrorErase(cv::Rect rect);
	void buildLUT(int maxVal);

	void handleLog(QString text, int level);

	void handleDrawToggled(bool val);
	void handleEraseToggled(bool val);
	void handleSizeChanged(int val);
	void handleLineToggled(bool val);
	void handleSaveDrawing();

	void onRightClick(const QPoint& pt);

signals:
    void dragStart(QPoint pos);
    void dragPos(QPoint pos);
    void dragEnd(QPoint pos);
	void lineDrawn(cv::Point start, cv::Point end);
    void rectSelect(cv::Rect);
    void log(QString text, int level);
    void label(container* lbl);
    void displayMat(cv::Mat img);
	void newROI(cv::Rect ROI);
	void sendPairedImage(cv::Mat img);
	void sendPairedImage(imgContainer* img);
	void sendPairedImage(cv::Mat img, cv::Rect ROI);
	void keyPress(int key);
	void drawPt(cv::Rect rect, cv::Mat tool);
	void erasePt(cv::Rect rect);
	void imageChanged(imgPtr img);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
    Ui::imageEdit *ui;
	// Original image, either greyscale or RGB version of image
    cv::Mat orgImg;
	// same as orgImg unless an overlay has been applied
	cv::Mat dispImg;
	// Used for region of interest and panning
    cv::Mat roiImg;
#ifdef OPENGL
	// Object for displaying the image
	GLimageEdit* imgDisp;
#else
	QLabel* imgDisp;
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

	cv::Point2f orgImgPoint;

	// Colormapping values
	double minVal;
	double maxVal;
	std::vector<cv::Vec3b> LUT;
	bool	updateLUT;
	QLineEdit*			lineEdit;
	int numThreads;

	// Image editing stuff
	// It would make more sense to have a color to set to instead... A lot of things would make more sense at this point
	bool		_editing;
	// Any edits will be temporary and erased on starting a new edit.  Only really useful for drawing multiple lines
	bool		_tempEdit; 
	bool		_eraseEnabled;
	bool		_drawEnabled;
	bool		_drawLine;
	QAction*	_editingEnable;
	QAction*	_erase;
	QAction*	_draw;
	cv::Mat		_drawTool;
	// 0 = 1 pt, 1 = 3x3, 2 = 5x5
	int			_toolSize;
	
};
#endif // IMAGEEDIT_H
