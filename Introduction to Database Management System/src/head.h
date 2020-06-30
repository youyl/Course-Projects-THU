#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER

#include <vector>
#include <iostream>
#include <algorithm>
#include <map> 
#include <fstream>
#include "pf/utils/pagedef.h"

using namespace std;

const bool is_debug=false;
bool is_sudden_debug=false;

namespace TypeName
{
	const int INT=0;
	const int DECIMAL=1;
	const int VARCHAR=2;
	const int CHAR=3;
	const int BIGINT=4;
	const int DATE=5;
	const int length[6]={2,3,2,0,3,4};
	const string tpname[6]={"INT","DECIMAL","VARCHAR","CHAR","BIGINT","DATE"};
}
/*
intvalue		INT		value	isnull
floatvalue		DECIMAL	value	value	isnull
varcharvalue	VARCHAR	posi	isnull
stringvalue		CHAR	---		isnull // length is always like 4n
intvalue		BIGINT	value	value	isnull
intvalue		DATE	year	month	day
*/

class VarcharStore
{
	public:
		map<int,string>inttovarchar;
		int cnt,num;
		inline void readfromfile(string filename)
		{
			inttovarchar.clear();
			ifstream fin(filename.c_str());
			if(!(fin>>num))num=0;
			cnt=0;
			int ip1;
			string ip2;
			for (int i=0;i<num;i++)
			{
				fin>>ip1;
				getline(fin,ip2);
				getline(fin,ip2);
				inttovarchar[ip1]=ip2;
				cnt=max(cnt,ip1);
			}
			fin.close();
		}
		inline void writetofile(string filename)
		{
			ofstream fout(filename.c_str());
			fout<<num<<endl;
			for (auto it=inttovarchar.begin();it!=inttovarchar.end();it++)
			{
				fout<<it->first<<endl;
				fout<<it->second<<endl;
			}
			fout.close();
		}
		inline string findvarchar(int idx)
		{
			return inttovarchar[idx];
		}
		inline int insertnewvarchar(string str)
		{
			cnt++;num++;
			inttovarchar[cnt]=str;
			return cnt;
		}
}varcharstorage;

inline int upperCut(int x)
{
	if((x&3)==0)return x;
	x+=4;
	x-=(x&3);
	return x;
}

inline string clearSpace(string _str)
{
	while (_str!=""&&_str[0]==' ')
	{
		_str=_str.substr(1);
	}
	while (_str[_str.length()-1]==' ')
	{
		_str=_str.substr(0,_str.length()-1);
	}
	return _str;
}

class AttrType
{
	public:
		int type;
		int offset;
		int length;
		AttrType()
		{
			type=1000;
			offset=0;
			length=0;
		}
		AttrType(int _type,int _offset=0,int _length=0)
		{
			type=_type;
			offset=_offset;
			length=upperCut(_length)/4+1;
			if(type!=TypeName::CHAR)
			{
				length=TypeName::length[type];
			}
		}
};

class AttrTypeList
{
	public:
		vector<AttrType>list;
		int length;
		int size;
		AttrTypeList()
		{
			list.clear();
			length=0;
			size=0;
		}
		inline void clear()
		{
			list.clear();
			length=0;
			size=0;
		}
		inline void push_back(AttrType _attr)
		{
			_attr.offset=length;
			length+=_attr.length;
			list.push_back(_attr);
			size++;
		}
		AttrTypeList(AttrType _attr)
		{
			list.clear();
			length=0;
			size=0;
			push_back(_attr);
		}
		inline void pop()
		{
			length-=list[size-1].length;
			size--;
			list.pop_back();
		}
		inline AttrType back()
		{
			return list[size-1];
		}
};

namespace ValueType
{
	const int stringvalue=0;
	const int intvalue=1;
	const int floatvalue=2;
	const int varcharvalue=3;
}

