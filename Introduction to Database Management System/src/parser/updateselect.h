#ifndef PARSER_UPDATESELECT
#define PARSER_UPDATESELECT

#include "parser.h"
#include "insdel.h"


bool QueryParser::breakUpdateSet(string str,string tbname,SetClause& stc)
{
	stc.col.clear();
	stc.val.clear();
	vector<string>strlist=getstrlist(str,false);
	int tbid=findtableid(tbname);
	FieldList fd=curhead->table[tbid].fid;
	for (auto ss: strlist)
	{
		string str1,str2;
		for (int i=0;i<(int)ss.length();i++)
		{
			if(ss[i]=='=')
			{
				str2=clearSpace(ss.substr(i+1));
				break;
			}
			str1.push_back(ss[i]);
			str1=clearSpace(str1);
		}
		if(str2=="")return false;
		Value val;
		bool flag=false;
		for (auto field: fd.list)
		{
			if(field.fieldname==str1)
			{
				if(!stringtovalue(str2,field.type,val))return false;
				flag=true;
				break;
			}
		}
		if(!flag)return false;
		stc.val.push_back(val);
		stc.col.push_back(str1);
	}
	return true;
}

bool QueryParser::updateSet(string tbname,SetClause setc)
{
	RecordScan rs;
	int recnum=findrechandle(curpath+"/rec."+tbname);
	rs.startScan(rechandle[recnum].second);
	int tbid=findtableid(tbname);
	FieldList fd=curhead->table[tbid].fid;
	vector<Rid>riD;
	do
	{
		Rid ridd=rs.getScanVal();
		riD.push_back(ridd);
	}while (rs.getNext());
//	is_sudden_debug=true;
	for (auto rid: riD)
	{
		//take it out
		ValueList val;
		DBRecord recc;
		BufType c;
		if(!rechandle[recnum].second->getRecordforScan(rid,recc))return false;
		c=recc.content;
		if(!buftovalue(c,val,fd.toatrlist()))return false;
		//make value change
		for (int i=0;i<(int)setc.col.size();i++)
		{
			string c1=setc.col[i];
			Value v1=setc.val[i];
			for (int j=0;j<(int)fd.list.size();j++)
			{
				if(fd.list[j].fieldname==c1)
				{
					val.list[j]=v1;
				}
			}
		}
		//delete by rid, and insert by value;
		if(!deleteByRid(tbname,rid))return false;
		if(!insertEntry(tbname,val))return false;
	}
	return true;
}

bool fitWhereItem(vector<Column>cols,ValueList val,WhereItem witem)
{
	Column c1=witem.col1;
	bool isv1=false,isv2=false;
	Value v1,v2;
	for (int i=0;i<(int)cols.size();i++)
	{
		if(cols[i]==c1)
		{
			isv1=true;
			v1=val.list[i];
			break;
		}
	}
	if(isv1==false)return true;
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
		isv2=true;
		v2=witem.val;
	}
	else
	{
		c1=witem.col2;
		for (int i=0;i<(int)cols.size();i++)
		{
			if(cols[i]==c1)
			{
				isv2=true;
				v2=val.list[i];
				break;
			}
		}
	}
	if(isv2==false)return true;
	return CompOp::judge(witem.cmpop,ValueList(v1),ValueList(v2));
}

bool fitWhereClause(vector<Column>cols,ValueList val,WhereClause wherec)
{
	for (auto itm: wherec.list)
	{
		if(!fitWhereItem(cols,val,itm))return false;
	}
	return true;
}

