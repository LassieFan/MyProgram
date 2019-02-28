/*************************************************************************
    > File Name: ThreadPool.h
    > Created Time: Mon 03 Dec 2018 02:27:44 PM CST
 ************************************************************************/
#ifndef __THREADPOOL_H
#define __THREADPOOL_H
#include<iostream>
#include<map>
#include<event.h>
#include<sys/types.h>
#include<sys/socket.h>
#include"WorkThread.h"
#define TNum 3
using namespace std;

class ThreadPool
{
public:
	ThreadPool(struct event_base* base)//需要将socketpair加入主服务器监听
	{
		for(int i=0;i<TNum;i++)  //每个工作线程都需要socketpair
		{
			int pipefd[2];
			if(-1==socketpair(PF_UNIX,SOCK_DGRAM,0,pipefd))
			{
				cout<<"socketpair create error"<<endl;
				return;
			}
			struct event* sock_event=event_new(base,pipefd[1],EV_READ|EV_PERSIST,ThreadPool::SockCall,NULL); //针对读事件sockpair[1]监听读事件
			if(NULL==sock_event)
			{
				cout<<"socketpair add fail"<<endl;
				return;
			}
			event_add(sock_event,NULL);
	        _mmap.insert(pair<int,int>(pipefd[1],0));
			wth[i]=new WorkThread(pipefd[0]);   //将工作线程与pipefd关联
		}
	}
	typedef map<int,int> MAP_T;
	void SendToWorkThread(int clifd); //将就绪的链接套接子通过socketpair发给子线程
	static void SockCall(int fd,short event,void *arg);   //socketpair可读事件发生
private:
	WorkThread *wth[3];
	static map<int,int> _mmap;
};

map<int,int> ThreadPool::_mmap;
void ThreadPool::SockCall(int fd,short event,void *arg)  //针对socketpair的回调,针对读事件触发，更新map表
{
	char buff[128]={0};
	if(-1==recv(fd,buff,127,0))
	{
		cout<<"sockpair[1] recv fail"<<endl;
		return;
	}
	int num;
	sscanf(buff,"%d",&num);
//	cout<<num<<endl;
	_mmap[fd]=num;
}

void ThreadPool::SendToWorkThread(int clifd)//从map表中查找一个线程当前监听个数最少的,通过sockpair键将值传给工作线程
{
	MAP_T::iterator min=_mmap.begin();  //记录最小的
	MAP_T::iterator it=_mmap.begin();    //用于遍历
	int num=it->second;   //标志加到那个键里面
	for(;it!=_mmap.end();it++)
	{
		cout<<it->first<<"    "<<it->second<<endl;
		if(it->second<=num)
		{
			num=it->second;
			min=it;
		}
	}
	char buff[10]={0};
	sprintf(buff,"%d",clifd);
	send(min->first,buff,strlen(buff),0);   //将套接子发送给了工作线程
}
#endif
