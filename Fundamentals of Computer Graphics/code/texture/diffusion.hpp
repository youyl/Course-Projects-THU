#ifndef TEX_DIFF
#define TEX_DIFF

#include "base.hpp"

class Diffusion: public Texture
{
	private:
		Pnt color;
	public:
		Diffusion(){}
		Diffusion(const Pnt&_c)
		{
			color=_c;
		}
		bool is_diffusion() const
		{
			return true;
		}
		bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &col,int depth) const
		{
			Pnt n1=hit.n;
			if(dot(hit.n,r_in.direction())>0)
			{
				n1=n1*(-1);
			}
			double p=max(color.x(),max(color.y(),color.z()));
			if(depth>5&&Settings::rand01()>p)
			{
				return false;
			}
			else if(depth>5)
			{
				col=color/p;
			}
			else
			{
				col=color;
			}
			double r1=Settings::rand01()*2*3.1415926535897932384626;
			double r2=Settings::rand01();
			double r2s=sqrt(r2);
			Pnt w=n1,u,v;
			if(dabs(w.x())>0.1)u=cross(Pnt(0,1,0),w).normalized();
			else u=cross(Pnt(1,0,0),w).normalized();
			v=cross(w,u);
			Pnt des=(u*cos(r1)*r2s+v*sin(r1)*r2s+w*sqrt(1-r2)).normalized();
			nxt=Ray(r_in.at(hit.t)+n1*epsi,des);
			return true;
		}
		Pnt emit() const
		{
			return Pnt(0,0,0);
		}
		Pnt getcolor() const
		{
			return color;
		}
};

#endif