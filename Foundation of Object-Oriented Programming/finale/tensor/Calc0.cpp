#include "../tt.h"

//比较运算符直接选用第一个元素作为键值
//非标量的张量没有定义比较大小的方式，因此用户应当避免

bool operator >=(const Tensor& x, const Tensor& y)
{
	return x.Val[0]>=y.Val[0];
}

bool operator <=(const Tensor& x, const Tensor& y)
{
	return x.Val[0]<=y.Val[0];
}

bool operator >(const Tensor& x, const Tensor& y)
{
	return x.Val[0]>y.Val[0];
}

bool operator <(const Tensor& x, const Tensor& y)
{
	return x.Val[0]<y.Val[0];
}

bool operator ==(const Tensor& x, const Tensor& y)
{
	return x.Val[0]==y.Val[0];
}

bool operator !=(const Tensor& x, const Tensor& y)
{
	return x.Val[0]!=y.Val[0];
}

//这里的几个数学函数都是对于张量的每个元素单独进行操作的

Tensor sigmoid(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=sigmoid(x.Val[i]);
	}
	return res;
}

Tensor sin(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=sin(x.Val[i]);
	}
	return res;
}

Tensor cos(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=cos(x.Val[i]);
	}
	return res;
}

Tensor exp(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=exp(x.Val[i]);
	}
	return res;
}

Tensor tanh(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=tanh(x.Val[i]);
	}
	return res;
}

Tensor cosh(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=cosh(x.Val[i]);
	}
	return res;
}

Tensor log(const Tensor& x)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		if(x.Val[i]>=Epsi)
		{
			res.Val[i]=log(x.Val[i]);
		}
	}
	return res;
}

Tensor inverse(const Tensor& x)//这个表示倒数
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		if(x.Val[i]>=Epsi||x.Val[i]<=-Epsi)
		{
			res.Val[i]=1.0/x.Val[i];
		}
	}
	return res;
}
