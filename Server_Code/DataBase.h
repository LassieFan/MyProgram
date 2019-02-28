/*************************************************************************
    > File Name: DataBase.h
    > Created Time: Sat 05 Jan 2019 11:29:09 AM CST
 ************************************************************************/
#ifndef __DATABASE_H
#define __DATABASE_H
#include<iostream>
#include<string.h>
#include<mysql/mysql.h>
#include<pthread.h>
using namespace std;
pthread_mutex_t m;
class DataBase
{
public:
	static DataBase* GetDataBase();
	int Insert(char *buff);
	int Delete(char* buff);
	int Update(char* buff);
	int Select(char* buff,string& ms);   //包里有fd
	int Login(char* buff);
	int Register(char* buff);
private:
	DataBase()
	{
		mpcon=mysql_init((MYSQL*)0);
		mysql_init(mpcon);
		if(!mysql_real_connect(mpcon,"127.0.0.1","root","",NULL,3306,NULL,0))
		{
			cout<<"sql connect fail"<<endl;
			return;
		}
		if(mysql_select_db(mpcon,"manage"))
		{
			cout<<"select database fail"<<endl;
			return;
		}
	}
	static DataBase* db;
	static MYSQL* mpcon;
};
DataBase* DataBase::db=NULL;
MYSQL* DataBase::mpcon=NULL;
DataBase* DataBase::GetDataBase()
{
	if(NULL==db)
	{
		pthread_mutex_lock(&m);
		if(NULL==db)
		{
			db=new DataBase();
		}
		pthread_mutex_unlock(&m);
	}
	return db;
}
int DataBase::Insert(char *buff)
{
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
		return 0;
	char cmd[100]="insert into vistor values('";
	strcat(cmd,val["name"].asString().c_str());
	strcat(cmd,"','");
	strcat(cmd,val["id"].asString().c_str());
	strcat(cmd,"','");
	char year[10]={0};
	sprintf(year,"%d",val["year"].asInt());
	strcat(cmd,year);
	strcat(cmd,"');");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
	{
		return 0;
	}
	return 1;
}
int DataBase::Delete(char* buff)
{
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
		return 0;
	char cmd[100]="delete from vistor where id='";
	strcat(cmd,val["id"].asString().c_str());
	strcat(cmd,"';");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
		return 0;
	return 1;
}
int DataBase::Update(char* buff)
{
	
}
int DataBase::Select(char* buff,string& ms)   //包里有fd
{
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
		return 0;
	char cmd[100]="select *from vistor where id='";
	strcat(cmd,val["id"].asString().c_str());
	strcat(cmd,"';");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
		return 0;
	MYSQL_RES* mp_res=mysql_store_result(mpcon);
	MYSQL_ROW mp_row=mysql_fetch_row(mp_res);
	if(val["fd"].asInt()==-1)  //主机进行查找的
	{
		if(mp_row==NULL)
			ms="don't have this vistor";
		else
		{
			ms=ms+"name:"+mp_row[0]+"  ";
			ms=ms+"id:"+mp_row[1]+"  ";
			ms=ms+"year"+mp_row[2];
		}
	}
	else     //信息采集器进行查找
	{
		if(mp_row==NULL)
			ms="this vistor can access";
		else
			ms="this vistor can't access";
	}
	return 1;
}
int DataBase::Login(char* buff)
{
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
		return 0;
	char cmd[100]="select *from manager where name='";
	strcat(cmd,val["name"].asString().c_str());
	strcat(cmd,"';");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
		return 0;
	MYSQL_RES* mp_res=mysql_store_result(mpcon);
	MYSQL_ROW mp_row=mysql_fetch_row(mp_res);
	if(mp_row==NULL)
		return 0;     //没有此用户
	else if(strcmp(mp_row[1],val["passwd"].asString().c_str())!=0)
		return 1;     //用户密码不正确
	else
		return 2;    //用户登录成功
}
int DataBase::Register(char* buff)
{
	Json::Value val;
	Json::Reader read;
	if(-1==read.parse(buff,val))
		return 0;
	char cmd[100]="insert into manager values('";
	strcat(cmd,val["name"].asString().c_str());
	strcat(cmd,"','");
	strcat(cmd,val["passwd"].asString().c_str());
	strcat(cmd,"');");
	if(mysql_real_query(mpcon,cmd,strlen(cmd)))
		return 0;
	return 1;
}
#endif
