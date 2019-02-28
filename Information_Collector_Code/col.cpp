/*************************************************************************
    > File Name: col.cpp
    > Created Time: Tue 08 Jan 2019 08:09:54 PM CST
 ************************************************************************/

#include<iostream>
#include"TcpClient.h"
#include"EnumType.h"
using namespace std;
int main()
{
	string ip;
	unsigned short port;
	string id;
	cout<<"please input your ip:"<<endl;
	cin>>ip;
	cout<<"please input your port"<<endl;
	cin>>port;
	TcpClient* t=new TcpClient(ip,port);
	cout<<"please input your id"<<endl;
	cin>>id;
	if(-1==t->Send(id))
	{
		cout<<"request fail"<<endl;
		return 0;
	}
	string r;
	t->Recv(r);
	cout<<r<<endl;
}

