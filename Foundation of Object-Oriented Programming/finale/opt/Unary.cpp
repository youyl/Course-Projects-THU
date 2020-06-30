#include "../tt.h"

//每个元素独立的数学运算符

Sin::Sin(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Sin";
	name="_"+type;
	pre.push_back(a);
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(a->data.GetDim(),0);
}
bool Sin::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	data=sin(a);
	return true;
}
bool Sin::Backward()
{
	Tensor a=pre[0]->data;
	pre[0]->AddGrad(cos(a)*gradi);
	return true;
}

Log::Log(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Log";
	name="_"+type;
	pre.push_back(a);
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(a->data.GetDim(),0);
}
bool Log::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	if(a<Epsi)
	{
		cout<<"ERROR: LOG operator's input must be positive"<<endl;
		return false;
	}
	data=log(a);
	return true;
}
bool Log::Backward()
{
	Tensor a=pre[0]->data;
	pre[0]->AddGrad(inverse(a)*gradi);
	return true;
}

Exp::Exp(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Exp";
	name="_"+type;
	pre.push_back(a);
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(a->data.GetDim(),0);
}
bool Exp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	data=exp(a);
	return true;
}
bool Exp::Backward()
{
	Tensor a=pre[0]->data;
	pre[0]->AddGrad(exp(a)*gradi);
	return true;
}

Tanh::Tanh(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Tanh";
	name="_"+type;
	pre.push_back(a);
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(a->data.GetDim(),0);
}
bool Tanh::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	data=tanh(a);
	return true;
}
bool Tanh::Backward()
{
	Tensor a=pre[0]->data;
	pre[0]->AddGrad(inverse(cosh(a)*cosh(a))*gradi);
	return true;
}

Sigmoid::Sigmoid(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Sigmoid";
	name="_"+type;
	pre.push_back(a);
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(a->data.GetDim(),0);
}
bool Sigmoid::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	data=sigmoid(a);
	return true;
}
bool Sigmoid::Backward()
{
	pre[0]->AddGrad((data-(data*data))*gradi);
	return true;
}

//打印运算符，对标量特化

Print::Print(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Print";
	name="_"+type;
	pre.push_back(a);
	if(!(a->data.IsScalar()))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool Print::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	cout.setf(ios::fixed);
	cout << setprecision(4) << "PRINT operator: " << pre[0]->name << " = " << a.GetVal()[0] << endl;
	data=a;
	return true;
}
bool Print::Backward()
{
	pre[0]->AddGrad(gradi);
	return true;
}
