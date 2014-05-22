#ifndef FILTERSELECTWIDGET_H
#define FILTERSELECTWIDGET_H

#include <QWidget>
#include <QList>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>

#include <filtersettingwidget.h>
#include <imgFilter.h>



namespace Ui {
class filterSelectWidget;
}

class filterSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit filterSelectWidget(QWidget *parent = 0, QTreeWidget* tree_ = 0);
    ~filterSelectWidget();
    QList<imgFilter*> filters;
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::filterSelectWidget *ui;
    QGridLayout* layout;
    cv::Mat tmpOutput;
    cv::Mat curImg;
    filterSettingWidget* currentSettingWidget;
    QTreeWidget* tree;
};

#endif // FILTERSELECTWIDGET_H
