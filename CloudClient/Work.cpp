#include "TaskWork.h"
#include "SendWork.h"
#include "ReceiveWork.h"
#include "Work.h"

Work::Work(QObject *parent)
    : QObject{parent}
{

}

Work::~Work()
{

}

Work& Work::getInstance()//获取单例模式
{
    static Work instance;
    return instance;
}

void Work::TaskStart()
{
    qDebug()<<"工作线程创建";
    QThread *TaskThread=new QThread;//申请创建一个工作线程
    m_ThreadList.append(TaskThread);//加入线程连接表中
    p_TaskWork=new TaskWork;//新建Task任务
    connect(this,&Work::connect_server,p_TaskWork,&TaskWork::ConnectServer);
    p_TaskWork->moveToThread(TaskThread);//将Task任务加入工作线程中
    TaskThread->start();//启动工作线程
    emit connect_server();
    connect(this,&Work::logon_request,p_TaskWork,&TaskWork::Send_Logon_Request);
    connect(this,&Work::getcode_request,p_TaskWork,&TaskWork::Send_Getcode_Request);
    connect(this,&Work::signup_request,p_TaskWork,&TaskWork::Send_Signup_Request);
    connect(this,&Work::currentpath_request,p_TaskWork,&TaskWork::Send_CurrentPath_Request);
    connect(this,&Work::built_dir_request,p_TaskWork,&TaskWork::Send_Built_Dir_Request);
    connect(this,&Work::copymove_request,p_TaskWork,&TaskWork::Send_CopyMove_Request);
    connect(this,&Work::cutmove_request,p_TaskWork,&TaskWork::Send_CutMove_Request);
    connect(this,&Work::rename_file_request,p_TaskWork,&TaskWork::Send_Rename_Request);
    connect(this,&Work::delete_file_request,p_TaskWork,&TaskWork::Send_Delete_Request);

    connect(p_TaskWork,&TaskWork::server_connected,this,&Work::Server_Online);
    connect(p_TaskWork,&TaskWork::server_disconnect,this,&Work::Server_Offline);

    connect(p_TaskWork,&TaskWork::logon_respond,this,&Work::Logon_Respond);
    connect(p_TaskWork,&TaskWork::getcode_respond,this,&Work::Getcode_Respond);
    connect(p_TaskWork,&TaskWork::signup_respond,this,&Work::Signup_Respond);
    connect(p_TaskWork,&TaskWork::forget_respond,this,&Work::Forget_Respond);
    connect(p_TaskWork,&TaskWork::currentpath_respond,this,&Work::CurrentPath_Respond);
    connect(p_TaskWork,&TaskWork::built_dir_respond,this,&Work::Built_Dir_Respond);
    connect(p_TaskWork,&TaskWork::rename_file_respond,this,&Work::Rename_File_Respond);
    connect(p_TaskWork,&TaskWork::delete_file_respond,this,&Work::Delete_File_Respond);
    connect(p_TaskWork,&TaskWork::copymove_respond,this,&Work::CopyMove_Respond);
    connect(p_TaskWork,&TaskWork::cutmove_respond,this,&Work::CutMove_Respond);



}

void Work::SendStart(const QString CurrentPath, const QString UploadFielPath)
{
    if(m_ThreadList.size()<maxThread)
    {
        QThread *SendThread=new QThread;//申请创建一个工作线程
        m_ThreadList.append(SendThread);//加入线程连接表中
        SendWork *pSendWork=new SendWork;//新建Send任务
        qDebug()<<pSendWork;
        connect(this,&Work::send_running,pSendWork,&SendWork::Running);
        pSendWork->moveToThread(SendThread);//将Send任务加入工作线程中
        SendThread->start();//启动工作线程
        emit send_running(CurrentPath,UploadFielPath);
        disconnect(this,&Work::send_running,0,0);
        connect(pSendWork,&SendWork::server_connected,this,&Work::Server_Online);
        connect(pSendWork,&SendWork::server_disconnect,this,&Work::Server_Offline);
        connect(pSendWork,&SendWork::upload_file_respond,this,&Work::Upload_file_Respond);
    }
    else
    {
        qDebug()<<"线程列表已满,请等待";
    }

}

void Work::ReceiveStart(const QString SavePath,const QString DownloadPath)
{
    if(m_ThreadList.size()<maxThread)
    {
        QThread *ReceiveThread=new QThread;//申请创建一个工作线程
        m_ThreadList.append(ReceiveThread);//加入线程连接表中
        ReceiveWork *pReceiveWork=new ReceiveWork;//新建Task任务
        connect(this,&Work::receive_running,pReceiveWork,&ReceiveWork::Running);
        pReceiveWork->moveToThread(ReceiveThread);//将Task任务加入工作线程中
        ReceiveThread->start();//启动工作线程
        emit receive_running(SavePath,DownloadPath);
        disconnect(this,&Work::receive_running,0,0);
        connect(pReceiveWork,&ReceiveWork::server_connected,this,&Work::Server_Online);
        connect(pReceiveWork,&ReceiveWork::server_disconnect,this,&Work::Server_Offline);
        connect(pReceiveWork,&ReceiveWork::download_file_respond,this,&Work::Download_file_Respond);
    }
    else
    {
        qDebug()<<"线程列表已满,请等待";
    }

}