bool QueryParser::getFromClause(string str,vector<string>& tblist)
{
	tblist=getstrlist(str,false);
	return true;
}
bool QueryParser::getSelector(string str,SelectList& sele)
{
	str=clearSpace(str);
	if(str=="*")
	{
		sele.aggtype=6;
	}
	else if(str=="COUNT(*)")
	{
		sele.aggtype=2;
	}
	else if(str.substr(0,7)=="AVERAGE")
	{
		str=str.substr(7);
		sele.aggtype=5;
		if(str[0]!='('||str[str.length()-1]!=')')return false;
		str=str.substr(1,str.length()-2);
		sele.aggcol=getColumn("***",str);
		if(!getTypefromColumn(sele.aggcol,sele.valuetype))
		{
			cout<<"Error: AGG column not exist"<<endl;
			return false;
		}
	}
	else if(str.substr(0,3)=="MAX")
	{
		str=str.substr(3);
		sele.aggtype=3;
		if(str[0]!='('||str[str.length()-1]!=')')return false;
		str=str.substr(1,str.length()-2);
		sele.aggcol=getColumn("***",str);
		if(!getTypefromColumn(sele.aggcol,sele.valuetype))
		{
			cout<<"Error: AGG column not exist"<<endl;
			return false;
		}
	}
	else if(str.substr(0,3)=="MIN")
	{
		str=str.substr(3);
		sele.aggtype=4;
		if(str[0]!='('||str[str.length()-1]!=')')return false;
		str=str.substr(1,str.length()-2);
		sele.aggcol=getColumn("***",str);
		if(!getTypefromColumn(sele.aggcol,sele.valuetype))
		{
			cout<<"Error: AGG column not exist"<<endl;
			return false;
		}
	}
	else if(str.substr(0,3)=="SUM")
	{
		str=str.substr(3);
		sele.aggtype=1;
		if(str[0]!='('||str[str.length()-1]!=')')return false;
		str=str.substr(1,str.length()-2);
		sele.aggcol=getColumn("***",str);
		if(!getTypefromColumn(sele.aggcol,sele.valuetype))
		{
			cout<<"Error: AGG column not exist"<<endl;
			return false;
		}
	}
	else//no agg function
	{
		sele.aggtype=0;
		vector<string>strlist=getstrlist(str,false);
		for (int i=0;i<(int)strlist.size();i++)
		{
			Column cc=getColumn("***",strlist[i]);
			if(cc.tbname=="***")return false;
			sele.cols.push_back(cc);
		}
	}
	if(sele.aggcol.tbname=="***")return false;
	return true;
}


bool QueryParser::fitrid(Rid rid,string tbname,WhereClause wherec)
{
	int recnum=findrechandle(curpath+"/rec."+tbname);
	int tbid=findtableid(tbname);
	if(recnum<0)return false;
	FieldList fd=curhead->table[tbid].fid;
	ValueList val;
	DBRecord recc;
	BufType c;
	if(!rechandle[recnum].second->getRecordforScan(rid,recc))return false;
	c=recc.content;
	if(!buftovalue(c,val,fd.toatrlist()))return false;
	vector<Column>col;
	for (int i=0;i<(int)fd.list.size();i++)
	{
		col.push_back(Column(tbname,fd.list[i].fieldname));
	}
	return fitWhereClause(col,val,wherec);
}

