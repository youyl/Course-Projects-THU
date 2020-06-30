#ifndef PARSER_CREATEDROP
#define PARSER_CREATEDROP

#include "parser.h"

vector<string> getstrlist(string str,bool brack=true)
{
	vector<string>res;
	str=clearSpace(str);
	if(brack)str=str.substr(1,str.length()-2);
	string cur="";
	int cnt=0;
	for (int i=0;i<(int)str.length();i++)
	{
		if(str[i]=='\'')
		{
			cnt++;
		}
		else if(str[i]==','&&cnt%2==0)
		{
			res.push_back(clearSpace(cur));
			cur="";
		}
		else
		{
			cur.push_back(str[i]);
		}
	}
	res.push_back(clearSpace(cur));
	return res;
}

bool breakTableColumn(string str,string& tbname,vector<string>& cols)
{
	if(is_debug)cout<<"break table and columns : "<<str<<endl;
	tbname="";
	for (int i=0;i<(int)str.length();i++)
	{
		if(str[i]=='(')
		{
			str=str.substr(i);
			break;
		}
		tbname.push_back(str[i]);
	}
	cols=getstrlist(str);
	return true;
}

int typetovaluetype(int x)
{
	if(x==TypeName::INT)return ValueType::intvalue;
	if(x==TypeName::BIGINT)return ValueType::intvalue;
	if(x==TypeName::CHAR)return ValueType::stringvalue;
	if(x==TypeName::VARCHAR)return ValueType::varcharvalue;
	if(x==TypeName::DECIMAL)return ValueType::floatvalue;
	if(x==TypeName::DATE)return ValueType::intvalue;
	return 0;
}

bool parseSingleField(string str,string tbname,FieldList& fdlist,vector<Constraint>& conslist)
{
	//if(!is_debug)cout<<"Single Field Parse: "<<str<<endl;
	if(str.substr(0,11)=="PRIMARY KEY")
	{
		str=str.substr(11);
		Constraint cs;
		cs.consname="unnamed-pm-key";
		cs.constype=0;
		cs.tbname1=tbname;
		cs.collist1=getstrlist(str);
		conslist.push_back(cs);
	}
	else if(str.substr(0,11)=="FOREIGN KEY")
	{
		str=str.substr(11);
		Constraint cs;
		cs.consname="unnamed-fr-key";
		cs.constype=1;
		cs.tbname1=tbname;
		string clst1;
		str=clearSpace(str);
		for (int i=0;i<(int)str.length();i++)
		{
			if(str.substr(i,11)==" REFERENCE ")
			{
				str=str.substr(i+11);
				break;
			}
			clst1.push_back(str[i]);
		}
		cs.collist1=getstrlist(clst1);
		if(!breakTableColumn(str,cs.tbname2,cs.collist2))return false;
		conslist.push_back(cs);
	}
	else
	{
		string fieldname,fieldattr;
		for (int i=0;i<(int)str.length();i++)
		{
			if(str[i]==' ')
			{
				fieldattr=str.substr(i);
				break;
			}
			else
			{
				fieldname.push_back(str[i]);
			}
		}
		Field fd;
		fieldname=clearSpace(fieldname);
		fieldattr=clearSpace(fieldattr);
//		if(is_debug)cout<<"True Field Parsed: "<<fieldname<<" with attris "<<fieldattr<<endl;
		fd.fieldname=fieldname;
		fd.charmaxlength=0;
		fd.varcharlength=0;
		if(fieldattr.substr(0,3)=="INT")
		{
			fieldattr=fieldattr.substr(3);
			fd.type=TypeName::INT;
		}
		else if(fieldattr.substr(0,6)=="BIGINT")
		{
			fieldattr=fieldattr.substr(6);
			fd.type=TypeName::BIGINT;
		}
		else if(fieldattr.substr(0,4)=="DATE")
		{
			fieldattr=fieldattr.substr(4);
			fd.type=TypeName::DATE;
		}
		else if(fieldattr.substr(0,7)=="DECIMAL")
		{
			fieldattr=fieldattr.substr(7);
			fd.type=TypeName::DECIMAL;
		}
		else if(fieldattr.substr(0,4)=="CHAR")
		{
			fieldattr=fieldattr.substr(4);
			fd.type=TypeName::CHAR;
			string charl;
			for (int i=0;i<(int)fieldattr.length();i++)
			{
				charl.push_back(fieldattr[i]);
				if(fieldattr[i]==')')
				{
					fieldattr=fieldattr.substr(i+1);
					break;
				}
			}
			charl=charl.substr(1,charl.length()-2);
			Value val;
			if(!stringtovalue(charl,TypeName::INT,val))return false;
			fd.charmaxlength=upperCut(val.intvalue);
		}
		else if(fieldattr.substr(0,7)=="VARCHAR")
		{
			fieldattr=fieldattr.substr(7);
			fd.type=TypeName::VARCHAR;
			string charl;
			for (int i=0;i<(int)fieldattr.length();i++)
			{
				charl.push_back(fieldattr[i]);
				if(fieldattr[i]==')')
				{
					fieldattr=fieldattr.substr(i+1);
					break;
				}
			}
			charl=charl.substr(1,charl.length()-2);
			Value val;
			if(!stringtovalue(charl,TypeName::INT,val))return false;
			fd.varcharlength=val.intvalue;
		}
		else
		{
			cout<<"parse failed: unknown type "<<fieldattr<<endl;
			return false;
		}
		fieldattr=clearSpace(fieldattr);
		if(fieldattr.substr(0,16)=="NOT NULL DEFAULT")
		{
			fd.isnotnull=true;
			fd.isdefault=true;
			fieldattr=fieldattr.substr(16);
			Value val;
			if(!stringtovalue(clearSpace(fieldattr),fd.type,val))return false;
			fd.deft=val;
		}
		else if(fieldattr.substr(0,7)=="DEFAULT")
		{
			fd.isdefault=true;
			fd.isnotnull=false;
			fieldattr=fieldattr.substr(7);
			Value val;
			if(!stringtovalue(clearSpace(fieldattr),fd.type,val))return false;
			fd.deft=val;
		}
		else if(fieldattr.substr(0,8)=="NOT NULL")
		{
			fd.isdefault=false;
			fd.isnotnull=true;
			fd.deft=Value(typetovaluetype(fd.type));
		}
		else if(fieldattr=="")
		{
			fd.isdefault=false;
			fd.isnotnull=false;
			fd.deft=Value(typetovaluetype(fd.type));
		}
		else
		{
			cout<<"parse failed: unknown constraint on type "<<fieldattr<<endl;
			return false;
		}
		fdlist.list.push_back(fd);
	}
	return true;
}

