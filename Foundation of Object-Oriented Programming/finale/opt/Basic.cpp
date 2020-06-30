#include "../tt.h"

Opt* Opt::rename(string new_name) {
	name = new_name;
	return this;
}

Opt::Opt(Graph* _graph)
{
	time_cnt=0;
	graph=_graph;
}

Opt::Opt()
{
	time_cnt=0;
}

void Opt::AddGrad(Tensor x)//修改梯度
{
	gradi=gradi+x;
}

void Opt::TPsort(vector<Opt*>&order)//获取拓扑序，用于正向和反向传播
{
	if(time_cnt==graph->time_cnt)return;//通过时间戳控制重复访问
	time_cnt=graph->time_cnt;
	gradi.SetZero();
	for (int i=0;i<pre.size();i++)
	{
		pre[i]->TPsort(order);
	}
	order.push_back(this);//实质上是获取深度优先搜索中的出栈顺序
}

PlaceHolder::PlaceHolder(Graph* _graph, const vector<int>& _dim): Opt(_graph)
{
	type = "PlaceHolder"; 
	data = Tensor(_dim,0);
	gradi = Tensor(_dim,0);
}
bool PlaceHolder::Forward(Session* _sess)
{
	if(_sess->PHs.find(this)==_sess->PHs.end())
	{
		cout<<"ERROR: Placeholder missing"<<endl;
		return false;
	}
	data=_sess->PHs[this];//占位符的正向传播应当从session里读取对应的值
	return true;
}
bool PlaceHolder::Backward()
{
	return true;
}

Constant::Constant(Graph* _graph, Tensor _data): Opt(_graph) 
{
	type = "Constant";
	data = _data;
	gradi = Tensor(_data.GetDim(),0);
}
bool Constant::Forward(Session* _sess)
{
	return true;
}
bool Constant::Backward()
{
	return true;
}

Variable::Variable(Graph* _graph, const vector<int>& _dim): Opt(_graph)
{
	type = "Variable";
	gradi = Tensor(_dim,0);
	data = Tensor(_dim,0);
}
bool Variable::Forward(Session* _sess)
{
	data=_sess->Parameter[this];//variable的正向传播应当从session中读取对应的值
	return true;
}
bool Variable::Backward()
{
	return true;
}
