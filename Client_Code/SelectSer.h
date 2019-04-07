/*************************************************************************
    > File Name: SelectSer.h
    > Created Time: Sun 25 Nov 2018 02:31:16 PM CST
 ************************************************************************/
#ifndef __SELECTSER_H
#define __SELECTSER_H
#include<iostream>
#include<event.h>
#include<pthread.h>
#include"TcpServer.h"
#include"WorkSer.h"
#include"EnumType.h"
#include<json/json.h>
#include<map>
using namespace std;
void *RunListen(void *arg)  //启动线程循环监听
{
	struct event_base *base=static_cast<struct event_base*>(arg);
	event_base_dispatch(base);
}


class SelectSer
{
public:
	SelectSer()
	{
		cout<<"please input your selectSer port"<<endl;
		int _mport;
		cin>>_mport;
		selser=new TcpServer("127.0.0.1",_mport);  //绑定信息采集器服务器
		base=event_base_new();  //初始化libevent
		struct event* lis_event=event_new(base,selser->GetSockfd(),EV_READ|EV_PERSIST,SelectSer::lis_call,selser);  //对于监听sockfd进行注册回调函数及加入到libevent中
		if(NULL==lis_event)  //注册失败
		{
			cout<<"event new fail"<<endl;
			return;
		}
		event_add(lis_event,NULL);  //将监听sockfd上所发生的事件加入libevent

		//循环监听会阻塞，所以新起一个线程用于循环监听
		pthread_t tid;  
		pthread_create(&tid,NULL,RunListen,base); //将base传给event_base_dispatch
	}
	static void lis_call(int fd,short event,void *arg); //event_new类型要匹配，不能用成员方法，多了this指针
	static void cli_call(int fd,short event,void *arg);
	void SendSelect(int fd,string &str);  //将服务器的消息发送给采集机
private:
	TcpServer *selser;   //用于与客户端信息采集机的交互服务器
	static event_base *base;  //libevent
	static map<int, struct event*>eventMap; //为了清除libevent中的事件
};
event_base *SelectSer::base=NULL;
map<int,struct event*>SelectSer::eventMap;

void SelectSer::lis_call(int fd,short event,void *arg)
{
	TcpServer *selser=static_cast<TcpServer*>(arg);
	//struct event_base* base=(struct event_base*)arg;  //传递来的libevent
	int clifd=selser->Accept();  //获取信息采集器的连接
	if(-1==clifd)
	{
		cout<<"accept fail"<<endl;
		return;
	}
	struct event* cli_event=event_new(base,clifd,EV_READ|EV_PERSIST,SelectSer::cli_call,selser);
	if(NULL==cli_event)
	{
		cout<<"cli event new fail"<<endl;
		return;
	}
	eventMap.insert(make_pair(clifd,cli_event));
	event_add(cli_event,NULL);  //将与客户端链接的文件描述符加入libevent
}
void SelectSer::cli_call(int fd,short event,void *arg)
{
	TcpServer *selser=static_cast<TcpServer*>(arg);
	//struct event_base* base=(struct event_base*)arg;
	string id;
	if(0>=selser->Recv(fd,id)) //接受信息采集机的请求
	{
		cerr<<"cli shot down;errno:"<<errno<<endl;
		event_free(eventMap[fd]);//将事件删除，需要找到事件，所以定义键值对
		eventMap.erase(fd);
		close(fd);
		return;
	}

	Json::Value val;
	val["type"]=SELECT;
	val["fd"]=fd;
	val["id"]=id.c_str();
	//直接将信息采集机的查询发送给工作服务器
	if(-1==WorkSer::GetWorkSer()->SendToMainSer(val.toStyledString()))
	{
		cout<<"collection request fail(Send fail)"<<endl;
		return;
	}
}
void SelectSer::SendSelect(int fd,string& str)
{
	selser->Send(fd,str);
}
#endif