bool likeadate(string str,long long&val)
{
	str=clearSpace(str);
	if(str.length()!=10)return false;
	if(!isdigit(str[0]))return false;
	if(!isdigit(str[1]))return false;
	if(!isdigit(str[2]))return false;
	if(!isdigit(str[3]))return false;
	if(str[4]!='-')return false;
	if(!isdigit(str[5]))return false;
	if(!isdigit(str[6]))return false;
	if(str[7]!='-')return false;
	if(!isdigit(str[8]))return false;
	if(!isdigit(str[9]))return false;
	val=(str[0]-'0')*10000000+(str[1]-'0')*1000000+(str[2]-'0')*100000+(str[3]-'0')*10000;
	val=val+(str[5]-'0')*1000+(str[6]-'0')*100+(str[8]-'0')*10+(str[9]-'0');
	return true;
}

class Value
{
	public:
		int valuetype;
		bool isnull;
		string stringvalue;
		long long intvalue;
		double floatvalue;
		Value(){}
		void print()
		{
			if(isnull)cout<<" [NULL] ";
			else
			{
				if(valuetype==ValueType::intvalue)
				{
					cout<<' '<<intvalue<<' ';
				}
				else if(valuetype==ValueType::floatvalue)
				{
					cout<<' '<<floatvalue<<' ';
				}
				else if(valuetype==ValueType::stringvalue)
				{
					cout<<' '<<stringvalue<<' ';
				}
				else
				{
					cout<<' '<<varcharstorage.findvarchar(intvalue)<<' ';
				}
			}
		}
		Value(int _tp)
		{
			isnull=true;
			valuetype=_tp;
		}
		Value(int _tp,string _strval)
		{
			isnull=false;
			valuetype=ValueType::stringvalue;
			stringvalue=_strval;
		}
		Value(int _tp,string _strval,int _opc)
		{
			isnull=false;
			valuetype=ValueType::varcharvalue;
			stringvalue=_strval;
			intvalue=_opc;
		}
		Value(int _tp,double _dbval)
		{
			isnull=false;
			valuetype=ValueType::floatvalue;
			floatvalue=_dbval;
		}
		Value(int _tp,int _opc,long long _intval)
		{
			isnull=false;
			valuetype=ValueType::intvalue;
			intvalue=_intval;
		}
};

bool stringtovalue(string str,int type,Value&val)
{
	str=clearSpace(str);
	if(type==TypeName::INT)
	{
		if(str=="NULL")
		{
			val=Value(ValueType::intvalue);
			return true;
		}
		long long res=0,isminus=1;
		if(str[0]=='-')
		{
			isminus=-1;
			str=str.substr(1);
		}
		for (int i=0;i<(int)str.length();i++)
		{
			res=res*10;
			if(!isdigit(str[i]))return false;
			res+=(str[i]-'0');
		}
		res*=isminus;
		val=Value(ValueType::intvalue,0,res);
	}
	else if(type==TypeName::BIGINT)
	{
		if(str=="NULL")
		{
			val=Value(ValueType::intvalue);
			return true;
		}
		long long res=0,isminus=1;
		if(str[0]=='-')
		{
			isminus=-1;
			str=str.substr(1);
		}
		for (int i=0;i<(int)str.length();i++)
		{
			res=res*10;
			if(!isdigit(str[i]))return false;
			res+=(str[i]-'0');
		}
		res*=isminus;
		val=Value(ValueType::intvalue,0,res);
	}
	else if(type==TypeName::DECIMAL)
	{
		if(str=="NULL")
		{
			val=Value(ValueType::floatvalue);
			return true;
		}
		double res=0,isminus=1;
		if(str[0]=='-')
		{
			isminus=-1;
			str=str.substr(1);
		}
		int digit=-1;
		for (int i=0;i<(int)str.length();i++)
		{
			if(str[i]=='.')
			{
				digit=i;
				break;
			}
			res=res*10;
			if(!isdigit(str[i]))return false;
			res+=(str[i]-'0');
		}
		if(digit!=-1)
		{
			double weight=1;
			for (int i=digit+1;i<(int)str.length();i++)
			{
				if(!isdigit(str[i]))return false;
				weight/=10.0;
				res+=(weight*(str[i]-'0'));
			}
		}
		res*=isminus;
		val=Value(ValueType::floatvalue,res);
	}
	else if(type==TypeName::DATE)
	{
		if(str=="NULL")
		{
			val=Value(ValueType::intvalue);
			return true;
		}
		long long tmp;
		if(!likeadate(str,tmp))return false;
		val=Value(ValueType::intvalue,0,tmp);
	}
	else if(type==TypeName::VARCHAR)
	{
		if(str=="NULL")
		{
			val=Value(ValueType::varcharvalue);
			return true;
		}
		if(str[0]=='\''&&str[str.length()-1]=='\'')str=str.substr(1,str.length()-2);
		val=Value(ValueType::intvalue,"*",varcharstorage.insertnewvarchar(str));
	}
	else if(type==TypeName::CHAR)
	{
		if(str=="NULL")
		{
			val=Value(ValueType::stringvalue);
			return true;
		}
		if(str[0]=='\''&&str[str.length()-1]=='\'')str=str.substr(1,str.length()-2);
		val=Value(ValueType::stringvalue,str);
	}
	return true;
}

