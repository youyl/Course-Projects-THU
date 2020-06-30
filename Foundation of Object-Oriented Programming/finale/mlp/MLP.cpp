#include "../tt.h"

MLP::MLP(Graph* _graph)
{
	sess=new Session(_graph);
	BatchSum=sess->Parameter;
}

void MLP::Init()//随机初始化所有参数
{
	sess->RandomInit();
}

void MLP::SetLearningRate(float x)//设置学习率
{
	Lrate=x;
}

void MLP::SampleSum()//将当前数据对应的梯度加入batchsum
{
	for (auto ptr: BatchSum)
	{
		string name=ptr.first->name;
		BatchSum[ptr.first]=BatchSum[ptr.first]+ptr.first->gradi;
	}
}

void MLP::ParameterUpdate(int BatchSize)//用存储的梯度更新参数
{
	for (auto ptr: BatchSum)
	{
		Tensor a=sess->Parameter[ptr.first];
		sess->Parameter[ptr.first]=a-((1.0/BatchSize)*Lrate*BatchSum[ptr.first]);
		BatchSum[ptr.first].SetZero();
	}
}

void MLP::MiniBatch(const vector<vector<float> >& batch, const vector<vector<float> >& ans)//minibatch训练，传入一个batch的训练数据，训练mlp并输出相关信息
{
	map<string,Tensor>mp;
	float totalloss=0;
	int correct=0;
	for (int i=0;i<batch.size();i++)
	{
		mp.clear();
		mp["Input"]=Tensor(batch[i]);
		mp["Answer"]=Tensor(ans[i]);
		sess->ForwardPropagation("Grad",mp);
		totalloss+=sess->GetCurVal("Loss").GetVal()[0];
		if(Judge(sess->GetCurVal("Output").GetVal(),ans[i]))correct++;
		SampleSum();
	}
	ParameterUpdate(batch.size());
	cout<<"Correction is "<<correct<<". Accuracy for this Batch is "<<correct*1.0/(batch.size())<<endl;
	cout<<"Total Loss for this Batch is "<<totalloss<<endl;
}

int MLP::TestBatch(const vector<vector<float> >& batch, const vector<vector<float> >& ans)//批量测试，对mlp不做改动，返回正确的数据组数
{
	map<string,Tensor>mp;
	int correct=0;
	for (int i=0;i<batch.size();i++)
	{
		mp.clear();
		mp["Input"]=Tensor(batch[i]);
		mp["Answer"]=Tensor(ans[i]);
		sess->ForwardPropagation("Grad",mp);
		if(Judge(sess->GetCurVal("Output").GetVal(),ans[i]))correct++;
	}
	return correct;
}

int MLP::SingleTest(const vector<float>& batch, const vector<float>& ans)//对mnist特化的测试，单独测试一个数据，返回判断的数值
{
	map<string,Tensor>mp;
	mp["Input"]=Tensor(batch);
	mp["Answer"]=Tensor(ans);
	sess->ForwardPropagation("Grad",mp);
	vector<float>tmp=sess->GetCurVal("Output").GetVal();
	int mx=0;
	for (int i=1;i<10;i++)
	{
		if(tmp[i]>tmp[mx])mx=i;
	}
	return mx;
}

bool MLP::Judge(const vector<float>& a,const vector<float>& b)//判断两个近似one-hot的数组是否表达同一个数
{
	int mx=0;
	for (int i=1;i<10;i++)
	{
		if(a[i]>a[mx])mx=i;
	}
	int mx2=0;
	for (int i=1;i<10;i++)
	{
		if(b[i]>b[mx2])mx2=i;
	}
	return (mx2==mx);
}

MLP::~MLP()
{
	delete sess;
}