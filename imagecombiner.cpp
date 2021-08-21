#include "imagecombiner.h"
#include "./ui_imagecombiner.h"

#include <iostream>
#include <cstdlib>
#include <QCoreApplication>

ImageCombiner::ImageCombiner(QWidget *parent)
    : QWidget(parent),
      mDescription(new QGraphicsView(this)),
      mList(new QGraphicsView(this)),
      mControl(new QGraphicsView(this)),
      mView(new QGraphicsView(this))
{
    setWindowIcon(QIcon("./app.ico"));
    setWindowTitle("Image Combiner");
    setFixedSize(1000, 800);
    mDescription->setGeometry(20,20,360,80);
    mList->setGeometry(20,120,360,540);
    mControl->setGeometry(20,680,360,100);
    mView->setGeometry(400,20,580,760);

    QLabel* description = new QLabel("Select folders containing body parts.", mDescription);
    QLabel* description2 = new QLabel("Photos in the folders below will overlap photos in the", mDescription);
    QLabel* description3 = new QLabel("previous ones.", mDescription);
    description->setGeometry(20,10,320,20);
    description2->setGeometry(20,30,320,20);
    description3->setGeometry(20,50,320,20);

    /* mView */
    QLabel* headlineView = new QLabel("EXAMPLE COMBINATION", mView);
    headlineView->setGeometry(215,20,150,40);
    headlineView->setStyleSheet("font-weight: bold; color: blue");

    /* Control block */
    mAddDir = new QPushButton("Add Directory", mControl);
    mAddDir->setGeometry(10,15,90,30);
    QObject::connect(mAddDir, SIGNAL (clicked()), this, SLOT(openDirectory()));

    mSubmit = new QPushButton("Process", mControl);
    mSubmit->setGeometry(280,15,70,70);
    QObject::connect(mSubmit, SIGNAL (clicked()), this, SLOT(submitInput()));

    mMaxOutputLabel = new QLabel("Max images: ", mControl);
    mMaxOutputLabel->setGeometry(115,15,80,30);

    mMaxOutputSpin = new QSpinBox(mControl);
    mMaxOutputSpin->setRange(0,999999999);
    mMaxOutputSpin->setValue(1000000);
    mMaxOutputSpin->setGeometry(190,15,80,30);
//    QObject::connect(mMaxOutputSpin, QOverload<int>::of(&QSpinBox::valueChanged),
//                     [=](int i){ mMaxOutput = i; });

    mProgressBar = new QProgressBar(mControl);
    mProgressBar->setGeometry(10,55,260,30);
    mProgressBar->setRange(0,100);
    mProgressBar->setTextVisible(false);


    lbl = new QLabel(mView);
    lbl->setGeometry(50,125,500,500);
    lbl->setAlignment(Qt::AlignCenter);
}

ImageCombiner::~ImageCombiner()
{
    /* Do something */
//    delete ui;
}

void ImageCombiner::openDirectory()
{
    if (isRunningMainFunction) return;

    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Add Directory"), "C:/Users/TRONG/Downloads/ImageCombinerTest", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
//    std::cout << dirPath.toStdString() << endl;
    if (dirPath.isEmpty()) return;

    QDir* dir = new QDir(dirPath);
    dirList.push_back(dir);

    dirPath.replace(0, dirPath.lastIndexOf('/') + 1, "");
    QPushButton* newBtn = new QPushButton(dirPath, mList);
    btnList.push_back(newBtn);

    QSpinBox* newSpin = new QSpinBox(mList);
    spinList.push_back(newSpin);
    newSpin->setRange(0, 100);
    newSpin->setValue(100);

    QPushButton* newCloseBtn = new QPushButton("X", mList);
    closeList.push_back(newCloseBtn);

    QLabel* newRate = new QLabel("Rate: ", mList);
    rateList.push_back(newRate);
    QLabel* newPercent = new QLabel("%", mList);
    percentList.push_back(newPercent);


    renderDir(newBtn, newSpin, newCloseBtn, newRate, newPercent);
    QObject::connect(newCloseBtn, &QPushButton::clicked, [=](){ this->deleteDir(newCloseBtn); });
    renderExamImage();
}

void ImageCombiner::renderDir(QPushButton* newBtn, QSpinBox* newSpin, QPushButton* newCloseBtn, QLabel* newRate, QLabel* newPercent)
{
    newBtn->setGeometry(20,dy+5,160,30);
    newBtn->show();

    newRate->setGeometry(195,dy,30,40);
    newRate->show();

    newSpin->setGeometry(225,dy+5,40,30);
    newSpin->show();

    newPercent->setGeometry(270,dy,10,40);
    newPercent->show();

    newCloseBtn->setGeometry(300,dy,40,40);
    newCloseBtn->show();

    dy += 50;
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
        spinList[i]->close();
        closeList[i]->close();
        rateList[i]->close();
        percentList[i]->close();
        dy -= 50;
    }

    /* Delete data of index */
    delete btnList[index];
    delete spinList[index];
    delete closeList[index];
    delete dirList[index];
    delete rateList[index];
    delete percentList[index];
    btnList[index] = nullptr;
    spinList[index] = nullptr;
    closeList[index] = nullptr;
    dirList[index] = nullptr;
    rateList[index] = nullptr;
    percentList[index] = nullptr;

    /* Move data after index */
    for (size_t i = index; i < dirList.size() - 1; ++i)
    {
        btnList[i] = btnList[i+1];
        spinList[i] = spinList[i+1];
        closeList[i] = closeList[i+1];
        dirList[i] = dirList[i+1];
        rateList[i] = rateList[i+1];
        percentList[i] = percentList[i+1];
    }
    btnList.pop_back();
    spinList.pop_back();
    closeList.pop_back();
    dirList.pop_back();
    rateList.pop_back();
    percentList.pop_back();

    /* Re-render */
    for (size_t i = index; i < dirList.size(); ++i) renderDir(btnList[i], spinList[i], closeList[i], rateList[i], percentList[i]);

    renderExamImage();
}


