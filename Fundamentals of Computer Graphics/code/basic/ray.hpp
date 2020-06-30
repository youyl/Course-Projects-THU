#ifndef RAY_MATH
#define RAY_MATH

#include "math.hpp"

class Ray
{
	private:
		Pnt ori,dir;
	public:
		Ray()
		{
			ori=Pnt(0,0,0);
			dir=Pnt(1,0,0);
		}
		Ray(const Pnt &_a,const Pnt &_b)
		{
			ori=_a;
			dir=_b;
		}
		inline Pnt direction() const
		{
			return dir;
		}
		inline Pnt origin() const
		{
			return ori;
		}
		inline Pnt at(double t) const
		{
			return ori+(dir*t);
		}
};

#endif