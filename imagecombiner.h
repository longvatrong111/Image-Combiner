#ifndef IMAGECOMBINER_H
#define IMAGECOMBINER_H

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

    void generateCombineImages(size_t index, vector<QImage>& combination, vector<int>& drawChance);
    vector<QImage>& getCombination() { return mCombination; }
    vector<int>& getDrawChance() { return mDrawChance; }

private:
    QGraphicsView* mDescription;
    QGraphicsView* mList;
    QGraphicsView* mControl;
    QGraphicsView* mView;

    /* Control block */
    QPushButton* mAddDir;
    QPushButton* mSubmit;
    QProgressBar* mProgressBar;

    /* Data */
    vector<QDir*> dirList;

    /* Display data */
    vector<QPushButton*> btnList;
    vector<QSpinBox*> spinList;
    vector<QPushButton*> closeList;
    vector<QLabel*> rateList;
    vector<QLabel*> percentList;
    int32_t dy = 10;

    QLabel* lbl;

    /* Generation data */
    QDir* mOutDir;
    vector<QFileInfoList*> imagesContainerList;

    vector<QImage> mCombination;
    vector<int> mDrawChance;

    QLabel* mMaxOutputLabel;
    QSpinBox* mMaxOutputSpin;
    int32_t mMaxOutput = 1000000;
    int32_t mNumberOfCombination = 1;
    int32_t mCurOutputIndex;
    int32_t mTotal; /* Min of max output by user or max number of combinations */
    bool randomAlg;

    bool isRunningMainFunction = false;

private:
//    Ui::MainWindow *ui;
    void renderDir(QPushButton* newBtn, QSpinBox* newSpin, QPushButton* newCloseBtn, QLabel* newRate, QLabel* newPercent);
    void renderExamImage();
    void saveImage(vector<QImage>& combination, vector<int>& drawChance);
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
        mParent->generateCombineImages(0, mParent->getCombination(), mParent->getDrawChance());
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
private:
    ImageCombiner* mParent;
};
