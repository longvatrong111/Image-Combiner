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
    QString description("- Select folders containing body parts.\n- Images in new added folder will overlap previous ones.\n- Background image is always at the bottom.");
    QLabel* descriptionLabel = new QLabel(description, mDirectoryList);
    descriptionLabel->setGeometry(PADDING,5,BWIDTH-(PADDING*2),50);
    descriptionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    descriptionLabel->setWordWrap(true);
//    descriptionLabel->setAlignment(Qt::AlignJustify);

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
    QObject::connect(mAddBackground, SIGNAL (clicked()), this, SLOT(selectBackground()));

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
    storeDirectoryData(dir);

    dirPath.replace(0, dirPath.lastIndexOf('/') + 1, "");
    QPushButton* newBtn = new QPushButton(dirPath, mDisplayDirList);
    QObject::connect(newBtn, &QPushButton::clicked, [=](){ this->renderImageList(newBtn); });
    btnList.push_back(newBtn);

    QPushButton* newCloseBtn = new QPushButton("X", mDisplayDirList);
    QObject::connect(newCloseBtn, &QPushButton::clicked, [=](){ this->deleteDir(newCloseBtn); });
    closeList.push_back(newCloseBtn);

    numDir++;

    renderDirectoryList(newBtn, newCloseBtn);
    updateExamImage();
}

void ImageCombiner::renderDirectoryList(QPushButton* newBtn, QPushButton* newCloseBtn)
{
    cout << dy << endl;
    newBtn->setGeometry(PADDING,dy,BWIDTH-60-(PADDING*2),40);
    newBtn->show();

    newCloseBtn->setGeometry(PADDING+newBtn->width(),dy,40,40);
    newCloseBtn->show();

    dy += 50;
    if (dy > mDisplayDirList->height() - 100) mDisplayDirList->resize(mDisplayDirList->width(),mDisplayDirList->height()+300);
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
        if (dy < mDisplayDirList->height() - 400) mDisplayDirList->resize(mDisplayDirList->width(),std::max(mDisplayDirList->height()-300,mScrollDir->height()));
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
    for (size_t i = index; i < dirList.size(); ++i) renderDirectoryList(btnList[i], closeList[i]);

    updateExamImage();
}

void ImageCombiner::selectBackground()
{
    if (isRunningMainFunction) return;

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select background"), "C:/Users/TRONG/Downloads/ImageCombinerTest", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirPath.isEmpty()) return;

    if (mBackgroundDir)
    {
        mBackgroundBtn->close();
        mBackgroundClose->close();
        delete mBackgroundBtn;
        delete mBackgroundClose;
        delete mBackgroundDir;
    }

    mBackgroundDir = new QDir(dirPath);
    storeDirectoryData(mBackgroundDir);

    dirPath.replace(0, dirPath.lastIndexOf('/') + 1, "");
    mBackgroundBtn = new QPushButton(dirPath, mDisplayDirList);
    QObject::connect(mBackgroundBtn, &QPushButton::clicked, [=](){ this->renderImageListBgr(); });

    mBackgroundClose = new QPushButton("X", mDisplayDirList);
    QObject::connect(mBackgroundClose, &QPushButton::clicked, [=](){ this->deleteBackgroundDir(mBackgroundClose); });

//    renderDirectoryList(mBackgroundBtn, mBackgroundClose);
    dy = 10;
    mBackgroundBtn->setGeometry(PADDING,dy,BWIDTH-60-(PADDING*2),40);
    mBackgroundBtn->show();
    mBackgroundClose->setGeometry(PADDING+mBackgroundBtn->width(),dy,40,40);
    mBackgroundClose->show();
    dy += 50;
    /* Re-render */
    for (size_t i = 0; i < dirList.size(); ++i) renderDirectoryList(btnList[i], closeList[i]);

    updateExamImage();
}

