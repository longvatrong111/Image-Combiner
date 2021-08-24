#include "imagecombiner.h"
#include "./ui_imagecombiner.h"

#include <iostream>
#include <cstdlib>
#include <QCoreApplication>


ImageCombiner::ImageCombiner(QWidget *parent)
    : QWidget(parent),
      mDirectoryList(new QGraphicsView(this)),
      mControl(new QGraphicsView(this)),
      mView(new QGraphicsView(this)),
      mImageList(new QGraphicsView(this))
{
      renderTemplate();
      renderDirBlock();
      renderImageBlock();
      renderControlBlock();
      renderViewBlock();
}

ImageCombiner::~ImageCombiner()
{
    /* Do something */
}

void ImageCombiner::renderTemplate()
{
    /* App */
    setWindowIcon(QIcon("./app.ico"));
    setWindowTitle("Image Combiner");
    setFixedSize((BWIDTH*3)+(PADDING*4), BHEIGHT+(PADDING*2));

    mDirectoryList->setGeometry(PADDING, PADDING, BWIDTH, BHEIGHT);
    mImageList->setGeometry((PADDING*2)+BWIDTH, PADDING, BWIDTH, BHEIGHT);
    mView->setGeometry((PADDING*3)+(BWIDTH*2), PADDING, BWIDTH, BHEIGHT-PADDING-140);
    mControl->setGeometry((PADDING*3)+(BWIDTH*2), PADDING+BHEIGHT-140, BWIDTH, 140);
}

void ImageCombiner::renderDirBlock()
{
    /* Directory list block */
    QString description("Select folders containing body parts...");
    QLabel* descriptionLabel = new QLabel(description, mDirectoryList);
    descriptionLabel->setGeometry(PADDING,PADDING,BWIDTH-(PADDING*2),40);
    descriptionLabel->setAlignment(Qt::AlignJustify);

    mScrollDir = new QScrollArea(mDirectoryList);
    mScrollDir->setGeometry(0,60,BWIDTH,BHEIGHT-140);

    mDisplayDirList = new QWidget();
    mScrollDir->setWidget(mDisplayDirList);
    mDisplayDirList->setGeometry(0,0,BWIDTH-20,BHEIGHT-140);

    /* Buttons */
    mButtonsArea = new QWidget(mDirectoryList);
    mButtonsArea->setGeometry(0,BHEIGHT-(PADDING*2)-40,BWIDTH,PADDING*2+40);

    mAddBackground = new QPushButton("SELECT BACKGROUND", mButtonsArea);
    mAddBackground->setGeometry(PADDING,PADDING,(BWIDTH-(3*PADDING))/2,40);

    mAddDir = new QPushButton("ADD DIRECTORY", mButtonsArea);
    mAddDir->setGeometry(PADDING*2+(BWIDTH-(3*PADDING))/2,PADDING,(BWIDTH-(3*PADDING))/2,40);
    QObject::connect(mAddDir, SIGNAL (clicked()), this, SLOT(openDirectory()));
}

void ImageCombiner::renderImageBlock()
{
    mScrollImage = new QScrollArea(mImageList);
    mScrollImage->setGeometry(0,0,BWIDTH,BHEIGHT);

    mDisplayImageList = new QWidget();
    mScrollImage->setWidget(mDisplayImageList);
    mDisplayImageList->setGeometry(0,0,BWIDTH-20,BHEIGHT);
}

void ImageCombiner::renderControlBlock()
{
    /* Control block */
    QLabel* maxOutputLabel = new QLabel("Max images: ", mControl);
    maxOutputLabel->setGeometry(PADDING,PADDING,80,40);

    mMaxOutputSpin = new QSpinBox(mControl);
    mMaxOutputSpin->setRange(0,999999999);
    mMaxOutputSpin->setValue(mMaxOutput); /* Default value */
    mMaxOutputSpin->setGeometry(PADDING+80,PADDING,80,40);

    mProcess = new QPushButton("Process", mControl);
    mProcess->setGeometry(BWIDTH-PADDING-80,PADDING,80,40);
    QObject::connect(mProcess, SIGNAL (clicked()), this, SLOT(submitInput()));

    mProgressBar = new QProgressBar(mControl);
    mProgressBar->setGeometry(PADDING,PADDING*2+40,BWIDTH-(PADDING*2),40);
    mProgressBar->setTextVisible(false);
}

void ImageCombiner::renderViewBlock()
{
    /* View block */
    QLabel* headlineView = new QLabel("EXAMPLE COMBINATION", mView);
    headlineView->setGeometry(PADDING,PADDING,BWIDTH-(PADDING*2),20);
    headlineView->setStyleSheet("font-weight: bold; color: blue");
    headlineView->setAlignment(Qt::AlignCenter);

    mExamImage = new QLabel(mView);
    mExamImage->setGeometry(PADDING,PADDING*2+20,BWIDTH-(PADDING*2),mView->height()-(PADDING*3)-20);
    mExamImage->setAlignment(Qt::AlignCenter);
}

