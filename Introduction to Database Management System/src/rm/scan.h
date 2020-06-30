#ifndef RECORD_SCAN
#define RECORD_SCAN

#include "manager.h"
#include "handle.h"

class RecordScan
{
	public: //tends to be private
		Rid cur;
		RecordHandle *handle;
		BufPageManager *bfm;
		int bufPage,curPage,bitmapBuf,bitmapPage,bitmapid;
		BufType bfp,btp;
		int _idx1(int x)
		{
			return (x>>5);
		}
		int _idx2(int x)
		{
			return (x&31);
		}
	public:
		RecordScan(){}
		~RecordScan(){}
		bool getNext()
		{
			int slotid=cur.getSlotID(),bufid;
		//	cout<<curPage<<' '<<slotid<<endl;
			bool flag=false;
			bfp=bfm->getPage(handle->fileid,curPage,bufPage);
			for (int i=slotid+1;i<handle->reNum;i++)
			{
				if(bfp[(handle->reStartPos)+i]!=0)
				{
					cur=Rid(curPage,i);
					flag=true;
					break;
				}
			}
			if(flag)return true;
			curPage=-1;
			for (int id=cur.pageID+1;id<handle->pageNum;id++)
			{
				if(id%65537==1)continue;
				bfp=bfm->getPage(handle->fileid,id,bufid);
				for (int i=0;i<handle->reNum;i++)
				{
					if(bfp[(handle->reStartPos)+i]!=0)
					{
						curPage=id;
						cur=Rid(curPage,i);
						break;
					}
				}
				if(curPage!=-1)break;
			}
			if(curPage==-1)
			{
				if(is_debug)cout<<"Error: unable to continue record scan -- no valid records further"<<endl;
				return false;
			}
			return true;
		}
		bool startScan(RecordHandle *_handle)
		{
			handle=_handle;
			bfm=handle->bfm;
			//get first
			int pagenum=handle->pageNum,bufid;
			curPage=-1;
			if(pagenum==2)
			{
				if(is_debug)cout<<"Error: unable to open record scan -- no records stored"<<endl;
				return false;
			}
			for (int id=2;id<pagenum;id++)
			{
				if(id%65537==1)continue;
				bfp=bfm->getPage(handle->fileid,id,bufid);
				for (int i=0;i<handle->reNum;i++)
				{
					if(bfp[(handle->reStartPos)+i]!=0)
					{
						curPage=id;
						cur=Rid(curPage,i);
						break;
					}
				}
				if(curPage!=-1)break;
			}
			if(curPage==-1)
			{
				if(is_debug)cout<<"Error: unable to open record scan -- no valid records stored"<<endl;
				return false;
			}
			return true;
		}
		Rid getScanVal()
		{
			return cur;
		}
};

#endif