class ValueList
{
	public:
		vector<Value>list;
		ValueList(){}
		ValueList(Value val)
		{
			list.push_back(val);
		}
		void print()
		{
			for (int i=0;i<(int)list.size();i++)
			{
				list[i].print();
				cout<<" | ";
			}
			cout<<endl;
		}
};

inline bool typefit(ValueList val,AttrTypeList atr)
{
	if((int)val.list.size()!=atr.size)
	{
		cout<<"Error: Type not fit in typefit process"<<endl;
		return false;
	}
	for (int i=0;i<(int)val.list.size();i++)
	{
		if(atr.list[i].type==TypeName::INT||atr.list[i].type==TypeName::BIGINT||atr.list[i].type==TypeName::DATE)
		{
			if(val.list[i].valuetype!=ValueType::intvalue)
			{
				cout<<"Error: Type not fit in typefit process"<<endl;
				return false;
			}
		}
		if(atr.list[i].type==TypeName::VARCHAR)
		{
			if(val.list[i].valuetype!=ValueType::varcharvalue)
			{
				cout<<"Error: Type not fit in typefit process"<<endl;
				return false;
			}
		}
		if(atr.list[i].type==TypeName::CHAR)
		{
			if(val.list[i].valuetype!=ValueType::stringvalue)
			{
				cout<<"Error: Type not fit in typefit process"<<endl;
				return false;
			}
		}
		if(atr.list[i].type==TypeName::DECIMAL)
		{
			if(val.list[i].valuetype!=ValueType::floatvalue)
			{
				cout<<"Error: Type not fit in typefit process"<<endl;
				return false;
			}
		}
	}
	return true;
}

/*
intvalue		INT		value	isnull
floatvalue		DECIMAL	value	value	isnull
varcharvalue	VARCHAR	posi	isnull
stringvalue		CHAR	---		isnull
intvalue		BIGINT	value	value	isnull
intvalue		DATE	year	month	day
*/

inline Value buftoint(BufType b,int length)//length 1-int 2-bigint 3-date
{
	int *tb=((int*)b);
	long long *ltb=((long long*)b);
	int xx=0;
	if(length==3)
	{
		return Value(ValueType::intvalue,xx,(long long)((*tb)*10000+(*(tb+1))*10+(*(tb+2))));
	}
	else if(length==2)
	{
		return Value(ValueType::intvalue,xx,(*ltb));
	}
	else
	{
		return Value(ValueType::intvalue,xx,(long long)(*tb));
	}
}

inline Value buftofloat(BufType b)
{
	double *tb=((double*)b);
	return Value(ValueType::floatvalue,(*tb));
}

inline Value buftostring(BufType b,int length)
{
	string res;
	int cc=0;
	BufType bf=new unsigned int[1];
	for (int i=0;i<length;i+=4)
	{
		bf[0]=b[cc];
		cc++;
		char *tb=((char*)bf);
		res.push_back(tb[0]);
		res.push_back(tb[1]);
		res.push_back(tb[2]);
		res.push_back(tb[3]);
	}
	return Value(ValueType::stringvalue,res);
}

inline Value buftovarchar(BufType b)
{
	int *tb=((int*)b);
	return Value(ValueType::varcharvalue,"*",(*tb));
}

