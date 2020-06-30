
class MLP
{
	public:
		Session* sess;//对应的session
		float Lrate;//学习率
		map<Opt*,Tensor>BatchSum;//存放每个batch的梯度总和
		MLP(Graph*);
		void MiniBatch(const vector<vector<float> >&, const vector<vector<float> >&);//minibatch训练
		int TestBatch(const vector<vector<float> >&, const vector<vector<float> >&);//批量测试
		bool Judge(const vector<float>&,const vector<float>&);//判断两个one-hot是否表达同一个数
		void SetLearningRate(float x);//设置学习率
		void SampleSum();//将当前数据对应的梯度加入batchsum
		void Init();//初始化参数
		void ParameterUpdate(int);//用存储的梯度更新参数
		int SingleTest(const vector<float>&, const vector<float>&);//单独测试一组数据
		~MLP();
};