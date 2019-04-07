/*************************************************************************
    > File Name: Sys.h
    > Created Time: Sun 25 Nov 2018 03:42:18 PM CST
 ************************************************************************/
#ifndef __SYS_H
#define __SYS_H
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include"Worker.h"  
#include"EnumType.h"
#include<json/json.h> //json包用于登录/注册发送给工作服务器用于验证或存储的
using namespace std;

class Sys  //主要负责登录模块
{
public:
	Sys()
	{
		_worker=new Worker();  //构造登录成功的工作者对象
	}	
	void Login();   //判断登录是否成功
	void Register();  //注册管理员
	~Sys()
	{
		delete _worker;
	}
private:
	Worker *_worker;  //用于登录成功后用与工作服务器进行交互的,真实的主机客户端
};
void Sys::Login()
{
	string name;  //管理员用户名
	string passwd; //密码
	cout<<"cin name:";
	fflush(stdout);
	cin>>name;
	cout<<"cin password:";
	fflush(stdout);
	cin>>passwd;

	//对于要发送给主服务器的数据都需要用json包
	//组装json包
	Json::Value val;
	val["type"]=LOGIN;   //将请求的数据类性说明，主服务器好解析
	val["name"]=name.c_str(); //将string转化为char*进行打包
	val["passwd"]=passwd.c_str();
	if(-1==WorkSer::GetWorkSer()->SendToMainSer(val.toStyledString())) //由工作服务器转发给主服务器
	{
		cerr<<"Login fail(Send fail);errno:"<<errno<<endl;
		return;
	}
	
	//接受服务器应答消息,将消息存储在recv_mess中
	string recv_mess;
	if(-1==WorkSer::GetWorkSer()->RecvFromMainSer(recv_mess)) 
	{
		cerr<<"Login fail(Recv fail);errno:"<<errno<<endl;
		return;
	}
	cout<<recv_mess<<endl;  //服务器直接回复login success或者login fail
	if(recv_mess=="login success")//不需要json，主服务器只发送一个简单的应答
	{
		_worker->Motion();  //工作者对象运行
	}
}
void Sys::Register()  //注册管理员
{
	string name;
	string passwd;
	cout<<"please input name and passward:";
	fflush(stdout);
	cin>>name>>passwd;

	//组装json包发送给工作服务器转发给主服务器
	Json::Value val;
	val["type"]=REGISTER;
	val["name"]=name.c_str();
	val["passwd"]=passwd.c_str();
	if(-1==WorkSer::GetWorkSer()->SendToMainSer(val.toStyledString()))
	{
		cerr<<"Register fail(Send fail);errno:"<<errno<<endl;
		return;
	}

	string recvmess;
	if(-1==WorkSer::GetWorkSer()->RecvFromMainSer(recvmess))
	{
		cerr<<"Register fail(Recv fail);errno:"<<errno<<endl;
		return;
	}
	cout<<recvmess<<endl;
}
#endif

