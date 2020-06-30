#ifndef OBJ_TRI
#define OBJ_TRI

#include "base.hpp"

class Triangle: public Object
{
	private:
		Pnt pos[3],n;
	public:
		Triangle(){}
		Triangle(const Pnt&_p1,const Pnt&_p2,const Pnt&_p3,Texture *_tex)
		{
			pos[0]=_p1;
			pos[1]=_p2;
			pos[2]=_p3;
			n=cross(pos[1]-pos[0],pos[2]-pos[0]).normalized();
			texture=_tex;
		}
		Pnt normalvector() const
		{
			return n;
		}
		Pnt node(int x) const
		{
			return pos[x];
		}
		bool intersect(const Ray&r_in,Hit&hit) const
		{
			Pnt e1=pos[1]-pos[0],e2=pos[2]-pos[0];
			Pnt P=cross(r_in.direction(),e2);
			double det=dot(e1,P);
			Pnt T;
			if(det>0)
			{
				T=r_in.origin()-pos[0];
			}
			else
			{
				T=pos[0]-r_in.origin();
				det=-det;
			}
			if(det<1e-5)return false;
			double u,v,t;
			u=dot(T,P);
			if(u<epsi||u>det)return false;
			Pnt Q=cross(T,e1);
			v=dot(r_in.direction(),Q);
			if(v<epsi||v+u>det)return false;
			t=dot(e2,Q);
			t/=det;
			hit=Hit(t,n);
			if(t<epsi)return false;
			return true;
		}
		friend bool cmptriangle(const Triangle&tri1,const Triangle&tri2);
};
bool cmptriangle(const Triangle&tri1,const Triangle&tri2)
{
	double mx1=max(tri1.pos[0].p(Settings::tricmp),max(tri1.pos[1].p(Settings::tricmp),tri1.pos[2].p(Settings::tricmp)));
	double mx2=max(tri2.pos[0].p(Settings::tricmp),max(tri2.pos[1].p(Settings::tricmp),tri2.pos[2].p(Settings::tricmp)));
	return mx1<mx2;
}
#endif