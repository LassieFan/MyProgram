/*************************************************************************
    > File Name: Model.h
    > Created Time: Sun 09 Dec 2018 05:07:57 PM CST
 ************************************************************************/

#include<iostream>
#include<json/json.h>
#include"View.h"
using namespace std;
class Model
{
public:
	Model()
	{
		mmap.insert(make_pair(INSERT,new InsertView));
		mmap.insert(make_pair(DELETE,new DeleteView));
		mmap.insert(make_pair(UPDATE,new UpdateView));
		mmap.insert(make_pair(SELECT,new SelectView));
		mmap.insert(make_pair(LOGIN,new LoginView));
		mmap.insert(make_pair(REGISTER,new RegisterView));
	}
	View* GetView(int type);
private:
	map<int,View*> mmap;
};
View* Model::GetView(int type)
{
	if(type<0||type>5)
		return NULL;
	return mmap[type];
}
