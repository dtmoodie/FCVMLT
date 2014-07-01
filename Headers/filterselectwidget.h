#ifndef FILTERSELECTWIDGET_H
#define FILTERSELECTWIDGET_H

#include <QWidget>
#include <QList>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>

//#include <filtersettingwidget.h>
//#include <imgFilter.h>
#include <FCVMLT.h>


namespace Ui {
class filterSelectWidget;
}

class filterSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit filterSelectWidget(QWidget *parent = 0, QTreeWidget* tree_ = 0);
    ~filterSelectWidget();
    QList<filterContainer*> filters;
public slots:
	void handleButtonPress();
signals:

private:
    Ui::filterSelectWidget *ui;
    QGridLayout* layout;
    cv::Mat tmpOutput;
    cv::Mat curImg;
    QWidget* currentSettingWidget;
    QTreeWidget* tree;
};

#endif // FILTERSELECTWIDGET_H
