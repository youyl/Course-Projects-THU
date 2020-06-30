#ifndef INDEX_MANAGER
#define INDEX_MANAGER

#include "../pf/bufmanager/BufPageManager.h"
#include "../pf/fileio/FileManager.h"
#include "../head.h"

class IndexHandle;

class IndexManager
{
	public:
		FileManager *fm;
		BufPageManager *bfm;
		map<string,int>fname;
		IndexManager(FileManager *_fm,BufPageManager *_bfm)
		{
			fm=_fm;
			bfm=_bfm;
		}
		~IndexManager(){}
		void createIndex(string curpath,const string& filename,string indexNo,AttrTypeList _atrlist)
		{
			string indexname=curpath+"/"+filename+".idx."+indexNo;
			fm->createFile(indexname.c_str());
			int tmpidx,bufid,tmp=3;
			fm->openFile(indexname.c_str(),tmpidx);
			fname[indexname]=tmpidx;
			BufType b=bfm->allocPage(tmpidx,0,bufid);
			b[0]=0;
			b[1]=0;
			b[2]=_atrlist.size;
			b[3]=_atrlist.length;
			for (int i=0;i<_atrlist.size;i++)
			{
				tmp++;
				b[tmp]=_atrlist.list[i].type;
				tmp++;
				b[tmp]=_atrlist.list[i].offset;
				tmp++;
				b[tmp]=_atrlist.list[i].length;
			}
			bfm->markDirty(bufid);
		}
		bool destroyIndex(string curpath,const string& filename,string indexNo)
		{
			string indexname=curpath+"/"+filename+".idx."+indexNo;
			if(fname.find(indexname)==fname.end())
			{
				cout<<"Error: fail to find index to destroy"<<endl;
				return false;
			}
		//	bfm->close();fm->closeBeforeRemoval(fname[indexname]);
			fname.erase(fname.find(indexname));
		//	if(remove(indexname.c_str())==0)return true;return false;
			return true;
		}
		void closeIndex(IndexHandle *handle);
		IndexHandle* openIndex(string curpath,const string& filename,string indexNo);
};

#endif
