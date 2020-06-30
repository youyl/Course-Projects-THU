#ifndef PARSER_INSDEL
#define PARSER_INSDEL

#include "parser.h"
#include "createdrop.h"

bool checknull(ValueList val)
{
	if(val.list.size()==0)return true;
	for (int i=0;i<(int)val.list.size();i++)
	{
		if(val.list[i].isnull)return true;
	}
	return false;
}

bool QueryParser::makeUpValueList(string insvalstring,string tbname,vector<string>cols,ValueList& val)
{
	int tbid=findtableid(tbname);
	FieldList fd=curhead->table[tbid].fid;
	vector<string>strlist=getstrlist(insvalstring,true);
	if(strlist.size()!=cols.size())
	{
		cout<<"Error: Insert Value too few"<<endl;
		return false;
	}
	val.list.clear();
	for (int i=0;i<(int)fd.list.size();i++)
	{
		int ip=-1;
		for (int j=0;j<(int)cols.size();j++)
		{
			if(cols[j]==fd.list[i].fieldname)
			{
				Value tmpval;
				if(!stringtovalue(strlist[j],fd.list[i].type,tmpval))
				{
					cout<<"Error: Insert Value Converting Failed on "<<strlist[j]<<endl;
					return false;
				}
				val.list.push_back(tmpval);
				ip=1;
				break;
			}
		}
		if(ip==-1)
		{
			val.list.push_back(fd.list[i].deft);
		}
	}
	return true;
}

bool checkandcompensate(Field fid,Value& val)
{
	if(fid.isnotnull)if(val.isnull)return false;
	if(fid.type==TypeName::INT)
	{
		if(val.valuetype!=ValueType::intvalue)return false;
		if(val.intvalue>=(1LL<<31)||val.intvalue<-(1LL<<31))return false;
	}
	if(fid.type==TypeName::DECIMAL)
	{
		if(val.valuetype!=ValueType::floatvalue)return false;
	}
	if(fid.type==TypeName::BIGINT)
	{
		if(val.valuetype!=ValueType::intvalue)return false;
	}
	if(fid.type==TypeName::DATE)
	{
		if(val.valuetype!=ValueType::intvalue)return false;
		if(val.intvalue>100000000)return false;
	}
	if(fid.type==TypeName::CHAR)
	{
		if(val.valuetype!=ValueType::stringvalue)return false;
		if((int)val.stringvalue.length()>upperCut(fid.charmaxlength))return false;
		while ((int)val.stringvalue.length()<fid.charmaxlength)val.stringvalue+=" ";
	}
	if(fid.type==TypeName::VARCHAR)
	{
		if(val.valuetype!=ValueType::varcharvalue)return false;
		string str=varcharstorage.findvarchar(val.intvalue);
		if((int)str.length()>fid.varcharlength)return false;
	}
	return true;
}

bool QueryParser::insertEntry(string tbname,ValueList val)
{
	int recnum=findrechandle(curpath+"/rec."+tbname);
	if(recnum<0)
	{
		cout<<"Error: Insert Table not found"<<endl;
		return false;
	}
	int tbid=findtableid(tbname);
	FieldList fd=curhead->table[tbid].fid;
	vector<string>fdlist=curhead->table[tbid].colname;
	AttrTypeList tableatr=fd.toatrlist();
	
	for (int i=0;i<(int)fd.list.size();i++)
	{
		if(!checkandcompensate(fd.list[i],val.list[i]))
		{
			val.list[i].print();
			cout<<"Error: Insert data not Valid"<<endl;
			return false;
		}
	}
	
	for (auto cs: curhead->cons)
	{
		if(cs.constype==0&&cs.tbname1==tbname)
		{
			int ixnum=findidxhandle(curpath+"/"+tbname+".idx.pm.key");
			IndexHandle *ixh=idxhandle[ixnum].second;
			ValueList val2;
			if(!extractValue(fdlist,val,cs.collist1,val2))return false;
			if(checknull(val2))return false;
			//ixh->debug();
			if(ixh->exist(val2))
			{
				cout<<"Error: Primary Key Check Failed"<<endl;
				return false;
			}
		}
	}
	
	for (auto cs: curhead->cons)
	{
		if(cs.constype==1&&cs.tbname1==tbname)
		{
			int ixnum=findidxhandle(curpath+"/"+cs.tbname2+".idx.pm.key");
			IndexHandle *ixh=idxhandle[ixnum].second;
			ValueList val2;
			if(!extractValue(fdlist,val,cs.collist1,val2))return false;
			if(!ixh->exist(val2))
			{
				cout<<"Error: Foreign Key Check Failed"<<endl;
				return false;
			}
		}
	}
	
	if(!valuetobuf(forinsert,val,tableatr))return false;
	Rid rid;
	if(!rechandle[recnum].second->insertRecord(forinsert,rid))return false;
	
	for (auto ixx: curhead->index)
	{
		if(ixx.tbname==tbname)
		{
			int ixnum=findidxhandle(curpath+"/"+ixx.tbname+".idx."+ixx.idxname);
			IndexHandle *ixh=idxhandle[ixnum].second;
			ValueList val2;
			if(!extractValue(fdlist,val,ixx.namelist,val2))return false;
			if(!ixh->insertEntry(val2,rid))return false;
		}
	}
	return true;
}

