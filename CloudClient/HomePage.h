#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "Network.h"
#include "Work.h"

#include <Information.h>
#include <QListWidget>
#include <QMenu>
#include <QWidget>

namespace Ui {
class HomePage;
}
enum ObjectType
{
    OBJECT_TYPE_NULL=0,
    OBJECT_TYPE_LIST,//文件列表
    OBJECT_TYPE_FILEITEM,//文件
    OBJECT_TYPE_DIRITEM,//文件夹
    OBJECT_TYPE_MAX=0xffffff
};

class HomePage : public QWidget
{
    Q_OBJECT
private:
    QAction *p_pathAction=nullptr;//当前路径显示框图标

    QString m_CurrentPath;//当前路径
    QString m_StartPath;//根路径

    Work *sp_Worker=nullptr;//工作对象
    Information *sp_Info=nullptr;//信息对象
    Network *sp_Network=nullptr;//网络对象

    QMenu *p_FileMenu = nullptr;//文件夹菜单
    QMenu *p_DirMenu = nullptr;//文件菜单
    QMenu *p_ListMenu = nullptr;//文件列表菜单
    //文件菜单项
    QAction *p_FileEnter = nullptr;//打开
    QAction *p_FileRename = nullptr;//重命名
    QAction *p_FileDelete = nullptr;//删除
    QAction *p_FileCopy = nullptr;//复制
    QAction *p_FileDownload = nullptr;//下载
    //文件夹菜单项
    QAction *p_DirEnter = nullptr;//打开
    QAction *p_DirCopy = nullptr;//打开
    QAction *p_DirRename = nullptr;//重命名
    QAction *p_DirDelete = nullptr;//删除
    QAction *p_DirSticky = nullptr;//粘贴
    //文件列表菜单项
    QAction *p_ListBuilt = nullptr;//新建
    QAction *p_ListRefresh = nullptr;//刷新
    QAction *p_ListUpload = nullptr;//上传文件
    QAction *p_ListReturn = nullptr;//返回上一级
    QAction *p_ListSticky = nullptr;//粘贴

    QListWidgetItem *p_SelectItem=nullptr;//选中的项
    QString m_CopyFilePath;//复制的文件路径

public:
    explicit HomePage(QWidget *parent = nullptr);//构造函数

    ~HomePage();//析构函数

    void Refresh();//刷新界面
private:
    void Init();//初始化

    void Append_Path(QString Filename);//将当前路径设置为下一级

    void Return_Path();//将当前路径返回到前一级

    void contextMenuEvent(QContextMenuEvent *event)override;//重写菜单事件
signals:
    //查询当前路径下所有文件请求信号
    void currentpath_request(QString currentpath);
    //新建文件夹请求信号
    void built_dir_request(QString currentpath,QString Dirname);
    //上传文件请求信号
    void upload_file_request(QString currentpath,QString UploaFilePath);
    //下载文件请求信号
    void download_file_request(QString SavePath,QString DownloadPath);
    //重命名文件请求信号
    void rename_file_request(const QString CurrentPath,const QString Filename,const QString Newname);
    //删除文件请求信号
    void delete_file_request(const QString CurrentPath,const QString Filename);
    //复制移动请求信号
    void copymove_request(const QString SourcePath,const QString DestPath);
    //剪切移动请求信号
    void cutmove_request(const QString SourcePath,const QString DestPath);

private slots:
    void on_pushButton_return_clicked();//返回按钮(槽函数)

    void on_pushButton_refresh_clicked();//刷新按钮(槽函数)

    void on_pushButton_built_clicked();//添加按钮(槽函数)

    void on_pushButton_upload_clicked();//上传按钮(槽函数)

    void Enter(QListWidgetItem* item);//打开

    void CurrentPath_Respond(unsigned int status,FILEINFO *FileList,int filecount);//查询当前路径下所有文件回复处理

    void Built_Dir_Respond(unsigned int status);//新建文件夹回复处理

    void Rename_File_Respond(unsigned int status);//重命名文件回复处理

    void Delete_File_Respond(unsigned int status);//删除文件回复处理

    void CopyMove_Respond(unsigned int status);//复制移动回复处理

    void CutMove_Respond(unsigned int status);//剪切移动回复处理

    void Upload_file_Respond(unsigned int status);//上传文件回复处理

    void Download_file_Respond(unsigned int status);//下载文件回复处理

    void FileMenuClicked(QAction *action);//文件菜单栏点击处理

    void DirMenuClicked(QAction *action);//文件夹菜单栏点击处理

    void ListMenuClicked(QAction *action);//文件列表菜单栏点击处理
private:
    Ui::HomePage *ui;
};

#endif // HOMEPAGE_H
