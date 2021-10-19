//
// Created by poorpool on 2021/10/17.
//

#include "Global.h"
#include "SRSender.h"

FILE * window_fp = nullptr;

#define LOG(args...) printf(args); \
                     fprintf(window_fp, args)

SRSender::SRSender(int seqLen, int cwnd) : SEQSPACE_(1 << seqLen), CWND_(cwnd) {
    base_ = 1;
    nextSeqNum_ = 1;
    sndpkts_ = new Packet[CWND_];
    window_fp = fopen("/home/poorpool/Documents/HUST/计算机网络/实验/lab2/SR/bin/sliding_window.txt",
                "w");
    acked_ = new bool[CWND_];
    memset(acked_, false, CWND_ * sizeof(bool));
}

SRSender::~SRSender() {
    delete[] sndpkts_;
    delete[] acked_;
    fclose(window_fp);
}

bool SRSender::getWaitingState() {
    return ((nextSeqNum_ - base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_ >= CWND_;
}

void SRSender::printSlidingWindow() {
    LOG("Sender Window [!1 sent, .1 acked | @available] : [ ");
    for (int i = base_; i != nextSeqNum_; i = (i + 1) % SEQSPACE_) {
        if (acked_[i % CWND_]) {
            LOG(".%d ", i);
        } else {
            LOG("!%d ", i);
        }
    }
    LOG("| ");
    for (int i = nextSeqNum_; i != (base_ + CWND_) % SEQSPACE_; i = (i + 1) % SEQSPACE_) {
        LOG("@%d ", i);
    }
    LOG("]\n");
}

bool SRSender::send(const Message &message) {
    if (getWaitingState()) {
        return false;
    }
    auto &packet(sndpkts_[nextSeqNum_ % CWND_]);  // 语法糖

    // 即刻发包
    packet.acknum = -1;  // 忽略该字段
    packet.seqnum = nextSeqNum_;
    packet.checksum = 0;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);
    pUtils->printPacket("发送方发送报文", packet);
    pns->sendToNetworkLayer(RECEIVER, packet);

    // 启动发送方的跟序号相关联的定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum_);

    nextSeqNum_ = (nextSeqNum_ + 1) % SEQSPACE_;
    printSlidingWindow();

    return true;
}

void SRSender::receive(const Packet &ackPkt) {
    // 检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(ackPkt);

    // 如果校验和正确
    if (checkSum == ackPkt.checksum) {
        int bias = ((ackPkt.acknum - base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_;

        // 收到的确认在发送窗口中
        if (bias <= CWND_ - 1) {
            // 停止定时器
            if (!acked_[ackPkt.acknum % CWND_]) {
                acked_[ackPkt.acknum % CWND_] = true;
                pns->stopTimer(SENDER, ackPkt.acknum);
            }

            // 窗口滑动
            while (acked_[base_ % CWND_]) {
                acked_[base_ % CWND_] = false;
                base_ = (base_ + 1) % SEQSPACE_;
            }
            pUtils->printPacket("发送方正确收到确认", ackPkt);
            printSlidingWindow();
        } else {
            // 过时确认直接忽略
            pUtils->printPacket("发送方收到过时的确认", ackPkt);
        }
    } else {
        // 错误确认也忽略
        pUtils->printPacket("发送方收到了错误的确认", ackPkt);
    }
}

void SRSender::timeoutHandler(int seqNum) {
    // 重启定时器并重发报文
    pns->stopTimer(SENDER, seqNum);
    int bias = ((seqNum - base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_;
    if (bias <= CWND_ - 1) { // 防止定时器超时的时候窗口已经移动过 seqNum
        pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
        pUtils->printPacket("发送方定时器时间到，重发报文：", sndpkts_[seqNum % CWND_]);
        pns->sendToNetworkLayer(RECEIVER, sndpkts_[seqNum % CWND_]);
    }
}
