#include "../tt.h"

//比较运算符

//>
BCmp::BCmp(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="BCmp";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if((!(a->data.IsScalar()))||(!(b->data.IsScalar())))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool BCmp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(a>b)data=Tensor(1.0);
	else data=Tensor(0.0);
	return true;
}
bool BCmp::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cmp"<<endl;
	return false;
}

//>=
BEQCmp::BEQCmp(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="BEQCmp";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if((!(a->data.IsScalar()))||(!(b->data.IsScalar())))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool BEQCmp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(a>=b)data=Tensor(1.0);
	else data=Tensor(0.0);
	return true;
}
bool BEQCmp::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cmp"<<endl;
	return false;
}

//<
LCmp::LCmp(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="LCmp";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if((!(a->data.IsScalar()))||(!(b->data.IsScalar())))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool LCmp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(a<b)data=Tensor(1.0);
	else data=Tensor(0.0);
	return true;
}
bool LCmp::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cmp"<<endl;
	return false;
}

//<=
LEQCmp::LEQCmp(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="LEQCmp";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if((!(a->data.IsScalar()))||(!(b->data.IsScalar())))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool LEQCmp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(a<=b)data=Tensor(1.0);
	else data=Tensor(0.0);
	return true;
}
bool LEQCmp::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cmp"<<endl;
	return false;
}

//==
EQCmp::EQCmp(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="EQCmp";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if((!(a->data.IsScalar()))||(!(b->data.IsScalar())))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool EQCmp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(a==b)data=Tensor(1.0);
	else data=Tensor(0.0);
	return true;
}
bool EQCmp::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cmp"<<endl;
	return false;
}

//!=
NEQCmp::NEQCmp(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="NEQCmp";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	data=Tensor(0.0);
	gradi=Tensor(0.0);
	if((!(a->data.IsScalar()))||(!(b->data.IsScalar())))
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool NEQCmp::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	if(a!=b)data=Tensor(1.0);
	else data=Tensor(0.0);
	return true;
}
bool NEQCmp::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cmp"<<endl;
	return false;
}
