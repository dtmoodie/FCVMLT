#include "cqtopencvviewergl.h"

CQtOpenCVViewerGl::CQtOpenCVViewerGl(QWidget *parent) :
    QGLWidget(parent)
{
    mSceneChanged = false;
    mBgColor = QColor::fromRgb(150, 150, 150);

    mOutH = 0;
    mOutW = 0;
    mImgRatio = 4.0f/3.0f;

    mPosX = 0;
    mPosY = 0;
	transZ = 0;
	installEventFilter(this);
}

void CQtOpenCVViewerGl::initializeGL()
{
    makeCurrent();
    qglClearColor(mBgColor.darker());
}

void CQtOpenCVViewerGl::resizeGL(int width, int height)
{
    makeCurrent();
    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width, 0, height, 0, 1);	// To Draw image in the center of the area

    glMatrixMode(GL_MODELVIEW);

    // ---> Scaled Image Sizes
    mOutH = width/mImgRatio;
    mOutW = width;

    if(mOutH>height)
    {
        mOutW = height*mImgRatio;
        mOutH = height;
    }

    emit imageSizeChanged( mOutW, mOutH );
    // <--- Scaled Image Sizes

    mPosX = (width-mOutW)/2;
    mPosY = (height-mOutH)/2;

    mSceneChanged = true;

    updateScene();
}

void CQtOpenCVViewerGl::updateScene()
{
    if( mSceneChanged && this->isVisible() )
        updateGL();
}

void CQtOpenCVViewerGl::paintGL()
{
    makeCurrent();

    if( !mSceneChanged )
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderImage();

    mSceneChanged = false;
}

void CQtOpenCVViewerGl::renderImage()
{
    makeCurrent();

    glClear(GL_COLOR_BUFFER_BIT);

    if (!mRenderQtImg.isNull())
    {
        glLoadIdentity();

        QImage image; // the image rendered

        glPushMatrix();
		
        {
            int imW = mRenderQtImg.width();
            int imH = mRenderQtImg.height();

            // The image is to be resized to fit the widget?
            if( imW != this->size().width() &&
                    imH != this->size().height() )
            {

                image = mRenderQtImg.scaled( //this->size(),
                                             QSize(mOutW,mOutH),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation
                                             );

                //qDebug( QString( "Image size: (%1x%2)").arg(imW).arg(imH).toAscii() );
            }
            else
                image = mRenderQtImg;

            // ---> Centering image in draw area            
			glTranslatef(0, 0, transZ);
            //glRasterPos2i( mPosX, mPosY );
            // <--- Centering image in draw area

            imW = image.width();
            imH = image.height();
			glDraw();
            //glDrawPixels( imW, imH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        }
        glPopMatrix();

        // end
        glFlush();
    }
}

bool CQtOpenCVViewerGl::showImage( cv::Mat image )
{
    image.copyTo(mOrigImage);

    mImgRatio = (float)image.cols/(float)image.rows;

	glEnable(GL_TEXTURE_RECTANGLE);
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_RECTANGLE, texID);
	if (image.type() == CV_8UC3)
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_BGR, image.cols, image.rows,
		0, GL_BGR, GL_UNSIGNED_INT8_VEC3_NV, image.data);
	else
		return false;
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(image.cols, 0);
	glVertex2f(width(), 0);
	glTexCoord2f(image.cols, image.rows);
	glVertex2f(width(), height());
	glTexCoord2f(0, image.rows);
	glVertex2f(0, height());
	glEnd();
	glDisable(GL_TEXTURE_RECTANGLE);

    if( mOrigImage.channels() == 3)
        mRenderQtImg = QImage((const unsigned char*)(mOrigImage.data),
                              mOrigImage.cols, mOrigImage.rows,
                              mOrigImage.step, QImage::Format_RGB888).rgbSwapped();
    else if( mOrigImage.channels() == 1)
        mRenderQtImg = QImage((const unsigned char*)(mOrigImage.data),
                              mOrigImage.cols, mOrigImage.rows,
                              mOrigImage.step, QImage::Format_Indexed8);
    else
        return false;

    mRenderQtImg = QGLWidget::convertToGLFormat(mRenderQtImg);

    mSceneChanged = true;

    updateScene();

    return true;
}

bool
CQtOpenCVViewerGl::eventFilter(QObject* o, QEvent* e)
{
	if (o == this)
	{
		if (e->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* me = dynamic_cast<QMouseEvent*>(e);
			QPoint pos = me->pos();
			float x = pos.x() - mPosX;
			float y = pos.y() - mPosY;
			x = x * mOrigImage.cols / mOutW;
			y = y * mOrigImage.rows / mOutH;
			//glTranslatef(0, 0, 1);
		}
		if (e->type() == QEvent::MouseButtonRelease)
		{

		}
		if (e->type() == QEvent::MouseMove)
		{

		}
		if (e->type() == QEvent::Wheel)
		{

		}
	}

	return false;
}
