#include "../tt.h"

void Session::SetParameter(string name, Tensor x)//修改参数为一个新的值
{
	Parameter[graph->dict[name]]=x;
}

void Session::AddParameter(string name, Tensor x)//修改参数为现有值加一个新的值
{
	Parameter[graph->dict[name]]=Parameter[graph->dict[name]]+x;
}

void Session::RandomInit()//随机化session中的参数
{
	for (auto ptr:Parameter)
	{
		Parameter[ptr.first].RandomInit();
	}
}

void Session::Export(string filename)//向文件输出session中的参数的值
{
	ofstream fout(filename.c_str());
	for (map<Opt*,Tensor>::iterator it=Parameter.begin();it!=Parameter.end();it++)
	{
		it->second.Export(fout);
	}
	fout.close();
}

void Session::Import(string filename)//从文件读入session中参数的值
{
	ifstream fin(filename.c_str());
	for (map<Opt*,Tensor>::iterator it=Parameter.begin();it!=Parameter.end();it++)
	{
		it->second.Import(fin);
	}
	fin.close();
}