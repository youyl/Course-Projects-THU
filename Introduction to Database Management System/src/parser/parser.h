#ifndef PARSER_MAIN
#define PARSER_MAIN

#include "../head.h"
#include "../rm/manager.h"
#include "../rm/handle.h"
#include "../rm/scan.h"
#include "../ix/manager.h"
#include "../ix/handle.h"
#include "../ix/scan.h"
#include <cstdlib>

using namespace std;

bool breakTableColumn(string str,string& tbname,vector<string>& cols);

class SystemHead
{
	public:
		set<string>dirname;
		SystemHead()
		{
			int num;
			dirname.clear();
			ifstream fin("dbnames.txt");
			if(!(fin>>num))
			{
				num=0;
			}
			else
			{
				string str;
				for (int i=1;i<=num;i++)
				{
					fin>>str;
					dirname.insert(str);
				}
			}
			fin.close();
		}
		~SystemHead()
		{
			ofstream fout("dbnames.txt");
			fout<<dirname.size()<<endl;
			for (auto i: dirname)
			{
				fout<<i<<endl;
			}
			fout.close();
		}
		void showdatabases()
		{
			for (auto i: dirname)
			{
				cout<<i<<endl;
			}
		}
		void insertdb(string dbname)
		{
			dirname.insert(dbname);
		}
		void removedb(string dbname)
		{
			dirname.erase(dbname);
		}
		bool find(string dbname)
		{
			if(dirname.find(dbname)==dirname.end())return false;
			return true;
		}
};

