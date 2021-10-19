// GBN.cpp : 定义控制台应用程序的入口点。

#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "SRSender.h"
#include "SRReceiver.h"

int main(int argc, char* argv[]) {
	RdtSender *ps = new SRSender();
	RdtReceiver * pr = new SRReceiver();
	pns->setRunMode(0);  // VERBOSE 模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("/home/poorpool/Documents/HUST/计算机网络/实验/lab2/SR/bin/input.txt");
	pns->setOutputFile("/home/poorpool/Documents/HUST/计算机网络/实验/lab2/SR/bin/output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;  // 指向唯一的工具类实例，只在 main 函数结束前 delete
	delete pns;  // 指向唯一的模拟网络环境类实例，只在 main 函数结束前 delete
	
	return 0;
}

