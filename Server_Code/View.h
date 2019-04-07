/*************************************************************************
    > File Name: View.h
    > Created Time: Sat 08 Dec 2018 07:22:20 PM CST
 ************************************************************************/
#ifndef __VIEW_H
#define __VIEW_H
#include<iostream>
#include"DataBase.h"
using namespace std;

class View
{
public:
	virtual void view_handle(char* buff,int fd)=0;
};
class InsertView:public View
{
public:
	void view_handle(char* buff,int fd);
};
class DeleteView:public View
{
public:
	void view_handle(char* buff,int fd);
};
class UpdateView:public View
{
public:
	void view_handle(char* buff,int fd);
};
class SelectView:public View
{
public:
	void view_handle(char* buff,int fd);
};
class LoginView:public View
{
public:
	void view_handle(char* buff,int fd);
};
class RegisterView:public View
{
public:
	void view_handle(char* buff,int fd);
};

void InsertView::view_handle(char *buff,int fd)  //增删改查用json对象
{
	Json::Value val;
	char response[100]={0};
	if(DataBase::GetDataBase()->Insert(buff))
	{
		char r[100]="insert to db success";
		strcpy(response,r);
	}
	else
	{
		char r[100]="insert to db fail;please again";
		cout<<r;
		strcpy(response,r);
	}
	val["type"]=INSERT;
	val["message"]=response;
	if(-1==send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cout<<"send to client fail"<<endl;
		return;
	}
}
void DeleteView::view_handle(char* buff,int fd)
{
	Json::Value val;
	char response[100]={0};
	if(DataBase::GetDataBase()->Delete(buff))
	{
		strcpy(response,"delete from db success");
	}
	else
	{
		strcpy(response,"delete from db fail;please again");
	}
	val["type"]=DELETE;
	val["message"]=response;
	if(-1==send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cout<<"send to client fail"<<endl;
		return;
	}
}
void UpdateView::view_handle(char* buff,int fd)
{
}
void SelectView::view_handle(char* buff,int fd) //参数的fd是一个客户端的fd
{
	Json::Value val;
	Json::Reader read;
	string ms;
	if(-1==read.parse(buff,val))
	{
		cout<<"read json fail"<<endl;
		return;
	}
	int n=val["fd"].asInt();
	Json::Value re;
	re["type"]=SELECT;
	re["fd"]=n;
	if(DataBase::GetDataBase()->Select(buff,ms))
	{
		re["message"]=ms.c_str();
		if(-1==send(fd,re.toStyledString().c_str(),strlen(re.toStyledString().c_str()),0))
		{
			cout<<"send fail"<<endl;
				return;
		}
	}
	else
	{
		cout<<"select error"<<endl;
		return;
	}
}
void LoginView::view_handle(char* buff,int fd)
{
	int n=DataBase::GetDataBase()->Login(buff);
	if(0==n)  //没有该用户
	{
		char response[100]="don't have this manager";
		if(-1==send(fd,response,strlen(response),0))
		{
			cout<<"send fail"<<endl;
			return;
		}
	}
	else if(1==n)  //用户密码不正确
	{
		char response[100]="manager password error";
		if(-1==send(fd,response,strlen(response),0))
		{
			cout<<"send fail"<<endl;
			return;
		}
	}
	else   //登录成功
	{
		char response[100]="login success";
		if(-1==send(fd,response,strlen(response),0))
		{
			cout<<"send fail"<<endl;
			return;
		}
	}
}
void RegisterView::view_handle(char* buff,int fd)//register和login是直接输出
{
	if(DataBase::GetDataBase()->Register(buff))
	{
		char response[100]="register successful";
		if(-1==send(fd,response,strlen(response),0))
		{
			cout<<"send fail"<<endl;
			return;
		}
	}
	else
	{
		char response[100]="register fail";
		if(-1==send(fd,response,strlen(response),0))
		{
			cout<<"send fail"<<endl;
			return;
		}
	}
}
#endif

