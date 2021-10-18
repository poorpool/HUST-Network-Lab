//
// Created by poorpool on 2021/10/17.
//
#ifndef GBN_GBNSENDER_H
#define GBN_GBNSENDER_H

#include "RdtSender.h"

class GBNSender : public RdtSender {
private:
    const int CWND_;  // 窗口长度
    int base_;  // 基序号
    int nextSeqNum_;  // 下一个序号
    Packet * sndpkts_;
    FILE * fp_;
public:
    GBNSender(int cwnd = 5);
    virtual ~GBNSender();

    bool getWaitingState();
    bool send(const Message &message);  // 发送应用层下来的 Message，由 NetworkServiceSimulator 调用
                                        // 如果发送方成功地将 Message 发送到网络层，返回 true；
                                        // 如果因为发送方处于等待正确确认状态而拒绝发送 Message，则返回 false
    void receive(const Packet &ackPkt);  // 接受确认Ack，将被 NetworkServiceSimulator 调用
    void timeoutHandler(int seqNum);  // Timeout handler，将被 NetworkServiceSimulator 调用
    void printSlidingWindow();
};
#endif //GBN_GBNSENDER_H
