#ifndef RECORD_MANAGER
#define RECORD_MANAGER

#include "../pf/bufmanager/BufPageManager.h"
#include "../pf/fileio/FileManager.h"
#include <string>
#include <map>
#include <vector>
#include "../compare.h"
#include "../head.h"

class RecordHandle;

class RecordManager
{
	public: //tends to be private
		map<string,int>fname;
		map<int,string>stringfname;
		BufPageManager *bfm;
		FileManager *fm;
		void _initFile(const string& filename,int recordSize)
		{
			int fileid,bufid;
			fm->openFile(filename.c_str(),fileid);
			fname[filename]=fileid;
			if(is_debug)cout<<"create file: "<<filename<<" with id "<<fileid<<endl;
			stringfname[fileid]=filename;
			BufType b=bfm->getPage(fileid,0,bufid);
			int renum=PAGE_SIZE/(recordSize+4);
			b[0]=renum*recordSize/4;
			b[1]=recordSize;
			b[2]=renum;
			b[3]=2;
			bfm->markDirty(bufid);
			b=bfm->getPage(fileid,1,bufid);
			for (int i=0;i<PAGE_SIZE/4;i++)
			{
				b[i]=0;
			}
			bfm->markDirty(bufid);
		}
	public:
		RecordManager(FileManager *_fm,BufPageManager *_bfm)
		{
			fname.clear();
			fm=_fm;
			bfm=_bfm;
		}
		~RecordManager(){}
		RecordHandle* openFile(const string& filename);
		bool closeFile(RecordHandle *handle);
		bool createFile(const string& filename,int recordSize)
		{
			if(recordSize%4!=0)
			{
				recordSize-=recordSize%4;
				recordSize+=4;
			}
			if(recordSize>=PAGE_SIZE)
			{
				cout<<"Error: too large record size"<<endl;
				return false;
			}
			fm->createFile(filename.c_str());
			_initFile(filename,recordSize);
			return true;
		}
		bool destroyFile(const string& filename)
		{
			if(fname.find(filename)==fname.end())
			{
				cout<<"Error: filename not fit when destroy record file"<<endl;
				return false;
			}
		//	bfm->close();fm->closeBeforeRemoval(fname[filename]);
			fname.erase(fname.find(filename));
		//	if(remove(filename.c_str())==0)return true;return false;
			return true;
		}
};

#endif