class QueryParser
{
	bool isclosed;
	public:
		string curpath;
		SystemHead *syshead;
		DatabaseHead *curhead;
		BufPageManager *bfm;
		FileManager *fm;
		RecordManager *rm;
		IndexManager *ix;
		BufType forinsert;
		vector<pair<string,IndexHandle*> >idxhandle;
		vector<pair<string,RecordHandle*> >rechandle;
		QueryParser()
		{
			isclosed=false;
			curhead=nullptr;
			syshead=new SystemHead();
			fm=new FileManager;
			bfm=new BufPageManager(fm);
			forinsert=bfm->allocMem();
			varcharstorage.readfromfile("varcharstorage.txt");
		}
		void closeBase()
		{
			if(isclosed)return;
			isclosed=true;
			if(curhead!=nullptr)curhead->writeback();
			curhead=nullptr;
			for (auto i: idxhandle)
			{
				ix->closeIndex(i.second);
			}
			idxhandle.clear();
			for (auto i: rechandle)
			{
				rm->closeFile(i.second);
			}
			rechandle.clear();
		//	bfm->close();
		//	fm->closeAll();
		}
		~QueryParser()
		{
			closeBase();
			bfm->close();
			fm->closeAll();
			varcharstorage.writetofile("varcharstorage.txt");
			delete syshead;
		}
		bool analysis(string ql)
		{
			if(ql=="SHOW DATABASES")
			{
				if(is_debug)cout<<"parsed: show databases"<<endl;
				syshead->showdatabases();
				return true;
			}
			else if(ql=="SHOW INDEXS")
			{
				if(is_debug)cout<<"parsed: show indexs"<<endl;
				if(curhead==nullptr)
				{
					cout<<"Error: no using database"<<endl;
					return false;
				}
				return curhead->showindexs();
			}
			else if(ql=="SHOW TABLES")
			{
				if(is_debug)cout<<"parsed: show tables"<<endl;
				if(curhead==nullptr)
				{
					cout<<"Error: no using database"<<endl;
					return false;
				}
				return curhead->showtables();
			}
			else if(ql.substr(0,4)=="USE ")
			{
				ql=ql.substr(4);
				if(is_debug)cout<<"parsed: use database "<<clearSpace(ql)<<endl;
				return useDatabase(clearSpace(ql));
			}
			else if(ql.substr(0,5)=="COPY ")
			{
				ql=ql.substr(5);
				string tbname,pathname;
				for (int i=0;i+5<(int)ql.length();i++)
				{
					if(ql.substr(i,6)==" FROM ")
					{
						tbname=clearSpace(ql.substr(0,i));
						pathname=clearSpace(ql.substr(i+6));
						break;
					}
				}
				if(tbname=="")
				{
					cout<<"Error: copy clause parsed failed"<<endl;
					return false;
				}
				if(is_debug)cout<<"parsed: copy "<<pathname<<" to "<<tbname<<endl;
				if(!copyFrom(tbname,pathname))return false;
			}
			else if(ql.substr(0,5)=="DESC ")
			{
				ql=ql.substr(5);
				ql=clearSpace(ql);
				if(is_debug)cout<<"parsed: describe table "<<ql<<endl;
				if(ql=="*")
				{
					curhead->showdatabase();
				}
				else if(!curhead->describetable(ql))return false;
			}
			else if(ql.substr(0,7)=="CREATE ")
			{
				ql=clearSpace(ql.substr(7));
				if(ql.substr(0,9)=="DATABASE ")
				{
					ql=clearSpace(ql.substr(9));
					if(is_debug)cout<<"parsed: create database "<<ql<<endl;
					if(!createDatabase(ql))return false;
				}
				else if(ql.substr(0,6)=="INDEX ")
				{
					ql=clearSpace(ql.substr(6));
					string tbname,idxname;
					vector<string>collist;
					if(!extractCreateIndex(ql,tbname,idxname,collist))return false;
					if(is_debug)cout<<"parsed: create index "<<tbname<<" with "<<idxname<<endl;
					if(!createIndex(tbname,idxname,collist))return false;
				}
				else if(ql.substr(0,6)=="TABLE ")
				{
					ql=clearSpace(ql.substr(6));
					string tbname,fdname;
					for (int i=0;i+1<(int)ql.length();i++)
					{
						if(ql.substr(i,1)=="(")
						{
							tbname=clearSpace(ql.substr(0,i));
							fdname=clearSpace(ql.substr(i));
							break;
						}
					}
					if(tbname=="")
					{
						cout<<"Error: create table clause parsed failed"<<endl;
						return false;
					}
					fdname=clearSpace(fdname);
					if(is_debug)cout<<"parsed: create table "<<clearSpace(tbname)<<" with fields "<<fdname<<endl;
					FieldList fdlist;
					vector<Constraint>conslist;
					if(!extractFieldFrom(fdname,tbname,fdlist,conslist))return false;
					if(!createTable(clearSpace(tbname),fdlist,conslist))return false;
				}
				else
				{
					cout<<"Failed to parse the command. -- CREATE Clause"<<endl;
					return false;
				}
			}
			else if(ql.substr(0,5)=="DROP ")
			{
				ql=clearSpace(ql.substr(5));
				if(ql.substr(0,9)=="DATABASE ")
				{
					ql=clearSpace(ql.substr(9));
					if(is_debug)cout<<"parsed: drop database "<<ql<<endl;
					if(!dropDatabase(ql))return false;
				}
				else if(ql.substr(0,6)=="INDEX ")
				{
					ql=clearSpace(ql.substr(6));
					string tbname,idxname;
					for (int i=0;i+3<(int)ql.length();i++)
					{
						if(ql.substr(i,4)==" ON ")
						{
							tbname=clearSpace(ql.substr(0,i));
							idxname=clearSpace(ql.substr(i+4));
							break;
						}
					}
					if(tbname=="")
					{
						cout<<"Error: drop index clause parsed failed"<<endl;
						return false;
					}
					if(is_debug)cout<<"parsed: drop index "<<tbname<<" with "<<idxname<<endl;
					if(!dropIndex(tbname,idxname))return false;
				}
				else if(ql.substr(0,6)=="TABLE ")
				{
					ql=clearSpace(ql.substr(6));
					if(is_debug)cout<<"parsed: drop table "<<ql<<endl;
					if(!dropTable(ql))return false;
				}
				else
				{
					cout<<"Failed to parse the command. -- DROP Clause"<<endl;
					return false;
				}
			}
			else if(ql.substr(0,12)=="INSERT INTO ")
			{
				ql=ql.substr(12);
				string tbcolstring,insvalstring;
				for (int i=0;i+6<(int)ql.length();i++)
				{
					if(ql.substr(i,7)==" VALUES")
					{
						tbcolstring=clearSpace(ql.substr(0,i));
						insvalstring=clearSpace(ql.substr(i+7));
						break;
					}
				}
				if(tbcolstring=="")
				{
					cout<<"Error: insert clause parsed failed"<<endl;
					return false;
				}
				if(is_debug)cout<<"parsed: insert target "<<tbcolstring<<" with value "<<insvalstring<<endl;
				//make up inserted value
				string tbname;
				vector<string>cols;
				ValueList val;
				if(!breakTableColumn(tbcolstring,tbname,cols))return false;
				if(!makeUpValueList(insvalstring,tbname,cols,val))return false;
				//then insert
				if(!insertEntry(tbname,val))return false;
			}
			else if(ql.substr(0,7)=="SELECT ")
			{
				ql=ql.substr(7);
				string selestr,fromstr,wherestr;
				int stage=0;
				for (int i=0;i<(int)ql.length();i++)
				{
					if(i+6<(int)ql.length()&&ql.substr(i,6)==" FROM ")
					{
						stage++;
						i+=5;
					}
					else if(i+7<(int)ql.length()&&ql.substr(i,7)==" WHERE ")
					{
						stage++;
						i+=6;
					}
					else
					{
						if(stage==0)
						{
							selestr.push_back(ql[i]);
						}
						else if(stage==1)
						{
							fromstr.push_back(ql[i]);
						}
						else
						{
							wherestr.push_back(ql[i]);
						}
					}
				}
				selestr=clearSpace(selestr);
				fromstr=clearSpace(fromstr);
				wherestr=clearSpace(wherestr);
				if(is_debug)cout<<"parsed: S-F-W : "<<selestr<<" | "<<fromstr<<" | "<<wherestr<<endl;
				vector<string>tblist;
				WhereClause whc;
				SelectList sele;
				if(wherestr!="")if(!findWhereClause(wherestr,"---",whc))return false;
				if(!getSelector(selestr,sele))return false;
				if(!getFromClause(fromstr,tblist))return false;
				if(!selectFromWhere(sele,tblist,whc))return false;
			}
			else if(ql.substr(0,7)=="UPDATE ")
			{
				ql=ql.substr(7);
				string tbname,ststring;
				SetClause stc;
				for (int i=0;i+4<(int)ql.length();i++)
				{
					if(ql.substr(i,5)==" SET ")
					{
						tbname=clearSpace(ql.substr(0,i));
						ststring=clearSpace(ql.substr(i+5));
						break;
					}
				}
				if(tbname=="")
				{
					cout<<"Error: update clause parsed failed"<<endl;
					return false;
				}
				if(is_debug)cout<<"parsed: update table "<<tbname<<" with setitem "<<ststring<<endl;
				if(!breakUpdateSet(ststring,tbname,stc))return false;
				if(!updateSet(tbname,stc))return false;
			}
			else if(ql.substr(0,12)=="DELETE FROM ")
			{
				ql=ql.substr(12);
				string tbname,wherestr;
				WhereClause wc;
				if(ql.find(" WHERE ")==string::npos)
				{
					tbname=ql;
					wherestr="";
				}
				else
				{
					for (int i=0;i+6<(int)ql.length();i++)
					{
						if(ql.substr(i,7)==" WHERE ")
						{
							tbname=clearSpace(ql.substr(0,i));
							wherestr=clearSpace(ql.substr(i+7));
							break;
						}
					}
				}
				if(is_debug)cout<<"parsed: delete from table "<<tbname<<" with "<<wherestr<<endl;
				if(!findWhereClause(wherestr,tbname,wc))return false;
				if(!deleteFromWhere(tbname,wc))return false;
			}
			else if(ql.substr(0,6)=="ALTER ")
			{
				if(ql.substr(0,22)=="ALTER ADD FOREIGN KEY ")
				{
					ql=ql.substr(22);
					ql=clearSpace(ql);
					//get csname;
					string csname,twotbc;
					for (int i=0;i<(int)ql.length();i++)
					{
						if(ql[i]==' ')
						{
							twotbc=clearSpace(ql.substr(i));
							break;
						}
						else
						{
							csname.push_back(ql[i]);
						}
					}
					csname=clearSpace(csname);
					if(is_debug)cout<<"alter add foreign key "<<csname<<" with tbcs like "<<twotbc<<endl;
					//break two tb(col)
					Constraint cs;
					cs.constype=1;
					cs.consname=csname;
					string tc1,tc2;
					for (int i=0;i+1<(int)twotbc.length();i++)
					{
						if(twotbc[i]==')')
						{
							tc1.push_back(twotbc[i]);
							tc2=twotbc.substr(i+1);
							break;
						}
						else
						{
							tc1.push_back(twotbc[i]);
						}
					}
					tc1=clearSpace(tc1);
					tc2=clearSpace(tc2);
					if(is_debug)cout<<"alter add foreign key "<<csname<<" with tbcs like "<<tc1<<" and "<<tc2<<endl;
					if(!breakTableColumn(tc1,cs.tbname1,cs.collist1))return false;
					if(!breakTableColumn(tc2,cs.tbname2,cs.collist2))return false;
					if(!addForeighKey(cs))return false;
				}
				else if(ql.substr(0,22)=="ALTER ADD PRIMARY KEY ")
				{
					ql=ql.substr(22);
					ql=clearSpace(ql);
					//break tb(col)
					string tbname;
					vector<string>col;
					if(!breakTableColumn(ql,tbname,col))return false;
					if(!addPrimaryKey(tbname,col))return false;
				}
				else if(ql.substr(0,23)=="ALTER DROP FOREIGN KEY ")
				{
					ql=ql.substr(23);
					ql=clearSpace(ql);
					if(!dropForeignKey(ql))return false;
				}
				else if(ql.substr(0,23)=="ALTER DROP PRIMARY KEY ")
				{
					ql=ql.substr(23);
					ql=clearSpace(ql);
					if(!dropPrimaryKey(ql))return false;
				}
				else
				{
					cout<<"Failed to parse the command. -- Alter Clause"<<endl;
					return false;
				}
			}
			else
			{
				cout<<"Failed to parse the command. -- initialize failed"<<endl;
				return false;
			}
			return true;
		}
		bool createDatabase(string dbname)
		{
			if(syshead->find(dbname))return false;
			syshead->insertdb(dbname);
			dbname="db."+dbname;
			system(("mkdir "+dbname).c_str());
			ofstream fout((dbname+"/_dbhead.txt").c_str());
			fout<<0<<' '<<0<<' '<<0<<endl;
			fout.close();
			return true;
		}
		bool dropDatabase(string dbname)
		{
			if(!syshead->find(dbname))
			{
				cout<<"Error: Db not found "<<dbname<<endl;
				return false;
			}
			syshead->removedb(dbname);
			dbname="db."+dbname;
			if(dbname==curpath)closeBase();
			dbname="rd /s "+dbname;
			system(dbname.c_str());
			return true;
		}
		void openRecIdx()
		{
			for (auto th: curhead->table)
			{
				rechandle.push_back(make_pair(curpath+"/rec."+th.tbname,nullptr));
				int recnum=rechandle.size()-1;
				rechandle[recnum].second=rm->openFile(curpath+"/rec."+th.tbname);
			}
			for (auto ixx: curhead->index)
			{
				idxhandle.push_back(make_pair(curpath+"/"+ixx.tbname+".idx."+ixx.idxname,nullptr));
				int idxnum=idxhandle.size()-1;
				idxhandle[idxnum].second=ix->openIndex(curpath,ixx.tbname,ixx.idxname);
			}
		}
		bool useDatabase(string dbname)
		{
			if(!syshead->find(dbname))
			{
				cout<<"Error: Db not found "<<dbname<<endl;
				return false;
			}
			dbname="db."+dbname;
			if(dbname==curpath)
			{
				cout<<"Error: database already used"<<endl;
				return false;
			}
			if(curhead!=nullptr)closeBase();
			curhead=new DatabaseHead(dbname);
			curpath=dbname;
			isclosed=false;
			rm=new RecordManager(fm,bfm);
			ix=new IndexManager(fm,bfm);
			openRecIdx();
			return true;
		}
		int findidxhandle(string sstr)
		{
			for (int i=0;i<(int)idxhandle.size();i++)
			{
				if(idxhandle[i].first==sstr)return i;
			}
			return -1;
		}
		int findrechandle(string sstr)
		{
			for (int i=0;i<(int)rechandle.size();i++)
			{
				if(rechandle[i].first==sstr)return i;
			}
			return -1;
		}
		int findtableid(string sstr)
		{
			for (int i=0;i<(int)curhead->table.size();i++)
			{
				if(curhead->table[i].tbname==sstr)return i;
			}
			return -1;
		}
		int findindexid(string tbname,string idxname)
		{
			for (int i=0;i<(int)curhead->index.size();i++)
			{
				if(curhead->index[i].tbname==tbname&&curhead->index[i].idxname==idxname)return i;
			}
			return -1;
		}
		bool getTypefromColumn(Column col,int& type);
		bool getFieldfromColumn(Column col,Field& fid);
		bool fitrid(Rid rid,string tbname,WhereClause wherec);
		//------------------------------create------------------------------------------
		bool extractFieldFrom(string str,string tbname,FieldList& fdlist,vector<Constraint>& conslist);
		bool createTable(string tbname,FieldList fdlist,vector<Constraint> conslist);
		bool extractCreateIndex(string str,string& tbname,string& idxname,vector<string>& collist);
		bool createIndex(string tbname,string idxname,vector<string> collist,bool exist=true);
		//------------------------------drop--------------------------------------------
		bool dropIndex(string tbname,string idxname);
		bool dropTable(string tbname);
		//------------------------------insert------------------------------------------
		bool makeUpValueList(string insvalstring,string tbname,vector<string>cols,ValueList& val);
		bool insertEntry(string tbname,ValueList val);
		bool copyFrom(string tbname,string pathname);
		//------------------------------delete------------------------------------------
		bool findWhereClause(string str,string tbname,WhereClause& wherec);
		bool deleteByRid(string tbname,Rid rid);
		bool deleteFromWhere(string tbname,WhereClause wherec);
		//------------------------------select------------------------------------------
		bool getFromClause(string str,vector<string>& tblist); 
		bool getSelector(string str,SelectList& sele);
		bool selectFromWhere(SelectList sele,vector<string>tblist,WhereClause wherec);
		//------------------------------update------------------------------------------
		bool breakUpdateSet(string str,string tbname,SetClause& stc);
		bool updateSet(string tbname,SetClause setc);
		//------------------------------alter-------------------------------------------
		bool duplicate(string tbname,string idxname);
		bool addPrimaryKey(string tbname,vector<string>col);
		bool addForeighKey(Constraint cs);
		bool dropPrimaryKey(string tbname);
		bool dropForeignKey(string csname);
		//give up when necessary
};

#endif 
