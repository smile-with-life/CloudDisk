#include "Information.h"

#include <QCoreApplication>
#include <QFile>
#include <QMessageBox>
#include <QSettings>

Information::Information()//构造函数
{
    LoadDevelopInfo();
    LoadLogonInfo();

}

Information::~Information()//析构函数
{

}

Information &Information::getInstance()//获取单例模式
{
    static Information instance;
    return instance;
}

void Information::LoadDevelopInfo()//加载开发信息
{
    QSettings setting(":/cloud.ini" , QSettings::IniFormat);
    if(QFile::exists(":/cloud.ini"))
    {
        m_Organization=setting.value("Organization", "组织名称").toString();
        //qDebug()<<m_Organization;
        m_AppName = setting.value("AppName", "应用程序名称").toString();
        //qDebug()<<m_AppName;
        m_AppVersion = setting.value("Version", "版本号").toString();
        //qDebug()<<m_AppVersion;
        m_Director = setting.value("Director", "项目负责人").toString();
        //qDebug()<<m_Director;
        m_Kit = setting.value("Kit", "编译套件").toString();
        //qDebug()<<m_Kit;
    }
    else
    {
        QMessageBox::critical(NULL,"打开配置文件","打开配置文件失败");
    }

}

void Information::LoadLogonInfo()//加载历史登录信息
{
    QString fileName = QCoreApplication::applicationDirPath() + "/LoadInfo.txt";
    QFile file(fileName);
    if(file.exists())//打开方式：只读
    {
        file.open(QIODeviceBase::ReadOnly|QFile::Text);
        while(!file.atEnd())
        {
            m_LogonInfoCount++;
            if(m_LogonInfoCount>10)
                break;
            QByteArray data=file.readLine();
            QString strData=data.toStdString().c_str();//把字节数组变为字符串
            QStringList strList= strData.split(" ");//以空格为分隔符把字符串分为两个字符串并把分完的字符串放到列表中
            m_LogonInfoList[m_LogonInfoCount-1].account=strList.at(0);
            //qDebug()<<m_LogonInfoList[m_LogonInfoCount-1].account;
            m_LogonInfoList[m_LogonInfoCount-1].password=strList.at(1);
            //qDebug()<<m_LogonInfoList[m_LogonInfoCount-1].password;
            m_LogonInfoList[m_LogonInfoCount-1].auto_logon=strList.at(2).toInt();
            //qDebug()<<m_LogonInfoList[m_LogonInfoCount-1].auto_logon;
            m_LogonInfoList[m_LogonInfoCount-1].remember_password=strList.at(3).toInt();
            //qDebug()<<m_LogonInfoList[m_LogonInfoCount-1].remember_password;
        }
    }
    else
    {
        file.open(QIODeviceBase::WriteOnly|QFile::Text);
        file.close();
    }

}

void Information::SetUserInfo(const QString username,const QString phone_number)//设置用户信息
{
    m_UserName=username;//用户名
    m_PhoneNumber=phone_number;//手机号

}

void Information::SetUploadFilePath(const QString UploadFielPath)
{
    m_UploadFielPath=UploadFielPath;
}

LOGONINFO &Information::getLogonInfoList(int index)//获取历史登录信息
{
    return m_LogonInfoList[index];
}

int &Information::getLogonInfoCount()//获取历史登录信息的数量
{
    return m_LogonInfoCount;
}

QString &Information::getUserName()//获取用户名
{
    return m_UserName;
}

QString &Information::getPhoneNumber()//获取手机号
{
    return m_PhoneNumber;
}

QString &Information::getUploadFielPath()
{
    return m_UploadFielPath;
}
