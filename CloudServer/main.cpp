#include "cloudserver.h"
#include<clouddb.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CloudDB::getInstance().initDB();
    CloudServer w;
    w.show();
    return a.exec();
}
