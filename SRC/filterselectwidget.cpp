#include "filterselectwidget.h"
#include "ui_filterselectwidget.h"

filterSelectWidget::filterSelectWidget(QWidget *parent, QTreeWidget* tree_) :
    QWidget(parent), currentSettingWidget(NULL), tree(tree_),
    ui(new Ui::filterSelectWidget)
{
	for (int i = 0; i < FILTER_TYPES.size(); ++i)
	{
		filterContainer* filter = new filterContainer(FILTER_TYPES[i]);
		filters.push_back(filter);
	}
    layout = new QGridLayout();
    for(unsigned int i = 0; i < filters.size(); ++i)
    {
        QPushButton* button = new QPushButton;
        //button->installEventFilter(this);
        button->setText(filters[i]->name);
        filters[i]->buttonPtr = button;
		connect(button, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
        layout->addWidget(button);
    }
    setLayout(layout);
}

filterSelectWidget::~filterSelectWidget()
{
    delete ui;
}

void filterSelectWidget::handleButtonPress()
{
	if (curImg.empty()) return;
	for (int i = 0; i < filters.size(); ++i)
	{
		if (sender() == (QObject*)filters[i]->buttonPtr)
		{
			if (currentSettingWidget != NULL) delete currentSettingWidget;
			currentSettingWidget = filters[i]->getParamControlWidget(this);
			layout->addWidget(currentSettingWidget, 0, 1);
			return;
		}
	}
}
