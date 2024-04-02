
#include "HomePage.h"
#include "ui_HomePage.h"

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <qmessagebox.h>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    Init();//初始化
}

HomePage::~HomePage()
{
    delete ui;
    if(p_pathAction!=nullptr)
        p_pathAction->deleteLater();
}

void HomePage::Init()//初始化
{
    //成员初始化
    //文件菜单初始化
    p_FileMenu = new QMenu;
    p_FileEnter = new QAction("打开", p_FileMenu);
    p_FileDownload = new QAction("下载", p_FileMenu);
    p_FileCopy = new QAction("复制", p_FileMenu);
    p_FileRename = new QAction("重命名", p_FileMenu);
    p_FileDelete = new QAction("删除", p_FileMenu);
    p_FileMenu->addAction(p_FileEnter);
    p_FileMenu->addAction(p_FileDownload);
    p_FileMenu->addAction(p_FileCopy);
    p_FileMenu->addAction(p_FileRename);
    p_FileMenu->addAction(p_FileDelete);
    //文件夹菜单初始化
    p_DirMenu = new QMenu;
    p_DirEnter = new QAction("打开", p_DirMenu);
    p_DirCopy = new QAction("复制",p_DirMenu);
    p_DirSticky = new QAction("粘贴", p_DirMenu);
    p_DirRename = new QAction("重命名", p_DirMenu);
    p_DirDelete = new QAction("删除", p_DirMenu);
    p_DirMenu->addAction(p_DirEnter);
    p_DirMenu->addAction(p_DirCopy);
    p_DirMenu->addAction(p_DirSticky);
    p_DirMenu->addAction(p_DirRename);
    p_DirMenu->addAction(p_DirDelete);
    //文件列表菜单初始化
    p_ListMenu = new QMenu;
    p_ListSticky = new QAction("粘贴", p_ListMenu);
    p_ListBuilt = new QAction("新建", p_ListMenu);
    p_ListUpload = new QAction("上传文件", p_ListMenu);
    p_ListRefresh = new QAction("刷新", p_ListMenu);
    p_ListReturn = new QAction("返回上一级", p_ListMenu);
    p_ListMenu->addAction(p_ListSticky);
    p_ListMenu->addAction(p_ListBuilt);
    p_ListMenu->addAction(p_ListUpload);
    p_ListMenu->addAction(p_ListRefresh);
    //成员初始化
    sp_Worker=&Work::getInstance();//关联通信模块
    sp_Info=&Information::getInstance();//关联信息模块
    sp_Network=&Network::getInstance();//关联网络模块
    m_StartPath=QString("./CloudDisk/%1").arg(sp_Info->getUserName()+sp_Info->getPhoneNumber());//设置跟路径
    m_CurrentPath=m_StartPath;//设置当前路径

    //页面设置
    p_pathAction = new QAction (ui->lineEdit_path);
    p_pathAction->setIcon (QIcon ( ":/images/path.png" ));
    ui->lineEdit_path->addAction (p_pathAction,QLineEdit::LeadingPosition);

    //信号与槽连接
    connect(this,&HomePage::currentpath_request,sp_Worker,&Work::Send_CurrentPath_Request);
    connect(this,&HomePage::built_dir_request,sp_Worker,&Work::Send_Built_Dir_Request);
    connect(this,&HomePage::rename_file_request,sp_Worker,&Work::Send_Rename_Request);
    connect(this,&HomePage::delete_file_request,sp_Worker,&Work::Send_Delete_Request);
    connect(this,&HomePage::copymove_request,sp_Worker,&Work::Send_CopyMove_Request);
    connect(this,&HomePage::cutmove_request,sp_Worker,&Work::Send_CutMove_Request);
    connect(this,&HomePage::upload_file_request,sp_Worker,&Work::Send_Upload_File_Request);
    connect(this,&HomePage::download_file_request,sp_Worker,&Work::Send_Download_File_Request);

    connect(sp_Worker,&Work::currentpath_respond,this,&HomePage::CurrentPath_Respond);
    connect(sp_Worker,&Work::built_dir_respond,this,&HomePage::Built_Dir_Respond);
    connect(sp_Worker,&Work::rename_file_respond,this,&HomePage::Rename_File_Respond);
    connect(sp_Worker,&Work::delete_file_respond,this,&HomePage::Delete_File_Respond);
    connect(sp_Worker,&Work::copymove_respond,this,&HomePage::CopyMove_Respond);
    connect(sp_Worker,&Work::cutmove_respond,this,&HomePage::CutMove_Respond);
    connect(sp_Worker,&Work::upload_file_respond,this,&HomePage::Upload_file_Respond);
    connect(sp_Worker,&Work::download_file_respond,this,&HomePage::Download_file_Respond);
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&HomePage::Enter);
    //...
    connect(p_FileMenu,&QMenu::triggered, this, &HomePage::FileMenuClicked);
    connect(p_DirMenu,&QMenu::triggered, this, &HomePage::DirMenuClicked);
    connect(p_ListMenu,&QMenu::triggered, this, &HomePage::ListMenuClicked);



    //待添加...
}