bool QueryParser::extractFieldFrom(string str,string tbname,FieldList& fdlist,vector<Constraint>& conslist)
{
	str=clearSpace(str);
	if(str[0]!='(')return false;
	str=str.substr(1);
	if(str[str.length()-1]!=')')return false;
	str=str.substr(0,str.length()-1);
	str=clearSpace(str);
	string cur;
	int cntbra=0;
	for (int i=0;i<(int)str.length();i++)
	{
		if(str[i]=='(')
		{
			cntbra++;
			cur.push_back(str[i]);
		}
		else if(str[i]==')')
		{
			cntbra--;
			cur.push_back(str[i]);
		}
		else if(str[i]==','&&cntbra==0)
		{
			cur=clearSpace(cur);
			if(!parseSingleField(cur,tbname,fdlist,conslist))return false;
			cur="";
		}
		else
		{
			cur.push_back(str[i]);
		}
	}
	cur=clearSpace(cur);
	if(!parseSingleField(cur,tbname,fdlist,conslist))return false;
	return true;
}

bool QueryParser::createTable(string tbname,FieldList fdlist,vector<Constraint> conslist)
{
	for (auto i: conslist)
	{
		curhead->cons.push_back(i);
	}
	TableHead th;
	th.tbname=tbname;
	th.fid=fdlist;
	for (auto i: fdlist.list)
	{
		th.colname.push_back(i.fieldname);
	}
//	if(is_debug)th.describe();
	curhead->table.push_back(th);
	if(!curhead->checkconsvalid())
	{
		curhead->table.pop_back();
		for (int i=0;i<(int)conslist.size();i++)
		{
			curhead->cons.pop_back();
		}
		cout<<"Constraint not valid in create table clause."<<endl;
		return false;
	}
	int recordsize=fdlist.toatrlist().length*4;
//	if(is_debug)cout<<"recordsize = "<<recordsize<<endl;
	rm->createFile(curpath+"/rec."+tbname,recordsize);
	int recnum=rechandle.size();
	rechandle.push_back(make_pair(curpath+"/rec."+tbname,nullptr));
	rechandle[recnum].second=rm->openFile(curpath+"/rec."+tbname);
	for (auto i: conslist)
	{
		if(i.constype==0)
		{
			if(!createIndex(tbname,"pm.key",i.collist1,false))return false;
		}
	}
	return true;
}

bool QueryParser::extractCreateIndex(string str,string& tbname,string& idxname,vector<string>& collist)
{
	idxname="";
	for (int i=0;i+3<(int)str.length();i++)
	{
		if(str.substr(i,4)==" ON ")
		{
			str=str.substr(i+3);
			break;
		}
		idxname.push_back(str[i]);
	}
	if(idxname==str)return false;
	str=clearSpace(str);
	tbname="";
	for (int i=0;i<(int)str.length();i++)
	{
		if(str[i]=='(')
		{
			str=str.substr(i);
			break;
		}
		tbname.push_back(str[i]);
	}
	if(str[0]!='('||str[str.length()-1]!=')')return false;
	tbname=clearSpace(tbname);
	idxname=clearSpace(idxname);
	collist=getstrlist(str);
	return true;
}