bool QueryParser::copyFrom(string tbname,string pathname)
{
	//iterator over file
	int cnt=0,line=0,tbid=findtableid(tbname);
	if(tbid<0)return false;
	string str;
	ifstream fin(pathname.c_str());
	FieldList fd=curhead->table[tbid].fid;
	while (!fin.eof())
	{
		line++;/*
		if(line>200000)
		{
			cout<<"Warning: too large dataset. cut off now"<<endl;
			break;
		}*/
		//make valuelist
		getline(fin,str);
		str=clearSpace(str);
		if(str==""||str.find("|")==string::npos)break;
		ValueList val;
		string tmp;
		int ip=0;
		for (int i=0;i<(int)str.length();i++)
		{
			if(str[i]=='|')
			{
				Value tmpval;
				if(!stringtovalue(tmp,fd.list[ip].type,tmpval))
				{
					cout<<"Error: copying line "<<line<<endl;
					cout<<str<<endl;
					return false;
				}
				val.list.push_back(tmpval);
				ip++;
				tmp="";
			}
			else 
			{
				tmp.push_back(str[i]);
			}
		}
		if(ip!=(int)fd.list.size())
		{
			val.print();
			cout<<"Error: copying line "<<line<<endl;
			return false;
		}
		//insert
		if(!insertEntry(tbname,val))return false;
		cnt++;
	}
	cout<<"Successfully Copied "<<cnt<<" Items to Table "<<tbname<<endl;
	return true;
}

//--------------------------------------------------------------------------------------------------------------

bool fitWhereItem(vector<string>cols,ValueList val,WhereItem witem)
{
	Column c1=witem.col1;
	Value v1,v2;
	for (int i=0;i<(int)cols.size();i++)
	{
		if(cols[i]==c1.colname)
		{
			v1=val.list[i];
			break;
		}
	}
	if(witem.isnull)
	{
		return v1.isnull;
	}
	if(witem.isnotnull)
	{
		return (!(v1.isnull));
	}
	if(witem.colcnt==1)
	{
		v2=witem.val;
	}
	else
	{
		c1=witem.col2;
		for (int i=0;i<(int)cols.size();i++)
		{
			if(cols[i]==c1.colname)
			{
				v2=val.list[i];
				break;
			}
		}
	}
	return CompOp::judge(witem.cmpop,ValueList(v1),ValueList(v2));
}

bool fitWhereClause(vector<string>cols,ValueList val,WhereClause wherec)
{
	for (auto itm: wherec.list)
	{
		if(!fitWhereItem(cols,val,itm))return false;
	}
	return true;
}

Column getColumn(string tbname,string str)
{
	bool flag=false;
	string str1,str2;
	for (int i=0;i<(int)str.length();i++)
	{
		if(str[i]=='.')
		{
			str2=str.substr(i+1);
			flag=true;
			break;
		}
		str1.push_back(str[i]);
	}
	if(!flag)
	{
		if(is_debug)cout<<"Column analysis get: "<<tbname<<" -- "<<str1<<endl;
		return Column(tbname,str1);
	}
	else
	{
		if(is_debug)cout<<"Column analysis get: "<<str1<<" -- "<<str2<<endl;
		return Column(str1,str2);
	}
}

bool QueryParser::getTypefromColumn(Column col,int& type)
{
	string tbname=col.tbname;
	string colname=col.colname;
	int tbid=findtableid(tbname);
	if(tbid<0)return false;
	FieldList fd=curhead->table[tbid].fid;
	for (int i=0;i<(int)fd.list.size();i++)
	{
		if(fd.list[i].fieldname==colname)
		{
			type=fd.list[i].type;
			return true;
		}
	}
	return false;
}
bool QueryParser::getFieldfromColumn(Column col,Field& fid)
{
	string tbname=col.tbname;
	string colname=col.colname;
	int tbid=findtableid(tbname);
	if(tbid<0)return false;
	FieldList fd=curhead->table[tbid].fid;
	for (int i=0;i<(int)fd.list.size();i++)
	{
		if(fd.list[i].fieldname==colname)
		{
			fid=fd.list[i];
			return true;
		}
	}
	return false;
}

