/*************************************************************************
    > File Name: cli.cpp
    > Created Time: Sun 25 Nov 2018 02:55:38 PM CST
 ************************************************************************/

#include<iostream>
#include "Sys.h"
#include"WorkSer.h"
#include"TcpClient.h"
#include"WorkSer.h"
using namespace std;
int main()
{

	Sys *Msys=new Sys();   //开启系统
	TcpClient* lb=new TcpClient("127.0.0.1",8700);
	string s;
	lb->Recv(s);
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(s.c_str(),val))
	{
		cout<<"request ip and port fail"<<endl;
		return 0;
	}
	string ip=val["ip"].asString();
	unsigned short port=val["port"].asInt();
	WorkSer::GetWorkSer(ip,port);  //目的就是将工作服务器和主服务器链接
	close(lb->GetSockfd());
	while(1) //像QQ登录界面那样
	{
		cout<<"1.login"<<endl;
		cout<<"2.register"<<endl;
		cout<<"3.exit"<<endl;
		int choice;
		cout<<"please input your choice: ";
		fflush(stdout);
		cin>>choice;
		switch(choice)  //4个以上的选择，switch跳转表效率高于if else
		{
			case 1:
				{
					Msys->Login();
					break;
				}
			case 2:
				{
					Msys->Register();
					break;
				}
			case 3:
				{
					break;
				}
			default:
				{
					cout<<"error choice,please chocie again"<<endl;
					break;
				}
		}
		if(3==choice)
		{
			cout<<"exit success"<<endl;
			break;
		}
	}
	delete Msys;
	return 0;
}