bool QueryParser::selectFromWhere(SelectList sele,vector<string>tblist,WhereClause wherec)
{
	if(is_debug)cout<<"Entering S-F-W process"<<endl;
	vector<vector<Rid> >res;
	res.resize(tblist.size());
	
	for (int i=0;i<(int)tblist.size();i++)
	{
		WhereItem wc;
		bool flag=false;
		string tbname=tblist[i],idxname;
		for (int j=0;j<(int)wherec.list.size();j++)
		{
			if(wherec.list[j].colcnt==1)
			{
				if(wherec.list[j].col1.tbname==tbname)
				{
					if(curhead->existidx(tbname,wherec.list[j].col1.colname,idxname))
					{
						flag=true;
						wc=wherec.list[j];
						break;
					}
				}
			}
		}
		if(flag)
		{
			//scan idx
			if(is_debug)cout<<"S-F-W Scan for Index"<<endl;
			int idxnum=findidxhandle(curpath+"/"+tbname+".idx."+idxname);
			if(idxnum<0)return false;
			IndexHandle *ixh=idxhandle[idxnum].second;
			IndexScan ixscan;
			if(!ixscan.openScan(ixh,wc.cmpop,ValueList(wc.val)))
			{
				cout<<"Error: Index Scan Open Failed, No Record availiable"<<endl;
				return false;
			}
			do
			{
				Rid rid=ixscan.getEntry();
				if(fitrid(rid,tbname,wherec))res[i].push_back(rid);
			}while(ixscan.nextEntry());
		}
		else
		{
			//scan record
			if(is_debug)cout<<"S-F-W Scan for Record"<<endl;
			int recnum=findrechandle(curpath+"/rec."+tbname);
			if(recnum<0)return false;
			RecordHandle *rh=rechandle[recnum].second;
			RecordScan rs;
			if(!rs.startScan(rh))
			{
				cout<<"Error: Record Scan Open Failed, No Record availiable"<<endl;
				return false;
			}
			do
			{
				Rid rid=rs.getScanVal();
				//cout<<"rid scan result "<<rid.pageID<<' '<<rid.slotID<<endl;
				if(fitrid(rid,tbname,wherec))res[i].push_back(rid);
			}while(rs.getNext());
		}
	}
	vector<int>id;
	for (int i=0;i<(int)tblist.size();i++)
	{
		id.push_back(0);
		if(is_debug);cout<<res[i].size()<<endl;
		if(res[i].size()==0)
		{
			cout<<"S-F-W process is Over, result is shown above."<<endl;
			return true;
		}
	}
	while (1)
	{
		//combine them together
		vector<Column>col;
		ValueList val;
		for (int i=0;i<(int)tblist.size();i++)
		{
			int tbid=findtableid(tblist[i]);
			FieldList fd=curhead->table[tbid].fid;
			int recnum=findrechandle(curpath+"/rec."+tblist[i]);
			Rid rid=res[i][id[i]];
			ValueList vv;
			DBRecord recc;
			BufType c;
			if(!rechandle[recnum].second->getRecordforScan(rid,recc))return false;
			c=recc.content;
			if(!buftovalue(c,vv,fd.toatrlist()))return false;
			for (int j=0;j<(int)fd.list.size();j++)
			{
				col.push_back(Column(tblist[i],fd.list[j].fieldname));
				val.list.push_back(vv.list[j]);
			}
		}
		//check if valid and print
		if(fitWhereClause(col,val,wherec))
		{
			if(sele.aggtype==6)
			{
				for (int j=0;j<(int)col.size();j++)
				{
					val.list[j].print();//cout<<" "<<val.list[j].valuetype<<" ";
					cout<<" | ";
				}
				cout<<endl;
			}
			else if(sele.aggtype!=0)
			{
				Value va;
				for (int i=0;i<(int)col.size();i++)
				{
					if(col[i]==sele.aggcol||sele.aggtype==2)
					{
						va=val.list[i];
						break;
					}
				}
				sele.add(va);
			}
			else
			{
				for (int i=0;i<(int)sele.cols.size();i++)
				{
					for (int j=0;j<(int)col.size();j++)
					{
						if(col[j]==sele.cols[i])
						{
							val.list[j].print();
							cout<<" | ";
						}
					}
				}
				cout<<endl;
			}
		}
		//get next one
		int tail=tblist.size()-1;
		while (tail!=-1&&id[tail]==(int)(res[tail].size())-1)
		{
			id[tail]=0;
			tail--;
		}
		if(tail==-1)break;
		id[tail]++;
	}
	if(sele.aggtype!=0&&sele.aggtype!=6)
	{
		cout<<"AGG Function Result:"<<endl;
		sele.print();
	}
	else
	{
		cout<<"S-F-W process is Over, result is shown above."<<endl;
	}
	return true;
}

#endif
