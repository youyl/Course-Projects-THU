#include "../tt.h"

//算术运算符对应的结点

Add::Add(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="Add";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(b->data.GetDim(),0);
}
bool Add::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	data=a+b;
	return true;
}
bool Add::Backward()
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	pre[0]->AddGrad(gradi);
	pre[1]->AddGrad(gradi);
	return true;
}

Subtract::Subtract(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="Subtract";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(b->data.GetDim(),0);
}
bool Subtract::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	data=a-b;
	return true;
}
bool Subtract::Backward()
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	pre[0]->AddGrad(gradi);
	pre[1]->AddGrad((-1.0)*gradi);
	return true;
}

Multiply::Multiply(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="Multiply";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(b->data.GetDim(),0);
}
bool Multiply::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	data=a*b;
	return true;
}
bool Multiply::Backward()
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	pre[0]->AddGrad(gradi*b);
	pre[1]->AddGrad(gradi*a);
	return true;
}

Divide::Divide(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="Divide";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(b->data.GetDim(),0);
}
bool Divide::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(b.HaveZero())
	{
		cout<<"ERROR: Division by zero"<<endl;
		return false;
	}
	data=a/b;
	return true;
}
bool Divide::Backward()
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	pre[0]->AddGrad(gradi*inverse(b));
	pre[1]->AddGrad((-1.0)*gradi*(a*inverse(b*b)));
	return true;
}
