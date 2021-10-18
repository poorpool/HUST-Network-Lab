//
// Created by poorpool on 2021/10/17.
//
#ifndef GBN_GBNRECEIVER_H
#define GBN_GBNRECEIVER_H

#include "RdtReceiver.h"

class GBNReceiver : public RdtReceiver {
private:
    int expectedSeqNum_;
    Packet sndpkt_;
public:
    GBNReceiver();
    virtual ~GBNReceiver();

    void receive(const Packet &packet);  //接收报文，将被 NetworkService 调用
};
#endif //GBN_GBNRECEIVER_H
