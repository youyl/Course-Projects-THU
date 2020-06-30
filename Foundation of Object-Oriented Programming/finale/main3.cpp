//牛顿迭代
#include <iostream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <vector>
#include "tt.h"
using namespace std;
vector<int>emptyDim;
int T,n;
float x1;
vector<float>coe,ans;
Graph* graph;
Session* sess;
map<string,Tensor>mp;
int main()
{
	cin>>T;
	while (T--)
	{
		ans.clear();
		graph=new Graph();
		
		cin>>n;
		coe.resize(n+1);
		for (int i=0;i<=n;i++)
		{
			cin>>coe[i];
		}
		reverse(coe.begin(),coe.end());
		
		graph->new_Variable("X",emptyDim);
		graph->new_Constant("X0",Tensor(1.0));
		graph->new_Constant("C-1",Tensor(0.0));
		graph->new_Constant("A0",Tensor(coe[0]));
		graph->new_Binary("C0","+","C-1","A0");//初始化常数项和附带的几个结点
		
		for (int i=1;i<=n;i++)
		{
			string xi="X"+to_string(i),xi1="X"+to_string(i-1);
			string yi="Y"+to_string(i),ci1="C"+to_string(i-1);
			string ai="A"+to_string(i),ci="C"+to_string(i);
			graph->new_Binary(xi,"*","X",xi1);//x的i次方
			graph->new_Constant(ai,Tensor(coe[i]));//系数ai
			graph->new_Binary(yi,"*",xi,ai);//ai*x^i
			graph->new_Binary(ci,"+",ci1,yi);//多项式的前i项之和
		}
		string cn="C"+to_string(n);//最终的多项式的值
		
		graph->new_Unary("G","GRAD",cn);
		graph->new_Binary("gx","AT","G","X");
		graph->new_Binary("div","/",cn,"gx");
		graph->new_Binary("minus","-","X","div");
		graph->new_Binary("Asn","ASSIGN","X","minus");//这5个结点负责让牛顿迭代的过程自动化
		
		sess=new Session(graph);//给建立的计算图分配一个session
		cin>>x1;
		ans.push_back(x1);
		for (int i=0;i<5;i++)
		{
			sess->SetParameter("X",ans[i]);
			sess->ForwardPropagation("Asn",mp);//对名为Asn的结点求值可以直接完成一步完整的牛顿迭代
			ans.push_back(sess->GetCurVal("X").GetVal()[0]);
		}
		for (int i=1;i<ans.size();i++)
		{
			cout.setf(ios::fixed);
			cout<<setprecision(4)<<ans[i]<<' ';
		}
		cout<<endl;
		delete sess;
		delete graph;
	}
	return 0;
}