void ImageCombiner::renderExamImage()
{
    if (dirList.size() == 0) return;

    vector<QImage> combination;
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";

    for (auto dir:dirList)
    {
        QFileInfoList infoList(dir->entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot));
        if (infoList.size() != 0)
        {
//            std::cout << "render" << endl;
            combination.push_back(QImage(infoList[0].absoluteFilePath()));
        }
//        imagesContainerList.push_back(infoList);
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
    pix = pix.scaled(lbl->size(),Qt::KeepAspectRatio);
    lbl->setPixmap(pix);
    lbl->show();

//    QGraphicsPixmapItem item( QPixmap::fromImage(res));
//    QGraphicsScene* scene = new QGraphicsScene();
//    QGraphicsView* view = new QGraphicsView(scene);
//    scene->addItem(&item);
//    view->show();    QString filename = "xxxxx";

//    QLabel* lbl = new QLabel(this);
//    /** set content to show center in label */
//    lbl->setAlignment(Qt::AlignCenter);
//    QPixmap pix;

//    /** to check wether load ok */
//    if(pix.load(filename)){
//        /** scale pixmap to fit in label'size and keep ratio of pixmap */
//        pix = pix.scaled(lbl->size(),Qt::KeepAspectRatio);
//        lbl->setPixmap(pix);
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
//        for (int i = 0; i < infoList->size(); ++i) std::cout << (*infoList)[i].absoluteFilePath().toStdString() << endl;
        imagesContainerList.push_back(infoList);
    }

    mNumberOfCombination = 1;
    for (size_t i = 0; i < imagesContainerList.size(); ++i)
    {
        mDrawChance.push_back(spinList[i]->value());
        mNumberOfCombination *= imagesContainerList[i]->size();
    }

    mCurOutputIndex = 0;
    mMaxOutput = mMaxOutputSpin->value();
    mTotal = std::min(mMaxOutput, mNumberOfCombination);
    randomAlg = mMaxOutput < mNumberOfCombination;

//    startWorkerThread();
    generateCombineImages(0, getCombination(), getDrawChance());
    QString dummy;
    handleResults(dummy);
}

void ImageCombiner::startWorkerThread()
{
    WorkerThread* mWorkerThread = new WorkerThread(this);
    connect(mWorkerThread, &WorkerThread::resultReady, this, &ImageCombiner::handleResults);
    connect(mWorkerThread, &WorkerThread::finished, mWorkerThread, &QObject::deleteLater);
    mWorkerThread->start();
}

void ImageCombiner::handleResults(const QString &s)
{
    /* Clear current data for a submition */
    mCombination.clear();
    mDrawChance.clear();
    imagesContainerList.clear();
    isRunningMainFunction = false;

    QMessageBox msgBox;
    msgBox.setText("Success!");
    msgBox.setInformativeText("Do you want to move to output folder?");
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Open);
    msgBox.setDefaultButton(QMessageBox::Open);
    auto res = msgBox.exec();
//    const int accept = 0;
//    std::cout << res << endl;
    if (res == 8192)
    {
        QDesktopServices::openUrl(QUrl(mOutDir->absolutePath()));
    }
}

void ImageCombiner::generateCombineImages(size_t index, vector<QImage>& combination, vector<int>& drawChance)
{
    if (mCurOutputIndex >= mMaxOutput) return;

    if (index == imagesContainerList.size() - 1)
    {
        /* generate image */
        for (size_t minorIndex = 0; minorIndex < imagesContainerList[index]->size(); ++minorIndex)
        {

            QImage curImage((*imagesContainerList[index])[minorIndex].absoluteFilePath());
            combination.push_back(curImage);
            saveImage(combination, drawChance);
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
        generateCombineImages(index + 1, combination, drawChance);
        combination.pop_back();
    }
}

void ImageCombiner::saveImage(vector<QImage>& combination, vector<int>& drawChance)
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
        if (std::rand() % 100 + 1 > drawChance[i]) continue;
        painter->drawImage(0, 0, combination[i]);
    }
    painter->end();
    delete painter;

    QString name(to_string(mCurOutputIndex).c_str());
    QString format = ".png";
    QString path = mOutDir->absolutePath() + "/" + name + format;
    if (res.save(path)) std::cout << "Save " << path.toStdString() << " successfully !!!" << endl;
//    res.save(path);
//    std::cout << "Before" << endl;
    mCurOutputIndex++;
    mProgressBar->setValue(mCurOutputIndex*100/mTotal);
    mNumberOfCombination--;
//    std::cout << "After" << endl;
}








































