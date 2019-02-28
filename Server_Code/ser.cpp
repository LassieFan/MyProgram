/*************************************************************************
    > File Name: ser.cpp
    > Created Time: Mon 10 Dec 2018 08:03:11 PM CST
 ************************************************************************/

#include<iostream>
#include"MainSer.h"
using namespace std;

int main()
{
	MainSer* p=new MainSer();
	p->SendToLoad();
	p->Run();
}