inline bool buftovalue(BufType b,ValueList& val,const AttrTypeList& atr)
{
	val.list.clear();
	for (int i=0;i<atr.size;i++)
	{
		unsigned int isn=b[atr.list[i].offset+atr.list[i].length-1];
		if(atr.list[i].type==TypeName::INT)
		{
			if(isn==1)
			{
				val.list.push_back(Value(ValueType::intvalue));
			}
			else
			{
				val.list.push_back(buftoint(b+atr.list[i].offset,1));
			}
		}
		else if(atr.list[i].type==TypeName::DECIMAL)
		{
			if(isn==1)
			{
				val.list.push_back(Value(ValueType::floatvalue));
			}
			else
			{
				val.list.push_back(buftofloat(b+atr.list[i].offset));
			}
		}
		else if(atr.list[i].type==TypeName::VARCHAR)
		{
			if(isn==1)
			{
				val.list.push_back(Value(ValueType::varcharvalue));
			}
			else
			{
				val.list.push_back(buftovarchar(b+atr.list[i].offset));
			}
		}
		else if(atr.list[i].type==TypeName::CHAR)
		{
			if(isn==1)
			{
				val.list.push_back(Value(ValueType::stringvalue));
			}
			else
			{
				val.list.push_back(buftostring(b+atr.list[i].offset,(atr.list[i].length-1)*4));
			}
		}
		else if(atr.list[i].type==TypeName::BIGINT)
		{
			if(isn==1)
			{
				val.list.push_back(Value(ValueType::intvalue));
			}
			else
			{
				val.list.push_back(buftoint(b+atr.list[i].offset,2));
			}
		}
		else if(atr.list[i].type==TypeName::DATE)
		{
			if(isn==1)
			{
				val.list.push_back(Value(ValueType::intvalue));
			}
			else
			{
				val.list.push_back(buftoint(b+atr.list[i].offset,3));
			}
		}
	}
	return true;
}

inline void inttobuf(BufType b,int val)
{
	int *y=new int(val);
	BufType tb=((BufType)y);
	b[0]=tb[0];
}

inline void biginttobuf(BufType b,long long val)
{
	long long *y=new long long (val);
	BufType tb=((BufType)y);
	b[0]=tb[0];
	b[1]=tb[1];
}

inline void datetobuf(BufType b,int val)
{
	int *y=new int[3];
	y[0]=val/10000;
	y[1]=(val%10000)/100;
	y[2]=val%100;
	BufType tb=((BufType)y);
	b[0]=tb[0];
	b[1]=tb[1];
	b[2]=tb[2];
}

inline void varchartobuf(BufType b,int val)
{
	int *y=new int(val);
	BufType tb=((BufType)y);
	b[0]=tb[0];
}

inline void chartobuf(BufType b,string val,int length)
{
	while ((int)val.length()<length)val=val+" ";
	char tb[4];
	int cc=0;
	for (int i=0;i<length;i+=4)
	{
		tb[0]=val[i];
		tb[1]=val[i+1];
		tb[2]=val[i+2];
		tb[3]=val[i+3];
		BufType bt=((BufType)tb);
		b[cc]=bt[0];
		cc++;
	}
}

inline void decimaltobuf(BufType b,double val)
{
	double *y=new double(val);
	BufType tb=((BufType)y);
	b[0]=tb[0];
	b[1]=tb[1];
}

inline bool valuetobuf(BufType b,ValueList& val,const AttrTypeList& atr)
{
	for (int i=0;i<atr.size;i++)
	{
		if(val.list[i].isnull)b[atr.list[i].offset+atr.list[i].length-1]=1;
		else
		{
			b[atr.list[i].offset+atr.list[i].length-1]=0;
			if(atr.list[i].type==TypeName::INT)
			{
				inttobuf(b+atr.list[i].offset,val.list[i].intvalue);
			}
			else if(atr.list[i].type==TypeName::DECIMAL)
			{
				decimaltobuf(b+atr.list[i].offset,val.list[i].floatvalue);
			}
			else if(atr.list[i].type==TypeName::VARCHAR)
			{
				varchartobuf(b+atr.list[i].offset,val.list[i].intvalue);
			}
			else if(atr.list[i].type==TypeName::CHAR)
			{
				chartobuf(b+atr.list[i].offset,val.list[i].stringvalue,(atr.list[i].length-1)*4);
			}
			else if(atr.list[i].type==TypeName::BIGINT)
			{
				biginttobuf(b+atr.list[i].offset,val.list[i].intvalue);
			}
			else if(atr.list[i].type==TypeName::DATE)
			{
				datetobuf(b+atr.list[i].offset,val.list[i].intvalue);
			}
		}
	}
	return true;
}

