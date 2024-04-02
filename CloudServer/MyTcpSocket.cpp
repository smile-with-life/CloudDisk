#include "MyTcpSocket.h"
#include"protocol.h"
#include "CloudDB.h"
#include "CopyData.h"

#include <QElapsedTimer>
#include <QEventLoop>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QtGlobal>
#include <QTime>
#include <QDir>
#include <QSettings>
#include <QMessageBox>
#include <QThreadPool>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}//构造函数
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(Client_Offline()));
}

void MyTcpSocket::recvMsg()//接收信息
{

    //qDebug()<<this->bytesAvailable();
    unsigned int TCP_PDULength=0;
    this->read((char*)&TCP_PDULength,sizeof(unsigned int));
    unsigned int MessageLength=TCP_PDULength-sizeof(TCP_PDU);

    TCP_PDU* pdu = ApplyTCP_PDU(MessageLength);
    this->read((char*)pdu+sizeof(unsigned int),TCP_PDULength-sizeof(unsigned int));
    switch(pdu->MessageType)
    {
    case LOGON_REQUEST_MESSAGE://登录请求
    {
        qDebug()<<"登录请求";
        QString UserName;
        char Account[16]={'\0'};
        char Password[16]={'\0'};
        strncpy(Account,pdu->Data,16);
        strncpy(Password,pdu->Data+16,16);
        //查询数据库
        STATUS Status=CloudDB::getInstance().logon(Account,Password);
        //登录回复
        TCP_PDU* respdu=ApplyTCP_PDU(0);
        respdu->MessageType=LOGON_RESPOND_MESSAGE;
        respdu->Status=Status;
        if(respdu->Status==LOGON_TRUE)
        {
            m_Account=Account;
            UserName=CloudDB::getInstance().getInformation(m_Account);
            strncpy(respdu->Data,UserName.toStdString().c_str(),16);
            strncpy(respdu->Data+16,Account,16);
        }
        write((char*)respdu,respdu->TCP_PDULength);//发送数据
        free(respdu);
        respdu=NULL;
        break;
    }
    case GETCODE_REQUEST_MESSAGE://获取验证码请求
    {
        qDebug()<<"获取验证码请求";
        char PhoneNumber[16]={'\0'};
        strncpy(PhoneNumber,pdu->Data,16);
        QString Code=GetContent();
        CloudDB::getInstance().addcode(PhoneNumber,Code);
        //Send_phonecode(PhoneNumber,Code);//发送验证码
        //获取验证码回复
        TCP_PDU* respdu=ApplyTCP_PDU(0);
        respdu->MessageType=GETCODE_RESPOND_MESSAGE;
        respdu->Status=GETCODE_TRUE;
        write((char*)respdu,respdu->TCP_PDULength);//发送数据
        free(respdu);
        respdu=NULL;
        break;

    }
    case SIGNUP_REQUEST_MESSAGE://注册请求
    {
        qDebug()<<"立即注册请求";
        char UserName[16]={'\0'};
        char Password[16]={'\0'};
        char PhoneNumber[16]={'\0'};
        char Code[16]={'\0'};
        strncpy(UserName,pdu->Data,16);
        strncpy(Password,pdu->Data+16,16);
        strncpy(PhoneNumber,pdu->Data+32,16);
        strncpy(Code,pdu->Data+48,16);

        //查询数据库
        STATUS Status=CloudDB::getInstance().signup(UserName,Password,PhoneNumber,Code);
        //创建个人文件夹
        if(Status==SIGNUP_TRUE)
        {
            //个人文件夹命名规则:用户名+手机号
            QString DiskName;
            DiskName.append(UserName);
            DiskName.append(PhoneNumber);
            QDir dir;
            dir.mkdir(QString("./CloudDisk/%1").arg(DiskName));
        }
        //注册回复
        TCP_PDU* respdu=ApplyTCP_PDU(0);
        respdu->MessageType=SIGNUP_RESPOND_MESSAGE;
        respdu->Status=Status;
        write((char*)respdu,respdu->TCP_PDULength);//发送数据
        free(respdu);
        respdu=NULL;
        break;

    }
    case CURRENTPATH_REQUEST_MESSAGE://查询当前路径下的文件请求
    {
        qDebug()<<"查询当前路径下的文件请求";
        QString CurrentPath=QString("%1").arg((char*)pdu->Message);
        // char *CurrentPath=new char[pdu->MessageLength];
        // memcpy(CurrentPath,pdu->Message,pdu->MessageLength);//获取当前所在路径
        QDir dir(CurrentPath);
        QFileInfoList FileList =dir.entryInfoList();//把该路径下的所有文件放到表中
        int FileCount=FileList.size();//获取文件的个数

        TCP_PDU*respdu=ApplyTCP_PDU(sizeof(FILEINFO)*FileCount);
        respdu->MessageType=CURRENTPATH_RESPOND_MESSAGE;
        FILEINFO *pFileInfo=nullptr;
        for(int i=0;i<FileCount;i++)
        {
            pFileInfo=(FILEINFO*)(respdu->Message)+i;
            strncpy(pFileInfo->FileName,FileList[i].fileName().toStdString().c_str(),64);
            if(FileList[i].isDir())
            {
                pFileInfo->FileType=FILETYPE_DIR;
            }
            else if(FileList[i].isFile())
            {
                pFileInfo->FileType=FILETYPE_FILE;
            }
            //文件大小
            //是否为共享文件
            //最后一次修改时间
        }
        write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;
        break;

    }
    case BUILT_DIR_REQUEST_MESSAGE://新建文件夹请求
    {
        qDebug()<<"新建文件夹请求";
        QString CurrentPath=QString("%1").arg((char*)pdu->Message);

        TCP_PDU* respdu=ApplyTCP_PDU(0);
        respdu->MessageType=BUILT_DIR_RESPOND_MESSAGE;
        QDir dir;
        if(dir.exists(CurrentPath))
        {
            char Dirname[64]={'\0'};
            memcpy(Dirname,pdu->Data,64);
            QString NewPath=CurrentPath+"/"+Dirname;
            if(dir.exists(NewPath))//文件夹已存在
            {
                respdu->Status=BUILT_DIR_FALSE_DIRNAME;
            }
            else//新建成功
            {
                dir.mkdir(NewPath);
                respdu->Status=BUILT_DIR_TRUE;
            }

        }
        else//当前路径不存在
        {
            respdu->Status=BUILT_DIR_FALSE_CURRENTPATH;
        }
        write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;
        break;

    }
    case COPYMOVE_REQUEST_MESSAGE://复制移动请求
    {
        qDebug()<<"复制移动请求";
        QString Message=QString("%1").arg((char*)pdu->Message);
        char strsourcelen[32];
        char strDestlen[32];
        strncpy(strsourcelen,pdu->Data,32);
        strncpy(strDestlen,pdu->Data+32,32);
        QString SourcePath=Message.mid(0,QString(strsourcelen).toInt());
        QString DestPath=Message.mid(QString(strsourcelen).toInt(),QString(strDestlen).toInt());
        qDebug()<<SourcePath;
        qDebug()<<DestPath;
        QFileInfo fileInfo(SourcePath);
        if(fileInfo.isFile())
        {
            CopyData *Data=new CopyData(SourcePath,DestPath);
            QThreadPool::globalInstance()->start(Data);
        }
        else if(fileInfo.isDir())
        {
            CopyDir(SourcePath,DestPath);
        }

        TCP_PDU* respdu=ApplyTCP_PDU(0);
        respdu->MessageType=COPYMOVE_RESPOND_MESSAGE;
        respdu->Status=COPYMOVE_FILE_TRUE;
        write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;
        break;
    }
    case CUTMOVE_REQUEST_MESSAGE://剪切移动
    {
        qDebug()<<"剪切移动请求";
        QString Message=QString("%1").arg((char*)pdu->Message);
        char strsourcelen[32];
        char strDestlen[32];
        strncpy(strsourcelen,pdu->Data,32);
        strncpy(strDestlen,pdu->Data+32,32);
        QString SourcePath=Message.mid(0,QString(strsourcelen).toInt());
        QString DestPath=Message.mid(QString(strsourcelen).toInt(),QString(strDestlen).toInt());
        int index=SourcePath.lastIndexOf('/');
        QString Filename=SourcePath.right(SourcePath.size()-index-1);
        DestPath=QString("%1/%2").arg(DestPath,Filename);
        CutMove(SourcePath,DestPath);
        TCP_PDU* respdu=ApplyTCP_PDU(0);
        respdu->MessageType=COPYMOVE_RESPOND_MESSAGE;
        respdu->Status=CUTMOVE_REQUEST_MESSAGE;
        write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;

        break;
    }
    case RENAME_FILE_REQUEST_MESSAGE://文件重命名请求
    {
        char Filename[64]={'\0'};
        char Newname[64]={'\0'};
        strncpy(Filename,pdu->Data,64);
        strncpy(Newname,pdu->Data+64,64);
        QString CurrentPath=QString("%1").arg((char*)pdu->Message);
        QString OldPath=QString("%1/%2").arg(CurrentPath,Filename);
        QString NewPath=QString("%1/%2").arg(CurrentPath,Newname);
        TCP_PDU*respdu=ApplyTCP_PDU(0);
        respdu->MessageType=RENAME_FILE_RESPOND_MESSAGE;
        QDir dir;
        if(dir.rename(OldPath,NewPath))
        {
            respdu->Status=RENAME_FILE_TRUE;
        }
        else
        {
            respdu->Status=RENAME_FILE_FALSE;
        }
        write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;
        break;

    }
    case DELETE_FILE_REQUEST_MESSAGE://文件删除请求
    {
        char Filename[64]={'\0'};
        strncpy(Filename,pdu->Data,64);
        QString CurrentPath=QString("%1").arg((char*)pdu->Message);
        QString DelPath=QString("%1/%2").arg(CurrentPath,Filename);
        QFileInfo FileInfo(DelPath);
        bool ret=false;
        if(FileInfo.isDir())
        {
            QDir dir;
            dir.setPath(DelPath);
            ret=dir.removeRecursively();//删除文件夹，并且包括路径内的所有文件
        }
        else if(FileInfo.isFile())
        {

            QDir dir;
            ret=dir.remove(DelPath);
        }
        TCP_PDU*respdu=ApplyTCP_PDU(0);
        respdu->MessageType=DELETE_FILE_RESPOND_MESSAGE;
        if(ret)
        {
            pdu->Status=DELETE_FILE_TRUE;
        }
        else
        {
            pdu->Status=DELETE_FILE_FALSE;
        }
        write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;
        break;

    }
    }
    free(pdu);
    pdu=NULL;

}

