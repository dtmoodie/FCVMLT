#ifndef SAVESTREAMDIALOG_H
#define SAVESTREAMDIALOG_H

#include <QDialog>
//#include "container.h"
#include <FCVMLT.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDate>
namespace Ui {
class saveStreamDialog;
}

class saveStreamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit saveStreamDialog(QWidget *parent = 0);
    ~saveStreamDialog();

private slots:
    void on_browse_clicked();

    void on_OK_clicked();

    void on_cancel_clicked();

    void on_checkBox_clicked();

    void handleNewImage(container* cont);

    void on_path_editingFinished();

    void on_checkBox_clicked(bool checked);

private:
    Ui::saveStreamDialog *ui;
    cv::VideoWriter* writer;
    bool first;
    bool opened;
	int fourCC;
    int counter;
	QString baseFileName;
};

#endif // SAVESTREAMDIALOG_H
