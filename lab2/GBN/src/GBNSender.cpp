//
// Created by poorpool on 2021/10/17.
//

#include "Global.h"
#include "GBNSender.h"

GBNSender::GBNSender(int seqLen, int cwnd) : SEQSPACE_(1 << seqLen), CWND_(cwnd) {
    base_ = 1;
    nextSeqNum_ = 1;
    sndpkts_ = new Packet[CWND_];
    fp_ = fopen("/home/poorpool/Documents/HUST/计算机网络/实验/lab2/GBN/bin/sliding_window.txt",
                "w");
}

GBNSender::~GBNSender() {
    delete[] sndpkts_;
    fclose(fp_);
}

bool GBNSender::getWaitingState() {
    return ((nextSeqNum_ - base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_>=  + CWND_;
}

void GBNSender::printSlidingWindow() {
    printf("Sliding Window [sent but not acked | available] : [ ");
    fprintf(fp_, "Sliding Window [sent but not acked | available] : [ ");
    for (int i = base_; i != nextSeqNum_; i = (i + 1) % SEQSPACE_) {
        printf("%d ", i);
        fprintf(fp_, "%d ", i);
    }
    printf("| ");
    fprintf(fp_, "| ");
    for (int i = nextSeqNum_; i != (base_ + CWND_) % SEQSPACE_; i = (i + 1) % SEQSPACE_) {
        printf("%d ", i);
        fprintf(fp_, "%d ", i);
    }
    printf("]\n");
    fprintf(fp_, "]\n");
}

bool GBNSender::send(const Message &message) {
    if (getWaitingState()) {
        return false;
    }
    auto &packet(sndpkts_[nextSeqNum_ % CWND_]);  // 语法糖

    // 构造发包
    packet.acknum = -1;  // 忽略该字段
    packet.seqnum = nextSeqNum_;
    packet.checksum = 0;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);
    pUtils->printPacket("发送方发送报文", packet);
    pns->sendToNetworkLayer(RECEIVER, packet);

    // 启动发送方唯一的定时器
    if (base_ == nextSeqNum_) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
    }

    nextSeqNum_ = (nextSeqNum_ + 1) % SEQSPACE_;
    printSlidingWindow();

    return true;
}

void GBNSender::receive(const Packet &ackPkt) {
    // 检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(ackPkt);

    // 如果校验和正确
    if (checkSum == ackPkt.checksum) {
        base_ = (ackPkt.acknum + 1) % SEQSPACE_;
        if (base_ == nextSeqNum_) {
            // 关闭定时器
            pns->stopTimer(SENDER, 1);
        } else {
            // 重启定时器
            pns->stopTimer(SENDER, 1);
            pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
        }
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        printSlidingWindow();
    } else {
        pUtils->printPacket("发送方收到了错误的确认", ackPkt);
        // 忽略该包
    }
}

void GBNSender::timeoutHandler(int seqNum) {
    // 唯一一个定时器,无需考虑 seqNum
    // 重启定时器
    pns->stopTimer(SENDER, 1);
    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
    int len = ((nextSeqNum_ - base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_;

    // 重新发送数据包
    for (int i = base_; i < base_ + len; i++) {
        pUtils->printPacket("发送方定时器时间到，重发报文：",
                            sndpkts_[(i % SEQSPACE_) % CWND_]);
        pns->sendToNetworkLayer(RECEIVER, sndpkts_[(i % SEQSPACE_) % CWND_]);
    }
}
