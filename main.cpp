#include "commonheader.h"

#include <QApplication>
#include <QPictureFormatPlugin>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
