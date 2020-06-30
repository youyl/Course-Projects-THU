#ifndef TEX_COL
#define TEX_COL

#include "../basic/basic_include.h"

class Texture
{
	private:
	public:
		Texture(){}
		virtual bool is_diffusion() const =0;
		virtual bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &color,int depth) const =0;
		virtual Pnt emit() const =0;
		virtual Pnt getcolor() const =0;
};

#endif