
class Session {
public:
	Graph* graph;
	vector<Tensor> ans;//记录历史答案
	bool innate;//判断session所对应的计算图是否是随着session生成的
	virtual ~Session() {if(innate==true)delete graph;}//如果是，则析构的时候要析构该计算图
	Session() { graph = new Graph; innate= true; }
	
	//用于完成基础功能的函数
	void declare(const string&);
	void code(const string&);
	void run(const string&);
	
	map<Opt*, Tensor>Parameter;//存储参数的值
	map<Opt*, Tensor>PHs;//计算时临时存放占位符的值
	vector<pair<Opt*, Opt*> >asnrecord;//记录一次运算中的assign操作
	
	Session(Graph*);//用一张现成的计算图初始化session
	//正向传播和反向传播
	bool BackPropagation(Opt*);
	bool ForwardPropagation(string, const map<string,Tensor>&);
	//提供对于参数和结点值的接口
	Tensor GetCurVal(string);
	void SetParameter(string,Tensor);
	void AddParameter(string,Tensor);
	//随机初始化参数
	void RandomInit();
	//文件操作
	void Export(string);
	void Import(string);
};

