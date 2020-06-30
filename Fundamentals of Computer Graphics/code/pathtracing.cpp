#include "basic/basic_include.h"
#include "texture/texture_include.hpp"
#include "objects/obj_include.h"
#include "parser.hpp"
#include <ctime>

using namespace std;

double pic[900][1300][3];//max size 1200*800;
string modelfile="models/bztest.txt";
string outputfile="test.ppm";
int samplecnt=500;
int aacnt=1;

Pnt radiance(const Ray&r_in,int level,Scene*world)
{
	Hit tmphit;
	int idx;
	if(!world->intersect(r_in,tmphit,idx))return Pnt(0,0,0);
	Pnt debug=r_in.at(tmphit.t);
	Texture*texture=world->objectlist(idx)->texture;
	Ray nxt;
	Pnt col;
	if(level>20000)return texture->emit();
	if(!texture->radiance(r_in,tmphit,nxt,col,level))return texture->emit();
	/*if(is_debug)cerr<<"debugidx: "<<idx<<' '<<tmphit.t<<endl;
	if(dabs(r_in.direction().z())<epsi&&idx==11)
	{
		cerr<<"dot res: "<<dot(tmphit.n,nxt.direction())<<' '<<dot(r_in.direction(),tmphit.n)<<endl;
		if(idx==11)is_debug=true;
		cerr<<"nxt ray: "<<nxt.direction().x()<<' '<<nxt.direction().y()<<' '<<nxt.direction().z()<<' '<<idx<<endl;
		cerr<<"distance show: "<<(nxt.origin()*nxt.origin()).x()+(nxt.origin()*nxt.origin()).y()<<' '<<0.10057*0.10057<<endl;
	}//*/
//	if(level==1)return texture->getcolor();
	return texture->emit()+col*radiance(nxt,level+1,world);
}

int main()
{
//	is_debug=true;
//-----------AA------------------
	Settings::AA=true;aacnt=4;
//-----------DOF-----------------
	Settings::DOF=true;
//-------------------------------
	srand(time(0));
	Scene *world;
	world=new Scene(modelfile);
	int x=world->x();
	int y=world->y();
	for (int i=0;i<x;i++)
	{
		#pragma omp parallel for schedule(dynamic, 1)
		for (int j=0;j<y;j++)
		{
			for (int k=0;k<samplecnt;k++)
			{
				if(!Settings::AA)
				{
					Ray ray=world->camera()->getRay(i,j);
					Pnt tmpres=radiance(ray,0,world);
					pic[i][j][0]+=clamp(tmpres.x());
					pic[i][j][1]+=clamp(tmpres.y());
					pic[i][j][2]+=clamp(tmpres.z());
					is_debug=false;
				}
				else
				{
					for (int bi=0;bi<2;bi++)
					{
						for (int bj=0;bj<2;bj++)
						{
							Ray ray=world->camera()->getRay(i,j,bi*0.5,bj*0.5);
							Pnt tmpres=radiance(ray,0,world);
							pic[i][j][0]+=clamp(tmpres.x());
							pic[i][j][1]+=clamp(tmpres.y());
							pic[i][j][2]+=clamp(tmpres.z());
						}
					}
				}
			}
		}cerr<<"Complete "<<i<<" lines"<<endl;
	}
	ofstream fout(outputfile.c_str());
	fout<<"P3\n"<<y<<' '<<x<<'\n'<<255<<'\n';
	for (int i=x-1;i>=0;i--)
	{
		for (int j=0;j<y;j++)
		{
			fout<<toInt(pic[i][j][0]/(samplecnt*aacnt))<<' ';
			fout<<toInt(pic[i][j][1]/(samplecnt*aacnt))<<' ';
			fout<<toInt(pic[i][j][2]/(samplecnt*aacnt))<<' ';
		}
	}
	fout.close();
	cerr<<debug_cnt<<endl;
	cerr<<hit_in_curve<<' '<<hit_in_box<<endl;
	return 0;
}