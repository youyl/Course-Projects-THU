#include "../tt.h"

At::At(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="At";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	data=Tensor(b->data.GetDim(),0);
	gradi=Tensor(b->data.GetDim(),0);
}
bool At::Forward(Session* _sess)
{
	data=pre[1]->gradi;//直接获取暂存的梯度
	return true;
}
bool At::Backward()
{
	cout<<"ERROR: Double gradient is not permitted"<<endl;
	return false;
}

Assign::Assign(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="At";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(b->data.GetDim(),0);
	gradi=Tensor(b->data.GetDim(),0);
}
bool Assign::Forward(Session* _sess)
{
	_sess->asnrecord.push_back(make_pair(pre[0],pre[1]));//将assign操作记录到session中
	data=pre[1]->data;
	return true;
}
bool Assign::Backward()
{
	pre[1]->AddGrad(gradi);
	return true;
}

Bind::Bind(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="Bind";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(a->data.GetDim(),0);
}
bool Bind::Forward(Session* _sess)
{
	data=pre[0]->data;
	return true;
}
bool Bind::Backward()
{
	pre[0]->AddGrad(gradi);
	return true;
}

Grad::Grad(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Grad";
	name="_"+type;
	pre.push_back(a);
	if(!(a->data.IsScalar()))//求梯度是对标量特化的
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool Grad::Forward(Session* _sess)
{
	return _sess->BackPropagation(pre[0]);//对于grad结点的正向传播会调用session里的反向传播
}
bool Grad::Backward()
{
	cout<<"ERROR: Double gradient is not permitted"<<endl;//对二阶导暂时无法处理
	return false;
}

Assert::Assert(Graph* _graph, Opt* a)
{
	graph=_graph;
	type="Assert";
	name="_"+type;
	pre.push_back(a);
	if(!(a->data.IsScalar()))//对标量特化的assert
	{
		cout<<"ERROR: Assert only Applied to Scalar"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool Assert::Forward(Session* _sess)
{
	Tensor a=pre[0]->data.GetVal()[0];
	data=Tensor(0.0);
	if(a<Epsi)
	{
		cout<<"ERROR: Assertion failed"<<endl;
		return false;
	}
	return true;
}
bool Assert::Backward()
{
	return true;
}

Cond::Cond(Graph* _graph, Opt* condition, Opt* a, Opt* b): Opt(_graph) {
	type = "Cond";
	name = "_" + type;
	pre.push_back(condition);
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Cond size unfitted"<<endl;
		exit(0);
	}
	data=Tensor(a->data.GetDim(),0);
	gradi=Tensor(data.GetDim(),0);
}
bool Cond::Forward(Session* _sess)
{
	Tensor a=pre[0]->data;
	Tensor b=pre[1]->data;
	Tensor c=pre[2]->data;
	if(a.GetVal()[0]>Epsi)data=b;
	else data=c;
	return true;
}
bool Cond::Backward()
{
	cout<<"ERROR: Undefined Gradient for Cond"<<endl;
	return false;
}

MatMul::MatMul(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="MatMul";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if((a->data.GetDim().size()!=2)||(b->data.GetDim().size()!=2))
	{
		cout<<"ERROR: MatMul are restricted to matrix"<<endl;//矩阵乘法对二维矩阵特化，无法处理高维情况
		exit(0);
	}
	vector<int>tmp1=a->data.GetDim();
	vector<int>tmp2=b->data.GetDim();
	if(tmp1[1]!=tmp2[0])
	{
		cout<<"ERROR: Unable to fit Matrix Size"<<endl;
		exit(0);
	}
	vector<int>tmp3;
	tmp3.push_back(tmp1[0]);
	tmp3.push_back(tmp2[1]);
	data=Tensor(tmp3,0);
	gradi=Tensor(tmp3,0);
}
bool MatMul::Forward(Session* _sess)
{
	data=MatMulti(pre[0]->data,pre[1]->data);
	return true;
}
bool MatMul::Backward()
{
	pre[0]->AddGrad(MatMulti(gradi,transpose(pre[1]->data)));
	pre[1]->AddGrad(MatMulti(transpose(pre[0]->data),gradi));//对于矩阵乘法的梯度处理
	return true;
}

Loss::Loss(Graph* _graph, Opt* a, Opt* b)
{
	graph=_graph;
	type="Loss";
	name="_"+type;
	pre.push_back(a);
	pre.push_back(b);
	if(a->data.GetDim()!=b->data.GetDim())
	{
		cout<<"ERROR: Operator construction failed"<<endl;
		exit(0);
	}
	data=Tensor(0.0);
	gradi=Tensor(0.0);
}
bool Loss::Forward(Session* _sess)
{
	Tensor a=pre[0]->data,b=pre[1]->data;
	data=cross_entrophy(a,b);
	return true;
}
bool Loss::Backward()
{
	float tmp=gradi.GetVal()[0];
	Tensor a=pre[0]->data,b=pre[1]->data;
	pre[0]->AddGrad(tmp*cross_entrophy_gradient(a,b));//对损失函数的求导
	return true;
}