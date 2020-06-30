//用于测试tensor，session，矩阵乘法的正确性
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include "tt.h"
using namespace std;
Graph* graph;
vector<int>emptyDim;
int main()
{
	Tensor res1({2,3},{1,1,1,1,2,1});
	Tensor res2({3,2},{1,2,1,1,1,1});
	MatMulti(res1,res2).Print();
	res1.Print();
	transpose(res1).Print();
	sin(res1).Print();
	res1.RandomInit();
	res1.Print();
	cout<<"-------------matrix test above----------------"<<endl;//测试矩阵的一些相关功能
	graph=new Graph();
	graph->new_Variable("X",{2,3});
	graph->new_Unary("Y","SIGMOID","X");
	Session* sess=new Session(graph);
	map<string,Tensor>mp;
	sess->SetParameter("X",Tensor({2,3},{1,1,1,1,1,1}));
	sess->ForwardPropagation("Y",mp);
	sess->GetCurVal("X").Print();
	sess->GetCurVal("Y").Print();
	cout<<endl;
	sess->Export("tests/tensortest1.txt");
	sess->SetParameter("X",Tensor({2,3},{1,1,2,1,1,2}));
	sess->ForwardPropagation("Y",mp);
	sess->GetCurVal("X").Print();
	sess->GetCurVal("Y").Print();
	cout<<endl;
	sess->Import("tests/tensortest1.txt");
	sess->ForwardPropagation("Y",mp);
	sess->GetCurVal("X").Print();
	sess->GetCurVal("Y").Print();
	cout<<endl;
	cout<<"-------------session test above----------------"<<endl;//测试session的一些相关功能
	delete sess;
	delete graph;
	graph=new Graph();
	graph->new_Variable("X",{2,3});
	graph->new_Variable("Y",{3,2});
	graph->new_Binary("Z","MATMUL","X","Y");
	sess=new Session(graph);
	sess->SetParameter("X",Tensor({2,3},{1,1,1,1,1,1}));
	sess->SetParameter("Y",Tensor({3,2},{1,1,1,1,1,1}));
	sess->ForwardPropagation("Z",mp);
	sess->GetCurVal("Z").Print();
	cout<<endl;
	sess->Export("tests/tensortest1.txt");
	sess->SetParameter("X",Tensor({2,3},{2,2,2,2,2,2}));
	sess->SetParameter("Y",Tensor({3,2},{2,2,2,2,2,2}));
	sess->ForwardPropagation("Z",mp);
	sess->GetCurVal("Z").Print();
	cout<<endl;
	sess->Import("tests/tensortest1.txt");
	sess->ForwardPropagation("Z",mp);
	sess->GetCurVal("Z").Print();//测试矩阵乘法
	return 0;
}
