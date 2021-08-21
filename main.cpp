#include "imagecombiner.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ImageCombiner w;
    w.show();

    return a.exec();
}
