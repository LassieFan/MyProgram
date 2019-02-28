/*************************************************************************
    > File Name: TcpServer.h
    > Created Time: Sun 25 Nov 2018 10:36:13 AM CST
 ************************************************************************/
#ifndef __TCPSERVER_H
#define __TCPSERVER_H
#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
//封装服务器的代码
class TcpServer
{
public:
	TcpServer(const string _ip,const unsigned short _port):port(_port),ip(_ip)  //创建socket及监听socket
	{
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		if(-1==sockfd)
		{
			cout<<"ser socket fail"<<endl;
			return;
		}
		struct sockaddr_in ser;
		ser.sin_family=AF_INET;
		ser.sin_port=htons(_port);
		ser.sin_addr.s_addr=inet_addr(_ip.c_str());
		int res=bind(sockfd,(struct sockaddr*)&ser,sizeof(ser));//命名socket
		if(-1==res)
		{
			cout<<"ser bind fail"<<endl;
			return;
		}
		if(-1==listen(sockfd,20))
		{
			cout<<"listen socketfd fail"<<endl;
			return;
		}
	}
	int Accept();
	ssize_t Recv(int fd,string &recvbuff);
	ssize_t Send(int fd,string &sendbuff);
	int GetSockfd();  //对于那些需要监听描述符的给外部接口
private:
	string ip; //服务器ip地址
	unsigned short port;  //服务器端口号
	int sockfd; //服务器的监听socket_fd
};
int TcpServer::Accept()  //返回与客户端建立连接的sockfd
{
	struct sockaddr_in cli;
	socklen_t len=sizeof(cli);
	return accept(sockfd,(struct sockaddr*)&cli,&len); //调用Accept的服务器需要自行判断返回值
}
ssize_t TcpServer::Recv(int fd,string &recvbuff)  //fd是accept获取的描述符
{
	char buff[128]={0};
	int n=recv(fd,buff,127,0);
	recvbuff=buff;  //将char*转化为string存储在recvbuff中
	return n;
}
ssize_t TcpServer::Send(int fd,string &sendbuff) 
{
	const char *buff=sendbuff.c_str(); //将string转化为char*
	return send(fd,buff,strlen(buff),0); 
}
int TcpServer::GetSockfd() //libevent需要这个监听socket
{
	return sockfd;
}
#endif