void HomePage::contextMenuEvent(QContextMenuEvent *event)//重写菜单事件
{
    //把鼠标位置转化成相对于QWidget的位置，然后判断是否在QListWidget内.
    QPoint pos = ui->listWidget->mapFromGlobal(event->globalPos());
    QRect rect(QPoint(0,0),ui->listWidget->size());
    if (rect.contains(pos))//判断右击位置是否在文件列表中
    {
        if(ui->listWidget->itemAt(pos) != nullptr)
        {
            p_SelectItem=ui->listWidget->itemAt(pos);
            QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
            FILEINFO FileList = variant.value<FILEINFO>();
            if(FileList.FileType==FILETYPE_DIR)
            {
                p_DirMenu->exec(event->globalPos());//打开文件夹菜单
            }
            else
            {
                p_FileMenu->exec(event->globalPos());//打开文件菜单
            }

        }
        else
        {
            p_SelectItem=nullptr;
            p_ListMenu->exec(event->globalPos());//打开文件列表菜单
        }
    }

}

void HomePage::Refresh()//刷新界面
{
    emit currentpath_request(m_CurrentPath);//查询当前路径下所有文件请求信号
}

void HomePage::Append_Path(QString filename)//将当前路径设置为下一级
{
    m_CurrentPath.append(QString("/%1").arg(filename));
    //qDebug()<<m_currentpath;
}

void HomePage::Return_Path()//将当前路径返回到前一级
{
    if(m_StartPath==m_CurrentPath)
    {
        QMessageBox::warning(this,"返回","返回失败：已经在最开始的文件夹目录中");
    }
    else
    {
        int index=m_CurrentPath.lastIndexOf('/');
        m_CurrentPath.remove(index,m_CurrentPath.size()-index);
    }

}

void HomePage::Enter(QListWidgetItem* item)//打开
{
    qDebug()<<"enter";
    QVariant variant = item->data(Qt::UserRole);  // 获取用户数据
    FILEINFO FileList = variant.value<FILEINFO>();
    if(FileList.FileType==FILETYPE_DIR)
    {
        QString EnterName=FileList.FileName;//获取进入文件夹的文件名
        Append_Path(EnterName);
        Refresh();
    }
    else
    {
        //打开文件
    }
}

void HomePage::on_pushButton_return_clicked()//返回按钮(槽函数)
{
    Return_Path();
    Refresh();
}

void HomePage::on_pushButton_refresh_clicked()//刷新按钮(槽函数)
{
    Refresh();
}

void HomePage::on_pushButton_built_clicked()//添加按钮(槽函数)
{
    QString Dirname=QInputDialog::getText(this,"新建文件夹","新文件夹名字");
    if(!Dirname.isEmpty())
    {
        if(Dirname.toLocal8Bit().length()<64)
        {
            emit built_dir_request(m_CurrentPath,Dirname);//创建文件夹信号
        }
        else
        {
             QMessageBox::warning(this,"新建文件夹","文件夹名不能超过64");
        }
    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","新文件夹名字不能为空");
    }

}

void HomePage::on_pushButton_upload_clicked()//上传按钮(槽函数)
{
    QString UploadFielPath;
    UploadFielPath=QFileDialog::getOpenFileName();
    qDebug()<<UploadFielPath;
    if(!UploadFielPath.isEmpty())
    {
        emit upload_file_request(m_CurrentPath,UploadFielPath);
    }
    else
    {
        QMessageBox::warning(this,"上传文件","上传文件名不能为空");
    }

}

void HomePage::CurrentPath_Respond(unsigned int status,FILEINFO *FileList,int filecount)//查询当前路径下所有文件回复处理
{
    // if(status!=CURRENTPATH_TRUE)
    //     exit(0);
    ui->listWidget->clear();//清空列表框
    ui->lineEdit_path->setText(m_CurrentPath);//设置当前路径
    for(int i=0;i<filecount;i++)
    {
        //循环打印
        if(!strcmp(".",FileList[i].FileName)||!strcmp("..",FileList[i].FileName))
        {
            continue;
        }
        //qDebug()<<FileList[i].FileName;
        //FileItem*fileItem=new FileItem;//创建文件项
        QListWidgetItem *Item=new QListWidgetItem();//创建列表项
        //设置图标
        if(FileList[i].FileType==FILETYPE_DIR)
            Item->setIcon(QIcon(":/images/dir.png"));
        else if(FileList[i].FileType==FILETYPE_FILE)
            Item->setIcon(QIcon(":/images/reg.png"));
        Item->setText(FileList[i].FileName);//设置描述
        Item->setData(Qt::UserRole,QVariant::fromValue(FileList[i]));
        //设置列表项大小
        Item->setSizeHint(QSize(500,30));
        //将自定义的(QWidget)FileItem,设置为Item的小部件
        ui->listWidget->addItem(Item);
    }

}