class Field
{
	public:
		Value deft;
		string fieldname;
		int type;
		int varcharlength;
		int charmaxlength;
		bool isnotnull;
		bool isdefault;
};

class FieldList
{
	public:
		vector<Field>list;
		AttrTypeList toatrlist()
		{
			AttrTypeList lst;
			for (int i=0;i<(int)list.size();i++)
			{
				if(list[i].type==TypeName::CHAR)lst.push_back(AttrType(list[i].type,0,list[i].charmaxlength));
				else lst.push_back(AttrType(list[i].type));
			}
			return lst;
		}
};

class TableHead
{
	public:
		string tbname;
		FieldList fid;
		/*
		Value deft;
			int valuetype;
			bool isnull;
			string stringvalue;
			long long intvalue;
			double floatvalue;
		string fieldname;
		int type;
		int varcharlength;
		int charmaxlength;
		bool isnotnull;
		bool isdefault;
		*/
		vector<string> colname;
		TableHead(){}
		void describe()
		{
			cout<<"Table Name: "<<tbname<<endl;
			for (int i=0;i<(int)colname.size();i++)
			{
				cout<<"Column No."<<i<<": "<<colname[i]<<' '<<TypeName::tpname[fid.list[i].type]<<endl;
				cout<<"Varchar length: "<<fid.list[i].varcharlength<<" || Char Max Length: "<<fid.list[i].charmaxlength<<endl;
				cout<<"Default Value: ";
				fid.list[i].deft.print();
				cout<<endl;
			}
			cout<<"Table Description Finished"<<endl;
		}
};

class Constraint
{
	public:
		string consname;
		int constype;//0 pmkey, 1 frkey
		string tbname1,tbname2;
		vector<string>collist1,collist2;
};

class TableIndex
{
	public:
		/*
		AttrTypeList atrlist;
		size
		length
		vector<attrtype>: type offset length
		*/
		string tbname;
		string idxname;
		vector<string>namelist;
};

