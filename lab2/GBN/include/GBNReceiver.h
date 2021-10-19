//
// Created by poorpool on 2021/10/17.
//

#ifndef GBN_GBNRECEIVER_H
#define GBN_GBNRECEIVER_H

#include "RdtReceiver.h"

// RdtReceiver 的具体实现类
class GBNReceiver : public RdtReceiver {
private:
    const int SEQSPACE_;  // 序列号空间在 [0, SEQSPACE_)
    int expectedSeqNum_;  // 期待收到的报文的 seq
    Packet sndpkt_;  // 每次的响应包
public:
    GBNReceiver(int seqLen = 3);
    virtual ~GBNReceiver();

    //接收报文，将被 NetworkService 调用
    void receive(const Packet &packet);
};
#endif //GBN_GBNRECEIVER_H
