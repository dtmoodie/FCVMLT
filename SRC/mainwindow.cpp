#include "mainwindow.h"
#include "ui_mainwindow.h"
/* ToDo: 
 *	Gradient Orientation filter
 *  Adjust filter parameters and preview final output 
 *  Adjust pre-loaded macro parameters
 *  Registration
 *    Manual
 *    Manual by selecting key points
 *    Automatic with SIFT
 *  
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    currentSourceImg(NULL),
    matViewer(NULL),
    matViewerDlg(NULL),
    scrollX(NULL),
    scrollY(NULL),
    scrollZ(NULL),
    ui(new Ui::MainWindow),
	logFile(NULL),
	regDialog(NULL),
	drawWidget(NULL),
	drawDialog(NULL)
    //saveDialog(new saveStreamDialog(this))
{

    layout = new QGridLayout(this);

    // ******************************* Source selection ************************************
    sources = new imgSourcesWidget(this);
#ifndef KEITH_BUILD
	sourceDock = new QDockWidget(this);
    sourceDock->setAllowedAreas(Qt::TopDockWidgetArea       |
                                Qt::BottomDockWidgetArea    |
                                Qt::LeftDockWidgetArea      |
                                Qt::RightDockWidgetArea);
    sourceDock->setFeatures(QDockWidget::DockWidgetClosable     |
                            QDockWidget::DockWidgetFloatable    |
                            QDockWidget::DockWidgetMovable);
    sourceDock->setWidget(sources);
    sourceDock->setWindowTitle("Image Sources");
    addDockWidget(Qt::RightDockWidgetArea, sourceDock);
#endif
    // **************************** Source view *********************************************
    QLabel* sourceLbl = new QLabel(this);
    sourceLbl->setText("Source Image");
    sourceLbl->setMaximumHeight(20);
    layout->addWidget(sourceLbl,0,0);
	sourceDisplayThread = new QThread();
	sourceDisplayThread->start();
    sourceImg = new imageEdit(this, sources);
    sourceImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(sourceImg,1,0);
	sourceImg->moveToThread(sourceDisplayThread);

	// ************************** Save stream dialog **************************************
	saveDialog = new saveStreamDialog(this);
	
#ifndef KEITH_BUILD
    // *************************** Filter widget ********************************************
    filter      = new filterWidget(this);
    filter->setWindowTitle("Filter Window");
    filterDock  = new QDockWidget(this);
    filterDock->setAllowedAreas(Qt::TopDockWidgetArea       |
                                Qt::BottomDockWidgetArea    |
                                Qt::LeftDockWidgetArea      |
                                Qt::RightDockWidgetArea);
    filterDock->setFeatures(QDockWidget::DockWidgetClosable     |
                            QDockWidget::DockWidgetFloatable    |
                            QDockWidget::DockWidgetMovable);

    filterDock->setWidget(filter);
    filterDock->setWindowTitle("Filter Window");
    addDockWidget(Qt::LeftDockWidgetArea, filterDock);



	// ********************************** Compound filter widget **********************************
	compoundFilterWidget = new compoundImgFunction(this);
	compoundFilterWidget->setWindowTitle("Compound Filter Window");
	compoundDock = new QDockWidget(this);
	compoundDock->setAllowedAreas(Qt::TopDockWidgetArea |
		Qt::BottomDockWidgetArea |
		Qt::LeftDockWidgetArea |
		Qt::RightDockWidgetArea);
	compoundDock->setFeatures(QDockWidget::DockWidgetClosable |
		QDockWidget::DockWidgetFloatable |
		QDockWidget::DockWidgetMovable);
	compoundDock->setWidget(compoundFilterWidget);
	compoundDock->setWindowTitle("Compound Filter Window");
	addDockWidget(Qt::LeftDockWidgetArea, compoundDock);
	compoundDock->hide();

    // ***************************** Filter Preview display ************************
    QLabel* filterLbl = new QLabel(this);
    filterLbl->setText("Filter Preview");
    filterLbl->setMaximumHeight(20);
    layout->addWidget(filterLbl,0,1);
	filteredDisplayThread = new QThread;
	filteredDisplayThread->start();
	filteredImg = new imageEdit(this, sources);
	//filteredImg = new CQtOpenCVViewerGl(this);
	filteredImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(filteredImg,1,1);
	filteredImg->moveToThread(filteredDisplayThread);


    // ***************************** Text console *************************************

    console = new QPlainTextEdit(this);
    console->setMaximumHeight(100);
    layout->addWidget(console,2,0,1,2);


    // ***************************** Machine Learning Widget Setup **************************
    machineLearning = new mlWidget(this,sources->sourceList);
    mlDock = new QDockWidget(this);
    mlDock->setAllowedAreas(Qt::TopDockWidgetArea       |
                                Qt::BottomDockWidgetArea    |
                                Qt::LeftDockWidgetArea      |
                                Qt::RightDockWidgetArea);
    mlDock->setFeatures(QDockWidget::DockWidgetClosable     |
                            QDockWidget::DockWidgetFloatable    |
                            QDockWidget::DockWidgetMovable);
    mlDock->setWidget(machineLearning);
    mlDock->setWindowTitle("Machine Learning");
    addDockWidget(Qt::LeftDockWidgetArea,mlDock);
    mlDock->hide();

#ifndef KEITH_BUILD
	// ******************************* Feature extraction **********************************
	stats = new imgStatsWidget(this, sources);
	statDock = new QDockWidget(this);
	statDock->setAllowedAreas(Qt::TopDockWidgetArea |
		Qt::BottomDockWidgetArea |
		Qt::LeftDockWidgetArea |
		Qt::RightDockWidgetArea);
	statDock->setFeatures(QDockWidget::DockWidgetClosable |
		QDockWidget::DockWidgetFloatable |
		QDockWidget::DockWidgetMovable);
	statDock->setWidget(stats);
	statDock->setWindowTitle("Image Stats");
	addDockWidget(Qt::LeftDockWidgetArea, statDock);
	statDock->hide();
#endif
	

    // ***************************** Signals and Slots **************************************

    //connect(sources, SIGNAL(sourceChange(cv::Mat)),     filter,     SLOT(receiveImgChange(cv::Mat)));
	//connect(sources, SIGNAL(sourceChange(container*)), filter, SLOT(handleImgChange(container*)), Qt::QueuedConnection);
	connect(sources, SIGNAL(sourceChange(containerPtr)), filter, SLOT(handleImgChange(containerPtr)));
	//connect(sources, SIGNAL(sourceChange(container*)), this, SLOT(handleSourceChange(container*)), Qt::QueuedConnection);
	connect(sources, SIGNAL(sourceChange(containerPtr)), stats, SLOT(handleImgChange(containerPtr)));
	connect(stats,	 SIGNAL(log(QString, int)),			this,		SLOT(handleLog(QString, int)));
	connect(filter,  SIGNAL(log(QString, int)),			this,		SLOT(handleLog(QString, int)));
#endif
    //connect(sources, SIGNAL(sourcePreview(cv::Mat)),    sourceImg,  SLOT(changeImg(cv::Mat)));
	//connect(sources, SIGNAL(sourcePreview(container*)), sourceImg, SLOT(changeImg(container*)));
	connect(sources, SIGNAL(sourcePreview(containerPtr)), sourceImg, SLOT(changeImg(containerPtr)));
	connect(sources, SIGNAL(sourceChange(container*)),	saveDialog, SLOT(handleNewImage(container*)));
	connect(sources, SIGNAL(viewMatrix(cv::Mat)), this, SLOT(viewMatrix(cv::Mat)));

    //connect(sources, SIGNAL(sourcePreview(container*)), stats,      SLOT(handleImgChange(container*)));
#ifndef KEITH_BUILD
	connect(sources->sourceList, SIGNAL(itemSelectionChanged()),machineLearning, SLOT(handleSelectionChange()));

    // Triggered when the selected filtered image is changed, updates the display of that image
	//connect(filter, SIGNAL(filterImgChanged(cv::Mat)), filteredImg, SLOT(changeImg(cv::Mat))); // , Qt::QueuedConnection);
	connect(filter, SIGNAL(filterImgChanged(containerPtr)), filteredImg, SLOT(changeImg(containerPtr)));
	connect(filter, SIGNAL(saveImage(cv::Mat, QString)), sources, SLOT(handleSaveImage(cv::Mat, QString)));
	//connect(filter, SIGNAL(filterImgChanged(cv::Mat)), filteredImg, SLOT(showImage(cv::Mat)));

	connect(filteredImg, SIGNAL(log(QString, int)),		this,		SLOT(handleLog(QString, int)));

    connect(sourceImg, SIGNAL(rectSelect(cv::Rect)),    filter,     SLOT(handleRectSelect(cv::Rect)));
    connect(sourceImg, SIGNAL(consoleOutput(QString)),  console,    SLOT(appendPlainText(QString)));
    connect(sourceImg, SIGNAL(label(cv::Mat,int)),      this,       SLOT(handleImgLabel(cv::Mat,int)));
    connect(sourceImg, SIGNAL(displayMat(cv::Mat)),     this,       SLOT(viewMatrix(cv::Mat)));
	connect(sourceImg, SIGNAL(log(QString, int)),		this,		SLOT(handleLog(QString, int)));
	connect(sourceImg, SIGNAL(label(container*)),		filteredImg,SLOT(changeImg(container*)));

    connect(stats, SIGNAL(extractedFeatures(cv::Mat, bool)), this,  SLOT(displayMat(cv::Mat, bool)));
    connect(stats, SIGNAL(saveFeatures(cv::Mat,QString)), sources,  SLOT(handleSaveFeatures(cv::Mat,QString)));
	connect(stats, SIGNAL(viewFeatures(cv::Mat)), this, SLOT(viewMatrix(cv::Mat)));

	// Drawing signals
	connect(sourceImg, SIGNAL(drawPt(cv::Rect, cv::Mat)), filteredImg, SLOT(mirrorDraw(cv::Rect, cv::Mat)));
	connect(sourceImg, SIGNAL(erasePt(cv::Rect)), filteredImg, SLOT(mirrorErase(cv::Rect)));
	connect(filteredImg, SIGNAL(drawPt(cv::Rect, cv::Mat)), sourceImg, SLOT(mirrorDraw(cv::Rect, cv::Mat)));
	connect(filteredImg, SIGNAL(erasePt(cv::Rect)), sourceImg, SLOT(mirrorErase(cv::Rect)));
	
#endif

    ui->setupUi(this);
#ifndef KEITH_BUILD
    ui->menuLabel_Images->addAction(filterDock->toggleViewAction());
    ui->menuLabel_Images->addAction(sourceDock->toggleViewAction());
    ui->menuLabel_Images->addAction(statDock->toggleViewAction());
    ui->menuLabel_Images->addAction(mlDock->toggleViewAction());
	ui->menuLabel_Images->addAction(compoundDock->toggleViewAction());
    ui->actionLabel_Images->setCheckable(true);
    connect(ui->actionUse_colormap, SIGNAL(toggled(bool)), sourceImg, SLOT(toggleColormap(bool)));
    connect(ui->actionUse_colormap, SIGNAL(toggled(bool)), filteredImg, SLOT(toggleColormap(bool)));
	connect(ui->actionLink_Image_Views, SIGNAL(toggled(bool)), this, SLOT(handleLinkViews(bool)));
	connect(ui->actionMerge_image_displays, SIGNAL(toggled(bool)), this, SLOT(on_actionMergeImageDisplays(bool)));
	connect(ui->actionRegistration, SIGNAL(triggered()), this, SLOT(handleOpenRegistration()));
	connect(ui->actionDraw_menu, SIGNAL(triggered()), this, SLOT(handleOpenDrawMenu()));
	
#endif
    ui->centralWidget->setLayout(layout);
    centralWidget()->setLayout(layout);
    setWindowTitle("Main Window");
}

MainWindow::~MainWindow()
{
	delete filter;
	delete sources;
    delete ui;
}

// Load all of the classes from a text file
void MainWindow::loadLabels(QString file)
{
	cv::FileStorage fs(file.toStdString(), cv::FileStorage::READ);
	int count = (int)fs["ClassCount"];
	for (int i = 0; i < count; ++i)
	{
		std::string nodeName = "Class-" + QString::number(i).toStdString();
		cv::FileNode lbl = fs[nodeName];
		cv::FileNodeIterator it = lbl.begin();
		cv::FileNodeIterator it_end = lbl.end();
		std::pair<QString, int> tmp(QString::fromStdString((std::string)(*it)["name"]), (int)(*it)["idx"]);
		sourceImg->addClass(tmp);
	}
}

void MainWindow::handleLinkViews(bool val)
{
	if (val == true)
	{
		connect(sourceImg, SIGNAL(newROI(cv::Rect)), filteredImg, SLOT(handleROIUpdate(cv::Rect)));
		connect(filteredImg, SIGNAL(newROI(cv::Rect)), sourceImg, SLOT(handleROIUpdate(cv::Rect)));
	}else
	{
		disconnect(sourceImg, SIGNAL(newROI(cv::Rect)), filteredImg, SLOT(handleROIUpdate(cv::Rect)));
		disconnect(filteredImg, SIGNAL(newROI(cv::Rect)), sourceImg, SLOT(handleROIUpdate(cv::Rect)));
	}
}
void
MainWindow::handleOpenRegistration()
{
	if (regDialog == NULL)
	{
		regDialog = new registrationDialog(this, sources);
		connect(sources, SIGNAL(sourcePreview(containerPtr)), regDialog, SLOT(handleImageSelect(containerPtr)));
		connect(regDialog, SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
	}
	regDialog->show();
}
void
MainWindow::handleOpenDrawMenu()
{
	if (drawWidget == NULL)
	{
		if (drawDialog == NULL)
		{
			drawDialog = new QDialog(this);
		}
		drawWidget = new drawToolWidget(drawDialog);
	}
	connect(drawWidget, SIGNAL(drawToggled(bool)), sourceImg, SLOT(handleDrawToggled(bool)));
	connect(drawWidget, SIGNAL(drawToggled(bool)), filteredImg, SLOT(handleDrawToggled(bool)));

	connect(drawWidget, SIGNAL(eraseToggled(bool)), sourceImg, SLOT(handleEraseToggled(bool)));
	connect(drawWidget, SIGNAL(eraseToggled(bool)), filteredImg, SLOT(handleEraseToggled(bool)));

	connect(drawWidget, SIGNAL(sizeChanged(int)), sourceImg, SLOT(handleSizeChanged(int)));
	connect(drawWidget, SIGNAL(sizeChanged(int)), filteredImg, SLOT(handleSizeChanged(int)));

	connect(drawWidget, SIGNAL(save()), sourceImg, SLOT(handleSaveDrawing()));
	connect(drawWidget, SIGNAL(save()), filteredImg, SLOT(handleSaveDrawing()));
	drawDialog->show();
}

void MainWindow::on_actionFiles_from_disk_triggered()
{
	
    QStringList files = QFileDialog::getOpenFileNames(this,"Select files","C:/Data/Images/Castrol MB 50/Castrol MB 50, 10.00m","Images (*.png *.jpg *.tiff *dcm)");
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	if(files.size() == 0) return;
    QFileInfo lblCheck(files[0]);
    QString dir = files[0].mid(0, files[0].size() - lblCheck.baseName().size() - 4);
    QDir lblDir(dir + "/labels");
	QFileInfoList entries = QDir(dir).entryInfoList();
	QStringList dirs;
	//QList<QStringList> extras;
	for (int i = 2; i < entries.size(); ++i)
	{
		if (entries[i].isDir())
		{
			dirs.push_back(entries[i].filePath());
			//extras.push_back(QDir(entries[i].filePath()).entryList());
		}
	}
	
    bool labelsExist = false;
    QStringList labelFiles;
    if(lblDir.exists())
    {
        // These images may be labeled
        labelsExist = true;
        labelFiles = lblDir.entryList();
		loadLabels(dir + "labels/Classes.yml");
    }
	for (int i = 0; i < files.size(); ++i)
	{
		imgPtr tmp( new imgContainer(files.at(i), sources->sourceList) );
		// Load all sub information
		for (int j = 0; j < dirs.size(); ++j)
		{
			tmp->loadChildFromDir(dirs[j]);
		}
		sources->sources.push_back(tmp);
	}
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	handleLog("Image loading complete in " + QString::number(delta.total_milliseconds()) + " milliseconds", 0);
}

void MainWindow::on_actionFolder_from_disk_triggered()
{

}

void MainWindow::on_actionCamera_triggered()
{

}

void MainWindow::on_actionCamera_Stream_triggered()
{
#ifdef KEITH_BUILD
	int itemCount = sources->sourceList->topLevelItemCount();
	for (int i = 0; i < itemCount; ++i)
	{
		container* tmp = dynamic_cast<container*>(sources->sourceList->topLevelItem(i));
		delete tmp;
	}
#endif


	QDialog dlg;
	QGridLayout* tmpLayout = new QGridLayout(&dlg);
	dlg.setLayout(tmpLayout);


	QSpinBox* selection = new QSpinBox(&dlg);
	QLabel* label = new QLabel(&dlg);
	label->setText("Select camera device ID");
    tmpLayout->addWidget(label,0,0);
    tmpLayout->addWidget(selection,0,1);
    
	QComboBox* resolution = new QComboBox(&dlg);
    QLabel* resLabel = new QLabel(&dlg);
    resLabel->setText("Select resolution");
    resolution->addItem("1920x1080");
    resolution->addItem("1280x720");
    resolution->addItem("640x480");
    tmpLayout->addWidget(resLabel,1,0);
    tmpLayout->addWidget(resolution,1,1);

	QSpinBox* fps = new QSpinBox(&dlg);
	fps->setValue(30);
	QLabel* fpsLabel = new QLabel(&dlg);
	fpsLabel->setText("Select framerate");
	tmpLayout->addWidget(fpsLabel, 2, 0);
	tmpLayout->addWidget(fps, 2, 1);

	dlg.exec();
    int idx = resolution->currentIndex();
	streamContainer* cont = new streamContainer(sources->sourceList);
    cont->setText(0,"Camera stream");
    cont->openCamera(selection->value(), idx, fps->value());
    connect(cont, SIGNAL(imgUpdated()), sources, SLOT(cameraUpdated()));
	#ifdef KEITH_BUILD
	sources->curCont = cont;
	sources->sourceList->selectAll();
	#endif
}

void MainWindow::on_actionToggle_Source_View_triggered()
{
	

}

void MainWindow::handleImgLabel(cv::Mat img, int label)
{
    if(currentSourceImg == NULL) return;
	labelContainer* parent = new labelContainer(sources->sourceList);
	parent->setText(0, sourceImg->classSelect->itemText(label) + " mask");
	parent->setText(1, QString::number(label));
	img.copyTo(parent->M());
	parent->isTop = false;
	QDir dir;
    dir.mkdir(currentSourceImg->dirName + "/labels");
    cv::imwrite((currentSourceImg->dirName + "/labels/" + currentSourceImg->baseName +"-" +  QString::number(label) + ".jpg").toStdString(),img);
	currentSourceImg->addChild(parent);
    QFile file(currentSourceImg->dirName + "/labels/Classes.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    for(int i = 0; i < sourceImg->classSelect->count(); ++i)
    {
        if(i != 0)
        {
            out << "\n";
        }
        if(sourceImg->classSelect->itemText(i) == "-1"              ||
                sourceImg->classSelect->itemText(i) == "Negative"   ||
                sourceImg->classSelect->itemText(i) == "negative")
        {
            out << -1 << " ";
        }else
        {
            if(sourceImg->classSelect->itemText(i) == "1"           ||
                sourceImg->classSelect->itemText(i) == "Positive"   ||
                sourceImg->classSelect->itemText(i) == "positive")
            {
                out << 1 << " ";
            }else
            {
                out << i << " ";
            }
        }
        out << sourceImg->classSelect->itemText(i);
    }
    file.close();
}

void MainWindow::handleSourceChange(container* cont)
{
    currentSourceImg = cont;
	if (!cont->isData) return;
	switch (cont->type)
	{
	case container::Img:
	{

		break;
	}
		case container::Matrix:
		{
			matrixContainer* tmp = dynamic_cast<matrixContainer*>(cont);
			viewMatrix(tmp->M());
			break;
		}
		case container::Features:
		{
			featureContainer* tmp = dynamic_cast<featureContainer*>(cont);
			viewMatrix(tmp->M());
		}
    }
}
void 
MainWindow::handleLog(QString line, int level)
{
	if (sender() == (QObject*)stats)
	{
		line = "[STATS] " + line;
	}
	if (level < VERBOSITY_LEVEL) return;
	if (logFile == NULL) logFile = new QFile(LOG_NAME);
	if (!logFile->isOpen()) logFile->open(QIODevice::Append | QIODevice::Text);
	QTextStream out(logFile);
	out << line;
	out << "\n";
	console->appendPlainText(line);
}
void 
MainWindow::on_actionLabel_Images_triggered()
{
    if(ui->actionLabel_Images->isChecked())
    {
        sourceImg->labelImg();
    }else
    {
        sourceImg->stopLabelImg();
    }
}
void 
MainWindow::on_actionFilter_Images_triggered()
{
}
void 
MainWindow::displayMat(cv::Mat features,bool isImg)
{
	if (features.empty()) return;
    if(isImg)
    {
		//filteredImg->showImage(features);
		filteredImg->changeImg(features);
    }
        // Display first row to the console
        //std::stringstream sstream;
        //sstream << features.row(0);
        //QString msg = "Extracted Features: \n" + QString::fromStdString(sstream.str());
        //console->appendPlainText(msg);
}
void 
MainWindow::on_actionApply_filters_to_all_iamges_triggered()
{
    for(int i = 0; i < sources->sourceList->topLevelItemCount(); ++i)
    {
        container* cont = dynamic_cast<container*>(sources->sourceList->topLevelItem(i));
		if (cont->type == container::Img)
		{
			imgContainer* tmp = dynamic_cast<imgContainer*>(cont);
			if (!tmp->load()) return;
			//if (tmp->M.empty()) tmp->M = cv::imread(cont->filePath.toStdString());
			filter->handleImgChange(tmp);
			tmp->_M.release();
		}
    }
}

void MainWindow::viewMatrix(cv::Mat img)
{
	if (matViewer != NULL && matViewerDlg != NULL)
	{
		matViewer->changeMat(img);
		matViewerDlg->show();
		return;
	}
    matViewerDlg = new QDialog(this);
    matViewer = new matView(this,img);
	QGridLayout* layout = new QGridLayout(matViewerDlg);
	matViewerDlg->setLayout(layout);
	layout->addWidget(matViewer);
	matViewerDlg->show();
	/*
    QGridLayout* matLayout = new QGridLayout(this);

    // ToDo: Add buttons to allow scrolling of the matrix
    scrollX = new QSpinBox(matViewerDlg);
    scrollY = new QSpinBox(matViewerDlg);
	scrollX->setMaximum(img.cols - 1);
	scrollY->setMaximum(img.rows - 1);
	QLabel* colLbl = new QLabel(matViewerDlg);
	colLbl->setText("Columns: " + QString::number(img.cols));
	QLabel* rowLbl = new QLabel(matViewerDlg);
	rowLbl->setText("Rows: " + QString::number(img.rows));
	QLabel* chLbl = new QLabel(matViewerDlg);
	chLbl->setText("Channels: " + QString::number(img.channels()));
    connect(scrollX, SIGNAL(valueChanged(int)), matViewer, SLOT(changeX(int)));
    connect(scrollY, SIGNAL(valueChanged(int)), matViewer, SLOT(changeY(int)));
    matLayout->addWidget(matViewer,0,1,4,1);
    matLayout->addWidget(scrollX,0,0);
    matLayout->addWidget(scrollY,1,0);
	matLayout->addWidget(colLbl, 2, 0);
	matLayout->addWidget(rowLbl, 3, 0);
	matLayout->addWidget(chLbl, 4, 0);
    matViewerDlg->setLayout(matLayout);
    matViewerDlg->show();
	matViewerDlg->setWindowTitle("Matrix Viewer");*/
}

void MainWindow::on_actionExtract_Stats_for_all_images_triggered()
{
	for (int i = 0; i < sources->sources.size(); ++i)
    {
		stats->handleImgChange(sources->sources[i]);
    }
}

void MainWindow::on_actionSave_Camera_Stream_triggered()
{
    saveDialog->show();
}

void 
MainWindow::on_actionMergeImageDisplays(bool val)
{
	if (val == true)
		connect(filteredImg, SIGNAL(sendPairedImage(cv::Mat)), sourceImg, SLOT(receivePairedImage(cv::Mat)));
	else
	{
		disconnect(filteredImg, SIGNAL(sendPairedImage(cv::Mat)), sourceImg, SLOT(receivePairedImage(cv::Mat)));
		//disconnect(filteredImg, SIGNAL(sendPairedImage(cv::Mat)));
		//disconnect()
	}
	
}