class DatabaseHead
{
	public:
		bool writebk;
		string path;
		vector<TableHead>table;
		vector<Constraint>cons;
		vector<TableIndex>index;
		DatabaseHead(string _path)//read in
		{
			writebk=false;
			path=_path;
			table.clear();
			cons.clear();
			index.clear();
			ifstream fin((_path+"/_dbhead.txt").c_str());
			int tbsize,consize,idxsize,x1;
			fin>>tbsize>>consize>>idxsize;
			for (int i=0;i<tbsize;i++)
			{
				TableHead tbhd;
				fin>>tbhd.tbname;
				fin>>x1;
				tbhd.colname.resize(x1);
				tbhd.fid.list.resize(x1);
				for (int j=0;j<x1;j++)
				{
					fin>>tbhd.colname[j];
				}
				for (int j=0;j<x1;j++)
				{
					fin>>tbhd.fid.list[j].fieldname;
					fin>>tbhd.fid.list[j].type;
					fin>>tbhd.fid.list[j].varcharlength;
					fin>>tbhd.fid.list[j].charmaxlength;
					fin>>tbhd.fid.list[j].isnotnull;
					fin>>tbhd.fid.list[j].isdefault;
					fin>>tbhd.fid.list[j].deft.valuetype;
					fin>>tbhd.fid.list[j].deft.isnull;
					fin>>tbhd.fid.list[j].deft.intvalue;
					fin>>tbhd.fid.list[j].deft.floatvalue;
					getline(fin,tbhd.fid.list[j].deft.stringvalue);
					getline(fin,tbhd.fid.list[j].deft.stringvalue);
				}
				table.push_back(tbhd);
			}
			for (int i=0;i<consize;i++)
			{
				Constraint cs;
				fin>>cs.consname;
				fin>>cs.constype;
				fin>>cs.tbname1;
				fin>>cs.tbname2;
				fin>>x1;
				cs.collist1.resize(x1);
				for (int j=0;j<x1;j++)
				{
					fin>>cs.collist1[j];
				}
				fin>>x1;
				cs.collist2.resize(x1);
				for (int j=0;j<x1;j++)
				{
					fin>>cs.collist2[j];
				}
				cons.push_back(cs);
			}
			for (int i=0;i<idxsize;i++)
			{
				TableIndex tbix;
				fin>>tbix.tbname;
				fin>>tbix.idxname;
				fin>>x1;
				tbix.namelist.resize(x1);
				for (int j=0;j<x1;j++)
				{
					fin>>tbix.namelist[j];
				}
				index.push_back(tbix);
			}
			fin.close();
		}
		void showdatabase()
		{
			cout<<"DataBase Name: "<<path<<endl;
			cout<<"Table List:"<<endl;
			showtables();
			cout<<"Constraint:"<<endl;
			showconstraint();
			cout<<"Index List:"<<endl;
			showindexs();
		}
		bool showtables()
		{
			cout<<"Table Count: "<<table.size()<<endl;
			for (int i=0;i<(int)table.size();i++)
			{
				cout<<"Table No."<<i<<" : "<<table[i].tbname<<endl;
			}
			return true;
		}
		bool showindexs()
		{
			cout<<"Index Count: "<<index.size()<<endl;
			for (int i=0;i<(int)index.size();i++)
			{
				cout<<"Index No."<<i<<" : "<<index[i].tbname<<" -- "<<index[i].idxname<<endl;
				cout<<"Columns: ";
				for (int j=0;j<(int)index[i].namelist.size();j++)
				{
					cout<<index[i].namelist[j]<<' ';
				}cout<<endl;
			}
			return true;
		}
		bool showconstraint()
		{
			cout<<"Constraint Count: "<<cons.size()<<endl;
			for (int i=0;i<(int)cons.size();i++)
			{
				cout<<"Cons No."<<i<<" : "<<cons[i].consname<<endl;
				if(cons[i].constype==0)
				{
					cout<<"Primary Key on Table : "<<cons[i].tbname1<<endl;
					cout<<"(";
					for (int j=0;j<(int)cons[i].collist1.size();j++)
					{
						cout<<" "<<cons[i].collist1[j]<<" ";
					}
					cout<<")"<<endl;
				}
				else
				{
					cout<<"Foreign Key on Table : "<<cons[i].tbname1<<" "<<cons[i].tbname2<<endl;
					cout<<"(";
					for (int j=0;j<(int)cons[i].collist1.size();j++)
					{
						cout<<" "<<cons[i].collist1[j]<<" ";
					}
					cout<<")  (";
					for (int j=0;j<(int)cons[i].collist2.size();j++)
					{
						cout<<" "<<cons[i].collist2[j]<<" ";
					}
					cout<<")"<<endl;
				}
			}
			return true;
		}
		bool describetable(string tbname)
		{
			for (int i=0;i<(int)table.size();i++)
			{
				if(tbname==table[i].tbname)
				{
					table[i].describe();
					return true;
				}
			}
			cout<<"Error: Table name < "<<tbname<<" > not Found"<<endl;
			return false;
		}
		void writeback()
		{
			writebk=true;
			ofstream fout((path+"/_dbhead.txt").c_str());
			fout<<table.size()<<' '<<cons.size()<<' '<<index.size()<<endl;
			for (auto tb: table)
			{
				fout<<tb.tbname<<endl;
				fout<<tb.colname.size()<<endl;
				for (auto i: tb.colname)
				{
					fout<<i<<endl;
				}
				for (auto i: tb.fid.list)
				{
					fout<<i.fieldname<<endl;
					fout<<i.type<<endl;
					fout<<i.varcharlength<<endl;
					fout<<i.charmaxlength<<endl;
					fout<<i.isnotnull<<endl;
					fout<<i.isdefault<<endl;
					fout<<i.deft.valuetype<<endl;
					fout<<i.deft.isnull<<endl;
					fout<<i.deft.intvalue<<endl;
					fout<<i.deft.floatvalue<<endl;
					fout<<i.deft.stringvalue<<endl;
				}
			}
			for (auto cs: cons)
			{
				fout<<cs.consname<<endl;
				fout<<cs.constype<<endl;
				fout<<cs.tbname1<<endl;
				fout<<cs.tbname2<<endl;
				fout<<cs.collist1.size()<<endl;
				for (auto i: cs.collist1)
				{
					fout<<i<<endl;
				}
				fout<<cs.collist2.size()<<endl;
				for (auto i: cs.collist2)
				{
					fout<<i<<endl;
				}
			}
			for (auto tbix: index)
			{
				fout<<tbix.tbname<<endl;
				fout<<tbix.idxname<<endl;
				fout<<tbix.namelist.size()<<endl;
				for (auto i: tbix.namelist)
				{
					fout<<i<<endl;
				}
			}
			fout.close();
		}
		~DatabaseHead()//write back
		{
			if(!writebk)writeback();
		}
		bool tbexist(string str)
		{
			for (auto th: table)
			{
				if(th.tbname==str)return true;
			}return false;
		}
		bool colexist(string tbname,vector<string>cols)
		{
			for (auto th: table)
			{
				if(th.tbname==tbname)
				{
					bool flag=true;
					for (auto colname: cols)
					{
						bool flag2=false;
						for (auto ssss: th.colname)
						{
							if(ssss==colname)
							{
								flag2=true;
								break;
							}
						}
						if(!flag2)
						{
							flag=false;
							break;
						}
					}
					return flag;
				}
			}
			return false;
		}
		vector<string>findpmkey(string tbname)
		{
			vector<string>res;
			for (auto cs: cons)
			{
				if(cs.constype==0)
				{
					if(cs.tbname1==tbname)
					{
						res=cs.collist1;
						break;
					}
				}
			}
			return res;
		}
		int findtype(string tbname,string colname)
		{
			for (auto tb: table)
			{
				if(tb.tbname==tbname)
				{
					for (auto cc: tb.fid.list)
					{
						if(cc.fieldname==colname)
						{
							return cc.type;
						}
					}
				}
			}
			return -1;
		}
		bool fktypefit(Constraint fk)
		{
			if(fk.collist1.size()!=fk.collist2.size())return false;
			for (int i=0;i<(int)fk.collist1.size();i++)
			{
				if(findtype(fk.tbname1,fk.collist1[i])!=findtype(fk.tbname2,fk.collist2[i]))return false;
			}return true;
		}
		bool checkconsvalid()
		{
			for (auto cs: cons)
			{
				if(!tbexist(cs.tbname1))
				{
					cout<<"Error: Constraint's table name not exist"<<endl;
					return false;
				}
				if(!colexist(cs.tbname1,cs.collist1))
				{
					cout<<"Error: Constraint's column name not exist"<<endl;
					return false;
				}
				if(cs.constype==1)
				{
					if(!tbexist(cs.tbname2))
					{
						cout<<"Error: Constraint's table name not exist (fk)"<<endl;
						return false;
					}
					if(!colexist(cs.tbname2,cs.collist2))
					{
						cout<<"Error: Constraint's column name not exist (fk)"<<endl;
						return false;
					}
				}
			}
			for (auto cs: cons)
			{
				if(cs.constype==1)
				{
					if(findpmkey(cs.tbname2)!=cs.collist2)
					{
						cout<<"Error: Foreign key not based on primary key"<<endl;
						return false;
					}
					if(!fktypefit(cs))
					{
						cout<<"Error: Foreign key type not fit"<<endl;
						return false;
					}
				}
				else
				{
					if(findpmkey(cs.tbname1)!=cs.collist1)
					{
						cout<<"Error: Multiple Primary Key"<<endl;
						return false;
					}
				}
			}
			return true;
		}
		bool existidx(string tbname,string colname,string& idxname)
		{
			for (auto ix: index)
			{
				if(ix.tbname==tbname)
				{
					if(ix.namelist.size()==1)
					{
						if(ix.namelist[0]==colname)
						{
							idxname=ix.idxname;
							return true;
						}
					}
				}
			}
			return false;
		}
};

