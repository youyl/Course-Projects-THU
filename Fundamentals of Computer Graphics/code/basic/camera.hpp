#ifndef CAMERA_MATH
#define CAMERA_MATH

#include "math.hpp"
#include "ray.hpp"

class Camera
{
	private:
		Pnt pos,dir,up,right;
		double focus,rad,halfv,halfh;
		int px,py;
	public:
		Camera(){}
		inline void setRadius(double _x)
		{
			rad=_x;
		}
		inline void setFocus(double _x)
		{
			focus=_x;
		}
		inline void setAngle(double _x,double _y)
		{
			halfv=_x/2;
			halfh=_y/2;
		}
		inline void setPixel(int _x,int _y)
		{
			px=_x;
			py=_y;
		}
		inline void setPos(const Pnt&_pos,const Pnt&_up,const Pnt&_right)
		{
			pos=_pos;
			up=_up.normalized();
			right=_right.normalized();
			dir=cross(up,right).normalized();
		//	cerr<<dir.x()<<' '<<dir.y()<<' '<<dir.z()<<endl;
		}
		inline Ray getRay(double x,double y,double biasx=0,double biasy=0) const
		{
			if(Settings::AA)
			{
				x=x+biasx;
				y=y+biasy;
			}
			if(Settings::is_Sppm)
			{
				double rdx=Settings::rand01()*2;
				double rdy=Settings::rand01()*2;
				double ssx;
				double ssy;
				if(rdx>1)ssx=1.0-sqrt(rdx-1.0)/2.0;
				else ssx=sqrt(rdx)/2.0;
				if(rdy>1)ssy=1.0-sqrt(rdy-1.0)/2.0;
				else ssy=sqrt(rdy)/2.0;
				x=x+ssx;
				y=y+ssy;
			}
			Pnt des,sta;
			des=dir*focus;
			des=des+up*(tan(halfv))*focus*(x-px/2)/(px*0.5);
			des=des+right*(tan(halfh))*focus*(y-py/2)/(py*0.5);
			des=des+pos;
			sta=pos;
			if(Settings::DOF)
			{
				sta=sta+Settings::circleRand(up,right,rad);
			}
			return Ray(sta,(des-sta).normalized());
		}
};

#endif