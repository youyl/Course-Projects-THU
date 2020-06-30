#pragma once
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cmath>

//所有代码公用的头文件

using namespace std;

class Opt;
class Graph;
class Session;

//前置声明

const float Epsi=1e-6;
const double Pi=3.1415926535897932;

//需要使用的浮点常数

inline float sigmoid(float x)//针对float类型的sigmoid函数
{
	return 1.0/(1.0+exp(-x));
}

inline int randv(int x)//随机生成[0，x]的随机整数
{
	int res=rand();
	while (res>x)
	{
		res=rand();
	}
	return res;
}

inline float gaussRand()//生成服从均值为0，方差为1的正态分布的随机数，用于神经网络的初始化
{
	static double U,V;
	static int phase=0;
	if(phase==1)
	{
		phase=1-phase;
		return V;
	}
	double x1,x2;
	do
	{
		x1=(rand()*1.0)/(RAND_MAX*1.0);
		x2=(rand()*1.0)/(RAND_MAX*1.0);
	}while (x1<Epsi||x2<Epsi);
	phase=1-phase;
	U=sqrt(-2.0*log(x1))*cos(2.0*Pi*x2);
	V=sqrt(-2.0*log(x1))*sin(2.0*Pi*x2);
	return U;
}

#include "include/Tensor.h"
#include "include/Opt.h"
#include "include/Graph.h"
#include "include/Session.h"
#include "include/MLP.h"

