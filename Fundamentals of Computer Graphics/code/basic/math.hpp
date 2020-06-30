#ifndef POINT_MATH
#define POINT_MATH

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cstring>

using namespace std;

const double epsi=1e-7;

bool is_debug=false;
int debug_cnt=0;

inline double dabs(double x)
{
	if(x>0)return x;
	return -x;
}

inline double clamp(double x)
{
	if(x<0)return 0;
	if(x>1)return 1;
	return x;
}

inline int toInt(double x)
{
	return (int)(pow(clamp(x),1/2.2)*255+0.5);
}

class Pnt
{
	private:
		double pos[3];
	public:
		Pnt()
		{
			pos[0]=0;
			pos[1]=0;
			pos[2]=0;
		}
		Pnt(const double &_x,const double &_y,const double &_z)
		{
			pos[0]=_x;
			pos[1]=_y;
			pos[2]=_z;
		}
		inline void setx(double _x)
		{
			pos[0]=_x;
		}
		inline void sety(double _x)
		{
			pos[1]=_x;
		}
		inline void setz(double _x)
		{
			pos[2]=_x;
		}
		inline double x() const
		{
			return pos[0];
		}
		inline double y() const
		{
			return pos[1];
		}
		inline double z() const
		{
			return pos[2];
		}
		inline double p(int idx) const
		{
			return pos[idx];
		}
		inline Pnt operator +(const Pnt &_b) const
		{
			return Pnt(pos[0]+_b.pos[0],pos[1]+_b.pos[1],pos[2]+_b.pos[2]);
		}
		inline Pnt operator -(const Pnt &_b) const
		{
			return Pnt(pos[0]-_b.pos[0],pos[1]-_b.pos[1],pos[2]-_b.pos[2]);
		}
		inline Pnt operator *(const Pnt &_b) const
		{
			return Pnt(pos[0]*_b.pos[0],pos[1]*_b.pos[1],pos[2]*_b.pos[2]);
		}
		inline Pnt operator /(const double &_b) const
		{
			return Pnt(pos[0]/_b,pos[1]/_b,pos[2]/_b);
		}
		inline Pnt operator *(const double &_b) const
		{
			return Pnt(pos[0]*_b,pos[1]*_b,pos[2]*_b);
		}
		inline double norm() const
		{
			return sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);
		}
		inline void normalize()
		{
			double _tmp=norm();
			pos[0]/=_tmp;
			pos[1]/=_tmp;
			pos[2]/=_tmp;
		}
		inline Pnt normalized() const
		{
			double _tmp=norm();
			return Pnt(pos[0]/_tmp,pos[1]/_tmp,pos[2]/_tmp);
		}
		friend double dot(const Pnt&_a, const Pnt&_b);
		friend Pnt cross(const Pnt&_a, const Pnt&_b);
};

double dot(const Pnt&_a, const Pnt&_b)
{
	return _a.pos[0]*_b.pos[0]+_a.pos[1]*_b.pos[1]+_a.pos[2]*_b.pos[2];
}

Pnt cross(const Pnt&_a, const Pnt&_b)
{
	return Pnt(_a.pos[1]*_b.pos[2]-_a.pos[2]*_b.pos[1],_a.pos[2]*_b.pos[0]-_a.pos[0]*_b.pos[2],_a.pos[0]*_b.pos[1]-_a.pos[1]*_b.pos[0]);
}

namespace Settings
{
	bool DOF=false;
	bool AA=false;
	bool is_Sppm=false;
	int tricmp=0;
	double rand01()
	{
		return (rand()*1.0)/(1.0*RAND_MAX);
	}
	Pnt circleRand(const Pnt&up,const Pnt&right,double rr)
	{
		double radius=rand01()*rr;
		double angle=rand01()*2*3.1415926535897932384626;
		return up*radius*(sin(angle))+right*radius*(cos(angle));
	}
};

class Hit
{
	public:
		Pnt n;
		double t;
		Hit(){}
		Hit(const double&_t,const Pnt&_n)
		{
			t=_t;
			n=_n;
		}
		inline void init()
		{
			t=1e10;
		}
		inline void update(const double&_t,const Pnt&_n)
		{
			if(_t<t)
			{
				t=_t;
				n=_n;
			}
		}
};

#endif