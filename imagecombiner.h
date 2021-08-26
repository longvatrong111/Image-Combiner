#ifndef IMAGECOMBINER_H
#define IMAGECOMBINER_H

#include <./definition.h>
//#include <./directoryData.h>

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QAbstractScrollArea>
#include <QPushButton>
#include <QFileDialog>
#include <QUrl>
#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QImage>
#include <QPainter>
#include <QLineEdit>
#include <QSpinBox>
#include <QProgressBar>
#include <QThread>
#include <QLabel>
#include <QMessageBox>
#include <QDesktopServices>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QIcon>
#include <QScrollArea>

#include <QFileSystemWatcher>
#include <QDesktopServices>

#include <vector>
#include <map>
#include <iostream>
using namespace std;

QT_BEGIN_NAMESPACE
//namespace Ui { class ImageCombiner; }
QT_END_NAMESPACE

/* Woker thread */
class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QFileInfoList* fileInfoList, vector<QLabel*>& displayImageList) : QThread()
    {
//        mParent = parent;
        mFileInfoList = fileInfoList;
        mDisplayImageList = displayImageList;
    }
private:
    void run() override {
//        QString result;
//        emit resultReady(result);
        int32_t n = mFileInfoList->size();
        for (int i = 0; i < n; ++i)
        {
            QLabel* displayImg = mDisplayImageList[i];
            QPixmap image((*mFileInfoList)[i].absoluteFilePath());
            image = image.scaled(displayImg->size(),Qt::KeepAspectRatio);
            displayImg->setPixmap(image);
        }
    }
signals:
    void resultReady(const QString &s);
private:
//    ImageCombiner* mParent;
//    DirectoryData* mParent;
    QFileInfoList* mFileInfoList;
    vector<QLabel*> mDisplayImageList;
};

class DirectoryData : public QObject
{
    Q_OBJECT
public:
    DirectoryData(QDir* dir, QWidget* parent)
    {
        mPath = dir->absolutePath();

        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.bmp";
        mFileInfoList = new QFileInfoList(dir->entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot));

        num = mFileInfoList->size();
        for (int i = 0; i < num; ++i)
        {
            QLabel* displayImg = new QLabel(parent);
            displayImg->resize(150,150);
            displayImg->setAlignment(Qt::AlignCenter);
            mDisplayImageList.push_back(displayImg);
//            QPixmap image((*mFileInfoList)[i].absoluteFilePath());
//            image = image.scaled(displayImg->size(),Qt::KeepAspectRatio);
//            displayImg->setPixmap(image);

            QSpinBox* chance = new QSpinBox(parent);
            chance->setRange(0,100);
            chance->setValue(100);
            mChanceList.push_back(chance);

            QLabel* text1 = new QLabel("Rate", parent);
            mRate.push_back(text1);
            QLabel* text2 = new QLabel("%", parent);
            mPercent.push_back(text2);

            displayImg->setGeometry(PADDING/2,PADDING/2+dy,150,150);
            text1->setGeometry(PADDING+150,PADDING/2+dy+60,30,30);
            chance->setGeometry(PADDING*3/2+180,PADDING/2+dy+60,60,30);
            text2->setGeometry(PADDING*2+240,PADDING/2+dy+60,30,30);

            dy += 170;
        }

//        std::cout << "store ok" << endl;
        startWorkerThread(mFileInfoList, mDisplayImageList);
    }

    ~DirectoryData()
    {
        delete mFileInfoList;
    }

    void renderImages ()
    {
        for (int i = 0; i < num; ++i)
        {
            mDisplayImageList[i]->show();
            mChanceList[i]->show();
            mRate[i]->show();
            mPercent[i]->show();
        }
    }

    void clearImages ()
    {
        for (int i = 0; i < num; ++i)
        {
            mDisplayImageList[i]->close();
            mChanceList[i]->close();
            mRate[i]->close();
            mPercent[i]->close();
        }
    }

    void clearAndDeleteImages(); // Delete from mImageList when the folder is removed from mDirectoryList.
    void startWorkerThread(QFileInfoList* mFileInfoList, vector<QLabel*>& mDisplayImageList)
    {
        WorkerThread* mWorkerThread = new WorkerThread(mFileInfoList, mDisplayImageList);
    //    connect(mWorkerThread, &WorkerThread::resultReady, this, &DirectoryData::handleResults);
        connect(mWorkerThread, &WorkerThread::finished, mWorkerThread, &QObject::deleteLater);
        mWorkerThread->start();
    }

    int size() {return num;};
    QString getPath() {return mPath;};

