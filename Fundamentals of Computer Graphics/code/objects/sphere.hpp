#ifndef OBJ_SPHERE
#define OBJ_SPHERE

#include "base.hpp"

class Sphere: public Object
{
	private:
		Pnt pos;
		double radius;
	public:
		Sphere(){}
		Sphere(const Pnt&_pos,double _r,Texture*_tex)
		{
			pos=_pos;
			radius=_r;
			texture=_tex;
		}
		bool intersect(const Ray&r_in,Hit&hit) const
		{
			double a=dot(r_in.direction(),r_in.direction());
			double b=2*dot(r_in.direction(),r_in.origin()-pos);
			double c=dot(r_in.origin()-pos,r_in.origin()-pos)-radius*radius;
			double det=b*b-4*a*c;
			if(det<epsi)return false;
			det=sqrt(det);
			double t1=(-b+det)/(2*a);
			double t2=(-b-det)/(2*a);
			double t;
			if(t1<epsi&&t2<epsi)return false;
			if(t1<epsi)t=t2;
			else if(t2<epsi)t=t1;
			else if(t1<t2)t=t1;
			else t=t2;
			Pnt des=r_in.at(t);
			hit=Hit(t,(des-pos).normalized());
			return true;
		}
};

#endif