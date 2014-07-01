#ifndef FEATUREEXTRACTORWIDGET_H
#define FEATUREEXTRACTORWIDGET_H

#include <QWidget>

namespace Ui {
class featureExtractorWidget;
}

class featureExtractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit featureExtractorWidget(QWidget *parent = 0);
    ~featureExtractorWidget();

private:
    Ui::featureExtractorWidget *ui;
};

#endif // FEATUREEXTRACTORWIDGET_H
