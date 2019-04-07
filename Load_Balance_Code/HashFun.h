/*************************************************************************
    > File Name: HashFun.h
    > Created Time: Thu 10 Jan 2019 07:54:06 PM CST
 ************************************************************************/
#ifndef __HASHFUN_H
#define __HASHFUN_H
#include<iostream>
#include<openssl/md5.h>
#include<algorithm>
#include<string.h>
using namespace std;  
class HashFun    //哈希函数类  通过使用MD5算法获取散列值即hash值，MD5不可逆
{                //获得的hash值加上密钥即可构成数字签名
public:
	HashFun(){}
	long GetHash(string key);     //输入key值获取hash值,保证长度即消息摘要
};
long HashFun::GetHash(string key)
{
	if(key.empty())
		return -1;
	long hash=0;
	unsigned char outmd[16];   //md5输出为16字节
	memset(outmd,0,sizeof(outmd));
	MD5_CTX ctx;    //MD5的这几个操作都是成功返回1失败返回0
	MD5_Init(&ctx);   //初始化ctx
	MD5_Update(&ctx,key.c_str(),key.size()); //计算MD5值
	MD5_Final(outmd,&ctx);  //输出MD5结果
	for(int i=0;i<4;i++)
	{                      //一致性哈希环上需要int型故转化，转化方式不唯一
		hash+=((long)(outmd[i*4+0]&0xFF)
				|(long)(outmd[i*4+1]&0xFF)<<8
				|(long)(outmd[i*4+2]&0xFF)<<16
				|(long)(outmd[i*4+3]&0xFF)<<24);
	}
	return hash;
}
#endif


