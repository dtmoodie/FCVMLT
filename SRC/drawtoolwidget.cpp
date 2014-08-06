#include "drawtoolwidget.h"
#include "ui_drawtoolwidget.h"

drawToolWidget::drawToolWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drawToolWidget)
{
    ui->setupUi(this);
	connect(ui->btnDraw, SIGNAL(clicked(bool)), this, SLOT(handleDrawToggled(bool)));
	connect(ui->btnErase, SIGNAL(clicked(bool)), this, SLOT(handleEraseToggled(bool)));
	connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(handleSizeChanged(int)));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(handleSaveClicked()));
}

drawToolWidget::~drawToolWidget()
{
    delete ui;
}

void
drawToolWidget::handleDrawToggled(bool val)
{
	if (val == true)
	{
		ui->btnErase->setChecked(false);
	}
	emit drawToggled(val);
}
void
drawToolWidget::handleEraseToggled(bool val)
{
	if (val == true)
	{
		ui->btnDraw->setChecked(false);
	}
	emit eraseToggled(val);
}
void
drawToolWidget::handleSizeChanged(int val)
{
	emit sizeChanged(val);
}
void drawToolWidget::handleSaveClicked()
{
	emit save();
}