class Rid
{
	public:
		int pageID,slotID;
	public:
		Rid(){}
		Rid(int _p,int _s)
		{
			pageID=_p;
			slotID=_s;
		}
		int getSlotID() const
		{
			return slotID;
		}
		int getPageID() const
		{
			return pageID;
		}
};

const Rid lazydelete=Rid(1000,10000);

bool operator ==(const Rid& x,const Rid& y)
{
	return (x.pageID==y.pageID&&x.slotID==y.slotID);
}

class DBRecord
{
	public:
		Rid id;
		BufType content;
	public:
		DBRecord(){}
		DBRecord(Rid _id,BufType _c)
		{
			id=_id;
			content=_c;
		}
		void setContent(BufType _c)
		{
			content=_c;
		}
		void setRid(Rid _id)
		{
			id=_id;
		}
		BufType getContent() const
		{
			return content;
		}
		Rid getRid() const
		{
			return id;
		}
};

class LeafNode
{
	public:
		int pageid,fat;
		int size,next;
		vector<ValueList>value;
		vector<Rid>rid;
		inline void clear()
		{
			value.clear();
			rid.clear();
			size=0;
			pageid=0;
			next=0;
		}
};

class NormalNode
{
	public:
		int pageid;
		int size,fat;
		vector<ValueList>value;
		vector<int>child;
		inline void clear()
		{
			value.clear();
			child.clear();
			size=0;
			pageid=0;
		}
};