void ImageCombiner::openDirectory()
{
    if (isRunningMainFunction) return;

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Add Directory"), "C:/Users/TRONG/Downloads/ImageCombinerTest", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirPath.isEmpty()) return;

    QDir* dir = new QDir(dirPath);
    dirList.push_back(dir);

    dirPath.replace(0, dirPath.lastIndexOf('/') + 1, "");
    QPushButton* newBtn = new QPushButton(dirPath, mDisplayDirList);
    btnList.push_back(newBtn);

    QPushButton* newCloseBtn = new QPushButton("X", mDisplayDirList);
    QObject::connect(newCloseBtn, &QPushButton::clicked, [=](){ this->deleteDir(newCloseBtn); });
    closeList.push_back(newCloseBtn);

    numDir++;

    updateDirectoryList(newBtn, newCloseBtn);
    updateExamImage();
//    QSpinBox* newSpin = new QSpinBox(mDisplayDirList);
//    spinList.push_back(newSpin);
//    newSpin->setRange(0, 100);
//    newSpin->setValue(100);

//    QLabel* newRate = new QLabel("Rate: ", mDisplayDirList);
//    rateList.push_back(newRate);
//    QLabel* newPercent = new QLabel("%", mDisplayDirList);
//    percentList.push_back(newPercent);
}

void ImageCombiner::updateDirectoryList(QPushButton* newBtn, QPushButton* newCloseBtn)
{
    newBtn->setGeometry(PADDING,dy,BWIDTH-60-(PADDING*2),40);
    newBtn->show();

    newCloseBtn->setGeometry(PADDING+newBtn->width(),dy,40,40);
    newCloseBtn->show();

    dy += 50;
    if (dy > mDisplayDirList->height() - 100) mDisplayDirList->resize(mDisplayDirList->width(),mDisplayDirList->height()+300);

    //    newRate->setGeometry(195,dy,30,40);
    //    newRate->show();

    //    newSpin->setGeometry(225,dy+5,40,30);
    //    newSpin->show();

    //    newPercent->setGeometry(270,dy,10,40);
    //    newPercent->show();
}

void ImageCombiner::deleteDir(QPushButton* btn)
{
    if (isRunningMainFunction) return;

    int index = 0;
    for (size_t i = 0; i < dirList.size(); ++i)
    {
        if (btn == closeList[i])
        {
            index = i;
            break;
        }
    }

    /* Close all */
    for (size_t i = index; i < dirList.size(); ++i)
    {
        btnList[i]->close();
        closeList[i]->close();
        dy -= 50;
        if (dy < mDisplayDirList->height() - 400) mDisplayDirList->resize(mDisplayDirList->width(),mDisplayDirList->height()-300);
    }

    /* Delete data of index */
    delete btnList[index];
    delete closeList[index];
    delete dirList[index];

    btnList[index] = nullptr;
    closeList[index] = nullptr;
    dirList[index] = nullptr;

    /* Move data after index */
    for (size_t i = index; i < dirList.size() - 1; ++i)
    {
        btnList[i] = btnList[i+1];
        closeList[i] = closeList[i+1];
        dirList[i] = dirList[i+1];
    }
    btnList.pop_back();
    closeList.pop_back();
    dirList.pop_back();

    numDir--;

    /* Re-render */
    for (size_t i = index; i < dirList.size(); ++i) updateDirectoryList(btnList[i], closeList[i]);

    updateExamImage();
}


void ImageCombiner::updateExamImage()
{
    if (dirList.size() == 0)
    {
        mExamImage->clear();
        return;
    }

    vector<QImage> combination;
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";

    for (auto dir:dirList)
    {
        QFileInfoList infoList(dir->entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot));
        if (infoList.size() != 0)
        {
            combination.push_back(QImage(infoList[0].absoluteFilePath()));
        }
    }

    QImage res(combination[0].width(), combination[0].height(), QImage::Format_RGBA8888);
    QPainter* painter = new QPainter(&res);
    for (size_t i = 0; i < combination.size(); ++i)
    {
        painter->drawImage(0, 0, combination[i]);
    }
    painter->end();
    delete painter;

    QPixmap pix = QPixmap::fromImage(res);
    pix = pix.scaled(mExamImage->size(),Qt::KeepAspectRatio);
    mExamImage->setPixmap(pix);
    mExamImage->show();

//    QGraphicsPixmapItem item( QPixmap::fromImage(res));
//    QGraphicsScene* scene = new QGraphicsScene();
//    QGraphicsView* view = new QGraphicsView(scene);
//    scene->addItem(&item);
//    view->show();    QString filename = "xxxxx";

//    QLabel* mExamImage = new QLabel(this);
//    /** set content to show center in label */
//    mExamImage->setAlignment(Qt::AlignCenter);
//    QPixmap pix;

