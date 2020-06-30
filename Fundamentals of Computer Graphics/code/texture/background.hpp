#ifndef TEX_BG
#define TEX_BG

#include "base.hpp"
#include <iostream>
#include <fstream>

class BackGround: public Texture
{
	private:
		int h,w;
		Pnt val[1000][1300];
	public:
		double antigamma(int x)
		{
			return pow(((x*1.0)/255.0),2.2);
		}
		bool is_diffusion() const
		{
			return true;
		}
		BackGround()
		{
			ifstream fffin("models/bkg.txt");
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
					if(val[i][j].x()>1||val[i][j].x()<0)cerr<<"Error X"<<endl;
					if(val[i][j].y()>1||val[i][j].y()<0)cerr<<"Error Y"<<endl;
					if(val[i][j].z()>1||val[i][j].z()<0)cerr<<"Error Z"<<endl;
				}
			}
			cerr<<h<<' '<<w<<endl;
			fffin.close();
		}
		Pnt getPosCol(const Pnt&nv) const
		{
			double vv=nv.y()+5,hh=nv.z();
			int ph,pw;
			ph=(int)(hh*170.0)+3;
			pw=(int)(vv*170.0)+3;
			if(ph>=h)ph=h-1;
			if(ph<0)ph=3;
			if(pw>=w)pw=w-1;
			if(pw<0)pw=3;
			return val[ph][pw];
		}
		bool radiance(const Ray&r_in,const Hit&hit,Ray &nxt,Pnt &col,int depth) const
		{
			Pnt color=getPosCol(r_in.at(hit.t));
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