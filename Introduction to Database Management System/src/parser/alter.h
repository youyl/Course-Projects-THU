#ifndef PARSER_ALTER
#define PARSER_ALTER

#include "parser.h"
#include "updateselect.h"
/*
sqlquery:
	COPY tbname FROM path ---------------------------
	CREATE DATABASE dbname ------------------------------
	CREATE TABLE tbname ( fieldlist ) ---------------------------
	CREATE INDEX idxname ON tbname ( collist ) ---------------------------
	USE dbname --------------------------
	DROP DATABASE dbname XXXXXXXXXXXXXXXXXXXXXXXXX
	DROP TABLE tbname -------------------------
	DROP INDEX idxname ON tbname --------------------
	SHOW DATABASES ---------------------
	SHOW INDEXS -----------------------
	SHOW TABLES -----------------------
	DESC tbname ------------------------
	INSERT INTO tbname ( collist ) VALUES ( valuelist ) ---------------------------------
	SELECT selector FROM fromclause WHERE whereclause --------------------------------
	SELECT selector FROM fromclause ----------------------------------------
	UPDATE tbname SET setclause -----------------------------------------
	DELETE FROM tbname WHERE whereclause ---------------------------------
	DELETE FROM tbname ---------------------------------
	ALTER ADD PRIMARY KEY tbname(collist) -------------------------------
	ALTER ADD FOREIGN KEY csname tbname(collist) tbname(collist) ---------------------------
	ALTER DROP FOREIGN KEY csname ----------------------------------
	ALTER DROP PRIMARY KEY tbname ----------------------------------
sqlquery:
	USE dbname
	SHOW DATABASES
	CREATE DATABASE dbname
	DROP DATABASE dbname
	SHOW INDEXS
	SHOW TABLES
	DESC tbname
	CREATE TABLE tbname ( fieldlist )
	DROP TABLE tbname
	CREATE INDEX idxname ON tbname ( collist )
	DROP INDEX idxname ON tbname
	COPY tbname FROM path 
	INSERT INTO tbname ( collist ) VALUES ( valuelist )
	SELECT selector FROM fromclause WHERE whereclause
	SELECT selector FROM fromclause
	UPDATE tbname SET setclause WHERE whereclause
	UPDATE tbname SET setclause
	DELETE FROM tbname WHERE whereclause
	DELETE FROM tbname
*/

bool QueryParser::duplicate(string tbname,string idxname)
{
	int idxnum=findidxhandle(curpath+"/"+tbname+".idx."+idxname);
	if(idxnum<0)
	{
		cout<<"Error: Look at line 60 in alter.h"<<endl;
		return false;
	}
	return idxhandle[idxnum].second->checkDuplicate();
}

bool QueryParser::addPrimaryKey(string tbname,vector<string>col)
{
	//checkvalid in cons
	Constraint cs;
	cs.consname="unnamed-pm-key";
	cs.constype=0;
	cs.tbname1=tbname;
	cs.collist1=col;
	curhead->cons.push_back(cs);
	if(!curhead->checkconsvalid())
	{
		cout<<"Error: Add constraint failed"<<endl;
		curhead->cons.pop_back();
		return false;
	}
	//checkvalid in index(build, if wrong, dropindex)
	if(!createIndex(tbname,"pm.key",col,true))
	{
		cout<<"Error: create index failed when add primary key"<<endl;
		curhead->cons.pop_back();
		return false;
	}
	if(duplicate(tbname,"pm.key"))
	{
		if(!dropIndex(tbname,"pm.key"))cout<<"Error: Danger! fail to stop this error"<<endl;
		return false;
	}
	return true;
}

bool QueryParser::addForeighKey(Constraint cs)
{
	//checkvalid in cons
	curhead->cons.push_back(cs);
	if(!curhead->checkconsvalid())
	{
		cout<<"Error: Add constraint failed"<<endl;
		curhead->cons.pop_back();
		return false;
	}
	curhead->cons.pop_back();
	//checkvalid among cons tb1(col1) tb2(col2), col2 is pk of tb2, check each tb1(col1) in tb2(col2)
	//iterator over tb1
	int recnum=findrechandle(curpath+"/rec."+cs.tbname1);
	int idxnum=findidxhandle(curpath+"/"+cs.tbname2+".idx.pm.key");
	int tbid1=findtableid(cs.tbname1);
	if(recnum<0||idxnum<0)
	{
		cout<<"Error: constraint's table name"<<endl;
		return false;
	}
	RecordScan rs;
	if(rs.startScan(rechandle[recnum].second))
	{
		do
		{
			Rid rid=rs.getScanVal();
			//get valuelist
			ValueList val,val2;
			DBRecord recc;
			BufType c;
			if(!rechandle[recnum].second->getRecordforScan(rid,recc)){if(is_debug)cout<<"err1"<<endl;return false;}
			c=recc.content;
			if(!buftovalue(c,val,curhead->table[tbid1].fid.toatrlist())){if(is_debug)cout<<"err2"<<endl;return false;}
			//use extractvalue to getvalue
			if(!extractValue(curhead->table[tbid1].colname,val,cs.collist1,val2)){if(is_debug)cout<<"err3"<<endl;return false;}
			if(!idxhandle[idxnum].second->exist(val2))
			{
				cout<<"Error: fk constraint broken"<<endl;
				return false;
			}
		}while(rs.getNext());
	}
	//add cs to list
	curhead->cons.push_back(cs);
	return true;
}

bool QueryParser::dropPrimaryKey(string tbname)
{
	if(is_debug)cout<<"drop primary key "<<tbname<<endl;
	int pd=-1;
	for (int i=0;i<(int)curhead->cons.size();i++)
	{
		if(curhead->cons[i].constype==0&&curhead->cons[i].tbname1==tbname)
		{
			pd=i;
			break;
		}
	}
	if(pd==-1)
	{
		cout<<"Error: No Primary Key Found"<<endl;
		return false;
	}
	Constraint cs=curhead->cons[pd];
	swap(curhead->cons[pd],curhead->cons[curhead->cons.size()-1]);
	curhead->cons.pop_back();
	if(!curhead->checkconsvalid())
	{
		cout<<"Error: unable to drop primary key"<<endl;
		curhead->cons.push_back(cs);
		return false;
	}
	if(!dropIndex(tbname,"pm.key"))return false;
	return true;
}

bool QueryParser::dropForeignKey(string csname)
{
	if(is_debug)cout<<"drop foreign key "<<csname<<endl;
	int pd=-1;
	for (int i=0;i<(int)curhead->cons.size();i++)
	{
		if(curhead->cons[i].consname==csname)
		{
			pd=i;
			break;
		}
	}
	if(pd==-1)
	{
		cout<<"Error: No Such Constraint Found"<<endl;
		return false;
	}
	swap(curhead->cons[pd],curhead->cons[curhead->cons.size()-1]);
	curhead->cons.pop_back();
	return true;
}
#endif
