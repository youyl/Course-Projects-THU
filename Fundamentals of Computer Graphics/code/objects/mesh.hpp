#ifndef OBJ_MESH
#define OBJ_MESH

#include "base.hpp"
#include "triangle.hpp"
#include "aacube.hpp"
#include <iostream>
#include <fstream>

class Mesh: public Object
{
	private:
		int f,v,cnt;
		Pnt core;
		Pnt point[10000];
		Triangle face[10000];
		AAcube aabb[50000];
		int lef[50000];
		int rig[50000];
		int lc[50000];
		int rc[50000];
		double flowf[10000][3],fhighf[10000][3];
		double blowb[10000][3],bhighb[10000][3];
		double xdiv,ydiv,zdiv;
		inline double sah(int le,int ri,int x)
		{
			double f01=(fhighf[x+1][0]-flowf[x+1][0])*(fhighf[x+1][1]-flowf[x+1][1]);
			double f02=(fhighf[x+1][0]-flowf[x+1][0])*(fhighf[x+1][2]-flowf[x+1][2]);
			double f12=(fhighf[x+1][2]-flowf[x+1][2])*(fhighf[x+1][1]-flowf[x+1][1]);
			double b01=(bhighb[x][0]-blowb[x][0])*(bhighb[x][1]-blowb[x][1]);
			double b02=(bhighb[x][0]-blowb[x][0])*(bhighb[x][2]-blowb[x][2]);
			double b12=(bhighb[x][2]-blowb[x][2])*(bhighb[x][1]-blowb[x][1]);
			return (b01+b02+b12)*(x-le+1)+(f01+f02+f12)*(ri-x);
		}
		inline void calcdiv(int le,int ri)
		{
			double sx=0,sy=0,sz=0;
			xdiv=0;ydiv=0;zdiv=0;
			for (int i=le;i<=ri;i++)
			{
				for (int j=0;j<3;j++)
				{
					sx+=face[i].node(j).x();
					sy+=face[i].node(j).y();
					sz+=face[i].node(j).z();
				}
			}
			sx=sx/(3.0*(ri-le+1));
			sy=sy/(3.0*(ri-le+1));
			sz=sz/(3.0*(ri-le+1));
			for (int i=le;i<=ri;i++)
			{
				for (int j=0;j<3;j++)
				{
					xdiv+=(face[i].node(j).x()-sx)*(face[i].node(j).x()-sx);
					ydiv+=(face[i].node(j).y()-sy)*(face[i].node(j).y()-sy);
					zdiv+=(face[i].node(j).z()-sz)*(face[i].node(j).z()-sz);
				}
			}
		}
		inline void calcbound(int le,int ri)
		{
			for (int i=0;i<3;i++)
			{
				blowb[le][i]=min(face[le].node(0).p(i),min(face[le].node(1).p(i),face[le].node(2).p(i)));
				bhighb[le][i]=max(face[le].node(0).p(i),max(face[le].node(1).p(i),face[le].node(2).p(i)));
				flowf[ri][i]=min(face[ri].node(0).p(i),min(face[ri].node(1).p(i),face[ri].node(2).p(i)));
				fhighf[ri][i]=max(face[ri].node(0).p(i),max(face[ri].node(1).p(i),face[ri].node(2).p(i)));
			}
			for (int i=le+1;i<=ri;i++)
			{
				for (int j=0;j<3;j++)
				{
					for (int k=0;k<3;k++)
					{
						blowb[i][j]=min(blowb[i-1][j],face[i].node(k).p(j));
						bhighb[i][j]=max(bhighb[i-1][j],face[i].node(k).p(j));
					}
				}
			}
			for (int i=ri-1;i>=le;i--)
			{
				for (int j=0;j<3;j++)
				{
					for (int k=0;k<3;k++)
					{
						flowf[i][j]=min(flowf[i+1][j],face[i].node(k).p(j));
						fhighf[i][j]=max(fhighf[i+1][j],face[i].node(k).p(j));
					}
				}
			}
		}
		inline AAcube calcbox(int le,int ri)
		{
			double mix=10000,miy=10000,miz=10000;
			double mx=-10000,my=-10000,mz=-10000;
			for (int i=le;i<=ri;i++)
			{
				for (int j=0;j<3;j++)
				{
					mix=min(mix,face[i].node(j).x());
					miy=min(miy,face[i].node(j).y());
					miz=min(miz,face[i].node(j).z());
					mx=max(mx,face[i].node(j).x());
					my=max(my,face[i].node(j).y());
					mz=max(mz,face[i].node(j).z());
				}
			}
		//	if(le==1)cerr<<mix<<'-'<<mx<<' '<<miy<<'-'<<my<<' '<<miz<<'-'<<mz<<endl;
			return AAcube(Pnt(mix,miy,miz),Pnt(mx,my,mz),1,texture);
		}
		inline void initialize()
		{
			cnt=1;
			lef[1]=1;
			rig[1]=f;
			int cur=0;
			while (cur<cnt)
			{
				cur++;
				lc[cur]=-1;
				rc[cur]=-1;
				calcdiv(lef[cur],rig[cur]);
				aabb[cur]=calcbox(lef[cur],rig[cur]);
			//	cerr<<rig[cur]-lef[cur]+1<<endl;
				if(lef[cur]==rig[cur])continue;
				if(xdiv>ydiv&&xdiv>zdiv)
				{
					Settings::tricmp=0;
					sort(face+lef[cur],face+rig[cur]+1,cmptriangle);
				}
				else if(ydiv>xdiv&&ydiv>zdiv)
				{
					Settings::tricmp=1;
					sort(face+lef[cur],face+rig[cur]+1,cmptriangle);
				}
				else
				{
					Settings::tricmp=2;
					sort(face+lef[cur],face+rig[cur]+1,cmptriangle);
				}
				calcbound(lef[cur],rig[cur]);
				int res=lef[cur];
				for (int i=lef[cur]+1;i<rig[cur];i++)
				{
					if(sah(lef[cur],rig[cur],i)<sah(lef[cur],rig[cur],res))res=i;
				}
			//	cerr<<lef[cur]<<' '<<res<<' '<<rig[cur]<<endl;
				cnt++;
				lc[cur]=cnt;
				lef[cnt]=lef[cur];
				rig[cnt]=res;
				cnt++;
				rc[cur]=cnt;
				lef[cnt]=res+1;
				rig[cnt]=rig[cur];
			}
		}
	public:
		Mesh(){}
		Mesh(double biasx,double biasy,double biasz,double scalee,const string &filename,Texture *_tex)
		{
			texture=_tex;
			ifstream ffin(filename.c_str());
			string str;
			int n,v;
			double x1,x2,x3;
			int v1,v2,v3;
			ffin>>str>>v>>f;
			core=Pnt(0,0,0);
			for (int i=1;i<=v;i++)
			{
				ffin>>str>>x1>>x2>>x3;
				Pnt tmpnt(x1*scalee+biasx,x2*scalee+biasy,x3*scalee+biasz);
				point[i]=tmpnt;
				core=core+tmpnt;
			}
			core=core/v;
			cerr<<core.x()<<' '<<core.y()<<' '<<core.z()<<endl;
			for (int i=1;i<=f;i++)
			{
				ffin>>str>>v1>>v2>>v3;
				Triangle tmptri(point[v1],point[v2],point[v3],texture);
				face[i]=tmptri;
			}
			ffin.close();
			cnt=0;
			initialize();
		}
		bool intersect(int cur,const Ray&r_in,Hit&hit) const
		{
			if(lef[cur]==rig[cur])
			{
				return face[lef[cur]].intersect(r_in,hit);
			}
			bool fc1=false;
			bool fc2=false;
			if(lc[cur]!=-1)fc1=aabb[lc[cur]].easyintersect(r_in);
			if(rc[cur]!=-1)fc2=aabb[rc[cur]].easyintersect(r_in);
			if(fc1&&fc2)
			{
				Hit ht;
				hit.init();
				bool flag=false;
				if(intersect(lc[cur],r_in,ht))
				{
					hit=ht;
					flag=true;
				}
				if(intersect(rc[cur],r_in,ht))
				{
					if(ht.t<hit.t)
					{
						hit=ht;
						flag=true;
					}
				}
				return flag;
			}
			if(fc1)return intersect(lc[cur],r_in,hit);
			if(fc2)return intersect(rc[cur],r_in,hit);
			return false;
		}
		bool intersect(const Ray&r_in,Hit&hit) const
		{
			return intersect(1,r_in,hit);
		}
};

#endif