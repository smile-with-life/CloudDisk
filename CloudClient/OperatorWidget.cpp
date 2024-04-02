#include "OperatorWidget.h"

#include "ui_OperatorWidget.h"

OperatorWidget::OperatorWidget(QWidget *parent)//构造函数
    : QWidget(parent)
    , ui(new Ui::OperatorWidget)
{
    ui->setupUi(this);
    Init();//初始化
}

OperatorWidget::~OperatorWidget()//析构函数
{
    delete ui;
    if(p_HomePage!=nullptr)
        p_HomePage->deleteLater();
    if(p_PersonalPage!=nullptr)
        p_PersonalPage->deleteLater();
}

void OperatorWidget::Init()//初始化
{
    //成员初始化
    sp_Worker=&Work::getInstance();//关联通信模块
    sp_Network=&Network::getInstance();//关联网络模块
    sp_Info=&Information::getInstance();//关联信息模块
    p_HomePage = new HomePage;
    p_PersonalPage = new PersonalPage;
    ui->stackedWidget->insertWidget(0,p_HomePage);
    ui->stackedWidget->insertWidget(1,p_PersonalPage);
    connect(ui->pushButton_homepage,&QPushButton::clicked,this,&OperatorWidget::show_HomePage);
    connect(ui->pushButton_personalpage,&QPushButton::clicked,this,&OperatorWidget::show_PersonalPage);
    //界面设置
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);//禁止窗口最大化
    setFixedSize(this->width(),this->height());//禁止拖动窗口大小
    ui->label_username->setText(sp_Info->getUserName());
    show_HomePage();

}

OperatorWidget &OperatorWidget::getInstance()//获取单例模式
{
    static OperatorWidget instance;
    return instance;
}

void OperatorWidget::show_HomePage()
{
    ui->stackedWidget->setCurrentIndex(0);
    p_HomePage->Refresh();
}

void OperatorWidget::show_PersonalPage()
{
    ui->stackedWidget->setCurrentIndex(1);

}

