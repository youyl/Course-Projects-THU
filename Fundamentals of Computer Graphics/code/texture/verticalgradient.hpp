#ifndef TEX_VGRA
#define TEX_VGRA

#include "base.hpp"
#include <iostream>
#include <fstream>

class VerGradient: public Texture
{
	private:
		Pnt cl1,cl2;
	public:
		VerGradient()
		{
			cl1=Pnt(0.75,0.05,0.05);
			cl2=Pnt(0.05,0.05,0.75);
		}
		Pnt getPosCol(const Pnt&nv) const
		{
			double p1=nv.z()/1.5;
			return cl1*p1+cl2*(1-p1);
		}
		bool is_diffusion() const
		{
			return true;
		}
		bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &col,int depth) const
		{
			Pnt color=getPosCol(r_in.at(hit.t));
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
			nxt=Ray(r_in.at(hit.t)+n1*(1e-6),des);
			return true;
		}
		Pnt emit() const
		{
			return Pnt(0,0,0);
		}
		Pnt getcolor() const
		{
			return Pnt(1,1,1);
		}
};

#endif