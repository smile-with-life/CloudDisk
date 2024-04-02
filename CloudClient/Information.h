#ifndef INFORMATION_H
#define INFORMATION_H

#include <QString>

struct LOGONINFO
{
    QString account;//账户
    QString password;//密码
    bool auto_logon;//自动登录
    bool remember_password;//记住密码
};
const int LogonInfoSize = 10;
const QString PasswordNull("******");
class Information
{
private://私有成员
    //开发信息
    QString m_Organization;//组织名称
    QString m_AppName;//应用程序名称
    QString m_AppVersion;//应用程序版本号
    QString m_Director;//项目负责人
    QString m_Kit;//编译套件
    //历史登录信息
    LOGONINFO m_LogonInfoList[LogonInfoSize];//历史登录记录列表
    int m_LogonInfoCount=0;//历史登录记录数量
    //用户信息
    QString m_UserName;//用户名
    QString m_PhoneNumber;//手机号
    QString m_UploadFielPath;//上传文件路径
    //软件设置信息
public://公有方法
    explicit Information();//构造函数

    ~Information();//析构函数

    static Information& getInstance();//获取单例模式

    void SetUserInfo(const QString username,const QString phone_number);//设置用户信息

    void SetUploadFilePath(const QString UploadFielPath);

    LOGONINFO &getLogonInfoList(int index);//获取历史登录信息

    int &getLogonInfoCount();//获取历史登录信息的数量

    QString &getUserName();//获取用户名

    QString &getPhoneNumber();//获取手机号

    QString &getUploadFielPath();//获取上传文件路径


private://私有方法
    void LoadDevelopInfo();//加载开发信息

    void LoadLogonInfo();//加载历史登录信息
};

#endif // INFORMATION_H