//    /** to check wether load ok */
//    if(pix.load(filename)){
//        /** scale pixmap to fit in label'size and keep ratio of pixmap */
//        pix = pix.scaled(mExamImage->size(),Qt::KeepAspectRatio);
//        mExamImage->setPixmap(pix);
//    }
}

void ImageCombiner::submitInput()
{
    if (isRunningMainFunction) return;

    isRunningMainFunction = true;

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"), "C:/Users/TRONG/Downloads/ImageCombinerTest", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirPath.isEmpty())
    {
        isRunningMainFunction = false;
        return;
    }

    for (auto dir:dirList)
    {
        if (dirPath == dir->absolutePath())
        {
            isRunningMainFunction = false;
            return;
        }
    }

    if (dirList.size() == 0)
    {
        isRunningMainFunction = false;
        return;
    }

    mOutDir = new QDir(dirPath);

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";

    for (auto dir:dirList)
    {
        QFileInfoList* infoList = new QFileInfoList(dir->entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot));
        imagesContainerList.push_back(infoList);
    }

    mNumberOfCombination = 1;
    for (size_t i = 0; i < imagesContainerList.size(); ++i)
    {
        int64_t temp = (int64_t)mNumberOfCombination * (int64_t)imagesContainerList[i]->size();
        if (temp > 999999999)
        {
            return;
        }
        mNumberOfCombination = (int32_t)temp;
    }

    mCurOutputIndex = 0;
    mMaxOutput = mMaxOutputSpin->value();
    mTotal = std::min(mMaxOutput, mNumberOfCombination);
    randomAlg = mMaxOutput < mNumberOfCombination;

    mProgressBar->setRange(0,mTotal);

    generateCombineImages(0, mCombination);
    endGenerating();
}

void ImageCombiner::endGenerating()
{
    /* Clear current data for a submition */
    mCombination.clear();
    imagesContainerList.clear();
    isRunningMainFunction = false;

    QMessageBox msgBox;
    msgBox.setText("Save images successfully!");
    msgBox.exec();
//    msgBox.setInformativeText("Do you want to move to output folder?");
//    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Open);
//    msgBox.setDefaultButton(QMessageBox::Open);
//    const int accept = 0;
//    std::cout << res << endl;
//    if (res == 8192)
//    {
//        QDesktopServices::openUrl(QUrl(mOutDir->absolutePath()));
//    }
}

void ImageCombiner::generateCombineImages(size_t index, vector<QImage>& combination)
{
    if (mCurOutputIndex >= mMaxOutput) return;

    if (index == imagesContainerList.size() - 1)
    {
        /* generate image */
        for (size_t minorIndex = 0; minorIndex < imagesContainerList[index]->size(); ++minorIndex)
        {

            QImage curImage((*imagesContainerList[index])[minorIndex].absoluteFilePath());
            combination.push_back(curImage);
            saveImage(combination);
            combination.pop_back();
            QCoreApplication::processEvents();
            if (mCurOutputIndex >= mMaxOutput) break;
        }
        return;
    }

    for (size_t minorIndex = 0; minorIndex < imagesContainerList[index]->size(); ++minorIndex)
    {
        QImage curImage((*imagesContainerList[index])[minorIndex].absoluteFilePath());
        combination.push_back(curImage);
        generateCombineImages(index + 1, combination);
        combination.pop_back();
    }
}

void ImageCombiner::saveImage(vector<QImage>& combination)
{
    if (randomAlg)
    {
        if (std::rand() % mNumberOfCombination + 1 >  mTotal - mCurOutputIndex)
        {
            mNumberOfCombination--;
            return;
        }
    }

    int n = combination.size();
    if (n == 0) return;

    QImage res(combination[0].width(), combination[0].height(), QImage::Format_RGBA8888);
    QPainter* painter = new QPainter(&res);
    for (size_t i = 0; i < n; ++i)
    {
//        if (std::rand() % 100 + 1 > drawChance[i]) continue;
        painter->drawImage(0, 0, combination[i]);
    }
    painter->end();
    delete painter;

    QString name(to_string(mCurOutputIndex+1).c_str());
    QString format = ".png";
    QString path = mOutDir->absolutePath() + "/" + name + format;
    if (res.save(path)) std::cout << "Save " << path.toStdString() << " successfully !!!" << endl;

    mCurOutputIndex++;
    mProgressBar->setValue(mCurOutputIndex);
    mNumberOfCombination--;
}

void ImageCombiner::startWorkerThread()
{
//    WorkerThread* mWorkerThread = new WorkerThread(this);
//    connect(mWorkerThread, &WorkerThread::resultReady, this, &ImageCombiner::handleResults);
//    connect(mWorkerThread, &WorkerThread::finished, mWorkerThread, &QObject::deleteLater);
//    mWorkerThread->start();
}






































