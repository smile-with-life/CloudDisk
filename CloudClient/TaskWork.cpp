#include "TaskWork.h"

#include <QFile>
#include <QMessageBox>
#include <QThread>

TaskWork::TaskWork(QObject *parent)
        : QObject{parent}
{
    Init();
}

TaskWork::~TaskWork()
{
    if(p_TaskSocket!=nullptr)
        p_TaskSocket->deleteLater();

}

void TaskWork::Init()
{
    LoadConfig();//加载配置文件
    m_IP="127.0.0.1";
    m_TaskPort=7777;
}

void TaskWork::LoadConfig()//加载配置文件
{
    QFile file(":/client.config");//打开文件
    if(file.open(QIODeviceBase::ReadOnly))//打开方式：只读
    {
        QByteArray data = file.readAll();//把文件信息放到一个字节数组里
        file.close();//关闭文件
        QString strData=data.toStdString().c_str();//把字节数组变为字符串
        strData.replace("\r\n"," ");//用空格替换\r\n
        QStringList strList= strData.split(" ");//以空格为分隔符把字符串分为两个字符串并把分完的字符串放到列表中
        m_IP = strList.at(0);//列表中的第一个元素存储IP地址
        m_TaskPort = strList.at(1).toUShort();//列表中的第二个元素存储端口地址
        qDebug()<<"ip:"<<m_IP<<"port:"<<m_TaskPort;
    }
    else
    {
        QMessageBox::critical(NULL,"打开配置文件","打开配置文件失败");
    }

}



void TaskWork::ConnectServer()//连接服务器
{
    qDebug()<<"线程id"<< QThread::currentThread();
    qDebug()<<"创建Socket";
    //创建Socket
    p_TaskSocket=new QTcpSocket;
    //使用IP地址和端口号连接
    qDebug()<<m_IP;
    qDebug()<<m_TaskPort;
    p_TaskSocket->connectToHost(QHostAddress(m_IP),m_TaskPort);//连接Task服务器
    qDebug()<<p_TaskSocket->waitForConnected();
    //处理TaskSocket发送的信号
    connect(p_TaskSocket,&QTcpSocket::connected,this,&TaskWork::Server_Online);
    connect(p_TaskSocket,&QTcpSocket::disconnected,this,&TaskWork::Server_Offline);
    connect(p_TaskSocket,&QTcpSocket::readyRead,this,&TaskWork::recvMsg);
}

void TaskWork::Send_Logon_Request(const QString Account,const QString Password)//发送登录请求
{
    qDebug()<<"发送登录请求";
    TCP_PDU* pdu = ApplyTCP_PDU(0);//申请一块通信协议数据
    pdu->MessageType=LOGON_REQUEST_MESSAGE;//登录请求
    strncpy(pdu->Data,Account.toStdString().c_str(),16);
    strncpy(pdu->Data+16,Password.toStdString().c_str(),16);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;
}

void TaskWork::Send_Getcode_Request(const QString PhoneNumber)//发送获取验证码请求
{
    qDebug()<<"发送获取验证码请求";
    TCP_PDU* pdu = ApplyTCP_PDU(0);//申请一块通信协议数据
    pdu->MessageType=GETCODE_REQUEST_MESSAGE;//获取验证码请求
    strncpy(pdu->Data,PhoneNumber.toStdString().c_str(),16);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;
}

void TaskWork::Send_Signup_Request(const QString UserName,const QString Password,const QString PhoneNumber,const QString Code)//发送注册请求
{
    qDebug()<<"发送注册请求";
    TCP_PDU* pdu = ApplyTCP_PDU(0);//申请一块通信协议数据
    pdu->MessageType=SIGNUP_REQUEST_MESSAGE;//立即注册请求
    strncpy(pdu->Data,UserName.toStdString().c_str(),16);
    strncpy(pdu->Data+16,Password.toStdString().c_str(),16);
    strncpy(pdu->Data+32,PhoneNumber.toStdString().c_str(),16);
    strncpy(pdu->Data+48,Code.toStdString().c_str(),16);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;

}

