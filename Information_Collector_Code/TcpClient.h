/*************************************************************************
    > File Name: TcpClient.h
    > Created Time: Sun 25 Nov 2018 12:15:35 PM CST
 ************************************************************************/
#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H
#include<iostream>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;
//封装客户端代码
class TcpClient
{
public:
	TcpClient(const string &_ip=NULL,const unsigned short _port=0):port(_port),ip(_ip)  //客户端进行与服务器连接
	{
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		if(-1==sockfd)
		{
			cout<<"client socket fail"<<endl;
			return;
		}
		struct sockaddr_in ser;
		ser.sin_family=AF_INET;
		ser.sin_port=htons(_port);
		ser.sin_addr.s_addr=inet_addr(_ip.c_str());
		if(-1==connect(sockfd,(struct sockaddr*)&ser,sizeof(ser)))
		{
			cout<<"client connect fail"<<endl;
			return;
		}
	}
	int Send(const string &recvbuff); //引用避免调用拷贝构造
	int Recv(string &sendbuff);
	int GetSockfd();  //获取到客户端与服务器交互的sockfd
private:
	unsigned short port;
	string ip;
	int sockfd; //客户端与服务器进行通讯的socket描述符
};
int TcpClient::Send(const string &sendbuff)
{
	const char*buff=sendbuff.c_str();  //将string转化为char *进行发送
	return send(sockfd,buff,strlen(buff),0);
}
int TcpClient::Recv(string &recvbuff)
{
	char buff[1024]={0};
	int n=recv(sockfd,buff,1023,0);
	recvbuff=buff;
	return n;
}
int TcpClient::GetSockfd()
{
	return sockfd;
}
#endif

