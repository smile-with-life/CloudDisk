#ifndef OPERATORWIDGET_H
#define OPERATORWIDGET_H

#include "HomePage.h"
#include "PersonalPage.h"
#include <Information.h>
#include <Network.h>
#include <QWidget>

namespace Ui {
class OperatorWidget;
}

class OperatorWidget : public QWidget
{
    Q_OBJECT
private:
    Work *sp_Worker=nullptr;//工作模块
    Information *sp_Info=nullptr;//信息模块
    Network *sp_Network=nullptr;//网络模块

    HomePage *p_HomePage=nullptr;//主页
    PersonalPage *p_PersonalPage=nullptr;//个人中心

public:
    explicit OperatorWidget(QWidget *parent = nullptr);//构造函数

    ~OperatorWidget();//析构函数

    static OperatorWidget& getInstance();//获取单例模式
private:
    void Init();//初始化
private slots:
    void show_HomePage();

    void show_PersonalPage();



private:
    Ui::OperatorWidget *ui;
};

#endif // OPERATORWIDGET_H
