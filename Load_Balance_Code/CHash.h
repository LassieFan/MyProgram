/*************************************************************************
    > File Name: CHash.h
    > Created Time: Thu 10 Jan 2019 07:48:48 PM CST
 ************************************************************************/
#ifndef __CHASH_H
#define __CHASH_H
#include<iostream>
#include"HashFun.h"
#include"Node.h"
#include"VirNode.h"
using namespace std;
class CHash
{
public:
	CHash()
	{
		fun=new HashFun();
	}
	long GetValue(string key);  //用于客户端获取hash值
	void Insert(Node* no);   //通过真实节点将虚拟节点插入一致性哈希上
	void Delete(Node* no);
	Node* FindNode(long hash);
private:
	HashFun *fun;	   //一致性哈希所使用的哈希函数
	static map<long,VirNode*> mmap;
};
map<long,VirNode*> CHash::mmap;    //通过map表来反映哈希环，有序的红黑树
long CHash::GetValue(string key)
{
	return fun->GetHash(key);
}
void CHash::Insert(Node *no)
{
	int num=no->GetNum();
	VirNode *vir;
	for(int i=0;i<num;i++)
	{
	    vir=new VirNode(no,i);
		string key=vir->GetString();
		long hash=fun->GetHash(key);
		map<long,VirNode*>::iterator it=mmap.find(hash);
		if(it==mmap.end())  //环上没有该节点，不产生冲突
		{
			if(vir!=NULL)
			{
				mmap.insert(make_pair(hash,vir));
				cout<<hash<<endl;
			}
		}
	}
}
void CHash::Delete(Node* no)
{
	int num=no->GetNum();
	VirNode* vir;
	for(int i=0;i<num;i++)
	{
	    vir=new VirNode(no,i);
		string key=vir->GetString();
		long hash=fun->GetHash(key);
		map<long,VirNode*>::iterator it=mmap.find(hash);
		if(it!=mmap.end())
			mmap.erase(it++);
	}
}
Node* CHash::FindNode(long hash)
{
	//返回第一个大于等于hash的迭代器
	map<long,VirNode*>::iterator it=mmap.lower_bound(hash);
	if(it==mmap.end())   //如果客户端的hash值最大，则顺时针找到最小的那一个
	{
		it=mmap.begin();
	}
	VirNode* p=it->second;
	if(p!=NULL)
		return p->GetNode(); 
	return NULL;
}
#endif

