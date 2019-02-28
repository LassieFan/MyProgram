/*************************************************************************
    > File Name: WorkSer.h
    > Created Time: Sun 25 Nov 2018 04:13:37 PM CST
 ************************************************************************/
#ifndef __WORKSER_H
#define __WORKSER_H
#include<iostream>
#include<string.h>
#include<pthread.h>
#include"TcpClient.h"
using namespace std;
pthread_mutex_t m;
class WorkSer   //单例模式处理客户主机与服务器的请求的工作服务器
{
public:
//	static WorkSer* GetWorkSer(const string& ip=NULL,const unsigned short port=0);静态方法不依赖对象，返回指针防止生成临时对象
	static WorkSer* GetWorkSer(string ip="12334",unsigned short port=0);
	int SendToMainSer(const string& sendbuff);//工作服务器将客户的请求发送给主服务器
	int RecvFromMainSer(string& recvbuff);
private:
	WorkSer(const string& ip,const unsigned short port) //构造函数私有化
	{
		cli=new TcpClient(ip,port); //ip,port是主服务器的 
	}
	static WorkSer* ser;  //唯一的对象,与唯一的主服务器交互
	TcpClient *cli;  //与主服务器进行交互的工作服务器（看作客户端）
};
WorkSer* WorkSer::ser=NULL;  //静态成员属性类外初始化
WorkSer* WorkSer::GetWorkSer(string ip,unsigned short port)
{
	if(NULL==ser)
	{
		pthread_mutex_lock(&m);
		if(NULL==ser)   //第一次需要给定主服务器的ip和端口
		{
			/*cout<<"please input ip and port"<<endl;
			string ip;
			unsigned short port;
			cin>>ip;
			cin>>port;*/
			ser=new WorkSer(ip,port);
		}
		pthread_mutex_unlock(&m);
	}
	return ser;
}
int WorkSer::SendToMainSer(const string& sendbuff)
{
	return cli->Send(sendbuff);
}
int WorkSer::RecvFromMainSer(string &recvbuff) //引用避免生成临时对象
{
	return cli->Recv(recvbuff);
}
#endif
