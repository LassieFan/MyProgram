/*************************************************************************
    > File Name: ThreadPool.h
    > Created Time: Tue 15 Jan 2019 11:01:53 AM CST
 ************************************************************************/
#ifndef __THREADPOOL_H
#define __THREADPOOL_H
#include<iostream>
#include"TcpServer.h"
#include"ShortLinkThread.h"
#include"CHash.h"
using namespace std;
class ThreadPool
{
public:
	ThreadPool(TcpServer* l,CHash* ca)
	{
		for(int i=0;i<3;i++)
		{
			slt[i]=new ShortLinkThread(l,ca);
		}
	}
private:
	ShortLinkThread* slt[3];
};
#endif


