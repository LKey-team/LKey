#include "lkey.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LKey w;
    w.show();
    return a.exec();
}
