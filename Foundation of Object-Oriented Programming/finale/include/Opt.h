
class Opt {
public:
	string type;//类型
	string name;//名字
	vector<Opt*> pre;//前置节点
	Graph* graph;
	
	int time_cnt;//时间戳
	Tensor data;//暂时存储结果
	
	virtual ~Opt() {}
	Opt* rename(string);//重命名，来自第一阶段的代码
	
	Tensor gradi;//存储该节点在当次操作中的梯度
	Opt();
	Opt(Graph*);
	void AddGrad(Tensor);//修改对应的梯度
	void TPsort(vector<Opt*>&);//获取拓扑排序
	virtual bool Forward(Session*)=0;//每个运算符的正向和反向传播都是不同的
	virtual bool Backward()=0;
};


class Loss: public Opt//损失函数整体作为一种运算符
{
	public:
		Loss(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

class MatMul: public Opt//矩阵乘法
{
	public:
		MatMul(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

//算术运算符

class Add: public Opt
{
	public:
		Add(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class Subtract: public Opt
{
	public:
		Subtract(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class Multiply: public Opt
{
	public:
		Multiply(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class Divide: public Opt
{
	public:
		Divide(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

//比较运算符

class BCmp: public Opt//>
{
	public:
		BCmp(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class BEQCmp: public Opt//>=
{
	public:
		BEQCmp(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class LCmp: public Opt//<
{
	public:
		LCmp(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class LEQCmp: public Opt//<=
{
	public:
		LEQCmp(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class EQCmp: public Opt//==
{
	public:
		EQCmp(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};


class NEQCmp: public Opt//!=
{
	public:
		NEQCmp(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Assign: public Opt//!=
{
	public:
		Assign(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Bind: public Opt//!=
{
	public:
		Bind(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

class At: public Opt//!=
{
	public:
		At(Graph*,Opt*,Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Sin: public Opt
{
	public:
		Sin(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Log: public Opt
{
	public:
		Log(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Exp: public Opt
{
	public:
		Exp(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Tanh: public Opt
{
	public:
		Tanh(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Sigmoid: public Opt
{
	public:
		Sigmoid(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Print: public Opt
{
	public:
		Print(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Grad: public Opt
{
	public:
		Grad(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Assert: public Opt
{
	public:
		Assert(Graph*, Opt*);
		bool Forward(Session*);
		bool Backward();
};

class Cond: public Opt {
public:
	Cond(Graph*, Opt*, Opt*, Opt*);
	bool Forward(Session*);
	bool Backward();
};

class PlaceHolder: public Opt {
public:
	PlaceHolder(Graph*, const vector<int>&);
	bool Forward(Session*);
	bool Backward();
};

class Constant: public Opt {
public:
	Constant(Graph*, Tensor);
	bool Forward(Session*);
	bool Backward();
};

class Variable: public Opt {
public:
	Variable(Graph*,const vector<int>&);
	bool Forward(Session*);
	bool Backward();
};