class IndexHead
{
	public:
		int valuesize;//calc
		int leafpagesize;//calc
		int nmlpagesize;//calc
		int root;//[0]
		int lastpage;//[1]
		AttrTypeList atrlist;
		/*
		size
		length
		vector<attrtype>: type offset length
		*/
};

class Column
{
	public:
		string tbname,colname;
		Column(){}
		Column(string _tbname,string _colname)
		{
			tbname=_tbname;
			colname=_colname;
		}
};

inline bool operator ==(const Column& x,const Column& y)
{
	return (x.tbname==y.tbname)&&(x.colname==y.colname);
}

class SetClause
{
	public:
		vector<string>col;
		vector<Value>val;
};

class WhereItem
{
	public:
		bool isnotnull;
		bool isnull;
		int cmpop;
		int colcnt;
		Column col1,col2;
		Value val;
		WhereItem()
		{
			colcnt=0;
			cmpop=0;
			isnotnull=false;
			isnull=false;
		}
};

class WhereClause
{
	public:
		vector<WhereItem>list;
};

class SelectValue
{
	public:
		vector<Column>cols;
		vector<Value>vals;
};

class SelectList
{
	public:
		vector<Column>cols;
		Column aggcol;
		int valuetype;
		long long intsum;
		long long maxint;
		long long minint;
		double floatsum;
		double maxfloat;
		double minfloat;
		int count;
		int aggtype;
		SelectList()
		{
			floatsum=0;
			intsum=0;
			count=0;
			maxint=-(1LL<<62);
			minint=(1LL<<62);
			maxfloat=-1e100;
			minfloat=1e100;
		}
		/*
		0--- no agg
		1--- sum
		2--- count(*)
		3--- max
		4--- min
		5--- average
		6--- all(*)
		*/
		void add(Value va)
		{
			floatsum+=va.floatvalue;
			intsum+=va.intvalue;
			maxint=max(maxint,va.intvalue);
			minint=min(minint,va.intvalue);
			maxfloat=max(maxfloat,va.floatvalue);
			minfloat=min(minfloat,va.floatvalue);
			count++;
		}
		void print()
		{
			if(valuetype==TypeName::DECIMAL)
			{
				if(aggtype==1)
				{
					cout<<"Result: "<<floatsum<<endl;
				}
				if(aggtype==2)
				{
					cout<<"Result: "<<count<<endl;
				}
				if(aggtype==3)
				{
					cout<<"Result: "<<maxfloat<<endl;
				}
				if(aggtype==4)
				{
					cout<<"Result: "<<minfloat<<endl;
				}
				if(aggtype==5)
				{
					cout<<"Result: "<<(floatsum*1.0)/count<<endl;
				}
			}
			else
			{
				if(aggtype==1)
				{
					cout<<"Result: "<<intsum<<endl;
				}
				if(aggtype==2)
				{
					cout<<"Result: "<<count<<endl;
				}
				if(aggtype==3)
				{
					cout<<"Result: "<<maxint<<endl;
				}
				if(aggtype==4)
				{
					cout<<"Result: "<<minint<<endl;
				}
				if(aggtype==5)
				{
					cout<<"Result: "<<(intsum*1.0)/count<<endl;
				}
			}
		}
};

#endif 
