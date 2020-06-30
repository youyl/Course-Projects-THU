#ifndef OBJ_BASE
#define OBJ_BASE

#include "../basic/basic_include.h"
#include "../texture/texture_include.hpp"

class Object
{
	private:
	public:
		Texture *texture;
		Object(){}
		Texture *getTexture()
		{
			return texture;
		}
		virtual bool intersect(const Ray&r_in,Hit&hit) const =0;
};

#endif