#ifndef FILTERSETTINGWIDGET_H
#define FILTERSETTINGWIDGET_H
#include <imgFilter.h>

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QSpinBox>
#include <QDoubleSpinBox>


namespace Ui {
class filterSettingWidget;
}

class filterSettingWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit filterSettingWidget(QWidget *parent = 0, imgFilter* filter_ = 0, int idx = 0);
    ~filterSettingWidget();
    
private slots:
    void paramChange(QString value);
    void acceptClicked();



signals:
    void paramChanged(int idx);
    void accepted(int idx);

private:
    Ui::filterSettingWidget *ui;
    QList<QLabel*> labels;
    imgFilter* filter;
    int idx;
};

#endif // FILTERSETTINGWIDGET_H
