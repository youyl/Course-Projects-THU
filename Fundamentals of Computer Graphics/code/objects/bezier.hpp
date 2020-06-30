#ifndef OBJ_BEZ
#define OBJ_BEZ

#include "base.hpp"
#include "sphere.hpp"
#include <vector>
#include <utility>
#include "aacube.hpp"

const int newtontries=200;
const double theta0=1;

int hit_in_box=0;
int hit_in_curve=0;

class Bezier: public Object
{
	private:
		AAcube*aabb;
		int n;
		vector<pair<double,double> >pos;
		double qx[1200],qy[1200],qx2[1200],c[200][200];
	public:
		inline double Y(double x) const
		{
			double ss=0,mm=1;
			for (int i=0;i<=n;i++)
			{
				ss+=qy[i]*mm;
				mm=mm*x;
			}
			return ss;
		}
		inline double X(double x) const
		{
			double ss=0,mm=1;
			for (int i=0;i<=n;i++)
			{
				ss+=qx[i]*mm;
				mm=mm*x;
			}
			return ss;
		}
		inline double Yp(double x) const
		{
			double ss=0,mm=1;
			for (int i=1;i<=n;i++)
			{
				ss+=qy[i]*mm*i;
				mm=mm*x;
			}
			return ss;
		}
		inline double Xp(double x) const
		{
			double ss=0,mm=1;
			for (int i=1;i<=n;i++)
			{
				ss+=qx[i]*mm*i;
				mm=mm*x;
			}
			return ss;
		}
		inline double G(double x,const vector<double>&qg) const
		{
			double ss=0,mm=1;
			for (int i=0;i<=n*2;i++)
			{
				ss+=qg[i]*mm;
				mm=mm*x;
			}
			return ss;
		}
		inline double Gp(double x,const vector<double>&qg) const
		{
			double ss=0,mm=1;
			for (int i=1;i<=n*2;i++)
			{
				ss+=qg[i]*mm*i;
				mm=mm*x;
			}
			return ss;
		}
		inline double solveG(double x0,const vector<double>&qg) const
		{
			double theta;
			for (int id=1;id<=50;id++)
			{
				if(x0<0||x0>1)return -1;
				double g=G(x0,qg),gp=Gp(x0,qg);
				if(dabs(g)<epsi)return x0;
				x0=x0-g/(gp+(1e-10)*g);
			}
			return -1;
		}
		inline bool newton(Hit&hit,const Ray&r_in,const vector<double>&qg) const
		{
			bool flag=false;
			hit.init();
			for (int id=0;id<newtontries;id++)
			{
			//	if(flag&&id>40)break;
				double t=solveG(Settings::rand01(),qg);
				if(t<0||t>1)continue;
				Hit ht;
				ht.t=(Y(t)-r_in.origin().z())/r_in.direction().z();
				Pnt des=r_in.at(ht.t);
				double xbias=des.x(),ybias=des.y();
				double costheta=xbias/sqrt(xbias*xbias+ybias*ybias);
				double sintheta=ybias/sqrt(xbias*xbias+ybias*ybias);
				Pnt gre(costheta,sintheta,Yp(t)/Xp(t));
				Pnt clk(-sintheta,costheta,0);
				ht.n=cross(clk,gre).normalized();
				if(ht.t<epsi)continue;
				if(ht.t<hit.t)
				{
					hit=ht;
					flag=true;
				}
			}
			//hit_in_curve+=flag;
			return flag;
		}
		Bezier(){}
		Bezier(const Pnt&_p1,const Pnt&_p2,const vector<pair<double,double> >&_pos,Texture *_tex)
		{
			pos=_pos;
			n=pos.size()-1;
			texture=_tex;
			aabb=new AAcube(_p1,_p2,1,_tex);
			for (int i=0;i<15;i++)
			{
				for (int j=0;j<15;j++)
				{
					c[i][j]=0;
				}
			}
			for (int i=0;i<15;i++)
			{
				c[i][0]=1;
				for (int j=1;j<=i;j++)
				{
					c[i][j]=c[i-1][j-1]+c[i-1][j];
				}
			}
			for (int i=0;i<100;i++)
			{
				qx[i]=0;
				qy[i]=0;
				qx2[i]=0;
			}
			for (int i=0;i<=n;i++)
			{
				double f=c[n][i];
				for (int j=0;j<=n-i;j++)
				{
					if(j%2==0)
					{
						qx[i+j]+=pos[i].first*f*c[n-i][j];
						qy[i+j]+=pos[i].second*f*c[n-i][j];
					}
					else
					{
						qx[i+j]-=pos[i].first*f*c[n-i][j];
						qy[i+j]-=pos[i].second*f*c[n-i][j];
					}
				}
			}
			for (int i=0;i<=n;i++)
			{
				for (int j=0;j<=n;j++)
				{
					qx2[i+j]+=qx[i]*qx[j];
				}
			}
		}
		bool dz0(const Ray&r_in,Hit&hit) const
		{
			vector<double>qg;
			qg.resize(2*n+1);
			for (int i=0;i<=2*n;i++)
			{
				qg[i]=0;
			}
			for (int i=0;i<=n;i++)
			{
				qg[i]=qy[i];
			}
			hit.init();
			bool flag=false;
			qg[0]-=r_in.origin().z();
			for (int id=0;id<20;id++)
			{
				Hit ht;
				double t=solveG(Settings::rand01(),qg);
				if(t<0||t>1)continue;
				Sphere spp(Pnt(0,0,Y(t)),X(t),texture);
				if(spp.intersect(r_in,ht))
				{
					Pnt des=r_in.at(ht.t);
					double xbias=des.x(),ybias=des.y();
					double costheta=xbias/sqrt(xbias*xbias+ybias*ybias);
					double sintheta=ybias/sqrt(xbias*xbias+ybias*ybias);
					Pnt gre(costheta,sintheta,Yp(t)/Xp(t));
					Pnt clk(-sintheta,costheta,0);
					ht.n=cross(clk,gre).normalized();
					if(ht.t<epsi)continue;
					if(ht.t<hit.t)
					{
						hit=ht;
						flag=true;
					}
				}
			}/*
			if(flag){
			cerr<<"rin: "<<r_in.origin().x()<<' '<<r_in.origin().y()<<' '<<r_in.origin().z()<<endl;
			cerr<<"     "<<r_in.direction().x()<<' '<<r_in.direction().y()<<' '<<r_in.direction().z()<<endl;
			cerr<<"des: "<<r_in.at(hit.t).x()<<' '<<r_in.at(hit.t).y()<<' '<<r_in.at(hit.t).z()<<endl;
			cerr<<"dis to axis: "<<r_in.at(hit.t).x()*r_in.at(hit.t).x()+r_in.at(hit.t).y()*r_in.at(hit.t).y()<<endl;
			cerr<<"htn: "<<hit.n.x()<<' '<<hit.n.y()<<' '<<hit.n.z()<<' '<<dot(hit.n,r_in.direction())<<endl;}//*/
			hit_in_curve+=flag;
			return flag;
		}
		bool intersect(const Ray&r_in,Hit&hit) const
		{
			if(!aabb->easyintersect(r_in))return false;
			hit_in_box++;
			if(dabs(r_in.direction().z())<1e-4)//epsi)
			{
				if(dz0(r_in,hit))return true;
				return false;
			}
			vector<double>tp,qt2,qg;
			tp.resize(2*n+1);
			qt2.resize(2*n+1);
			qg.resize(2*n+1);
			for (int i=0;i<=n;i++)
			{
				tp[i]=qy[i];
			}
			tp[0]-=r_in.origin().z();
			for (int i=0;i<=n;i++)
			{
				tp[i]/=r_in.direction().z();
			}
			for (int i=0;i<=2*n;i++)
			{
				qt2[i]=0;
			}
			for (int i=0;i<=n;i++)
			{
				for (int j=0;j<=n;j++)
				{
					qt2[i+j]+=tp[i]*tp[j];
				}
			}
			for (int i=0;i<=n*2;i++)
			{
				qg[i]=qx2[i];
			}
			for (int i=0;i<=n*2;i++)
			{
				qg[i]-=(r_in.direction().x()*r_in.direction().x())*qt2[i];
				qg[i]-=(r_in.direction().y()*r_in.direction().y())*qt2[i];
			}
			for (int i=0;i<=n;i++)
			{
				qg[i]-=(2*r_in.origin().x()*tp[i]*r_in.direction().x());
				qg[i]-=(2*r_in.origin().y()*tp[i]*r_in.direction().y());
			}
			qg[0]-=r_in.origin().x()*r_in.origin().x();
			qg[0]-=r_in.origin().y()*r_in.origin().y();
			if(newton(hit,r_in,qg))return true;
			return false;
		}
};

#endif