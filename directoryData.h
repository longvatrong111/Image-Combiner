#ifndef DIRECTORYDATA_H
#define DIRECTORYDATA_H

#endif // DIRECTORYDATA_H

#include <QObject>
#include <QWidget>
#include <QAbstractScrollArea>
#include <QPushButton>
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
#include <QString>

#include <vector>
#include <map>
using namespace std;

class DirectoryData : public QObject
{
    Q_OBJECT
public:
    DirectoryData(QDir* dir)
    {
        mPath = dir->absolutePath();

        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.bmp";
        mFileInfoList = new QFileInfoList(dir->entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot));

        num = mFileInfoList->size();
        for (int i = 0; i < num; ++i)
        {
            QPixmap* image = new QPixmap((*mFileInfoList)[i].absoluteFilePath());
            mImageList.push_back(image);

            QSpinBox* chance = new QSpinBox();
            chance->setRange(0,100);
            chance->setValue(100);
            mChanceList.push_back(chance);
        }
    }

    ~DirectoryData()
    {
        delete mFileInfoList;
    }

    void renderImages(QWidget* parent)
    {
        for (int i = 0; i < num; ++i)
        {
//            QLabel* img = new Qlabel();
//            QLabel* img = new Qlabel();
//            QLabel* img = new Qlabel();
        }
    }
private:
    QString mPath;
    QFileInfoList* mFileInfoList;
    //* imagesList;
    vector<QPixmap*> mImageList;
    vector<QSpinBox*> mChanceList;

    int32_t num;
};
