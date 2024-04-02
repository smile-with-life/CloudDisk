#ifndef WORK_H
#define WORK_H


#include "protocol.h"

#include <QObject>
#include <QThread>

const int maxThread=4;//最大可使用线程数
class TaskWork;
class SendWork;
class ReceiveWork;
class Work : public QObject
{
    Q_OBJECT
private:
    QList<QThread*>m_ThreadList;//线程连接表
    TaskWork *p_TaskWork=nullptr;

public:
    explicit Work(QObject *parent = nullptr);//构造函数

    ~Work();//析构函数

    static Work& getInstance();//获取单例模式

    void TaskStart();//业务开始工作

    void SendStart(const QString CurrentPath, const QString UploadFielPath);//发送开始工作

    void ReceiveStart(const QString SavePath,const QString DownloadPath);//接收开始工作

    void SendFinish();//发送完成

    void ReceiveFinish();//接收完成
signals:
    void connect_server();//业务连接

    void send_running(const QString CurrentPath, const QString UploadFielPath);//发送连接

    void receive_running(const QString SavePath,const QString DownloadPath);//接收连接

    void server_connected();//服务器连接成功信号

    void server_disconnect();//服务器断开连接信号

    void showfinish();

    void logon_request(QString account,QString password);//登录请求信号

    void getcode_request(QString phone_number);//获取验证码信号

    void signup_request(QString username,QString password,QString phone_number,QString code);//注册信号

    void currentpath_request(QString currentpath);//查询当前路径下所有文件请求信号

    void built_dir_request(QString currentpath,QString Dirname);//新建文件夹请求信号

    void copymove_request(const QString SourcePath,const QString DestPath);

    void cutmove_request(const QString SourcePath,const QString DestPath);

    void rename_file_request(const QString CurrentPath,const QString Filename,const QString Newname);

    void delete_file_request(const QString CurrentPath,const QString Filename);

    void logon_respond(unsigned int Status,QString UserName,QString PhoneNumber);//登录回复信号

    void getcode_respond(unsigned int Status);//获取验证码回复

    void signup_respond(unsigned int Status);//注册回复信号

    void forget_respond(unsigned int Status);//忘记密码回复

    void currentpath_respond(unsigned int Status,FILEINFO *FileList,int FileCount);//查询当前路径下文件回复信号

    void built_dir_respond(unsigned int Status);//新建文件夹回复信号

    void rename_file_respond(unsigned int Status);//重命名文件回复信号

    void delete_file_respond(unsigned int Status);//删除文件回复信号

    void copymove_respond(unsigned int Status);//复制移动回复信号

    void cutmove_respond(unsigned int Status);//剪切移动回复信号

    void upload_file_respond(unsigned int Status);//上传文件回复信号

    void download_file_respond(unsigned int Status);//下载文件回复信号
public slots:
    void ConnectServer();//连接服务器

    void Send_Logon_Request(const QString Account,const QString Password);//登录请求

    void Send_Getcode_Request(const QString PhoneNumber);//获取验证码请求

    void Send_Signup_Request(const QString UserName,const QString Password,const QString PhoneNumber,const QString Code);//注册请求

    void Send_CurrentPath_Request(const QString CurrentPath);//查询当前路径下文件请求

    void Send_Built_Dir_Request(const QString CurrentPath,const QString Dirname);//新建文件夹请求

    void Send_CopyMove_Request(const QString SourcePath,const QString DestPath);//文件复制移动请求

    void Send_CutMove_Request(const QString SourcePath,const QString DestPath);//文件剪切移动请求

    void Send_Rename_Request(const QString CurrentPath,const QString Filename,const QString Newname);//文件重命名请求

    void Send_Delete_Request(const QString CurrentPath,const QString Filename);//文件删除请求

    void Send_Upload_File_Request(const QString CurrentPath,const QString UploadPath);//上传文件请求

    void Send_Download_File_Request(const QString SavePath,const QString DownloadPath);//上传文件请求

    void Logon_Respond(unsigned int Status,QString UserName,QString PhoneNumber);

    void Getcode_Respond(unsigned int Status);

    void Signup_Respond(unsigned int Status);

    void Forget_Respond(unsigned int Status);

    void CurrentPath_Respond(unsigned int Status,FILEINFO *FileList,int FileCount);

    void Built_Dir_Respond(unsigned int Status);

    void Rename_File_Respond(unsigned int status);//重命名文件回复处理

    void Delete_File_Respond(unsigned int status);//删除文件回复处理

    void CopyMove_Respond(unsigned int status);//复制移动回复处理

    void CutMove_Respond(unsigned int status);//剪切移动回复处理

    void Upload_file_Respond(unsigned int status);//上传文件回复处理

    void Download_file_Respond(unsigned int status);//下载文件回复处理

    void Server_Online(QObject *myWork);

    void Server_Offline(QObject *myWork);








    //void Upload_File_Request(const QString CurrentPath,const QString UploadFilePath);//上传文件请求


private slots:

};

#endif // WORK_H