void TaskWork::Send_CurrentPath_Request(const QString CurrentPath)//发送查询当前路径下所有文件请求
{
    qDebug()<<"发送查询当前路径下文件请求";
    TCP_PDU* pdu = ApplyTCP_PDU(CurrentPath.toLocal8Bit().length()+1);//申请一块通信协议数据
    pdu->MessageType=CURRENTPATH_REQUEST_MESSAGE;
    strncpy((char*)pdu->Message,CurrentPath.toStdString().c_str(),CurrentPath.toLocal8Bit().length()+1);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;

}

void TaskWork::Send_Built_Dir_Request(const QString CurrentPath,const QString Dirname)//发送新建文件夹请求
{
    qDebug()<<"发送新建文件夹请求";
    TCP_PDU* pdu = ApplyTCP_PDU(CurrentPath.toLocal8Bit().length()+1);//申请一块通信协议数据
    pdu->MessageType=BUILT_DIR_REQUEST_MESSAGE;
    strncpy((char*)pdu->Message,CurrentPath.toStdString().c_str(),CurrentPath.toLocal8Bit().length()+1);
    strncpy(pdu->Data,Dirname.toStdString().c_str(),64);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;
}

void TaskWork::Send_CopyMove_Request(const QString SourcePath, const QString DestPath)//文件复制移动请求
{
    qDebug()<<"文件复制移动请求";
    QString Message=SourcePath+DestPath;
    TCP_PDU* pdu = ApplyTCP_PDU(Message.toLocal8Bit().length()+1);//申请一块通信协议数据
    pdu->MessageType=COPYMOVE_REQUEST_MESSAGE;
    strncpy((char*)pdu->Message,Message.toStdString().c_str(),Message.toLocal8Bit().length()+1);
    strncpy(pdu->Data,QString::number(SourcePath.size()).toStdString().c_str(),32);//传送字符个数而不是字节数
    strncpy(pdu->Data+32,QString::number(DestPath.size()).toStdString().c_str(),32);//传送字符个数而不是字节数
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;

}

void TaskWork::Send_CutMove_Request(const QString SourcePath, const QString DestPath)//文件剪切移动请求
{
    qDebug()<<"文件剪切移动请求";
    QString Message=SourcePath+DestPath;
    TCP_PDU* pdu = ApplyTCP_PDU(Message.toLocal8Bit().length()+1);//申请一块通信协议数据
    pdu->MessageType=CUTMOVE_REQUEST_MESSAGE;
    strncpy((char*)pdu->Message,Message.toStdString().c_str(),Message.toLocal8Bit().length()+1);
    strncpy(pdu->Data,QString::number(SourcePath.size()).toStdString().c_str(),32);//传送字符个数而不是字节数
    strncpy(pdu->Data+32,QString::number(DestPath.size()).toStdString().c_str(),32);//传送字符个数而不是字节数
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;

}

void TaskWork::Send_Rename_Request(const QString CurrentPath, const QString Filename, const QString Newname)//文件重命名请求
{
    qDebug()<<"文件重命名请求";
    TCP_PDU* pdu = ApplyTCP_PDU(CurrentPath.toLocal8Bit().length()+1);//申请一块通信协议数据
    pdu->MessageType=RENAME_FILE_REQUEST_MESSAGE;
    strncpy((char*)pdu->Message,CurrentPath.toStdString().c_str(),CurrentPath.toLocal8Bit().length()+1);
    strncpy(pdu->Data,Filename.toStdString().c_str(),64);
    strncpy(pdu->Data+64,Newname.toStdString().c_str(),64);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;

}

