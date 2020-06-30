#include "obj_include.h"
#include <iostream>
#include <cstdio>

using namespace std;

Mesh *mesh;

int main()
{
	Texture*tex;
	mesh=new Mesh(0,0,0,"../models/dinosaur.obj",tex);
	return 0;
}