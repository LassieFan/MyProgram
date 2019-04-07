/*************************************************************************
    > File Name: Worker.h
    > Created Time: Mon 26 Nov 2018 06:37:49 PM CST
 ************************************************************************/
#ifndef __WORKER_H
#define __WORKER_H
#include<iostream>
#include"WorkSer.h"
#include"SelectSer.h"
#include"EnumType.h"
using namespace std;
//用于客户端系统的一个子集，用其给工作服务器发送与接受消息，并且将与信息采集器服务器进行交互

void* AcceptFromMainSer(void *arg) //将SlectSer服务器对象通过参数传给函数线程
{
	SelectSer *sel=static_cast<SelectSer*>(arg); 
	while(1)
	{
		string recv_message;
		if(-1!=WorkSer::GetWorkSer()->RecvFromMainSer(recv_message))
		{
			Json::Value val;
			Json::Reader read;
			if(-1==read.parse(recv_message.c_str(),val))  //将字符串转化为json包
			{
				cerr<<"Change fail;errno:"<<errno<<endl;
				continue;
			}
			//判断接受到的信息是发送给主机的还是信息采集机
			//fd为-1即是发送给主机的消息，反之发送给信息采集机
			if(val["type"].asInt()==SELECT&&val["fd"].asInt()!=-1)
			{
				int fd=val["fd"].asInt();  //获取到信息采集器的链接描述符
				string str=val["message"].asString();
				sel->SendSelect(fd,str);//用采集机服务器将消息回复给采集机
			}
			else  //发送给本机的数据,直接输出
			{
				cout<<val["message"].asString()<<endl;
			}
		}
		else
		{
			cerr<<"Recv fail;errno:"<<errno<<endl;
			continue;
		}
	}
}
class Worker
{
public:
	Worker()
	{
		sel=new SelectSer();
	}
	void Motion();  //工作者运行
	void CreateRecvThread();  //用于创建读线程
	void Insert();  //增删改查
	void Delete();
	void Update();
	void Select();
private:
	SelectSer *sel;  //信息采集机的直接服务器
};
void Worker::Motion()
{
	CreateRecvThread();
	while(1)
	{
		cout<<"1.insert"<<endl;
		cout<<"2.delete"<<endl;
		cout<<"3.update"<<endl;
		cout<<"4.select"<<endl;
		cout<<"please input your choice:";
		fflush(stdout);
		int choice;
		cin>>choice;
		switch(choice)
		{
			case 1:
			{
				Insert();
				break;
			}
			case 2:
			{
				Delete();
				break;
			}
			case 3:
			{
				Update();
				break;
			}
			case 4:
			{
				Select();
				break;
			}
			default:	
			{
				cout<<"your choice have error;input again"<<endl;
				break;
			}
		}
	}
}
void Worker::Insert()
{
	string name;
	cout<<"please input name"<<endl;
	cin>>name;

	string id;
	cout<<"please input id"<<endl;
	cin>>id;

	int year;
	cout<<"please input limit time:(year)"<<endl;
	cin>>year;

	Json::Value val;
	val["type"]=INSERT;
	val["name"]=name.c_str();
	val["id"]=id.c_str();
	val["year"]=year;
	if(-1==WorkSer::GetWorkSer()->SendToMainSer(val.toStyledString()))
	{
		cout<<"Insert fail(Send fail)"<<endl;
		return;
	}
}
void Worker::Delete()
{
	string id;
	cout<<"please input id"<<endl;
	cin>>id;

	Json::Value val;
	val["type"]=DELETE;
	val["id"]=id.c_str();
	if(-1==WorkSer::GetWorkSer()->SendToMainSer(val.toStyledString()))
	{
		cout<<"Delete fail(Send fail)"<<endl;
		return;
	}
}
void Worker::Select()
{
	string id;
	cout<<"please input id"<<endl;
	cin>>id;

	Json::Value val;
	val["type"]=SELECT;
	val["id"]=id.c_str();
	val["fd"]=-1;   //区分是主机发送给主服务器的信息还是信息采集机发送的
	if(-1==WorkSer::GetWorkSer()->SendToMainSer(val.toStyledString()))
	{
		cout<<"Select fail(Send fail)"<<endl;
		return;
	}
}
void Worker::Update()
{
	
}
void Worker::CreateRecvThread()
{
	pthread_t id; //创建从主服务器专门接受数据的线程
	if(-1==pthread_create(&id,NULL,AcceptFromMainSer,sel))
	{
		cout<<"Recv Thread Create fail"<<endl;
		return;
	}
}
#endif


