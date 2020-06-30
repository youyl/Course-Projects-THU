#include "../tt.h"


Tensor transpose(const Tensor& a)//对矩阵特化的操作，求矩阵的转置
{
	vector<int> tmp=a.Dim;
	if(tmp.size()!=2)
	{
		cout<<"ERROR: Transpose are restricted to matrix"<<endl;
		exit(0);
	}
	swap(tmp[0],tmp[1]);
	Tensor res(tmp,0);
	for (int i=0;i<a.Dim[0];i++)
	{
		for (int j=0;j<a.Dim[1];j++)
		{
			res.Val[res.Pos(j,i)]=a.Val[a.Pos(i,j)];
		}
	}
	return res;
}

Tensor MatMulti(const Tensor& a, const Tensor& b)//矩阵乘法
{
	vector<int> tmp1=a.Dim, tmp2=b.Dim,tmp3;
	if(tmp1.size()!=2||tmp2.size()!=2)
	{
		cout<<"ERROR: MatMul are restricted to matrix (Calc)"<<endl;
		exit(0);
	}
	tmp3.push_back(tmp1[0]);
	tmp3.push_back(tmp2[1]);
	if(tmp1[1]!=tmp2[0])
	{
		cout<<"ERROR: Unable to fit Matrix Size (Calc)"<<endl;
		exit(0);
	}
	Tensor res(tmp3,0);
	for (int i=0;i<tmp1[0];i++)
	{
		for (int j=0;j<tmp2[1];j++)
		{
			for (int k=0;k<tmp1[1];k++)
			{
				res.Val[res.Pos(i,j)]+=a.Val[a.Pos(i,k)]*b.Val[b.Pos(k,j)];
			}
		}
	}
	return res;
}

Tensor cross_entrophy(const Tensor& a, const Tensor& b)//softmax交叉熵损失函数，返回一个标量
{
	Tensor sm=a;
	sm.softmax();
	float res=0;
	for (int i=0;i<a.Val.size();i++)
	{
		res-=b.Val[i]*log(sm.Val[i]);
	}
	return Tensor(res);
}

Tensor cross_entrophy_gradient(const Tensor& a, const Tensor& b)//交叉熵损失函数的梯度
{
	Tensor sm=a;
	sm.softmax();
	return sm-b;
}