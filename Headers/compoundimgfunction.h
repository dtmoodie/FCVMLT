#ifndef COMPOUNDIMGFUNCTION_H
#define COMPOUNDIMGFUNCTION_H

#include <QWidget>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <FCVMLT.h>



namespace Ui {
class compoundImgFunction;
}


/*!
 * \brief The compoundImgFunction class allows for multiple image selection for compound
 *        functions such as subtracting two images or aligning two images.
 */

class compoundImgFunction : public QWidget
{
    Q_OBJECT

public:
    explicit compoundImgFunction(QWidget *parent = 0);
    ~compoundImgFunction();

private:
    Ui::compoundImgFunction *ui;
};

#endif // COMPOUNDIMGFUNCTION_H
