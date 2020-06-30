#ifndef RECORD_HANDLE
#define RECORD_HANDLE

#include "manager.h"

class RecordHandle
{
	public:
		RecordManager *rm;
		BufPageManager *bfm;
		FileManager *fm;
		int reStartPos,reSize,reNum,pageNum;
		int fileid;
		int _idx1(int x)
		{
			return (x>>5);
		}
		int _idx2(int x)
		{
			return (x&31);
		}
		void _freeFile()
		{
			int bufid;
			BufType b=bfm->getPage(fileid,0,bufid);
			b[0]=reStartPos;
			b[1]=reSize;
			b[2]=reNum;
			b[3]=pageNum;
			bfm->markDirty(bufid);
			if(is_debug)printf("file free attributes: %d %d %d %d\n",reStartPos,reSize,reNum,pageNum);
		}
		void _updatePage(int page,bool val)
		{
		//	if(is_debug)cout<<"record update page "<<page<<' '<<val<<endl;
			int bufid,pageid;
			pageid=(page-(page-1)%65537);
			int offset=page-pageid-1;
			BufType b=bfm->getPage(fileid,pageid,bufid);
			b[_idx1(offset)]|=(1u<<_idx2(offset));
			if(!val)
			{
				b[_idx1(offset)]-=(1u<<_idx2(offset));
			}
			bfm->markDirty(bufid);
		}
		RecordHandle(){}
		~RecordHandle(){}
		void init(RecordManager *_rm,int _fileid)
		{
			fileid=_fileid;
			rm=_rm;
			fm=_rm->fm;
			bfm=_rm->bfm;
			int bufid;
			BufType b=bfm->getPage(fileid,0,bufid);
			reStartPos=b[0];
			reSize=b[1];
			reNum=b[2];
			pageNum=b[3];
			if(is_debug)printf("file handle ready attributes: %d %d %d %d\n",reStartPos,reSize,reNum,pageNum);
		}
		bool getRecordforScan(const Rid& rid,DBRecord &rec)
		{
			int bufid,slotid=rid.getSlotID(),pageid=rid.getPageID();
			BufType b=bfm->getPage(fileid,pageid,bufid);
			if(b[reStartPos+slotid]==0)return false;
			/*BufType c=allocMem();
			for (int i=0;i<reSize/4;i++)
			{
				c[i]=b[reSize*slotid/4+i];
			}*/
			rec.setRid(rid);
			rec.setContent((b+(reSize*slotid/4)));
			return true;
		}
		bool deleteRecord(const Rid& rid)
		{
			int bufid,slotid=rid.getSlotID(),pageid=rid.getPageID();
			BufType b=rm->bfm->getPage(fileid,pageid,bufid);
			int flgpos=reStartPos+slotid;
			if(b[flgpos]==0)
			{
				cout<<"Error: you are deleting an empty record"<<endl;
				return false;
			}
			b[flgpos]=0;
			_updatePage(pageid,false);
			bfm->markDirty(bufid);
			return true;
		}
		bool insertRecord(BufType buf,Rid& rid)
		{
			int bufid,slotid,pageid=-1,pagenum=pageNum;
			for (int i=1;i<=pagenum;i+=65537)
			{
				BufType b=rm->bfm->getPage(fileid,i,bufid);
				if(i==pagenum)
				{
					for (int j=0;j<PAGE_SIZE/4;j++)
					{
						b[j]=0;
					}
					bfm->markDirty(bufid);
					pageid=i+1;
					break;
				}
				for (int id=0;id<65536;id++)
				{
					if((b[_idx1(id)]&(1u<<_idx2(id)))==0)
					{
						pageid=i+id+1;
						break;
					}
				}
				if(pageid!=-1)break;
			}
			if(pageid>=pagenum)pageNum=pageid+1;
			BufType b=rm->bfm->getPage(fileid,pageid,bufid);
			for (int i=reStartPos;i<reStartPos+reNum;i++)
			{
				if(b[i]==0)
				{
					slotid=i-reStartPos;
					break;
				}
			}
		//	if(is_debug)cout<<"record insertion start "<<reSize*slotid/4<<" end "<<reSize*(slotid+1)/4<<endl;
			for (int i=reSize*slotid/4;i<reSize*(slotid+1)/4;i++)
			{
				b[i]=buf[i-reSize*slotid/4];
			}
			int flgpos=reStartPos+slotid;
		//	if(is_debug)cout<<"record insertion flgpos "<<flgpos<<endl;
			b[flgpos]=1;
			bool isfull=true;
			for (int i=reStartPos;i<reStartPos+reNum;i++)
			{
				if(b[i]==0)
				{
					isfull=false;
					break;
				}
			}
			if(isfull)_updatePage(pageid,true);
			rm->bfm->markDirty(bufid);
			rid=Rid(pageid,slotid);
			return true;
		}
};

RecordHandle* RecordManager::openFile(const string& filename)
{
	int idx=fname[filename];
	RecordHandle* rh=new RecordHandle();
	rh->init(this,idx);
	return rh;
}

bool RecordManager::closeFile(RecordHandle *handle)
{
	handle->_freeFile();
	return true;
}

#endif
