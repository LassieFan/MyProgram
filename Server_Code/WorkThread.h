/*************************************************************************
    > File Name: WorkThread.h
    > Created Time: Sat 08 Dec 2018 07:02:39 PM CST
 ************************************************************************/
#ifndef __WORKTHREAD_H
#define __WORKTHREAD_H
#include<iostream>
#include<pthread.h>
#include<event.h>
#include<string.h>
#include<string>
#include<json/json.h>
#include<sys/socket.h>
#include<sys/types.h>
#include"Contral.h"
using namespace std;

class WorkThread   
{
public:
	WorkThread(int fd)  //fd是sockpair[0]
	{
		pthread_t id;
		_fd=fd;
		pthread_create(&id,NULL,DealEvent,this);
		con=new Contral();
	}
	static void CliCall(int fd,short event,void *arg);
	static void SockCall(int fd,short event,void *arg); //sockpair事件发生
	static void	TimeCall(int fd,short event,void* arg); //定时事件，发送数量
	static void* DealEvent(void *arg);
private:
	event_base *ebase;    //不能用static，因为每个线程都有libevent
	int count;   //记录当前线程中监听多少个套接子
	int _fd;
	map<int,struct event*>eventMap;  //清除链接的事件
	static Contral* con;   //model对象，用于解析json包
	
};
Contral* WorkThread::con=NULL;
void* WorkThread::DealEvent(void *arg)
{
	WorkThread* p=static_cast<WorkThread*>(arg);
	p->ebase=event_base_new();
	struct event* sock_event=event_new(p->ebase,p->_fd,EV_READ|EV_PERSIST,SockCall,p);   //将sockpair加入lievent
	if(sock_event==NULL)
	{
		cout<<"sockpair event bulid fail"<<endl;
		return NULL;
	}
	event_add(sock_event,NULL);

	//定时器事件
	struct timeval tv={2,0};
	struct event* time_event=evtimer_new(p->ebase,TimeCall,p);
	event_add(time_event,&tv);

	event_base_dispatch(p->ebase);  //dispatch在哪里libevent就在那个线程中
}
void WorkThread::TimeCall(int fd,short event,void* arg)
{
	WorkThread* p=static_cast<WorkThread*>(arg);
	char buff[128]={0};
	sprintf(buff,"%d",p->count);
	if(-1==send(p->_fd,buff,127,0))
		cout<<"send error"<<endl;
}

void WorkThread::SockCall(int fd,short event,void *arg)  //sockpair有事件
{
	WorkThread* p=static_cast<WorkThread*>(arg);
	char buff[128]={0};
	int n=recv(fd,buff,127,0);
	if(n<=0)
		return;
	int c;
	sscanf(buff,"%d",&c);
	struct event* cli_event=event_new(p->ebase,c,EV_READ|EV_PERSIST,WorkThread::CliCall,p); 
	if(cli_event==NULL)
	{
		cout<<"clievent build fail"<<endl;
		return;
	}
	event_add(cli_event,NULL);
	p->eventMap.insert(make_pair(c,cli_event));
	(p->count)++;   //接受一个链接count++
}
void WorkThread::CliCall(int fd,short event,void *arg) //事件发生发送给model
{
	WorkThread* p=static_cast<WorkThread*>(arg);
	char buff[128]={0};
	int n=recv(fd,buff,127,0);
	if(n<=0)
	{
		cout<<"one link down line"<<endl;
		event_free(p->eventMap[fd]);
		p->eventMap.erase(fd);
		p->count--;
		close(fd);
		return;
	}
/*	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
	{
		cout<<"json read error"<<endl;
		return;
	}
	cout<<val.toStyledString()<<endl;  为了测试写的输出语句，没什么问题*/
	WorkThread::con->Process(buff,fd);	
}
#endif
