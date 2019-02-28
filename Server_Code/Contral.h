/*************************************************************************
    > File Name: Contral.h
    > Created Time: Sun 09 Dec 2018 05:43:39 PM CST
 ************************************************************************/

#include<iostream>
#include<json/json.h>
#include"Model.h"
using namespace std;
class Contral
{
public:
	Contral()
	{
		_m=new Model();	
	}
	void Process(char* sendbuff,int fd);
	int Judge(char* sendbuff);
private:
	Model* _m;
};
int Contral::Judge(char *buff)   //解析json包将type类型返回给process
{
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
	{
		cout<<"read json fail"<<endl;
		return -1;
	}
	return val["type"].asInt();   //将要操作的类型返回
}
void Contral::Process(char* sendbuff,int fd)
{
	char buff[128]={0};
	strcpy(buff,sendbuff);
	int num=Judge(buff);
	if(_m->GetView(num)!=NULL)
		_m->GetView(num)->view_handle(sendbuff,fd); //多态调用
}
