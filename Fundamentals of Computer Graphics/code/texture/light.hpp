#ifndef TEX_LIGHT
#define TEX_LIGHT

#include "base.hpp"

class Light: public Texture
{
	private:
		Pnt color;
	public:
		Light(){}
		Light(const Pnt&_c)
		{
			color=_c;
		}
		bool is_diffusion() const
		{
			return false;
		}
		bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &col,int depth) const
		{
			return false;
		}
		Pnt emit() const
		{
			return color;
		}
		Pnt getcolor() const
		{
			return color;
		}
};

#endif