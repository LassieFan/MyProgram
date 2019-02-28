/*************************************************************************
    > File Name: ShortLinkThread.h
    > Created Time: Tue 15 Jan 2019 11:10:44 AM CST
 ************************************************************************/

#ifndef __SHORTLINKTHREAD_H
#define __SHORTLINKTHREAD_H
#include<iostream>
#include<string.h>
#include<json/json.h>
#include"TcpServer.h"
#include"CHash.h"
#include"Node.h"

using namespace std;
class ShortLinkThread
{
public:
	ShortLinkThread(TcpServer* l,CHash* ch)
	{
		ts=l;
		cha=ch;
		pthread_t id;
		pthread_create(&id,NULL,DealEvent,this);
	}
	static void* DealEvent(void *arg);
	TcpServer* ts;
	CHash * cha;
};

void* ShortLinkThread::DealEvent(void *arg)
{
	ShortLinkThread* l=static_cast<ShortLinkThread*>(arg);
	while(1)
	{
		int c=l->ts->Accept();
		if(-1==c)
		{
			cout<<"accept fail"<<endl;
			return NULL;
		}
		struct sockaddr_in peeraddr;
		socklen_t len=sizeof(peeraddr);
		if(-1==getpeername(c,(struct sockaddr*)&peeraddr,&len))
		{
			cout<<"get ip and port fail"<<endl;
			return NULL;
		}
		string ip=inet_ntoa(peeraddr.sin_addr);
		short port=ntohs(peeraddr.sin_port);
		char mport[10]={0};
		sprintf(mport,"%d",port);
		string key=ip+mport;
		long hash=l->cha->GetValue(key);
		Node *p=l->cha->FindNode(hash);
		string sip=p->GetIp();
		unsigned short sport=(unsigned short)p->GetPort();
		Json::Value val;
		val["ip"]=sip;
		val["port"]=sport;
		if(-1==send(c,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
		{
			cout<<"json send fail"<<endl;
			return NULL;
		}
		close(c);
	}
}
#endif
