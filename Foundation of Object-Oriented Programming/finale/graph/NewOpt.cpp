#include "../tt.h"

Opt* Graph::creat_BinaryOpt(string type, string a, string b) {//加入新的双目运算符结点
	if(dict.find(a)==dict.end()||dict.find(b)==dict.end())
	{
		cout<<"ERROR: Wrong Name"<<endl;
		exit(0);
	}
	Opt* a_opt = dict[a];
	Opt* b_opt = dict[b];
	Opt* Binary_opt;
	if(type == "+")Binary_opt=		 new Add(this, a_opt, b_opt)      ;
	if(type == "-")Binary_opt=		 new Subtract(this, a_opt, b_opt) ;
	if(type == "*")Binary_opt=       new Multiply(this, a_opt, b_opt) ;
	if(type == "/")Binary_opt=       new Divide(this, a_opt, b_opt)   ;
	if(type == ">")Binary_opt=       new BCmp(this, a_opt, b_opt)     ;
	if(type == ">=")Binary_opt=      new BEQCmp(this, a_opt, b_opt)   ;
	if(type == "<")Binary_opt=       new LCmp(this, a_opt, b_opt)     ;
	if(type == "<=")Binary_opt=      new LEQCmp(this, a_opt, b_opt)   ;
	if(type == "==")Binary_opt=      new EQCmp(this, a_opt, b_opt)    ;
	if(type == "!=")Binary_opt=      new NEQCmp(this, a_opt, b_opt)   ;
	if(type == "AT")Binary_opt=      new At(this, a_opt, b_opt)       ;
	if(type == "BIND")Binary_opt=    new Bind(this, a_opt, b_opt)     ;
	if(type == "ASSIGN")Binary_opt=  new Assign(this, a_opt, b_opt)   ;
	if(type == "MATMUL")Binary_opt=  new MatMul(this, a_opt, b_opt)   ;
	if(type == "LOSS")Binary_opt=    new Loss(this, a_opt, b_opt)     ;
	opts.push_back(Binary_opt);
	return Binary_opt;
}

Opt* Graph::creat_UnaryOpt(string type, string a) {//加入新的单目运算符结点
	if(dict.find(a)==dict.end())
	{
		cout<<"ERROR: Wrong Name"<<endl;
		exit(0);
	}
	Opt* a_opt = dict[a];
	Opt* Unary_opt;
	if(type == "SIN")Unary_opt=      new Sin(this, a_opt)     ;
	if(type == "LOG")Unary_opt=      new Log(this, a_opt)     ;
	if(type == "EXP")Unary_opt=      new Exp(this, a_opt)     ;
	if(type == "TANH")Unary_opt=     new Tanh(this, a_opt)    ;
	if(type == "SIGMOID")Unary_opt=  new Sigmoid(this, a_opt) ;
	if(type == "PRINT")Unary_opt=    new Print(this, a_opt)   ;
	if(type == "GRAD")Unary_opt=     new Grad(this, a_opt)    ;
	if(type == "ASSERT")Unary_opt=   new Assert(this, a_opt)  ;
	opts.push_back(Unary_opt);
	return Unary_opt;
}

Opt* Graph::creat_CondOpt(string condition, string a, string b) {//加入新的cond结点
	if(dict.find(a)==dict.end()||dict.find(b)==dict.end())
	{
		cout<<"ERROR: Wrong Name"<<endl;
		exit(0);
	}
	Opt* a_opt = dict[a];
	Opt* b_opt = dict[b];
	Opt* condition_opt = dict[condition];
	Opt* Cond_opt = new Cond(this, condition_opt, a_opt, b_opt);
	opts.push_back(Cond_opt);
	return Cond_opt;
}

Opt* Graph::creat_PlaceHolderOpt(const vector<int>& x) {
	Opt* PlaceHolder_opt = new PlaceHolder(this,x);
	opts.push_back(PlaceHolder_opt);
	return PlaceHolder_opt;
}

Opt* Graph::creat_ConstantOpt(Tensor data) {
	Opt* Constant_opt = new Constant(this, data);
	opts.push_back(Constant_opt);
	return Constant_opt;
}

Opt* Graph::creat_VariableOpt(const vector<int>& x) {
	Opt* Variable_opt = new Variable(this,x);
	opts.push_back(Variable_opt);
	return Variable_opt;
}

//通常使用的接口

void Graph::new_Binary(string name, string type, string a, string b) {
	dict[name] = creat_BinaryOpt(type, a, b) -> rename(name);
}

void Graph::new_Unary(string name, string type, string a) {
	dict[name] = creat_UnaryOpt(type, a) -> rename(name);
//	if(type == "PRINT")
//		dict[name] -> rename(a);这里是原先第一阶段代码的一个错误之处，这两行应当直接删去
}

void Graph::new_Cond(string name, string condition, string a, string b) {
	dict[name] = creat_CondOpt(condition, a, b) -> rename(name);
}

void Graph::new_PlaceHolder(string name, const vector<int>& x) {
	dict[name] = creat_PlaceHolderOpt(x) -> rename(name);
}

void Graph::new_Constant(string name, Tensor data) {
	dict[name] = creat_ConstantOpt(data) -> rename(name);
}

void Graph::new_Variable(string name, const vector<int>& x) {
	dict[name] = creat_VariableOpt(x) -> rename(name);
}

