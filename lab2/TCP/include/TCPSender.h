//
// Created by poorpool on 2021/10/17.
//
#ifndef GBN_TCPSENDER_H
#define GBN_TCPSENDER_H

#include "RdtSender.h"

// RdtSender 的具体实现类
class TCPSender : public RdtSender {
private:
    const int SEQSPACE_;  // 序列号空间在 [0, SEQSPACE_)
    const int CWND_;  // 窗口长度
    int base_;  // 基序号
    int nextSeqNum_;  // 下一个发包的序号
    Packet * sndpkts_;  // 发送窗口中的包
    int last_ack_;  // 上一个收到的 ack
    int last_ack_cnt_;  // 连续收到的相同 ack 个数
public:
    TCPSender(int seqLen = 3, int cwnd = 4);
    virtual ~TCPSender();

    // 返回 TCPSender 是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回 true
    bool getWaitingState();

    // 发送应用层下来的 Message，由 NetworkServiceSimulator 调用
    // 如果发送方成功地将 Message 发送到网络层，返回 true；
    // 如果因为发送方处于等待正确确认状态而拒绝发送 Message，则返回 false
    bool send(const Message &message);

    // 接受确认Ack，将被 NetworkServiceSimulator 调用
    void receive(const Packet &ackPkt);

    // Timeout handler，将被 NetworkServiceSimulator 调用
    void timeoutHandler(int seqNum);

    // 打印滑动窗口辅助函数
    void printSlidingWindow();
};
#endif //GBN_TCPSENDER_H
