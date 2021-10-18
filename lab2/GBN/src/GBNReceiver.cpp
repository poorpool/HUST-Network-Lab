//
// Created by poorpool on 2021/10/17.
//

#include "Global.h"
#include "GBNReceiver.h"

GBNReceiver::GBNReceiver() : expectedSeqNum_(1) {
    sndpkt_.acknum = 0; // 从 1 到 acknum 的都收到了
    sndpkt_.checksum = 0;
    sndpkt_.seqnum = -1;	//忽略该字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++){
        sndpkt_.payload[i] = '.';
    }
    sndpkt_.checksum = pUtils->calculateCheckSum(sndpkt_);
}

GBNReceiver::~GBNReceiver() = default;

void GBNReceiver::receive(const Packet &packet) {
    // 检查校验和是否正确
    int checkSum = pUtils->calculateCheckSum(packet);

    // 如果校验和正确，同时收到报文的序号与接收方期待收到的报文序号一致
    if (checkSum == packet.checksum && expectedSeqNum_ == packet.seqnum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        // 取出 Message，向上递交给应用层
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        sndpkt_.acknum = packet.seqnum; //确认序号等于收到的报文序号
        sndpkt_.checksum = pUtils->calculateCheckSum(sndpkt_);
        pUtils->printPacket("接收方发送确认报文", sndpkt_);
        pns->sendToNetworkLayer(SENDER, sndpkt_);	// 调用模拟网络环境的 sendToNetworkLayer，通过网络层发送确认报文到对方
        expectedSeqNum_++;  // 接收序号增加
    }
    else {
        if (checkSum != packet.checksum) {
            pUtils->printPacket("接收方没有正确收到发送方的报文，数据校验错误", packet);
        }
        else {
            pUtils->printPacket("接收方没有正确收到发送方的报文，报文序号不对", packet);
        }
        pUtils->printPacket("接收方重新发送上次的确认报文", sndpkt_);
        pns->sendToNetworkLayer(SENDER, sndpkt_);	// 调用模拟网络环境的 sendToNetworkLayer，通过网络层发送上次的确认报文
    }
}
