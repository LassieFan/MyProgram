/*************************************************************************
    > File Name: Node.h
    > Created Time: Wed 09 Jan 2019 08:33:18 PM CST
 ************************************************************************/
#ifndef __NODE_H
#define __NODE_H
#include<iostream>
using namespace std;
class Node   //真实节点类
{
public:
	Node(string& _ip,unsigned short _port,int _num)
	{
		ip=_ip;
		port=_port;
		num=_num;
	}
    string& GetIp();
	unsigned short GetPort();
	int GetNum();
private:
	string ip;
	unsigned short port;
	int num;
};
string& Node::GetIp()
{
	return ip;
}
unsigned short Node::GetPort()
{
	return port;
}
int Node::GetNum()
{
	return num;
}

#endif

