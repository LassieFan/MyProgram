/*************************************************************************
    > File Name: MainSer.h
    > Created Time: Mon 03 Dec 2018 02:27:32 PM CST
 ************************************************************************/

#ifndef __MAINSER_H
#define __MAINSER_H
#include<iostream>
#include<event.h>
#include<json/json.h>
#include"TcpServer.h"
#include"EnumType.h"
#include"ThreadPool.h"
#include"TcpClient.h"
using namespace std;

class MainSer
{
public:
	MainSer()
	{
		cout<<"please input your ser port"<<endl;
		unsigned _port;
		cin>>_port;
		port=_port;
		ser=new TcpServer("127.0.0.1",port);  //绑定主服务器的ip端口
		cli=new TcpClient("127.0.0.1",8600);
		base=event_base_new();
		struct event* lis_event=event_new(base,ser->GetSockfd(),EV_READ|EV_PERSIST,MainSer::Lis_call,ser);  //thpl
		if(NULL==lis_event)
		{
			cout<<"new listen_event fail"<<endl;
			return;
		}
		event_add(lis_event,NULL);
		thpl=new ThreadPool(MainSer::base);  //线程池创建
	}
	void SendToLoad();
	void Run();  //循环监听
	static void Lis_call(int fd,short event,void *arg); //静态方法不能访问访问非静态成员

private:
	TcpServer *ser;
	TcpClient* cli;    //用于启动后连接负载均衡
	unsigned short port;
	static struct event_base *base;   //libevent监听是否有连接
	static ThreadPool *thpl;
};
event_base * MainSer::base=NULL;
ThreadPool* MainSer::thpl=NULL;
void MainSer::SendToLoad()
{
	Json::Value val;
	val["ip"]="127.0.0.1";
	val["port"]=port;
	val["num"]=5;
	if(-1==cli->Send(val.toStyledString()))
	{
		cout<<"send to load banalance fail"<<endl;
		return;
	}
}
void MainSer::Run()
{
	event_base_dispatch(MainSer::base);
}
void MainSer::Lis_call(int fd,short event,void *arg) //事件发生的回调函数
{
	TcpServer *ser=static_cast<TcpServer*>(arg);
	int clifd=ser->Accept();
	if(-1==clifd)
	{
		cout<<"accept fail"<<endl;
		return;
	}
	thpl->SendToWorkThread(clifd);   //将链接描述符发送给线程池
}

#endif
