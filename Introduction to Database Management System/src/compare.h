#ifndef GLOBAL_COMPARE
#define GLOBAL_COMPARE

#include "pf/bufmanager/BufPageManager.h"
#include "pf/fileio/FileManager.h"
#include <string>
#include <map>
#include <vector>
#include "head.h"

using namespace std;

namespace CompOp
{
	const int NoCmp=0;
	const int Greater=1;
	const int Less=2;
	const int Equal=3;
	const int GreaterOrEqual=4;
	const int LessOrEqual=5;
	const int NonEqual=6;
	bool judge(int Op,ValueList arg1,ValueList arg2)
	{/*
		cout<<Op<<" compare begin:"<<endl;
		arg1.print();
		arg2.print();//*/
		if(Op==0)return true;
		for (int i=0;i<(int)arg1.list.size();i++)
		{
			if(arg1.list[i].isnull&&arg2.list[i].isnull)
			{
				continue;
			}
			else if(arg1.list[i].isnull)
			{
				if(Op==1)return false;
				else if(Op==2)return true;
				else if(Op==3)return false;
				else if(Op==4)return false;
				else if(Op==5)return true;
				else if(Op==6)return true;
			}
			else if(arg2.list[i].isnull)
			{
				if(Op==1)return true;
				else if(Op==2)return false;
				else if(Op==3)return false;
				else if(Op==4)return true;
				else if(Op==5)return false;
				else if(Op==6)return true;
			}
			else if(arg1.list[i].valuetype==ValueType::varcharvalue)
			{
				string str1=varcharstorage.findvarchar(arg1.list[i].intvalue);
				string str2=varcharstorage.findvarchar(arg2.list[i].intvalue);
				if(str1>str2)
				{
					if(Op==1)return true;
					else if(Op==2)return false;
					else if(Op==3)return false;
					else if(Op==4)return true;
					else if(Op==5)return false;
					else if(Op==6)return true;
				}
				if(str1<str2)
				{
					if(Op==1)return false;
					else if(Op==2)return true;
					else if(Op==3)return false;
					else if(Op==4)return false;
					else if(Op==5)return true;
					else if(Op==6)return true;
				}
			}
			else if(arg1.list[i].valuetype==ValueType::intvalue)
			{
				long long str1=arg1.list[i].intvalue;
				long long str2=arg2.list[i].intvalue;
				if(str1>str2)
				{
					if(Op==1)return true;
					else if(Op==2)return false;
					else if(Op==3)return false;
					else if(Op==4)return true;
					else if(Op==5)return false;
					else if(Op==6)return true;
				}
				if(str1<str2)
				{
					if(Op==1)return false;
					else if(Op==2)return true;
					else if(Op==3)return false;
					else if(Op==4)return false;
					else if(Op==5)return true;
					else if(Op==6)return true;
				}
			}
			else if(arg1.list[i].valuetype==ValueType::floatvalue)
			{
				double str1=arg1.list[i].floatvalue;
				double str2=arg2.list[i].floatvalue;
				if(str1>str2)
				{
					if(Op==1)return true;
					else if(Op==2)return false;
					else if(Op==3)return false;
					else if(Op==4)return true;
					else if(Op==5)return false;
					else if(Op==6)return true;
				}
				if(str1<str2)
				{
					if(Op==1)return false;
					else if(Op==2)return true;
					else if(Op==3)return false;
					else if(Op==4)return false;
					else if(Op==5)return true;
					else if(Op==6)return true;
				}
			}
			else if(arg1.list[i].valuetype==ValueType::stringvalue)
			{
				string str1=arg1.list[i].stringvalue;
				string str2=arg2.list[i].stringvalue;
				if(str1>str2)
				{
					if(Op==1)return true;
					else if(Op==2)return false;
					else if(Op==3)return false;
					else if(Op==4)return true;
					else if(Op==5)return false;
					else if(Op==6)return true;
				}
				if(str1<str2)
				{
					if(Op==1)return false;
					else if(Op==2)return true;
					else if(Op==3)return false;
					else if(Op==4)return false;
					else if(Op==5)return true;
					else if(Op==6)return true;
				}
			}
		}
		if(Op==3||Op==4||Op==5)return true;
		return false;
	}
}

#endif
