//
// Created by poorpool on 2021/10/17.
//

#include "Global.h"
#include "TCPSender.h"

#define LOG(args...) printf(args); \
                     fprintf(window_fp, args)

FILE *window_fp;

TCPSender::TCPSender(int seqLen, int cwnd) : SEQSPACE_(1 << seqLen), CWND_(cwnd) {
    base_ = 1;
    nextSeqNum_ = 1;
    sndpkts_ = new Packet[CWND_];
    window_fp = fopen("/home/poorpool/Documents/HUST/计算机网络/实验/lab2/TCP/bin/sliding_window.txt",
                "w");
    last_ack_ = -1;
    last_ack_cnt_ = 1;
}

TCPSender::~TCPSender() {
    delete[] sndpkts_;
    fclose(window_fp);
}

bool TCPSender::getWaitingState() {
    return ((nextSeqNum_ - base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_>=  + CWND_;
}

void TCPSender::printSlidingWindow() {
    LOG("Sliding Window [sent but not acked | available] : [ ");
    for (int i = base_; i != nextSeqNum_; i = (i + 1) % SEQSPACE_) {
        LOG("%d ", i);
    }
    LOG("| ");
    for (int i = nextSeqNum_; i != (base_ + CWND_) % SEQSPACE_; i = (i + 1) % SEQSPACE_) {
        LOG("%d ", i);
    }
    LOG("]\n");
}

bool TCPSender::send(const Message &message) {
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

void TCPSender::receive(const Packet &ackPkt) {
    // 检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(ackPkt);

    // 如果校验和正确
    if (checkSum == ackPkt.checksum) {
        if (ackPkt.acknum == last_ack_) {
            last_ack_cnt_++;
        } else {
            last_ack_ = ackPkt.acknum;
            last_ack_cnt_ = 1;
        }
        // 本质上是累计确认
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
        if (last_ack_cnt_ >= 3) {
            last_ack_cnt_ = 0;
            if (base_ != nextSeqNum_) {
                pUtils->printPacket("发送方快速重传！！！", sndpkts_[base_ % CWND_]);
                pns->sendToNetworkLayer(RECEIVER, sndpkts_[base_ % CWND_]);
                LOG("！！！连续收到对%d的三次确认，快速重传\n", last_ack_);
            }
        }
        printSlidingWindow();
    } else {
        // 忽略该包
        pUtils->printPacket("发送方收到了错误的确认", ackPkt);
    }
}

void TCPSender::timeoutHandler(int seqNum) {
    // 唯一一个定时器,无需考虑 seqNum
    // 重启定时器
    pns->stopTimer(SENDER, 1);
    if (base_ != nextSeqNum_) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
        // 重新发送数据包
        pUtils->printPacket("发送方定时器时间到，重发报文：", sndpkts_[base_ % CWND_]);
        pns->sendToNetworkLayer(RECEIVER, sndpkts_[base_ % CWND_]);
    }

}
