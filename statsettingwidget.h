#ifndef STATSETTINGWIDGET_H
#define STATSETTINGWIDGET_H

#include <QWidget>
#include <imgFilter.h>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>


namespace Ui {
class statSettingWidget;
}

class statSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit statSettingWidget(QWidget *parent = 0, featureWindow* stats_ = 0);
    ~statSettingWidget();
public slots:
    void paramChange(QString val);
signals:
    void paramChange();
private:
    Ui::statSettingWidget *ui;
    featureWindow* stats;
    QGridLayout* layout;

};


#endif // STATSETTINGWIDGET_H