void MyTcpSocket::Send_PhoneCode(QString PhoneNumber,QString Code)//向手机发送验证码
{
    //获取云信接口的账号和密码
    QString SDK_ID;//云信账户
    QString SDK_Key;//云信接口密码
    QSettings setting(":/SDK.ini" , QSettings::IniFormat);
    if(QFile::exists(":/SDK.ini"))
    {
        SDK_ID=setting.value("SDK_ID", "").toString();
        SDK_Key = setting.value("SDK_Key", "").toString();
    }
    else
    {
        qDebug()<<"打开配置文件失败";
    }
    //调用云信接口向手机发送验证码
    QString baseUrl = QString("http://api.sms.cn/sms/");
    QUrl url(baseUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("ac", "send");
    urlQuery.addQueryItem("uid", "cui1727146972");//云信账户
    urlQuery.addQueryItem("pwd", "1ad1c5d869c79036e91c8be4590b87c5"); //云信接口密码
    urlQuery.addQueryItem("template", "100000");//模板短信ID
    urlQuery.addQueryItem("mobile", PhoneNumber);//接收号码
    urlQuery.addQueryItem("content", QString("{\"code\":\"%1\"}").arg(Code));//短信内容
    url.setQuery(urlQuery);
    QNetworkAccessManager manager;
    manager.get(QNetworkRequest(url));
}

QString MyTcpSocket::GetContent()//获得验证码
{
    //生成验证码
    int n=4;
    QString Code;
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0;i<n;i++)
        Code.append(QString::number(rand()%9));
    qDebug()<<Code;
    return Code;
}

