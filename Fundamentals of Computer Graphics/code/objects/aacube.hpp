#ifndef OBJ_AAC
#define OBJ_AAC

#include "base.hpp"

class AAcube: public Object
{
	private:
		double sx,sy,sz,tx,ty,tz;
		int nv;//1 outside -1 inside
	public:
		AAcube(){}
		AAcube(const Pnt&_p1,const Pnt&_p2,int _n,Texture *_tex)
		{
			sx=min(_p1.x(),_p2.x());
			sy=min(_p1.y(),_p2.y());
			sz=min(_p1.z(),_p2.z());
			tx=max(_p1.x(),_p2.x());
			ty=max(_p1.y(),_p2.y());
			tz=max(_p1.z(),_p2.z());
			nv=_n;
			texture=_tex;
		}
		bool intersect(const Ray&r_in,Hit&hit) const
		{
			hit.init();
			Pnt n,des;
			double t;
			bool flag=false;
			//sx plane
			if(dabs(r_in.direction().x())>epsi)
			{
				n=Pnt(-nv,0,0);
				t=(sx-r_in.origin().x())/r_in.direction().x();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.z()>=sz&&des.z()<=tz)
				{
					hit.update(t,n);
					flag=true;
				}
			}
			//sy plane
			if(dabs(r_in.direction().y())>epsi)
			{
				n=Pnt(0,-nv,0);
				t=(sy-r_in.origin().y())/r_in.direction().y();
				des=r_in.at(t);
				if(t>epsi&&des.x()>=sx&&des.x()<=tx&&des.z()>=sz&&des.z()<=tz)
				{
					hit.update(t,n);
					flag=true;
				}
			}
			//sz plane
			if(dabs(r_in.direction().z())>epsi)
			{
				n=Pnt(0,0,-nv);
				t=(sz-r_in.origin().z())/r_in.direction().z();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.x()>=sx&&des.x()<=tx)
				{
					hit.update(t,n);
					flag=true;
				}
			}
			//tx plane
			if(dabs(r_in.direction().x())>epsi)
			{
				n=Pnt(nv,0,0);
				t=(tx-r_in.origin().x())/r_in.direction().x();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.z()>=sz&&des.z()<=tz)
				{
					hit.update(t,n);
					flag=true;
				}
			}
			//ty plane
			if(dabs(r_in.direction().y())>epsi)
			{
				n=Pnt(0,nv,0);
				t=(ty-r_in.origin().y())/r_in.direction().y();
				des=r_in.at(t);
				if(t>epsi&&des.x()>=sx&&des.x()<=tx&&des.z()>=sz&&des.z()<=tz)
				{
					hit.update(t,n);
					flag=true;
				}
			}
			//tz plane
			if(dabs(r_in.direction().z())>epsi)
			{
				n=Pnt(0,0,nv);
				t=(tz-r_in.origin().z())/r_in.direction().z();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.x()>=sx&&des.x()<=tx)
				{
					hit.update(t,n);
					flag=true;
				}
			}
			return flag;
		}
		bool easyintersect(const Ray&r_in) const
		{
			double t;
			Pnt des;
			//sx plane
			if(dabs(r_in.direction().x())>epsi)
			{
				t=(sx-r_in.origin().x())/r_in.direction().x();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.z()>=sz&&des.z()<=tz)
				{
					return true;
				}
			}
			//sy plane
			if(dabs(r_in.direction().y())>epsi)
			{
				t=(sy-r_in.origin().y())/r_in.direction().y();
				des=r_in.at(t);
				if(t>epsi&&des.x()>=sx&&des.x()<=tx&&des.z()>=sz&&des.z()<=tz)
				{
					return true;
				}
			}
			//sz plane
			if(dabs(r_in.direction().z())>epsi)
			{
				t=(sz-r_in.origin().z())/r_in.direction().z();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.x()>=sx&&des.x()<=tx)
				{
					return true;
				}
			}
			//tx plane
			if(dabs(r_in.direction().x())>epsi)
			{
				t=(tx-r_in.origin().x())/r_in.direction().x();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.z()>=sz&&des.z()<=tz)
				{
					return true;
				}
			}
			//ty plane
			if(dabs(r_in.direction().y())>epsi)
			{
				t=(ty-r_in.origin().y())/r_in.direction().y();
				des=r_in.at(t);
				if(t>epsi&&des.x()>=sx&&des.x()<=tx&&des.z()>=sz&&des.z()<=tz)
				{
					return true;
				}
			}
			//tz plane
			if(dabs(r_in.direction().z())>epsi)
			{
				t=(tz-r_in.origin().z())/r_in.direction().z();
				des=r_in.at(t);
				if(t>epsi&&des.y()>=sy&&des.y()<=ty&&des.x()>=sx&&des.x()<=tx)
				{
					return true;
				}
			}
			return false;
		}
};

#endif