void Work::ConnectServer()
{
    TaskStart();
}

void Work::Send_Logon_Request(const QString Account, const QString Password)//登录请求处理
{
    emit logon_request(Account,Password);
}

void Work::Send_Getcode_Request(const QString PhoneNumber)//获取验证码请求处理
{
    emit getcode_request(PhoneNumber);
}

void Work::Send_Signup_Request(const QString UserName, const QString Password, const QString PhoneNumber, const QString Code)//注册请求处理
{
    emit signup_request(UserName,Password,PhoneNumber,Code);
}

void Work::Send_CurrentPath_Request(const QString CurrentPath)//查询当前路径下文件请求处理
{
    emit currentpath_request(CurrentPath);
}

void Work::Send_Built_Dir_Request(const QString CurrentPath, const QString Dirname)//新建文件夹请求处理
{
    emit built_dir_request(CurrentPath,Dirname);
}

void Work::Send_CopyMove_Request(const QString SourcePath,  const QString DestPath)
{
    emit copymove_request(SourcePath,DestPath);
}

void Work::Send_CutMove_Request(const QString SourcePath, const QString DestPath)
{
    emit cutmove_request(SourcePath,DestPath);
}

void Work::Send_Rename_Request(const QString CurrentPath, const QString Filename,const QString Newname)
{
    emit rename_file_request(CurrentPath,Filename,Newname);
}

void Work::Send_Delete_Request(const QString CurrentPath, const QString Filename)
{
    emit delete_file_request(CurrentPath,Filename);
}

void Work::Send_Upload_File_Request(const QString CurrentPath, const QString UploadFielPath)
{
    SendStart(CurrentPath,UploadFielPath);
}

void Work::Send_Download_File_Request(const QString SavePath, const QString DownloadPath)
{
    ReceiveStart(SavePath,DownloadPath);
}

void Work::Logon_Respond(unsigned int Status, QString UserName, QString PhoneNumber)
{
    emit logon_respond(Status,UserName,PhoneNumber);
}

void Work::Getcode_Respond(unsigned int Status)
{
    emit getcode_respond(Status);
}

void Work::Signup_Respond(unsigned int Status)
{
    emit signup_respond(Status);
}

void Work::Forget_Respond(unsigned int Status)
{
    emit forget_respond(Status);
}

void Work::CurrentPath_Respond(unsigned int Status, FILEINFO *FileList, int FileCount)
{
    emit currentpath_respond(Status,FileList,FileCount);
}

void Work::Built_Dir_Respond(unsigned int Status)
{
    emit built_dir_respond(Status);
}

void Work::Rename_File_Respond(unsigned int Status)
{
    emit rename_file_respond(Status);

}

void Work::Delete_File_Respond(unsigned int Status)
{
    emit delete_file_respond(Status);
}

void Work::CopyMove_Respond(unsigned int Status)
{
    emit copymove_respond(Status);
}

void Work::CutMove_Respond(unsigned int Status)
{
    emit cutmove_respond(Status);
}

void Work::Upload_file_Respond(unsigned int Status)
{
    emit upload_file_respond(Status);
}

void Work::Download_file_Respond(unsigned int Status)
{
    emit download_file_respond(Status);
}

void Work::Server_Online(QObject *myWork)
{
    if(myWork->metaObject()->className()==QStringLiteral("TaskWork"))
    {
        qDebug()<<"TaskWork connected Server";
    }
    if(myWork->metaObject()->className()==QStringLiteral("SendWork"))
    {
        qDebug()<<"SendWork connected Server";
    }
    if(myWork->metaObject()->className()==QStringLiteral("ReceiveWork"))
    {
        qDebug()<<"ReceiveWork connected Server";
    }

}

void Work::Server_Offline(QObject *myWork)
{
    if(myWork->metaObject()->className()==QStringLiteral("TaskWork"))
    {
        qDebug()<<"TaskWork disconnect Server";
    }
    if(myWork->metaObject()->className()==QStringLiteral("SendWork"))
    {
        qDebug()<<"SendWork disconnected Server";
    }
    if(myWork->metaObject()->className()==QStringLiteral("ReceiveWork"))
    {
        qDebug()<<"ReceiveWork disconnected Server";
    }
    QThread *thread=myWork->thread();
    QList<QThread*>::iterator iter;
    for(iter=m_ThreadList.begin();iter!=m_ThreadList.end();)
    {
        if(thread==*iter)
        {
            myWork->deleteLater();
            myWork=nullptr;
            (*iter)->quit();  // 请求线程停止
            (*iter)->wait();  // 等待线程停止
            (*iter)->deleteLater();
            *iter=nullptr;
            m_ThreadList.erase(iter);
            qDebug()<<thread<<"删除成功";
            break;
        }
        else
        {
            iter++;
        }
    }
}

