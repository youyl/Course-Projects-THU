#include <iostream>
#include <string>
#include "head.h"
#include "compare.h"
#include "parser/all.h"

using namespace std;

QueryParser *psr;
string str;
int main()
{
	MyBitMap::initConst();
	psr=new QueryParser();
	cout<<"Parser initialization completed."<<endl;
	while (1)
	{
		getline(cin,str);
		str=clearSpace(str);
		if(str=="EXIT")
		{
			break;
		}
		else
		{
			if(!psr->analysis(str))cout<<"Error Occured!"<<endl;
			else cout<<"Success"<<endl;
		}
	}
	delete psr;
	system("pause");
	return 0;
}