private:
    QString mPath;
    QFileInfoList* mFileInfoList;
//    //* imagesList;
//    vector<QPixmap*> mImageList;
    vector<QLabel*> mDisplayImageList;
    vector<QSpinBox*> mChanceList;
    vector<QLabel*> mRate;
    vector<QLabel*> mPercent;

    int32_t num;
    int32_t dy = 0;
};

class ImageCombiner : public QWidget
{
    Q_OBJECT

public:
    ImageCombiner(QWidget* parent = nullptr);
    ~ImageCombiner();

private:
    QGraphicsView* mDirectoryList;
    QGraphicsView* mImageList;
    QGraphicsView* mView;
    QGraphicsView* mControl;

    int32_t numDir = 0;
    vector<int> numImage;

    /* Directory list area */
    QScrollArea* mScrollDir;
    QWidget* mDisplayDirList;
    QWidget* mButtonsArea;
    QPushButton* mAddDir;
    QPushButton* mAddBackground;

    /* Image list area */
    QScrollArea* mScrollImage;
    QWidget* mDisplayImageList;
    DirectoryData* currentDirData = nullptr;

    /* View */
    QLabel* mExamImage;
    QProgressBar* mProgressBar;

    /* Data */
    vector<QDir*> dirList;
    vector<QFileInfoList*> imagesContainerList;
    QDir* mBackgroundDir = nullptr;
    map<QString, DirectoryData*> mDataMap;

    /* Dynamic display data */
    vector<QPushButton*> btnList;
    vector<QPushButton*> closeList;
    QPushButton* mBackgroundBtn;
    QPushButton* mBackgroundClose;
    int32_t dy = 10;

    /* Data for processing */
    QDir* mOutDir;
    vector<QImage> mCombination;
//    vector<int> mDrawChance;

    /* Number of output */
    QPushButton* mProcess;

    QSpinBox* mMaxOutputSpin;
    int32_t mMaxOutput = 1000000; /* mMaxOutputSpin.value(), default = 1000000 */

    int32_t mNumberOfCombination = 1;
    int32_t mCurOutputIndex;
    int32_t mTotal; /* Min of max output by user or max number of combinations */
    bool randomAlg;

    bool isRunningMainFunction = false;
    bool stopFlag = false;

private:
//    Ui::MainWindow *ui;
    void renderTemplate();
    void renderDirBlock();
    void renderImageBlock();
    void renderControlBlock();
    void renderViewBlock();

    void renderDirectoryList(QPushButton* newBtn, QPushButton* newCloseBtn);
    void updateExamImage();
    void storeDirectoryData(QDir* dir);
    bool generateCombineImages(size_t index, vector<QImage>& combination);
    void saveImage(vector<QImage>& combination);
    void endGenerating(bool res);

    void startWorkerThread();
    void handleResults(const QString &s);

public slots:
    void selectBackground();
    void deleteBackgroundDir(QPushButton* btn);
    void openDirectory();
    void deleteDir(QPushButton* btn);
    void renderImageList(QPushButton* btn);
    void renderImageListBgr();
    void submitInput();

};
#endif // IMAGECOMBINER_H

//void DirectoryData::startWorkerThread(QFileInfoList* mFileInfoList, vector<QLabel*>& mDisplayImageList)
//{
//    WorkerThread* mWorkerThread = new WorkerThread(mFileInfoList, mDisplayImageList);
////    connect(mWorkerThread, &WorkerThread::resultReady, this, &DirectoryData::handleResults);
//    connect(mWorkerThread, &WorkerThread::finished, mWorkerThread, &QObject::deleteLater);
//    mWorkerThread->start();
//}
