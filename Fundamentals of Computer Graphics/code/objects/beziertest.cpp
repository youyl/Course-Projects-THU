#include "obj_include.h"
#include <iostream>
#include <cstdio>

using namespace std;

Bezier *bz;

//BEZ 0 -0.2 -0.2 0 0.2 0.2 1.5 5 0 1.5 0.2 1.4 -0.2 1 0.5 0.3 0 -0.2
//BEZ 0 1.5 0.35 1.4 -0.35 1 0.55 0.3 0 -0.2
int main()
{
	//freopen("bztmp.txt","w",stdout);
	Texture*tex;
	vector<pair<double,double> >vec;
	vec.push_back(make_pair(0,1.5));
	vec.push_back(make_pair(0.35,1.4));
	vec.push_back(make_pair(-0.35,1));
	vec.push_back(make_pair(0.55,0.3));
	vec.push_back(make_pair(0,-0.2));
	bz=new Bezier(Pnt(-0.5,-0.5,-1),Pnt(0.5,0.5,1.5),vec,tex);
	Hit hit;
	Pnt pp(4.5,-0.5,1.2);
	Ray into(pp,Pnt(-0.995856,0.0908718,-0.00373446));
	cout<<bz->intersect(into,hit)<<endl;
	cout<<hit.t<<endl;
	cout<<hit.n.x()<<' '<<hit.n.y()<<' '<<hit.n.z()<<endl;
	Pnt des=into.at(hit.t);
	cout<<des.x()<<' '<<des.y()<<' '<<des.z()<<endl;
	cout<<dot(hit.n,into.direction())<<endl;
	return 0;
}
/*origin: 4.5 -0.5 1.2
direct: -0.995856 0.0908718 -0.00373446*/
