//
// Created by poorpool on 2021/10/17.
//

#include "Global.h"
#include "GBNSender.h"

GBNSender::GBNSender(int cwnd) : CWND_(cwnd) {
    base_ = 1;
    nextSeqNum_ = 1;
    sndpkts_ = new Packet[CWND_];
    fp_ = fopen("/home/poorpool/Documents/HUST/计算机网络/实验/lab2/GBN/bin/sliding_window.txt", "w");
}

GBNSender::~GBNSender() {
    delete[] sndpkts_;
    fclose(fp_);
}

bool GBNSender::getWaitingState() {
    return nextSeqNum_ >= base_ + CWND_;
}

void GBNSender::printSlidingWindow() {
    printf("Sliding Window: [%d,%d) sent but not acked, [%d,%d) available\n", base_, nextSeqNum_, nextSeqNum_, base_ + CWND_);
    fprintf(fp_, "Sliding Window: [%d,%d) sent but not acked, [%d,%d) available\n", base_, nextSeqNum_, nextSeqNum_, base_ + CWND_);
}

bool GBNSender::send(const Message &message) {
    if (getWaitingState()) {
        return false;
    }
    auto &packet(sndpkts_[nextSeqNum_ % CWND_]);
    packet.acknum = -1;  // 忽略该字段
    packet.seqnum = nextSeqNum_;
    packet.checksum = 0;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);
    pUtils->printPacket("发送方发送报文", packet);
    pns->sendToNetworkLayer(RECEIVER, packet);  // 调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    if (base_ == nextSeqNum_) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, 1);  // 启动发送方唯一的定时器
    }
    nextSeqNum_++;
    printSlidingWindow();
    return true;
}

void GBNSender::receive(const Packet &ackPkt) {
    // 检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(ackPkt);

    // 如果校验和正确
    if (checkSum == ackPkt.checksum) {
        base_ = ackPkt.acknum + 1;
        if (base_ == nextSeqNum_) {
            pns->stopTimer(SENDER, 1);		// 关闭定时器
        } else {
            pns->stopTimer(SENDER, 1);
            pns->startTimer(SENDER, Configuration::TIME_OUT, 1);  // 重启定时器
        }
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        printSlidingWindow();
    } else {
        pUtils->printPacket("发送方收到了错误的确认", ackPkt);
    }
}

void GBNSender::timeoutHandler(int seqNum) {
    // 唯一一个定时器,无需考虑 seqNum
    pns->stopTimer(SENDER, 1);										//首先关闭定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);			//重新启动发送方定时器
    for (int i = base_; i < nextSeqNum_; i++) {
        pUtils->printPacket("发送方定时器时间到，重发报文：", sndpkts_[i % CWND_]);
        pns->sendToNetworkLayer(RECEIVER, sndpkts_[i % CWND_]);			//重新发送数据包
    }
}
