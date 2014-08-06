#ifndef DRAWTOOLWIDGET_H
#define DRAWTOOLWIDGET_H

#include <QWidget>

namespace Ui {
class drawToolWidget;
}

class drawToolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit drawToolWidget(QWidget *parent = 0);
    ~drawToolWidget();
signals:
	void drawToggled(bool val);
	void eraseToggled(bool val);
	void sizeChanged(int val);
	void save();
private:
    Ui::drawToolWidget *ui;

private slots:
void handleDrawToggled(bool val);
void handleEraseToggled(bool val);
void handleSizeChanged(int val);
void handleSaveClicked();
};

#endif // DRAWTOOLWIDGET_H
