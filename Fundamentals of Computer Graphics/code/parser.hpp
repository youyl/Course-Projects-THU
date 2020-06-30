#ifndef PARSER
#define PARSER

#include "basic/basic_include.h"
#include "objects/obj_include.h"
#include "texture/texture_include.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class Scene
{
	private:
		vector<Object*>objects;
		vector<Texture*>textures;
		Camera*cam;
		int px,py;
	public:
		Scene(){}
		Scene(const string&filename)
		{
			double x1,x2,x3,x4,x5,x6,x7,x8,x9;
			int xx1,xx2;
			ifstream fin(filename.c_str());
			cam=new Camera();
			fin>>x1>>x2>>x3;
			fin>>x4>>x5>>x6;
			fin>>x7>>x8>>x9;
			cam->setPos(Pnt(x1,x2,x3),Pnt(x4,x5,x6),Pnt(x7,x8,x9));
			fin>>x1>>x2;
			cam->setAngle(x1,x2);
			fin>>xx1>>xx2;
			px=xx1;py=xx2;
			cam->setPixel(xx1,xx2);
			fin>>x1>>x2;
			cam->setFocus(x1);
			cam->setRadius(x2);
			int texcnt;
			fin>>texcnt;
			string str;
			cerr<<texcnt<<" Textures Detected"<<endl; 
			while (texcnt--)
			{
				fin>>str;
				if(str=="SPECULAR")
				{
					fin>>x1>>x2>>x3;
					Texture*tex=new Specular(Pnt(x1,x2,x3));
					textures.push_back(tex);
				}
				else if(str=="DIELECTRIC")
				{
					fin>>x1>>x2>>x3>>x4;
					Texture*tex=new Dielectric(x4,Pnt(x1,x2,x3));
					textures.push_back(tex);
				}
				else if(str=="LIGHT")
				{
					fin>>x1>>x2>>x3;
					Texture*tex=new Light(Pnt(x1,x2,x3));
					textures.push_back(tex);
				}
				else if(str=="DIFFUSION")
				{
					fin>>x1>>x2>>x3;
					Texture*tex=new Diffusion(Pnt(x1,x2,x3));
					textures.push_back(tex);
				}
				else if(str=="SPHERE")
				{
					fin>>str;
					Texture*tex=new SphTexture(str);
					textures.push_back(tex);
				}
				else if(str=="GRADIENT")
				{
					Texture*tex=new VerGradient();
					textures.push_back(tex);
				}
				else if(str=="BACKG")
				{
					Texture*tex=new BackGround();
					textures.push_back(tex);
				}
			}
			int objcnt;
			fin>>objcnt;
			cerr<<objcnt<<" Objects Detected"<<endl;
			while (objcnt--)
			{
				fin>>str;
				fin>>xx2;
				if(str=="AAC")
				{
					fin>>x1>>x2>>x3>>x4>>x5>>x6>>xx1;
					Object* objpnt=new AAcube(Pnt(x1,x2,x3),Pnt(x4,x5,x6),xx1,textures[xx2]);
					objects.push_back(objpnt);
				}
				else if(str=="TRI")
				{
					fin>>x1>>x2>>x3>>x4>>x5>>x6>>x7>>x8>>x9;
					Object* objpnt=new Triangle(Pnt(x1,x2,x3),Pnt(x4,x5,x6),Pnt(x7,x8,x9),textures[xx2]);
					objects.push_back(objpnt);
				}
				else if(str=="SPH")
				{
					fin>>x1>>x2>>x3>>x4;
					Object* objpnt=new Sphere(Pnt(x1,x2,x3),x4,textures[xx2]);
					objects.push_back(objpnt);
				}
				else if(str=="BEZ")
				{
					fin>>x1>>x2>>x3>>x4>>x5>>x6;
					vector<pair<double,double> >tmpv;
					fin>>xx1;
					while (xx1--)
					{
						fin>>x7>>x8;
						tmpv.push_back(make_pair(x7,x8));
					}
					Object* objpnt=new Bezier(Pnt(x1,x2,x3),Pnt(x4,x5,x6),tmpv,textures[xx2]);
					objects.push_back(objpnt);
				}
				else if(str=="MESH")
				{
					fin>>x1>>x2>>x3>>x4>>str;
					Object* objpnt=new Mesh(x1,x2,x3,x4,str,textures[xx2]);
					cerr<<str<<endl;
					objects.push_back(objpnt);
				}
			}
			fin.close();
			cerr<<"Parser Complete"<<endl;
		}
		int x() const
		{
			return px;
		}
		int y() const
		{
			return py;
		}
		Camera* camera()
		{
			return cam;
		}
		Texture *texturelist(int x)
		{
			return textures[x];
		}
		Object *objectlist(int x)
		{
			return objects[x];
		}
		bool intersect(const Ray&r_in,Hit&hit,int&idx)
		{
			hit.init();
			idx=-1;
			for (int i=0;i<objects.size();i++)
			{
				Hit ht;
				if(objects[i]->intersect(r_in,ht))
				{
					if(dabs(dot(ht.n,r_in.direction()))>epsi)
					{
						if(ht.t<hit.t)
						{
							hit=ht;
							idx=i;
						}
					}
				}
			}
			if(idx<0)return false;
			return true;
		}
};

#endif