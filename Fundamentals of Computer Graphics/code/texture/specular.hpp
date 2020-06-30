#ifndef TEX_SPEC
#define TEX_SPEC

#include "base.hpp"

class Specular: public Texture
{
	private:
		Pnt color;
	public:
		Specular(){}
		Specular(const Pnt&_col)
		{
			color=_col;
		}
		bool is_diffusion() const
		{
			return false;
		}
		bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &col,int depth) const
		{
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
			nxt=Ray(r_in.at(hit.t),(r_in.direction()-hit.n*2*dot(hit.n,r_in.direction())).normalized());
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