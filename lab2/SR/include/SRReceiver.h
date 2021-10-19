//
// Created by poorpool on 2021/10/17.
//

#ifndef GBN_GBNRECEIVER_H
#define GBN_GBNRECEIVER_H

#include "RdtReceiver.h"

// RdtReceiver 的具体实现类
class SRReceiver : public RdtReceiver {
private:
    const int SEQSPACE_;  // 序列号空间在 [0, SEQSPACE_)
    const int CWND_;  // 接收的滑动窗口
    int rcv_base_;  // 滑动窗口的基
    Packet sndpkt_;  // 每次的响应包
    Message * msgs_;  // 收到的消息
    bool *received_;  // 是否收到了
public:
    SRReceiver(int seqLen = 3, int cwnd = 4);
    virtual ~SRReceiver();

    //接收报文，将被 NetworkService 调用
    void receive(const Packet &packet);
};
#endif //GBN_GBNRECEIVER_H