void TaskWork::Send_Delete_Request(const QString CurrentPath, const QString Filename)//文件删除请求
{
    qDebug()<<"文件删除请求";
    TCP_PDU* pdu = ApplyTCP_PDU(CurrentPath.toLocal8Bit().length()+1);//申请一块通信协议数据
    pdu->MessageType=DELETE_FILE_REQUEST_MESSAGE;
    strncpy((char*)pdu->Message,CurrentPath.toStdString().c_str(),CurrentPath.toLocal8Bit().length()+1);
    strncpy(pdu->Data,Filename.toStdString().c_str(),64);
    p_TaskSocket->write((char*)pdu,pdu->TCP_PDULength);//发送数据
    free(pdu);
    pdu=NULL;

}

void TaskWork::recvMsg()
{
    qDebug()<<p_TaskSocket->bytesAvailable();
    unsigned int TCP_PDULength=0;
    p_TaskSocket->read((char*)&TCP_PDULength,sizeof(unsigned int));
    unsigned int MessageLength=TCP_PDULength-sizeof(TCP_PDU);

    TCP_PDU* pdu = ApplyTCP_PDU(MessageLength);
    p_TaskSocket->read((char*)pdu+sizeof(unsigned int),TCP_PDULength-sizeof(unsigned int));
    switch(pdu->MessageType)
    {
    case LOGON_RESPOND_MESSAGE://登录回复
    {
        char UserName[16]={'\0'};
        char PhoneNumber[16]={'\0'};
        strncpy(UserName,pdu->Data,16);
        strncpy(PhoneNumber,pdu->Data+16,16);
        emit logon_respond(pdu->Status,UserName,PhoneNumber);
        break;
    }
    case GETCODE_RESPOND_MESSAGE://获取验证码回复
    {
        emit getcode_respond(pdu->Status);
    }
    case SIGNUP_RESPOND_MESSAGE://注册回复
    {
        emit signup_respond(pdu->Status);
        break;
    }
    case FORGET_RESPOND_MESSAGE://忘记密码回复
    {
        emit forget_respond(pdu->Status);
        break;
    }
    case CURRENTPATH_RESPOND_MESSAGE://查询当前路径下的文件回复
    {
        FILEINFO* pfileinfo=nullptr;//文件结构的指针
        int filecount=pdu->MessageLength/sizeof(FILEINFO);//数据中包含的文件结构的个数
        unsigned int status=pdu->Status;
        FILEINFO FileList[filecount];//文件列表
        for(int i=0;i<filecount;i++)
        {
            //循环打印
            pfileinfo=(FILEINFO*)(pdu->Message)+i;
            strncpy(FileList[i].FileName,pfileinfo->FileName,64);
            //qDebug()<<FileList[i].FileName;
            FileList[i].FileType=pfileinfo->FileType;
        }
        emit currentpath_respond(status,FileList,filecount);
        QThread::msleep(100);//阻塞延时100ms
        break;
    }
    case BUILT_DIR_RESPOND_MESSAGE://新建文件夹回复
    {
        emit built_dir_respond(pdu->Status);
        break;
    }
    case RENAME_FILE_RESPOND_MESSAGE://重命名文件回复
    {
        emit rename_file_respond(pdu->Status);
        break;
    }
    case DELETE_FILE_RESPOND_MESSAGE://删除文件回复
    {
        emit delete_file_respond(pdu->Status);
        break;
    }
    case COPYMOVE_RESPOND_MESSAGE://复制移动回复
    {
        emit copymove_respond(pdu->Status);
        break;
    }
    case CUTMOVE_RESPOND_MESSAGE://剪切移动
    {
        emit cutmove_respond(pdu->Status);
        break;
    }
    }

    free(pdu);
    pdu=nullptr;

}

void TaskWork::Server_Online()
{
    qDebug()<<"服务器连接成功";
    emit server_connected(this);

}

void TaskWork::Server_Offline()
{
    qDebug()<<"服务器断开连接";
    emit server_disconnect(this);

}
