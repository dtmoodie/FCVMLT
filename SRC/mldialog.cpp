#include "mldialog.h"
#include "ui_mldialog.h"

mlDialog::mlDialog(QWidget *parent, imgSourcesWidget *sources_) :
    QDialog(parent),
	sources(sources_),
    ui(new Ui::mlDialog)
{
	ui->setupUi(this);
	//connect(ui->back, SIGNAL(clicked()), this, SLOT(on_back_clicked()));
	//connect(ui->next, SIGNAL(clicked()), this, SLOT(on_next_clicked()));
	//connect(ui->cancel, SIGNAL(clicked()), this, SLOT(on_cancel_clicked()));
	ui->stackedWidget->setCurrentIndex(0);
	int maxRows = 10;
	for (int i = 0; i < MACHINE_LEARNING_ALGORITHMS.size(); ++i)
	{
		algorithms.push_back(mlPtr(new mlContainer(MACHINE_LEARNING_ALGORITHMS[i])));
		algorithmSelectBoxes.push_back(new QCheckBox(algorithms[i]->name, this));
		ui->algorithmSelectLayout->addWidget(algorithmSelectBoxes[i], i%maxRows, i / maxRows);
	}
}

mlDialog::~mlDialog()
{
    delete ui;
}

void 
mlDialog::updateFeatureSelection()
{
	if (sources == NULL) return;
	if (sources->sources.empty()) return;
	containerPtr cont = sources->sources[0];
	for (int i = 0; i < cont->childContainers.size(); ++i)
	{
		if (cont->childContainers[i]->type == container::Features)
		{
			// Found a feature container, set it for display
			QTreeWidgetItem* newItem = new QTreeWidgetItem(ui->featureSelection);
			newItem->setText(0, cont->childContainers[i]->name);
			newItem->setFlags(newItem->flags() | Qt::ItemIsUserCheckable);
			newItem->setCheckState(0, Qt::Checked);
		}
	}
}
void 
mlDialog::updateSettingPage()
{
	// Delete any old control widgets
	for (int i = 0; i < currentControlWidgets.size(); ++i)
	{
		delete currentControlWidgets[i];
	}
	currentControlWidgets.clear();
	int rowCount = 0;
	int colCount = 0;
	// Find which algorithms are selected
	for (int i = 0; i < algorithmSelectBoxes.size(); ++i)
	{
		if (algorithmSelectBoxes[i]->isChecked())
		{
			// Add control widgets for selected algorithm
			QWidget* tmp = algorithms[i]->getParamControlWidget(this);
			currentControlWidgets.push_back(tmp);
			ui->algorithmSettingLayout->addWidget(tmp, rowCount, colCount);
			++rowCount;
			if (rowCount > 3)
			{
				++colCount;
				rowCount = 0;
			}
		}
	}
}
void 
mlDialog::on_next_clicked()
{
	int idx = ui->stackedWidget->currentIndex();
	if (idx == 2)
	{
		// change next to save so that on the next click the results will be saved to disk
		ui->next->setText("Save");
	}
	if (idx == 3)
	{
		// Save has been clicked, save results to disk
		return;
	}
	++idx;
	ui->stackedWidget->setCurrentIndex(idx);
}
void 
mlDialog::on_back_clicked()
{
	int idx = ui->stackedWidget->currentIndex();
	if (idx == 0)
	{
		return;
	}
	if (idx == 3)
	{
		ui->next->setText("Next");
	}
	--idx;
	ui->stackedWidget->setCurrentIndex(idx);
}
void 
mlDialog::on_cancel_clicked()
{
	this->hide();
}