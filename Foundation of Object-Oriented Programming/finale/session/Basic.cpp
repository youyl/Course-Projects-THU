#include "../tt.h"

Session::Session(Graph* _graph)
{
	graph=_graph;
	innate=false;//用现成的图进行初始化时，需要设置这个标签为false
	for (Opt* _opt: _graph->opts)
	{
		if(_opt->type=="Variable")Parameter[_opt]=Tensor(_opt->data.GetDim(),0);//将所有的variable当作parameter存储起来
	}
}

Tensor Session::GetCurVal(string xx)
{
	Opt* tmp=graph->dict[xx];
	if(Parameter.find(tmp)!=Parameter.end())return Parameter[tmp];//如果他是参数，则有可能经过assign操作后，图中存放的值并不是最新的值
	return tmp->data;//否则返回图中暂存的值
}

bool Session::BackPropagation(Opt* x)
{
	vector<Opt*>Order;
	graph->clean();
	x->TPsort(Order);//获取拓扑序
	x->gradi=Tensor(1.0);
	for (int i=Order.size()-1;i>=0;i--)//反向遍历拓扑序，进行反向传播
	{
		if(!Order[i]->Backward())return false;
	}
	return true;
}

bool Session::ForwardPropagation(string xx, const map<string,Tensor>& _phs)
{
	Opt* x=graph->dict[xx];
	PHs.clear();
	for (auto ptr: _phs)
	{
		PHs[graph->dict[ptr.first]]=ptr.second;//将字符串到值的映射转化为结点到值的映射
	}
	vector<Opt*>Order;
	graph->clean();
	x->TPsort(Order);//获取拓扑序
	asnrecord.clear();
	for (int i=0;i<Order.size();i++)//正向遍历拓扑序，进行正向传播
	{
		if(!Order[i]->Forward(this))return false;
	}
	for (auto i: asnrecord)//执行assign操作
	{
		Parameter[i.first]=i.second->data;
	}
	return true;
}