bool extractValue(vector<string>colname,ValueList val,vector<string>collist,ValueList&res)
{
	res.list.clear();
//	for (int i=0;i<colname.size();i++)cout<<colname[i]<<endl;cout<<endl;
//	for (int i=0;i<collist.size();i++)cout<<collist[i]<<endl;cout<<endl;
	for (int tp=0;tp<(int)collist.size();tp++)
	{
		for (int i=0;i<(int)colname.size();i++)
		{
			if(colname[i]==collist[tp])
			{
				res.list.push_back(val.list[i]);
				break;
			}
		}
	}
//	cout<<res.list.size()<<' '<<collist.size()<<endl;
	if(res.list.size()<collist.size())return false;
	return true;
}

bool extractAtrlist(vector<string>colname,AttrTypeList val,vector<string>collist,AttrTypeList&res)
{
	res.clear();
	for (int tp=0;tp<(int)collist.size();tp++)
	{
		for (int i=0;i<(int)colname.size();i++)
		{
			if(colname[i]==collist[tp])
			{
				res.push_back(val.list[i]);
				break;
			}
		}
	}
	if(res.list.size()<collist.size())return false;
	return true;
}

bool QueryParser::createIndex(string tbname,string idxname,vector<string> collist,bool exist)
{
	int recnum=findrechandle(curpath+"/rec."+tbname);
	int tbid=findtableid(tbname);
	if(recnum<0)return false;
	AttrTypeList tableatr=curhead->table[tbid].fid.toatrlist();
	AttrTypeList atrlist;
	if(!extractAtrlist(curhead->table[tbid].colname,tableatr,collist,atrlist))return false;
	ix->createIndex(curpath,tbname,idxname,atrlist);
	string sstr=curpath+"/"+tbname+".idx."+idxname;
	idxhandle.push_back(make_pair(sstr,nullptr));
	int idxnum=idxhandle.size()-1;
	idxhandle[idxnum].second=ix->openIndex(curpath,tbname,idxname);
	TableIndex tbix;
	tbix.tbname=tbname;
	tbix.idxname=idxname;
	tbix.namelist=collist;
	curhead->index.push_back(tbix);
	if(exist)
	{
		int line=0;
		RecordScan rs;
		if(!rs.startScan(rechandle[recnum].second))return true;
		do
		{
			Rid rid=rs.getScanVal();
			line++;
			if(line%10000==0)cout<<"Already Insert "<<line<<" Indexes into pages "<<idxhandle[idxnum].second->head.lastpage<<endl;
			ValueList val,val2;
			DBRecord recc;
			BufType c;
			if(!rechandle[recnum].second->getRecordforScan(rid,recc))return false;
			c=recc.content;
			if(!buftovalue(c,val,tableatr))return false;
			if(!extractValue(curhead->table[tbid].colname,val,collist,val2))return false;
			idxhandle[idxnum].second->insertEntry(val2,rid);
		}while (rs.getNext());
	}
	return true;
}

bool QueryParser::dropIndex(string tbname,string idxname)
{
	int idnum=findidxhandle(curpath+"/"+tbname+".idx."+idxname);
	if(idnum<0)return false;
	ix->closeIndex(idxhandle[idnum].second);
	if(!ix->destroyIndex(curpath,tbname,idxname))return false;
	/*
	vector<string>sblist;
	sblist.push_back(tbname);
	SelectList sssss;
	sssss.aggtype=6;
	selectFromWhere(sssss,sblist,WhereClause());//*/
	swap(idxhandle[idxhandle.size()-1],idxhandle[idnum]);
	idxhandle.pop_back();
	idnum=-1;
	for (int i=0;i<(int)curhead->index.size();i++)
	{
		if(curhead->index[i].tbname==tbname&&curhead->index[i].idxname==idxname)
		{
			idnum=i;
			break;
		}
	}
	swap(curhead->index[curhead->index.size()-1],curhead->index[idnum]);
	curhead->index.pop_back();
	return true;
}

bool QueryParser::dropTable(string tbname)
{
	int recnum=findrechandle(curpath+"/rec."+tbname);
	if(recnum<0)return false;
	rm->closeFile(rechandle[recnum].second);
	if(!rm->destroyFile(curpath+"/rec."+tbname))return false;
	swap(rechandle[rechandle.size()-1],rechandle[recnum]);
	rechandle.pop_back();
	recnum=-1;
	for (int i=0;i<(int)curhead->table.size();i++)
	{
		if(curhead->table[i].tbname==tbname)
		{
			recnum=i;
			break;
		}
	}
	swap(curhead->table[curhead->table.size()-1],curhead->table[recnum]);
	curhead->table.pop_back();
	return true;
}

#endif