void HomePage::Built_Dir_Respond(unsigned int status)//新建文件夹回复处理
{
    // if(status!=BUILT_DIR_TRUE)
    //     exit(0);
    Refresh();
}

void HomePage::Rename_File_Respond(unsigned int status)
{
     Refresh();

}

void HomePage::Delete_File_Respond(unsigned int status)
{
    Refresh();
}

void HomePage::CopyMove_Respond(unsigned int status)
{
    Refresh();

}

void HomePage::CutMove_Respond(unsigned int status)
{
     Refresh();

}

void HomePage::Upload_file_Respond(unsigned int status)
{
     Refresh();

}

void HomePage::Download_file_Respond(unsigned int status)
{
    QMessageBox::information(this,"下载文件","下载文件成功");

}

void HomePage::DirMenuClicked(QAction *action)//文件夹菜单栏点击处理
{
    if(action==p_DirEnter)//打开
    {
        qDebug()<<"打开";
        Enter(p_SelectItem);
    }
    if(action== p_DirCopy)//复制
    {
        qDebug()<<"复制";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        m_CopyFilePath=QString("%1/%2").arg(m_CurrentPath,FileList.FileName);
        qDebug()<<m_CopyFilePath;
    }
    if(action== p_DirSticky)//粘贴
    {
        qDebug()<<"粘贴";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        int index=m_CopyFilePath.lastIndexOf('/');
        QString Filename=m_CopyFilePath.right(m_CopyFilePath.size()-index-1);
        emit copymove_request(m_CopyFilePath,QString("%1/%2/%3").arg(m_CurrentPath,FileList.FileName,Filename));
    }
    if(action==p_DirRename)//重命名
    {
        qDebug()<<"重命名";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        QString NewName=QInputDialog::getText(this,"重命名文件","请输入新的文件名");
        emit rename_file_request(m_CurrentPath,FileList.FileName,NewName);
    }
    if(action==p_DirDelete)//删除
    {
        qDebug()<<"删除";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        emit delete_file_request(m_CurrentPath,FileList.FileName);
    }

}

void HomePage::ListMenuClicked(QAction *action)//文件列表菜单栏点击处理
{
    if(action==p_ListSticky)//粘贴
    {
        qDebug()<<"粘贴";
        int index=m_CopyFilePath.lastIndexOf('/');
        QString Filename=m_CopyFilePath.right(m_CopyFilePath.size()-index-1);
        QString DestPath=QString("%1/%2").arg(m_CurrentPath,Filename);
        emit copymove_request(m_CopyFilePath,DestPath);
    }
    if(action==p_ListBuilt)//新建
    {
        qDebug()<<"新建";
        on_pushButton_built_clicked();
    }
    if(action==p_ListUpload)//上传文件
    {
        qDebug()<<"上传文件";
        on_pushButton_upload_clicked();
    }
    if(action==p_ListRefresh)//刷新
    {
        qDebug()<<"刷新";
        on_pushButton_refresh_clicked();
    }
    if(action==p_ListReturn)//返回上一级
    {
        qDebug()<<"返回上一级";
        on_pushButton_return_clicked();
    }

}

void HomePage::FileMenuClicked(QAction *action)//文件菜单栏点击处理
{
    if(action==p_FileEnter)//打开
    {
        qDebug()<<"打开";
        Enter(p_SelectItem);
    }
    if(action==p_FileDownload )//下载
    {
        qDebug()<<"下载";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        QString DownloadPath=QString("%1/%2").arg(m_CurrentPath,FileList.FileName);
        QString SavePath=QFileDialog::getSaveFileName();
        emit download_file_request(SavePath,DownloadPath);
    }
    if(action==p_FileCopy)//复制
    {
        qDebug()<<"复制";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        m_CopyFilePath=QString("%1/%2").arg(m_CurrentPath,FileList.FileName);
        qDebug()<<m_CopyFilePath;
    }
    if(action==p_FileRename)//重命名
    {
        qDebug()<<"重命名";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        QString NewName=QInputDialog::getText(this,"重命名文件","请输入新的文件名");
        emit rename_file_request(m_CurrentPath,FileList.FileName,NewName);

    }
    if(action==p_FileDelete)//删除
    {
        qDebug()<<"删除";
        QVariant variant = p_SelectItem->data(Qt::UserRole);  // 获取用户数据
        FILEINFO FileList = variant.value<FILEINFO>();
        emit delete_file_request(m_CurrentPath,FileList.FileName);
    }

}

