#ifndef PROTOCOL_H
#define PROTOCOL_H
enum MESSAGE_TYPE
{
    MESSAGE_TYPE_NULL=0,
    LOGON_REQUEST_MESSAGE,//登录请求
    LOGON_RESPOND_MESSAGE,//登录回复
    GETCODE_REQUEST_MESSAGE,//获取验证码请求
    GETCODE_RESPOND_MESSAGE,//获取验证码回复
    SIGNUP_REQUEST_MESSAGE,//注册请求
    SIGNUP_RESPOND_MESSAGE,//注册回复
    FORGET_REQUEST_MESSAGE,//找回请求
    FORGET_RESPOND_MESSAGE,//找回回复
    CURRENTPATH_REQUEST_MESSAGE,//查询当前路径下的文件请求
    CURRENTPATH_RESPOND_MESSAGE,//查询当前路径下的文件回复
    BUILT_DIR_REQUEST_MESSAGE,//新建文件夹请求
    BUILT_DIR_RESPOND_MESSAGE,//新建文件夹回复
    OPEN_FILE_REQUEST_MESSAGE,//打开文件请求
    OPEN_FILE_RESPOND_MESSAGE,//打开文件回复
    RENAME_FILE_REQUEST_MESSAGE,//重命名文件请求
    RENAME_FILE_RESPOND_MESSAGE,//重命名文件回复
    DELETE_FILE_REQUEST_MESSAGE,//删除文件请求
    DELETE_FILE_RESPOND_MESSAGE,//删除文件回复
    COPYMOVE_REQUEST_MESSAGE,//复制移动请求
    COPYMOVE_RESPOND_MESSAGE,//复制移动回复
    CUTMOVE_REQUEST_MESSAGE,//剪切移动请求
    CUTMOVE_RESPOND_MESSAGE,//剪切移动请求
    UPLOAD_FILE_REQUEST_MESSAGE,//上传文件请求
    UPLOAD_FILE_RESPOND_MESSAGE,//上传文件回复
    DOWNLOAD_FILE_REQUEST_MESSAGE,//下载文件请求
    DOWNLOAD_FILE_RESPOND_MESSAGE,//下载文件回复
    MESSAGE_TYPE_MAX=0xffffff
};
enum STATUS
{
    STATUS_NULL=0,
    LOGON_TRUE,//登录成功
    LOGON_FALSE_ACCOUNT,//登录失败:账号或密码不正确
    LOGON_FALSE_ONLINE,//登录失败:账号已在其它地方登录
    SIGNUP_TRUE,//注册成功
    SIGNUP_FALSE_CODE,//注册失败:验证码不正确
    SIGNUP_FALSE_ACCOUNT,//注册失败:账号已被注册
    GETCODE_TRUE,//获取验证码成功
    GETCODE_FALSE,//获取验证码失败
    CURRENTPATH_TRUE,//查询当前路径下的文件成功
    CURRENTPATH_FALSE,//查询当前路径下的文件失败
    BUILT_DIR_TRUE,//新建文件夹成功
    BUILT_DIR_FALSE_CURRENTPATH,//新建文件夹失败:当前路径不存在
    BUILT_DIR_FALSE_DIRNAME,//新建文件夹失败:文件夹已存在
    OPEN_FILE_TRUE,//打开文件成功
    OPEN_FILE_FALSE,//打开文件失败
    RENAME_FILE_TRUE,//重命名文件成功
    RENAME_FILE_FALSE,//重命名文件失败
    DELETE_FILE_TRUE,//删除文件成功
    DELETE_FILE_FALSE,//删除文件失败
    COPYMOVE_FILE_TRUE,//复制移动成功
    COPYMOVE_FILE_FALSE,//复制移动失败
    CUTMOVE_FILE_TRUE,//剪切移动成功
    CUTMOVE_FILE_FALSE,//剪切移动失败
    STATUS_MAX=0xffffff
};
enum FILETYPE//文件类型
{
    FILETYPE_NULL=0,
    FILETYPE_DIR,//文件夹
    FILETYPE_FILE,//普通文件
    //待添加...
    FILETYPE_MAX=0xffffff
};

struct FILEINFO//文件结构
{
    char FileName[64];//文件名字
    unsigned int FileType;//文件类型
    // unsigned int FileSize;//文件大小
    // bool Share;//是否为共享文件
    // char FileMtime[32];//最后修改时间
};
struct TCP_PDU//传输协议
{
    unsigned int TCP_PDULength;//协议数据单元大小
    unsigned int MessageType;//消息类型
    unsigned int Status;//状态码
    unsigned int MessageLength;//实际消息长度
    char Data[128];//定长数据
    int Message[];//实际消息（不定长数据）
};
struct TCP_PDU* ApplyTCP_PDU(unsigned int MessageLength);//申请传输协议数据空间
#endif // PROTOCOL_H
