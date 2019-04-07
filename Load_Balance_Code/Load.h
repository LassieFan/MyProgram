/*************************************************************************
    > File Name: load.h
    > Created Time: Wed 09 Jan 2019 06:32:51 PM CST
 ************************************************************************/
#ifndef __LOAD_H
#define __LOAD_H
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<event.h>
#include<map>
#include<json/json.h>
#include<algorithm>
#include<sys/types.h>
#include"TcpServer.h"
#include"Node.h"
#include"CHash.h"
#include"ThreadPool.h"
class Load
{
public:
	Load()
	{
		ss=new TcpServer("127.0.0.1",8600);
		cs=new TcpServer("127.0.0.1",8700);
		
		base=event_base_new();
		struct event* ss_event=event_new(base,ss->GetSockfd(),EV_READ|EV_PERSIST,Load::LinkSs_call,this);
		if(NULL==ss_event)
		{
			cout<<"new ss_event create fail"<<endl;
			return;
		}
		/*struct event* cs_event=event_new(base,cs->GetSockfd(),EV_READ|EV_PERSIST,Load::LinkCs_call,this);
		if(NULL==cs_event)
		{
			cout<<"new cs_event create fail"<<endl;
		}*/
		event_add(ss_event,NULL);
	//	event_add(cs_event,NULL);
		cha=new CHash();
		thp=new ThreadPool(cs,cha);
	}
	void Run();
	static void LinkSs_call(int fd,short event,void* arg);
	static void Ss_call(int fd,short event,void* arg);
//	static void LinkCs_call(int fd,short event,void* arg);
private:
	TcpServer* ss;   //连接服务器的负载均衡服务器
	TcpServer* cs;   //连接客户端的负载均衡服务器
	static struct event_base *base;   
	static map<int,Node*> mmap;   //服务器fd与实体节点的链接，删除链接用
	static map<int,struct event*>eventMap; 
	static CHash *cha;   //一致性哈希环
	ThreadPool* thp;
};
struct event_base* Load::base=NULL;
map<int,struct event*> Load::eventMap;
map<int,Node*> Load::mmap;
CHash* Load::cha=NULL;
void Load::Run()
{
	event_base_dispatch(base);
}
void Load::LinkSs_call(int fd,short event,void* arg) //服务器链接回调函数
{
	Load* l=static_cast<Load*>(arg);
	int sfd=l->ss->Accept();   //用于与服务器传输数据的socket
	if(-1==sfd)
	{
		cout<<"accept fail"<<endl;
		return;
	}
	struct event* ss_event=event_new(Load::base,sfd,EV_READ|EV_PERSIST,Load::Ss_call,l);
	if(NULL==ss_event)
	{
		cout<<"ss_cli event create fail"<<endl;
		return;
	}
	eventMap.insert(make_pair(sfd,ss_event));
	event_add(ss_event,NULL);
}
void Load::Ss_call(int fd,short event,void *arg)  //与服务器交互数据的回调
{
	Load* l=static_cast<Load*>(arg);
	string buff;
	int n=l->ss->Recv(fd,buff);
	if(0>=n)   //一个服务器断开链接或者宕机 
	{
		cout<<"one ser down"<<endl;
		map<int,Node*>::iterator it=mmap.find(fd);//删除哈希环上的实节点对应的虚节点
		if(it!=mmap.end())
		{
			cha->Delete(it->second);
		}
		mmap.erase(it++);  
		event_free(eventMap[fd]);
		eventMap.erase(fd);
		close(fd);
		return;
	}
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff.c_str(),val))
	{
		cout<<"read json error"<<endl;
		return;
	}
	string ip=val["ip"].asString();
	unsigned short port=val["port"].asInt();
	int loadnum=val["num"].asInt();

	Node *nod=new Node(ip,port,loadnum);  
	mmap.insert(make_pair(fd,nod));  
	cha->Insert(nod);
}
/*void Load::LinkCs_call(int fd,short event,void* arg) //监听到客户端的链接
{
	Load* l=static_cast<Load*>(arg);
	int cfd=l->cs->Accept();
	if(-1==cfd)
	{
		cout<<"accept fail"<<endl;
		return;
	}
	struct sockaddr_in peeraddr;  //用socket获取对端ip和port
	socklen_t len=sizeof(peeraddr);
	if(-1==getpeername(cfd,(struct sockaddr*)&peeraddr,&len))
	{
		cout<<"get ip and port fail"<<endl;
		return;
	}
	string ip=inet_ntoa(peeraddr.sin_addr); 
	short port=ntohs(peeraddr.sin_port);
	char mport[10]={0};
	sprintf(mport,"%d",port);
	string key=ip+mport;
	long hash=cha->GetValue(key);
	Node *p=cha->FindNode(hash);   //找到了顺时针的第一个
	string sip=p->GetIp();
	unsigned short sport=(unsigned short)p->GetPort();
	Json::Value val;
	val["ip"]=sip;
	val["port"]=sport;
	if(-1==send(cfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cout<<"json send fail"<<endl;
		return;
	}
	close(cfd);   //断开连接
}*/
#endif


