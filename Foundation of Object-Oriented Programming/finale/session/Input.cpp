#include "../tt.h"

//负责实现基础功能，以通过oj的测试
//与第一阶段的代码非常相似

void Session::declare(const string& input) {
	istringstream in(input);
	string name;
	char type;
	float data = 0.0;
	
	in >> name >> type;
	
	vector<int>Scal;
	
	if(type == 'C' or type == 'V') {
		in >> data;
	}
	if(type == 'P') graph -> new_PlaceHolder(name,Scal);
	if(type == 'C') graph -> new_Constant(name, Tensor(data));
	if(type == 'V')
	{
		graph -> new_Variable(name,Scal);
		Parameter[graph->dict[name]]=data;//注意处理variable的时候要存入参数表
	}
}

void Session::code(const string& input) {
	istringstream in(input);
	string name, opt, a, b, c;
	char eq;
	
	in >> name >> eq;
	in >> opt;
	if(opt == "COND") {//生成特殊的cond运算符
		in >> a >> b >> c;
		graph -> new_Cond(name, a, b, c);
	}
	else if(opt=="ASSIGN" or opt=="BIND")//生成比较特别的两个双目运算符
	{
		in>>a>>b;
		graph-> new_Binary(name,opt,a,b);
	}
	else if(opt == "SIN" or opt == "LOG" or opt == "TANH" or
			opt == "EXP" or opt == "SIGMOID" or opt == "PRINT" or opt == "GRAD" or opt == "ASSERT") {
		in >> a;
		graph -> new_Unary(name, opt, a);//生成单目运算符
	}
	else {
		a = opt;
		in >> opt >> b;
		graph -> new_Binary(name, opt, a, b);//生成双目运算符
	}
}

void Session::run(const string& input) {
	
	istringstream in(input);
	string opt, name;
	in >> opt >> name;
	if(opt == "EVAL") {
		int feed_dict_num = 0;
		string feed_name;
		float feed_data;
		in >> feed_dict_num;
		map<string,Tensor>_phs;
		for(int i = 0; i < feed_dict_num; i ++) {
			in >> feed_name >> feed_data;
			_phs[feed_name]=Tensor(feed_data);//存入占位符的值
		}
		if(ForwardPropagation(name,_phs))
			printf("%.4f\n", GetCurVal(name).GetVal()[0]);
		ans.push_back(GetCurVal(name));
	}
	if(opt == "SETCONSTANT") {
		float data;
		in >> data;
		Parameter[graph->dict[name]]=Tensor(data);
		ans.push_back(Tensor(0.0));
	}
	if(opt == "SETANSWER") {
		int idx;
		in >> idx;
		Tensor data = ans[idx - 1];
		Parameter[graph->dict[name]]=data;
		ans.push_back(Tensor(0.0));
	}
}
