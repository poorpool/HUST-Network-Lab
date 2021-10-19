//
// Created by poorpool on 2021/10/17.
//

#include "Global.h"
#include "SRReceiver.h"

#define LOG(args...) printf(args); \
                     fprintf(window_fp, args)

SRReceiver::SRReceiver(int seqLen, int cwnd) : SEQSPACE_(1 << seqLen), CWND_(cwnd), rcv_base_(1) {
    // 设置一个没有什么意义的初始回包
    sndpkt_.acknum = 0;
    sndpkt_.checksum = 0;
    sndpkt_.seqnum = -1;	//忽略该字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++){
        sndpkt_.payload[i] = '.';
    }
    sndpkt_.checksum = pUtils->calculateCheckSum(sndpkt_);

    msgs_ = new Message[CWND_];
    received_ = new bool[CWND_];
    memset(received_, false, CWND_ * sizeof(bool));
}

SRReceiver::~SRReceiver() {
    delete[] msgs_;
    delete[] received_;
}

void SRReceiver::printSlidingWindow() {
    LOG("Receiver Window [!1 want, .1 acked] : [ ");
    for (int i = 0; i < CWND_; i++) {
        int idx = (rcv_base_ + i) % SEQSPACE_ % CWND_;
        if (received_[idx]) {
            LOG(".%d ", (rcv_base_ + i) % SEQSPACE_);
        } else {
            LOG("!%d ", (rcv_base_ + i) % SEQSPACE_);
        }
    }
    LOG("]\n");
}

void SRReceiver::receive(const Packet &packet) {
    // 检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(packet);

    // 如果校验和正确
    if (checkSum == packet.checksum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);

        // 如果 seq 在接收方的滑动窗口中
        int seq = packet.seqnum;
        int bias = ((seq - rcv_base_) % SEQSPACE_ + SEQSPACE_) % SEQSPACE_;
        if (bias <= CWND_ - 1) {
            int idx = seq % SEQSPACE_ % CWND_;

            // 如果以前没收到过，则缓存该分组
            if (!received_[idx]) {
                received_[idx] = true;
                memcpy(msgs_[idx].data, packet.payload, sizeof(packet.payload));
            }

            // 连续交付给上层
            while (received_[rcv_base_ % CWND_]) {
                received_[rcv_base_ % CWND_] = false;
                pns->delivertoAppLayer(RECEIVER, msgs_[rcv_base_ % CWND_]);
                rcv_base_ = (rcv_base_ + 1) % SEQSPACE_;
            }

            printSlidingWindow();
        }

        // 这里的 ack 不是 TCP 的累计确认，而是来啥确认啥
        sndpkt_.acknum = seq;
        sndpkt_.checksum = pUtils->calculateCheckSum(sndpkt_);

        // 回包
        pUtils->printPacket("接收方发送确认报文", sndpkt_);
        pns->sendToNetworkLayer(SENDER, sndpkt_);
    }
    else {
        pUtils->printPacket("接收方没有正确收到发送方的报文，数据校验错误", packet);
        pUtils->printPacket("接收方重新发送上次的确认报文", sndpkt_);
        pns->sendToNetworkLayer(SENDER, sndpkt_);
    }
}
