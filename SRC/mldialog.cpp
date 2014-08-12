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
		connect(algorithms[i].get(), SIGNAL(results(QString)), this, SLOT(on_result(QString)));
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
mlDialog::trainAlgorithms()
{
	for (int i = 0; i < algorithmSelectBoxes.size(); ++i)
	{
		if (algorithmSelectBoxes[i]->isChecked())
		{
			algorithms[i]->train(X, Y);
		}
	}
}
void 
mlDialog::buildFeatures()
{
	X.release();
	Y.release();
	QStringList featureNames;
	for (int i = 0; i < ui->featureSelection->topLevelItemCount(); ++i)
	{
		if (ui->featureSelection->topLevelItem(i)->checkState(0))
		{
			featureNames << ui->featureSelection->topLevelItem(i)->text(0);
		}
	}
	for (int i = 0; i < sources->sources.size(); ++i)
	{
		// TODO figure out how to concatenate different feature sources and different labels
		// For now just stack feature and matrix containers
		//cv::Mat F;
		//for (int j = 0; j < featureNames.size(); ++j)
		//{
		//	if (F.empty())
		//		F = sources->sources[i]->getChild(featureNames[j])
		//}
		for (int j = 0; j < featureNames.size(); ++j)
		{
			featurePtr F = boost::dynamic_pointer_cast<featureContainer, container>(sources->sources[i]->getChild(featureNames[j]));
			if (X.empty())
			{
				X = F->M();
				Y = F->lbl();
			}else
			{
				cv::vconcat(X, F->M(), X);
				cv::vconcat(Y, F->lbl(), Y);
			}
		}
	}
}
void 
mlDialog::on_next_clicked()
{
	int idx = ui->stackedWidget->currentIndex();
	if (idx == 1)
	{
		updateSettingPage();
	}
	if (idx == 2)
	{
		// change next to save so that on the next click the results will be saved to disk
		buildFeatures();
		trainAlgorithms();
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
void
mlDialog::on_result(QString result)
{
	ui->resultWindow->appendPlainText(result);
}