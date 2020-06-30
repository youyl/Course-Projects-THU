#ifndef TEX_DIEL
#define TEX_DIEL

#include "base.hpp"

int reflcnt,refrcnt;

class Dielectric: public Texture
{
	private:
		double refra;
		Pnt color;
	public:
		Dielectric(){}
		Dielectric(double _n,const Pnt&_p)
		{
			refra=_n;
			color=_p;
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
			Pnt xpnt=r_in.at(hit.t),n=hit.n;
			Pnt nl;
			if(dot(n,r_in.direction())<0)nl=n;
			else nl=n*(-1);
			Ray reflray(xpnt,(r_in.direction()-n*2*dot(n,r_in.direction())).normalized());
			bool into=(dot(n,nl)>0);
			double nc=1,nt=refra;
			double nnt;
			if(into)nnt=nc/nt;
			else nnt=nt/nc;
			double ddn=dot(r_in.direction(),nl);
			double cos2t=1.0-nnt*nnt*(1.0-ddn*ddn);
			if(cos2t<0)
			{
				nxt=reflray;
				return true;
			}
			Pnt tdir;
			if(into)tdir=(r_in.direction()*nnt-n*(ddn*nnt+sqrt(cos2t))).normalized();
			else tdir=(r_in.direction()*nnt-n*((-1)*(ddn*nnt+sqrt(cos2t)))).normalized();
			double a=nt-nc,b=nt+nc;
			double R0=a*a/(b*b);
			double c;
			if(into)c=1-(-ddn);
			else c=1-dot(tdir,n);
			double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=0.25+0.5*Re,RP=Re/P,TP=Tr/(1-P);
			/*if(is_debug)
			{
				cerr<<P<<' '<<into<<' '<<reflcnt<<' '<<refrcnt<<' '<<depth<<' '<<hit.t<<endl;
				cerr<<xpnt.x()<<' '<<xpnt.y()<<' '<<xpnt.z()<<endl;
				cerr<<r_in.origin().x()<<' '<<r_in.origin().y()<<' '<<r_in.origin().z()<<endl;
				cerr<<r_in.direction().x()<<' '<<r_in.direction().y()<<' '<<r_in.direction().z()<<endl;
				cerr<<tdir.x()<<' '<<tdir.y()<<' '<<tdir.z()<<endl;
				debug_cnt++;
				if(debug_cnt==10)is_debug=false;
			}*/
			if(Settings::rand01()<P)
			{
				reflcnt++;
				nxt=reflray;
				col=col*RP;
			}
			else
			{
				refrcnt++;
				nxt=Ray(xpnt,tdir);
				col=col*TP;
			}
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