
class Graph {
public:
	int time_cnt;//时间戳
	vector<Opt*> opts;//记录创建过的结点，用于析构
	map<string, Opt*> dict;//记录结点名字和地址的对应关系
	
	virtual ~Graph();
	Graph() {
		time_cnt = 0;
	}
	
	//创建各种类型的结点
	Opt* creat_BinaryOpt(string, string, string);
	Opt* creat_UnaryOpt(string, string);
	Opt* creat_CondOpt(string, string, string);
	
	Opt* creat_PlaceHolderOpt(const vector<int>&);
	Opt* creat_ConstantOpt(Tensor);
	Opt* creat_VariableOpt(const vector<int>&);
	
	//通常使用的接口
	void new_Binary(string, string, string, string);
	void new_Unary(string, string, string);
	void new_Cond(string, string, string, string);
	
	void new_PlaceHolder(string, const vector<int>&);
	void new_Constant(string, Tensor);
	void new_Variable(string, const vector<int>&);
	
	void clean();
};

