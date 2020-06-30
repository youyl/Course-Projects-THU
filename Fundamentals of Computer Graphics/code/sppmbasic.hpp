#ifndef SPPM_BAS
#define SPPM_BAS

#include "basic/basic_include.h"

int sppmsortidx=0;
int photonhit;
int queryhit;
double queryhit2;

class Hitpoint
{
	public:
		int idh,idw;
		double rad;
		Pnt pos,col,normal;
		bool flag;
		bool operator<(const Hitpoint&ht)
		{
			return pos.p(sppmsortidx)<ht.pos.p(sppmsortidx);
		}
};

class RoundLight
{
	private:
		Pnt pos,n,d1,d2;
		double r;
	public:
		RoundLight(){}
		RoundLight(const Pnt&_p1,double rad)
		{
			r=rad;
			pos=_p1;
			n=Pnt(0,0,-1);
			d1=Pnt(1,0,0);
			d2=Pnt(0,1,0);
		}
		inline Ray randomray()
		{
			double r1=Settings::rand01()*2*3.1415926535897932384626;
			double r2=Settings::rand01()*0.99;
			double r2s=sqrt(r2);
			Pnt des=(d1*cos(r1)*r2s+d2*sin(r1)*r2s+n*sqrt(1-r2)).normalized();
			r1=Settings::rand01()*2*3.1415926535897932384626;
			r2=Settings::rand01()*r;
			Pnt sta=pos+Pnt(r2*cos(r1),r2*sin(r1),0)+n*epsi;
			return Ray(sta,des);
		}
};

class kdtree
{
	private:
		int lc[1000000];
		int rc[1000000];
		int n,root;
		Hitpoint node[1000000];
		double lb[1000000][3],ub[1000000][3];
	public:
		double pic[900][1300][3];
		kdtree()
		{
			for (int i=0;i<850;i++)
			{
				for (int j=0;j<1250;j++)
				{
				//	phcnt[i][j]=0;
					for (int k=0;k<3;k++)
					{
						pic[i][j][k]=0;
					}
				}
			}
		}
		inline void update(int x)
		{
			for (int i=0;i<3;i++)
			{
				if(lc[x]!=-1)lb[x][i]=min(lb[x][i],lb[lc[x]][i]);
				if(lc[x]!=-1)ub[x][i]=max(ub[x][i],ub[lc[x]][i]);
				if(rc[x]!=-1)lb[x][i]=min(lb[x][i],lb[rc[x]][i]);
				if(rc[x]!=-1)ub[x][i]=max(ub[x][i],ub[rc[x]][i]);
			}
		}
		inline int build(int le,int ri,int dim)
		{
			int mi=(le+ri)/2;
			sppmsortidx=dim;
			nth_element(node+le,node+mi,node+ri+1);
			for (int i=0;i<3;i++)
			{
				lb[mi][i]=node[mi].pos.p(i)-node[mi].rad;
				ub[mi][i]=node[mi].pos.p(i)+node[mi].rad;
			}
			lc[mi]=-1;
			rc[mi]=-1;
			if(le<mi)lc[mi]=build(le,mi-1,(dim+1)%3);
			if(mi<ri)rc[mi]=build(mi+1,ri,(dim+1)%3);
			update(mi);
			return mi;
		}
		inline void init(const vector<Hitpoint>&vc)
		{
			n=vc.size();
			for (int i=0;i<n;i++)
			{
				node[i+1]=vc[i];
			}
			root=-1;
			root=build(1,n,0);
			cerr<<root<<endl;
			cerr<<lb[root][0]<<' '<<lb[root][1]<<' '<<lb[root][2]<<endl;
			cerr<<ub[root][0]<<' '<<ub[root][1]<<' '<<ub[root][2]<<endl;
		}
		inline bool withinbox(int x,const Pnt&p) const
		{
			for (int i=0;i<3;i++)
			{
				if(p.p(i)<lb[x][i])return false;
				if(p.p(i)>ub[x][i])return false;
			}return true;
		}
		inline void query(int x,const Pnt&p,const Pnt&c,const Pnt&nn)
		{
			queryhit2++;
			double dx=(p.x()-node[x].pos.x())*(p.x()-node[x].pos.x());
			double dy=(p.y()-node[x].pos.y())*(p.y()-node[x].pos.y());
			double dz=(p.z()-node[x].pos.z())*(p.z()-node[x].pos.z());
			if(node[x].rad*node[x].rad>dx+dy+dz&&dot(nn,node[x].normal)>0)
			{
				photonhit++;//cerr<<node[x].idh<<' '<<node[x].idw<<endl;
				pic[node[x].idh][node[x].idw][0]+=c.x()*node[x].col.x();
				pic[node[x].idh][node[x].idw][1]+=c.y()*node[x].col.y();
				pic[node[x].idh][node[x].idw][2]+=c.z()*node[x].col.z();
		//		phcnt[node[x].idh][node[x].idw]+=1;
			}
			if(lc[x]!=-1)
			{
				if(withinbox(lc[x],p))
				{
					query(lc[x],p,c,nn);
				}
			}
			if(rc[x]!=-1)
			{
				if(withinbox(rc[x],p))
				{
					query(rc[x],p,c,nn);
				}
			}
		}
		inline void degrade(double d)
		{
			for (int i=0;i<850;i++)
			{
				for (int j=0;j<1250;j++)
				{
					for (int k=0;k<3;k++)
					{
						pic[i][j][k]*=(d*d);
					}
			//		phcnt[i][j]*=(d*d);
				}
			}
		}
		inline void query(const Pnt&p,const Pnt&c,const Pnt&nn)
		{
			queryhit++;
			query(root,p,c,nn);
		}
};

#endif