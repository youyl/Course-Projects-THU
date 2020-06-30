#ifndef TEX_SPH
#define TEX_SPH

#include "base.hpp"
#include <iostream>
#include <fstream>

class SphTexture: public Texture
{
	private:
		int h,w;
		Pnt val[1100][1100];
	public:
		double antigamma(int x)
		{
			return pow(((x*1.0)/255.0),2.2);
		}
		SphTexture(){}
		SphTexture(const string&filename)
		{
			ifstream fffin(filename.c_str());
			fffin>>h>>w;
			int x1,x2,x3;
			for (int i=0;i<h;i++)
			{
				for (int j=0;j<w;j++)
				{
					fffin>>x1;
					fffin>>x2;
					fffin>>x3;
					val[i][j]=Pnt(clamp(antigamma(x1)),clamp(antigamma(x2)),clamp(antigamma(x3)));
				}
			}
			cerr<<filename<<' '<<h<<' '<<w<<endl;
			fffin.close();
		}
		bool is_diffusion() const
		{
			return true;
		}
		Pnt getPosCol(const Pnt&nv) const
		{
			Pnt nm=nv.normalized();
			double phi=atan2(nm.y(),nm.x());
			double theta=asin(nm.z());
			double pi=3.1415926535897932384626;
			double v=1-(phi+pi)/(2*pi);
			double u=(theta+pi/2)/pi;
			int pu=(int)((1-u)*h);
			int pv=(int)((1-v)*w);
			if(pu>=h)pu=h-1;
			if(pu<0)pu=0;
			if(pv>=w)pv=w-1;
			if(pv<0)pv=0;
			return val[pu][pv];
		}
		bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &col,int depth) const
		{
			Pnt color=getPosCol(hit.n);
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
			Pnt w=hit.n,u,v;
			if(dabs(w.x())>0.1)u=cross(Pnt(0,1,0),w).normalized();
			else u=cross(Pnt(1,0,0),w).normalized();
			v=cross(w,u);
			Pnt des=(u*cos(r1)*r2s+v*sin(r1)*r2s+w*sqrt(1-r2)).normalized();
			nxt=Ray(r_in.at(hit.t),des);
			return true;
		}
		Pnt emit() const
		{
			return Pnt(0,0,0);
		}
		Pnt getcolor() const
		{
			return Pnt(0,0,0);
		}
};

#endif