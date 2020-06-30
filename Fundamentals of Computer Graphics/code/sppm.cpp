#include "basic/basic_include.h"
#include "texture/texture_include.hpp"
#include "objects/obj_include.h"
#include "parser.hpp"
#include "sppmbasic.hpp"
#include <ctime>

using namespace std;

const int iternum=100;
string modelfile="models/sppmtest.txt";
string outputfile="test.ppm";
const double drate=0.968;
const double illu=1;
const double r0=0.05;
//max size 1200*800;
Scene world;
kdtree tree;
vector<Hitpoint>pt;
int n,m;
RoundLight rdlight(Pnt(3,-3,4),0.3);

inline Hitpoint eyepass(const Ray&r_in,int depth,Pnt colbefore)
{
	Hit tmphit;
	int idx;
	Hitpoint hh;
	if(depth>20000)
	{
		hh.flag=false;
		return hh;
	}
	if(!world.intersect(r_in,tmphit,idx))
	{
		hh.flag=false;
		return hh;
	}
	Texture*texture=world.objectlist(idx)->texture;
	if(texture->is_diffusion())
	{
		hh.flag=true;
		Ray nxt;
		Pnt col;
		if(!texture->radiance(r_in,tmphit,nxt,col,depth))
		{
			hh.flag=false;
			return hh;
		}
		hh.col=col*colbefore;
		hh.pos=nxt.origin();
		Pnt n1=tmphit.n;
		if(dot(tmphit.n,r_in.direction())>0)
		{
			n1=n1*(-1);
		}
		hh.normal=n1;
		return hh;
	}
	else
	{
		Ray nxt;
		Pnt col;
		if(!texture->radiance(r_in,tmphit,nxt,col,depth))
		{
			hh.flag=false;
			return hh;
		}
		return eyepass(nxt,depth+1,colbefore*col);
	}
}

inline void photonpass(const Ray&r_in,int depth,Pnt colbefore)
{
	Hit tmphit;
	int idx;
	if(depth>20000)return;
	if(!world.intersect(r_in,tmphit,idx))return;
	Texture*texture=world.objectlist(idx)->texture;
	if(texture->is_diffusion())
	{
		Ray nxt;
		Pnt col;
		Pnt n1=tmphit.n;
		if(dot(tmphit.n,r_in.direction())>0)
		{
			n1=n1*(-1);
		}
		if(!texture->radiance(r_in,tmphit,nxt,col,depth))
		{
			tree.query(nxt.origin(),colbefore,n1);
			return;
		}
		tree.query(nxt.origin(),colbefore,n1);
		photonpass(nxt,depth+1,colbefore*col);
	}
	else
	{
		Ray nxt;
		Pnt col;
		if(!texture->radiance(r_in,tmphit,nxt,col,depth))return;
		photonpass(nxt,depth+1,colbefore*col);
	}
}

int main()
{
	srand(time(0));
	Settings::is_Sppm=true;
//---------DOF------------
	Settings::DOF=true;
//------------------------
	world=Scene(modelfile);
	n=world.x();
	m=world.y();
	int photoncnt=n*m*5;
	double globalradius=r0;
	pt.resize(n*m);
	int iter=iternum;
	while (iter--)
	{
		queryhit=0;
		queryhit2=0;
		photonhit=0;
		globalradius*=drate;
		for (int i=0;i<n;i++)
		{
			#pragma omp parallel for schedule(dynamic, 1)
			for (int j=0;j<m;j++)
			{
				Ray ray=world.camera()->getRay(i,j);
				pt[i*m+j]=eyepass(ray,0,Pnt(1,1,1));
				pt[i*m+j].idh=i;
				pt[i*m+j].idw=j;
				pt[i*m+j].rad=globalradius;
			}
		}
		int trueptcnt=0;
		vector<Hitpoint>truept;
		for (int i=0;i<pt.size();i++)
		{
			if(pt[i].flag)
			{
				truept.push_back(pt[i]);
				trueptcnt++;
			}
		}
		cerr<<"True hit point number on this iteration: "<<trueptcnt<<endl;
		tree.init(truept);
		cerr<<"KDT built"<<endl;
		for (int fff=1;fff<=2;fff++)
		{
			#pragma omp parallel for schedule(dynamic, 1)
			for (int i=1;i<=photoncnt;i++)
			{
				Ray ray=rdlight.randomray();
				tree.query(ray.origin(),Pnt(1,1,1),Pnt(0,0,-1));
				photonpass(ray,0,Pnt(1,1,1));
			}
			cerr<<"Photon part "<<fff<<" complete"<<endl;
		}
		if(iter!=0)tree.degrade(drate);
		cerr<<"Iteration "<<iter<<" remaining"<<endl;
		cerr<<photonhit<<' '<<globalradius<<endl;
	}
	ofstream fout(outputfile.c_str());
	fout<<"P3\n"<<m<<' '<<n<<'\n'<<255<<'\n';
	for (int i=n-1;i>=0;i--)
	{
		for (int j=0;j<m;j++)
		{
			double denot=(1.0*photoncnt*iternum*globalradius*globalradius/illu);
			fout<<toInt(tree.pic[i][j][0]/denot)<<' ';
			fout<<toInt(tree.pic[i][j][1]/denot)<<' ';
			fout<<toInt(tree.pic[i][j][2]/denot)<<' ';//*/
		}
	}
	fout.close();
	return 0;
}