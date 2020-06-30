#include "../tt.h"

Tensor::Tensor(){}

Tensor::Tensor(float x)
{
	Dim.clear();
	Val.resize(1);
	Val[0]=x;
}

Tensor::Tensor(const vector<float>&_val)//使用一个浮点数创建一个列向量
{
	Dim.resize(2);
	Dim[0]=_val.size();
	Dim[1]=1;
	Val=_val;
}
Tensor::Tensor(const vector<int>& _dim, float _val)//创建一个指定大小的张量，并用相同的浮点数填充
{
	Dim=_dim;
	Val.resize(GetSize());
	for (int i=0;i<Val.size();i++)
	{
		Val[i]=_val;
	}
}
Tensor::Tensor(const vector<int>& _dim, const vector<float>& _val)//创建一个指定了大小和内容的张量
{
	Dim=_dim;
	Val=_val;
}

bool Tensor::IsScalar()//判断一个张量是不是标量（维数为0）
{
	return (Dim.size()==0);
}

int Tensor::GetSize() const//返回张量中的元素个数
{
	int res=1;
	for (int i=0;i<Dim.size();i++)
	{
		res*=Dim[i];
	}
	return res;
}


int Tensor::Pos(int x,int y) const//对矩阵特化，返回(x,y)这一点在存储容器中的位置
{
	return x*Dim[1]+y;
}

vector<float> Tensor::GetVal()//返回张量的内容
{
	return Val;
}

vector<int> Tensor::GetDim()//返回张量的大小
{
	return Dim;
}

bool Tensor::HaveZero()//检查张量中是否有零
{
	for (int i=0;i<Val.size();i++)
	{
		if(Val[i]>-Epsi&&Val[i]<Epsi)return true;//考虑浮点数的误差
	}return false;
}

bool Tensor::HaveLTZero()//检查张量中是否有小于零的元素
{
	for (int i=0;i<Val.size();i++)
	{
		if(Val[i]<Epsi)return true;//考虑浮点数的误差
	}return false;
}

void Tensor::SetZero()//将该张量中的元素全部置为0
{
	for (int i=0;i<Val.size();i++)
	{
		Val[i]=0;
	}
}

void Tensor::softmax()//对该张量做softmax操作
{
	float sum=0;
	for (int i=0;i<Val.size();i++)
	{
		sum+=exp(Val[i]);
	}
	for (int i=0;i<Val.size();i++)
	{
		Val[i]=exp(Val[i])/sum;
	}
}

void Tensor::Export(ofstream& fout)//向指定文件流输出张量内容
{
	for (int i=0;i<Val.size();i++)
	{
		fout<<Val[i]<<' ';
	}
	fout<<endl;
}

void Tensor::Import(ifstream& fin)//从指定文件流读入张量内容
{
	for (int i=0;i<Val.size();i++)
	{
		fin>>Val[i];
	}
}

void Tensor::Print() const//打印张量的大小和内容，用于调试
{
	cout<<"Dim: ";
	for (int i=0;i<Dim.size();i++)
	{
		cout<<Dim[i]<<' ';
	}cout<<endl;
	cout<<"Val: ";
	for (int i=0;i<Val.size();i++)
	{
		cout<<Val[i]<<' ';
	}cout<<endl;
}

void Tensor::RandomInit()//对矩阵特化，采用Xavier方法确定方差，并用正态分布的随机数初始化矩阵
{
	if(Dim.size()!=2)
	{
		cout<<"ERROR: RandomInit only applied to matrix"<<endl;
		exit(0);
	}
	double sigma;
	if(Dim[0]!=1&&Dim[1]!=1)sigma=2.0/((Dim[0]+Dim[1])*1.0);
	else sigma=1.0/(Dim[0]*Dim[1]*1.0);
	sigma=sqrt(sigma);
	for (int i=0;i<Val.size();i++)
	{
		Val[i]=gaussRand()*sigma;
	}
}