bool QueryParser::findWhereClause(string sstr,string tbname,WhereClause& wherec)
{
	wherec.list.clear();
	sstr=clearSpace(sstr);
	if(sstr=="")return true;
	vector<string>strlist=getstrlist(sstr,false);
	for (auto str: strlist)
	{
		string str1,str2;
		for (int i=0;i<(int)str.length();i++)
		{
			if(str[i]==' ')
			{
				str2=str.substr(i+1);
				break;
			}
			str1.push_back(str[i]);
		}
		WhereItem wc;
		wc.colcnt=1;
		wc.col1=getColumn(tbname,str1);
		str2=clearSpace(str2);
		if(str2=="IS NOT NULL")
		{
			wc.isnotnull=true;
		}
		else if(str2=="IS NULL")
		{
			wc.isnull=true;
		}
		else
		{
			if(str2.substr(0,3)=="== ")
			{
				wc.cmpop=CompOp::Equal;
				str2=str2.substr(3);
			}
			else if(str2.substr(0,3)==">= ")
			{
				wc.cmpop=CompOp::GreaterOrEqual;
				str2=str2.substr(3);
			}
			else if(str2.substr(0,3)=="<= ")
			{
				wc.cmpop=CompOp::LessOrEqual;
				str2=str2.substr(3);
			}
			else if(str2.substr(0,3)=="<> ")
			{
				wc.cmpop=CompOp::NonEqual;
				str2=str2.substr(3);
			}
			else if(str2.substr(0,2)=="> ")
			{
				wc.cmpop=CompOp::Greater;
				str2=str2.substr(2);
			}
			else if(str2.substr(0,2)=="< ")
			{
				wc.cmpop=CompOp::Less;
				str2=str2.substr(2);
			}
			else
			{
				cout<<"Error: unable to parser the whereItem: "<<str<<endl;
				return false;
			}
			//test str is column or value
			str2=clearSpace(str2);
			if(str2[0]=='('&&str2[str2.length()-1]==')')//value
			{
				str2=str2.substr(1,str2.length()-2);
				int type1;
				if(!getTypefromColumn(wc.col1,type1))return false;
				if(!stringtovalue(str2,type1,wc.val))return false;
				Field fd;
				if(!getFieldfromColumn(wc.col1,fd))return false;
				if(!checkandcompensate(fd,wc.val))return false;
			}
			else
			{
				wc.colcnt=2;
				wc.col2=getColumn(tbname,str2);
				int type1,type2;
				if(!getTypefromColumn(wc.col1,type1))return false;
				if(!getTypefromColumn(wc.col2,type2))return false;
				if(type1!=type2)
				{
					cout<<"Error: where clause type unfit"<<endl;
					return false;
				}
			}
		}
		wherec.list.push_back(wc);
	}
	return true;
}

bool QueryParser::deleteByRid(string tbname,Rid rid)
{
	int recnum=findrechandle(curpath+"/rec."+tbname);
//	int tbid=findtableid(tbname);
	if(recnum<0)return false;
	if(!rechandle[recnum].second->deleteRecord(rid))return false;
	
	for (auto ixx: curhead->index)
	{
		if(ixx.tbname==tbname)
		{
			int ixnum=findidxhandle(curpath+"/"+tbname+".idx."+ixx.idxname);
			if(ixnum<0)return false;
			if(!idxhandle[ixnum].second->deleteByRid(rid))return false;
		}
	}
	return true;
}

bool QueryParser::deleteFromWhere(string tbname,WhereClause wherec)
{
	for (auto cs: curhead->cons)
	{
		if(cs.constype==1&&cs.tbname2==tbname)
		{
			cout<<"Error: Try to Delete From a Table with Foreign Key"<<endl;
			return false;
		}
	}
	RecordScan rs;
	int recnum=findrechandle(curpath+"/rec."+tbname);
	int tbid=findtableid(tbname);
	if(recnum<0)return false;
	if(!rs.startScan(rechandle[recnum].second))return false;
	AttrTypeList tableatr=curhead->table[tbid].fid.toatrlist();
	vector<Rid>fordel;
	do
	{
		Rid rd=rs.getScanVal();
		ValueList val;
		DBRecord recc;
		BufType c;
		if(!rechandle[recnum].second->getRecordforScan(rd,recc))return false;
		c=recc.content;
		if(!buftovalue(c,val,tableatr))return false;
		if(fitWhereClause(curhead->table[tbid].colname,val,wherec))
		{
			//foreign key with delete too hard
			fordel.push_back(rd);
		}
	}while (rs.getNext());
	for (auto rid: fordel)
	{
		deleteByRid(tbname,rid);
	}
	return true;
}

#endif