void ImageCombiner::renderImageListBgr()
{
    QString path = mBackgroundDir->absolutePath();
    if (currentDirData)
    {
        currentDirData->clearImages();
        currentDirData = nullptr;
        mDisplayImageList->resize(mDisplayImageList->width(), BHEIGHT);
    }

    auto it = mDataMap.find(path);
    if (it != mDataMap.end()) currentDirData = it->second;
    else return;

    mDisplayImageList->resize(mDisplayImageList->width(), std::max(currentDirData->size()/2*110+10,BHEIGHT));
    currentDirData->renderImages();
}

void ImageCombiner::deleteBackgroundDir(QPushButton* mBackgroundClose)
{
    mBackgroundBtn->close();
    mBackgroundClose->close();
    delete mBackgroundBtn;
    delete mBackgroundClose;
    mBackgroundBtn = nullptr;
    mBackgroundClose = nullptr;

    delete mBackgroundDir;
    mBackgroundDir = nullptr;

    dy = 10;
    for (size_t i = 0; i < dirList.size(); ++i)
    {
        btnList[i]->close();
        closeList[i]->close();
    }
    for (size_t i = 0; i < dirList.size(); ++i) renderDirectoryList(btnList[i], closeList[i]);
    updateExamImage();
}

void ImageCombiner::storeDirectoryData(QDir* dir)
{
    QString key = dir->absolutePath();
    if (mDataMap.find(key) == mDataMap.end()) mDataMap[key] = new DirectoryData(dir, mDisplayImageList);
}

void ImageCombiner::renderImageList(QPushButton* btn)
{
    if (isRunningMainFunction) return;

    int index = 0;
    for (size_t i = 0; i < btnList.size(); ++i)
    {
        if (btn == btnList[i])
        {
            index = i;
            break;
        }
    }

    QString path = dirList[index]->absolutePath();
    if (currentDirData)
    {
        currentDirData->clearImages();
        currentDirData = nullptr;
        mDisplayImageList->resize(mDisplayImageList->width(), BHEIGHT);
    }

    auto it = mDataMap.find(path);
    if (it != mDataMap.end()) currentDirData = it->second;
    else return;

    mDisplayImageList->resize(mDisplayImageList->width(), std::max(currentDirData->size()/2*110+10,BHEIGHT));
    currentDirData->renderImages();
}

void ImageCombiner::updateExamImage()
{
    if (dirList.size() == 0 && mBackgroundDir == nullptr)
    {
        mExamImage->clear();
        return;
    }

    vector<QImage> combination;

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";

    if (mBackgroundDir != nullptr)
    {
        QFileInfoList infoList(mBackgroundDir->entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot));
        if (infoList.size() != 0)
        {
            combination.push_back(QImage(infoList[0].absoluteFilePath()));
        }
    }

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
    pix = pix.scaled(mExamImage->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    mExamImage->setPixmap(pix);
    mExamImage->show();
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
    if (isRunningMainFunction)
    {
        stopFlag = true;
        return;
    }

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

    mProcess->setText("Stop");

    bool res = generateCombineImages(0, mCombination);
    endGenerating(res);
}

void ImageCombiner::endGenerating(bool res)
{
    /* Clear current data for a submition */
    mCombination.clear();
    imagesContainerList.clear();
    isRunningMainFunction = false;
    stopFlag = false;

    QMessageBox msgBox;
    QPixmap pixmap = QPixmap("./app.ico");
    msgBox.setWindowIcon(QIcon(pixmap));

    if (res) msgBox.setText("Save images successfully!");
    else msgBox.setText("The process has been aborted");

    mProcess->setText("Process");

    msgBox.exec();
}

bool ImageCombiner::generateCombineImages(size_t index, vector<QImage>& combination)
{
    if (mCurOutputIndex >= mMaxOutput) return !stopFlag;
    if (stopFlag) return !stopFlag;

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
            if (stopFlag) break;
        }
        return !stopFlag;
    }

    for (size_t minorIndex = 0; minorIndex < imagesContainerList[index]->size(); ++minorIndex)
    {
        QImage curImage((*imagesContainerList[index])[minorIndex].absoluteFilePath());
        combination.push_back(curImage);
        generateCombineImages(index + 1, combination);
        combination.pop_back();
    }

    return !stopFlag;
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






