bool MyTcpSocket::CutMove(QString SourcePath, QString DestPath)
{
    QFileInfo FileInfo(SourcePath);
    bool ret=false;
    if(FileInfo.isDir())
    {
        QDir dir;
        ret=dir.rename(SourcePath,DestPath);
    }
    else if(FileInfo.isFile())
    {
        QFile file;
        ret=file.rename(SourcePath,DestPath);
    }
    return ret;


}

void MyTcpSocket::CopyDir(QString SourceDirPath, QString DestDirPath)
{
    QDir dir;
    //qDebug()<<strSourceDir;
    //qDebug()<<strDestDir;
    dir.mkdir(DestDirPath);
    dir.setPath(SourceDirPath);
    QFileInfoList fileInfoList= dir.entryInfoList();
    QString SourceTmp;
    QString DestTmp;
    for(int i=0;i< fileInfoList.size();i++)
    {
        qDebug()<<fileInfoList[i].fileName();
        if(fileInfoList[i].isFile())
        {
            SourceTmp=SourceDirPath+'/'+fileInfoList[i].fileName();
            DestTmp=DestDirPath+'/'+fileInfoList[i].fileName();
            CopyData *Data=new CopyData(SourceTmp,DestTmp);
            QThreadPool::globalInstance()->start(Data);
        }
        else if(fileInfoList[i].isDir())
        {
            if(QString(".")==fileInfoList[i].fileName()||QString("..")==fileInfoList[i].fileName())
            {
                continue;
            }

            SourceTmp=SourceDirPath+'/'+fileInfoList[i].fileName();
            DestTmp=DestDirPath+'/'+fileInfoList[i].fileName();
            //qDebug()<<sourceTmp;
            //qDebug()<<destTmp;
            CopyDir(SourceTmp,DestTmp);

        }
    }

}

void MyTcpSocket::Client_Offline()//客户端断开连接后的处理函数
{
    qDebug()<<"client disconnect";
    if(m_Account!="")
    {
        CloudDB::getInstance().offonline(m_Account);
    }
    emit offline(this);//发送一个删除客户端表中信息的一个信号
}
