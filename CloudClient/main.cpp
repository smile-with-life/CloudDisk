#include "CloudClient.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);//实例化一个应用程序对象
    CloudClient w;//实例化一个登录界面对象
    w.show();//显示登录界面
    return app.exec();//事件循环

}

