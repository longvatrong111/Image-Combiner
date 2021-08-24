#ifndef IMAGECOMBINER_H
#define IMAGECOMBINER_H

#include <./definition.h>

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
using namespace std;

QT_BEGIN_NAMESPACE
//namespace Ui { class ImageCombiner; }
QT_END_NAMESPACE


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

    /* View */
    QLabel* mExamImage;
    QProgressBar* mProgressBar;

    /* Data */
    vector<QDir*> dirList;
    vector<QFileInfoList*> imagesContainerList;

    /* Dynamic display data */
    vector<QPushButton*> btnList;
    vector<QPushButton*> closeList;
    int32_t dy = 10;

//    vector<QSpinBox*> spinList;
//    vector<QLabel*> rateList;
//    vector<QLabel*> percentList;

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

private:
//    Ui::MainWindow *ui;
    void renderTemplate();
    void renderDirBlock();
    void renderImageBlock();
    void renderControlBlock();
    void renderViewBlock();

    void updateDirectoryList(QPushButton* newBtn, QPushButton* newCloseBtn);
    void updateExamImage();
    void generateCombineImages(size_t index, vector<QImage>& combination);
    void saveImage(vector<QImage>& combination);
    void endGenerating();

    void startWorkerThread();
    void handleResults(const QString &s);

public slots:
    void openDirectory();
    void deleteDir(QPushButton* btn);
    void submitInput();

};
#endif // IMAGECOMBINER_H

/* Woker thread */
class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(ImageCombiner* parent) : QThread()
    {
        mParent = parent;
    }
private:
    void run() override {
        QString result;

        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
private:
    ImageCombiner* mParent;
};
