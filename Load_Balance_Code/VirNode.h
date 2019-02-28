/*************************************************************************
    > File Name: VirNode.h
    > Created Time: Fri 11 Jan 2019 07:25:21 PM CST
 ************************************************************************/
#ifndef __VIRNODE_H
#define __VIRNODE_H
#include<iostream>
#include"Node.h"
using namespace std;
class VirNode   //虚拟节点
{
public:
	VirNode(Node* p,int _order){
		nod=p;
		order=_order;   //序号顺序
	}
	string& GetString();
	Node* GetNode();
private:
	Node* nod;    //虚拟节点指向的实体节点
	int order;
	string key;
};
string& VirNode::GetString()
{
	string ip=nod->GetIp();
	key=key+ip;
	unsigned short _port=nod->GetPort();
	char port[10]={0};
	sprintf(port,"%d",_port);
	key=key+port;
	char morder[10]={0};
	sprintf(morder,"%d",order);
	key=key+morder;
	return key;
}
Node* VirNode::GetNode()
{
	return nod;
}
#endif

