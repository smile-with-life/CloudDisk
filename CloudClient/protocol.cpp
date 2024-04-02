#include"protocol.h"
#include <stdlib.h>
#include <unistd.h>
struct TCP_PDU *ApplyTCP_PDU(unsigned int MessageLength)//申请协议数据空间
{
    unsigned int Length=sizeof(TCP_PDU)+MessageLength;
    TCP_PDU* pdu = (TCP_PDU*)malloc(Length);//申请协议数据大小的空间，用于存储要发送的数据
    if(pdu == NULL)//空间申请失败
    {
        exit(EXIT_FAILURE);
    }
    //memset((char*)(pdu),0,Length);//把整个数据空间初始化为0
    pdu->TCP_PDULength = Length;//传输协议的大小
    pdu->MessageLength=MessageLength;//实际消息长度
    return pdu;//返回申请后的传输协议数据空间
}
