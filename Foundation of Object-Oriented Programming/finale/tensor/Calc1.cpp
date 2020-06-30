#include "../tt.h"

//张量的四则运算，每个对应的元素分别计算

Tensor operator +(const Tensor& x, const Tensor& y)
{
	if(x.Dim!=y.Dim)return Tensor();
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=x.Val[i]+y.Val[i];
	}
	return res;
}

Tensor operator -(const Tensor& x, const Tensor& y)
{
	if(x.Dim!=y.Dim)return Tensor();
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=x.Val[i]-y.Val[i];
	}
	return res;
}

Tensor operator *(const Tensor& x, const Tensor& y)
{
	if(x.Dim!=y.Dim)return Tensor();
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=x.Val[i]*y.Val[i];
	}
	return res;
}

Tensor operator *(const float& x, const Tensor& y)
{
	Tensor res(y.Dim,0);
	for (int i=0;i<y.GetSize();i++)
	{
		res.Val[i]=x*y.Val[i];
	}
	return res;
}

Tensor operator *(const Tensor& x, const float& y)
{
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		res.Val[i]=x.Val[i]*y;
	}
	return res;
}

Tensor operator /(const Tensor& x, const Tensor& y)
{
	if(x.Dim!=y.Dim)return Tensor();
	Tensor res(x.Dim,0);
	for (int i=0;i<x.GetSize();i++)
	{
		if(y.Val[i]!=0.0)
		{
			res.Val[i]=x.Val[i]/y.Val[i];
		}
	}
	return res;
}