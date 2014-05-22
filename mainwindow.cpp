#include "mainwindow.h"
#include "ui_mainwindow.h"
/* ToDo: Gradient Orientation filter
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
    ui(new Ui::MainWindow)
    //saveDialog(new saveStreamDialog(this))
{

    layout = new QGridLayout(this);
#ifndef KEITH_BUILD
    // ******************************* Feature extraction **********************************
    stats = new imgStatsWidget(this);
    statDock = new QDockWidget(this);
    statDock->setAllowedAreas(Qt::TopDockWidgetArea       |
                                Qt::BottomDockWidgetArea    |
                                Qt::LeftDockWidgetArea      |
                                Qt::RightDockWidgetArea);
    statDock->setFeatures(QDockWidget::DockWidgetClosable     |
                            QDockWidget::DockWidgetFloatable    |
                            QDockWidget::DockWidgetMovable);
    statDock->setWidget(stats);
    statDock->setWindowTitle("Image Stats");
    addDockWidget(Qt::LeftDockWidgetArea, statDock);
    statDock->hide();
#endif
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

    sourceImg = new imageEdit(this);
    sourceImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(sourceImg,1,0);


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



    // ***************************** Filter Preview display ************************
    QLabel* filterLbl = new QLabel(this);
    filterLbl->setText("Filter Preview");
    filterLbl->setMaximumHeight(20);
    layout->addWidget(filterLbl,0,1);
    filteredImg = new imageEdit(this);
    filteredImg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(filteredImg,1,1);


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


    // ***************************** Signals and Slots **************************************

    //connect(sources, SIGNAL(sourceChange(cv::Mat)),     filter,     SLOT(receiveImgChange(cv::Mat)));
    connect(sources, SIGNAL(sourceChange(container*)),  filter,     SLOT(handleImgChange(container*)));
    connect(sources, SIGNAL(sourceChange(container*)),  this,       SLOT(handleSourceChange(container*)));
    connect(sources, SIGNAL(sourceChange(container*)),  stats,      SLOT(handleImgChange(container*)));
#endif
    //connect(sources, SIGNAL(sourcePreview(cv::Mat)),    sourceImg,  SLOT(changeImg(cv::Mat)));
	connect(sources, SIGNAL(sourcePreview(container*)), sourceImg, SLOT(changeImg(container*)));
	connect(sources, SIGNAL(sourceChange(container*)), saveDialog, SLOT(handleNewImage(container*)));
    //connect(sources, SIGNAL(sourcePreview(container*)), stats,      SLOT(handleImgChange(container*)));
#ifndef KEITH_BUILD
	connect(sources->sourceList, SIGNAL(itemSelectionChanged()),
            machineLearning, SLOT(handleSelectionChange()));


    // Triggered when the selected filtered image is changed, updates the display of that image
    connect(filter, SIGNAL(filterImgChanged(cv::Mat)),  filteredImg,    SLOT(changeImg(cv::Mat)));
    connect(filter, SIGNAL(consoleOutput(QString)),     console,        SLOT(appendPlainText(QString)));
    connect(filter, SIGNAL(saveImage(cv::Mat,QString)), sources,        SLOT(handleSaveImage(cv::Mat,QString)));

    connect(sourceImg, SIGNAL(rectSelect(cv::Rect)),    filter,     SLOT(handleRectSelect(cv::Rect)));
    connect(sourceImg, SIGNAL(consoleOutput(QString)),  console,    SLOT(appendPlainText(QString)));
    connect(sourceImg, SIGNAL(label(cv::Mat,int)),      this,       SLOT(handleImgLabel(cv::Mat,int)));
    connect(sourceImg, SIGNAL(displayMat(cv::Mat)),     this,       SLOT(viewMatrix(cv::Mat)));

    connect(stats, SIGNAL(extractedFeatures(cv::Mat, bool)), this,  SLOT(displayMat(cv::Mat, bool)));
    connect(stats, SIGNAL(saveFeatures(cv::Mat,QString)), sources,  SLOT(handleSaveFeatures(cv::Mat,QString)));
#endif


    ui->setupUi(this);
#ifndef KEITH_BUILD
    ui->menuLabel_Images->addAction(filterDock->toggleViewAction());
    ui->menuLabel_Images->addAction(sourceDock->toggleViewAction());
    ui->menuLabel_Images->addAction(statDock->toggleViewAction());
    ui->menuLabel_Images->addAction(mlDock->toggleViewAction());
    ui->actionLabel_Images->setCheckable(true);
#endif
    ui->centralWidget->setLayout(layout);
    centralWidget()->setLayout(layout);
    setWindowTitle("Main Window");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadLabels(QString file)
{
	QFile lblFile(file);
	if(!lblFile.open(QIODevice::Text | QIODevice::ReadOnly)) return;
	if (lblFile.size() == 0) return;
	QStringList labels; // asdfasdfasdf
}

void MainWindow::on_actionFiles_from_disk_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this,"Select files","C:/Data/Images");
    if(files.size() == 0) return;
    QFileInfo lblCheck(files[0]);
    QString dir = files[0].mid(0, files[0].size() - lblCheck.baseName().size() - 4);
    QDir lblDir(dir + "/labels");
    bool labelsExist = false;
    QStringList labelFiles;
    if(lblDir.exists())
    {
        // These images may be labeled
        labelsExist = true;
        labelFiles = lblDir.entryList();


    }
    for(int i = 0; i < files.size(); ++i)
    {
        QFileInfo file(files.at(i));
        if(!file.exists())
            continue;
        if(	file.suffix().compare("jpg",Qt::CaseInsensitive) == 0 || 
			file.suffix().compare("gif",Qt::CaseInsensitive) == 0 || 
			file.suffix().compare("png", Qt::CaseInsensitive) == 0 ||
			file.suffix().compare("tiff", Qt::CaseInsensitive) == 0)
        {
            container* tmp = new container(sources->sourceList);
            tmp->baseName = file.baseName();
            tmp->filePath = files.at(i);
            tmp->dirName = files.at(i).mid(0, tmp->filePath.size() - tmp->baseName.size() - 4);
            tmp->setText(0,tmp->baseName);
            tmp->isImg = true;

            container* child = new container();
            child->isTop = false;
            child->setText(0,"Directory");
            child->setText(1,tmp->dirName);
            tmp->addChild(dynamic_cast<QTreeWidgetItem*>(child));

            // search for a label
            if(labelsExist)
            {
                for(int i = 2; i < labelFiles.size(); ++i)
                {
                    if(labelFiles[i].startsWith(tmp->baseName))
                    {
                        container* label = new container();
                        label->baseName = labelFiles[i];
                        label->filePath = tmp->dirName + "/labels/" + labelFiles[i];
                        label->dirName = tmp->dirName + "/labels";
                        label->isTop = false;
                        label->isImg = true;
                        QString txtLabel = labelFiles[i].mid(tmp->baseName.size() + 1,(labelFiles[i].size() - 5 - tmp->baseName.size()));
                        label->label = txtLabel.toInt();

                        label->setText(0, txtLabel + " mask");
                        label->setText(1, txtLabel);
                        label->isTop = false;
                        label->isLabel = true;


                        tmp->addChild(label);
                        break;
                    }
                }
            }
        }

        if(file.suffix() == "txt")
        {
            // Load a feature matrix from this file?  Check if there is a
            if(file.baseName() == "Classes")
            {
                // Load pre defined classes into image edit
                if(sourceImg->classSelect == NULL)
                {
                    sourceImg->classSelect = new QComboBox(sourceImg);
                }
                QFile inputFile(files.at(i));
                if(inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QTextStream in(&inputFile);
                    while(!in.atEnd())
                    {
                        QString line = in.readLine();
                        QStringList items = line.split(" ");
                        sourceImg->classSelect->addItem(items.at(1));
                    }
                    inputFile.close();
                }
            }
            // For each already loaded image, check if the names match

        }
    }
}

void MainWindow::on_actionFolder_from_disk_triggered()
{

}

void MainWindow::on_actionCamera_triggered()
{

}

void MainWindow::on_actionCamera_Stream_triggered()
{
//#ifdef KEITH_BUILD
	int itemCount = sources->sourceList->topLevelItemCount();
	for (int i = 0; i < itemCount; ++i)
	{
		container* tmp = dynamic_cast<container*>(sources->sourceList->topLevelItem(i));
		delete tmp;
	}
//#endif


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
    container* cont = new container(sources->sourceList);
	cont->frameRate = fps->value();
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
    container* child = new container(sources->sourceList);
    child->setText(0, sourceImg->classSelect->itemText(label) + " mask");
    child->setText(1, QString::number(label));
    child->img = img;
    child->isTop = false;
    child->isImg = true;
    child->isLabel = true;
	QDir dir;
    dir.mkdir(currentSourceImg->dirName + "/labels");
    cv::imwrite((currentSourceImg->dirName + "/labels/" + currentSourceImg->baseName +"-" +  QString::number(label) + ".jpg").toStdString(),img);
    currentSourceImg->addChild(child);
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
    if(currentSourceImg->isFeatures)
    {
        // This is where I would trigger a matrix view dialog to help view this jank yo
        viewMatrix(cont->img);
    }
}

void MainWindow::on_actionLabel_Images_triggered()
{
    if(ui->actionLabel_Images->isChecked())
    {
        sourceImg->labelImg();
    }else
    {
        sourceImg->stopLabelImg();
    }
}

void MainWindow::on_actionFilter_Images_triggered()
{
}

void MainWindow::displayMat(cv::Mat features,bool isImg)
{
    if(isImg)
    {
        filteredImg->changeImg(features);
    }
        // Display first row to the console
        std::stringstream sstream;
        sstream << features.row(0);
        QString msg = "Extracted Features: \n" + QString::fromStdString(sstream.str());
        console->appendPlainText(msg);
}

void MainWindow::on_actionApply_filters_to_all_iamges_triggered()
{
    for(int i = 0; i < sources->sourceList->topLevelItemCount(); ++i)
    {
        container* cont = dynamic_cast<container*>(sources->sourceList->topLevelItem(i));
        if(cont->img.empty()) cont->img = cv::imread(cont->filePath.toStdString());
        filter->handleImgChange(cont);
        cont->img.release();
    }
}

void MainWindow::viewMatrix(cv::Mat img)
{
    if(matViewer != NULL) delete matViewer;
    if(matViewerDlg != NULL) delete matViewerDlg;
    matViewerDlg = new QDialog(this);
    matViewer = new matView(this,img);
    QGridLayout* matLayout = new QGridLayout(this);

    // ToDo: Add buttons to allow scrolling of the matrix
    scrollX = new QSpinBox(matViewerDlg);
    scrollY = new QSpinBox(matViewerDlg);
    scrollZ = new QSpinBox(matViewerDlg);
    connect(scrollX, SIGNAL(valueChanged(int)), matViewer, SLOT(changeX(int)));
    connect(scrollY, SIGNAL(valueChanged(int)), matViewer, SLOT(changeY(int)));
    connect(scrollZ, SIGNAL(valueChanged(int)), matViewer, SLOT(changeZ(int)));
    matLayout->addWidget(matViewer,0,1,4,1);
    matLayout->addWidget(scrollX,0,0);
    matLayout->addWidget(scrollY,1,0);
    matLayout->addWidget(scrollZ,2,0);
    matViewerDlg->setLayout(matLayout);
    matViewerDlg->show();
    matViewerDlg->setWindowTitle("Matrix Viewer");
}

void MainWindow::on_actionExtract_Stats_for_all_images_triggered()
{
    for(int i = 0; i < sources->sourceList->topLevelItemCount(); ++i)
    {
        container* cont = dynamic_cast<container*>(sources->sourceList->topLevelItem(i));
        if(cont->img.empty()) cont->img = cv::imread(cont->filePath.toStdString());
        stats->handleImgChange(cont);
        cont->img.release();
    }
}

void MainWindow::on_actionSave_Camera_Stream_triggered()
{
    saveDialog->show();
}
