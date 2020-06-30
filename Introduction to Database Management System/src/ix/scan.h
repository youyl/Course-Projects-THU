#ifndef INDEX_SCAN
#define INDEX_SCAN

#include "../pf/bufmanager/BufPageManager.h"
#include "../pf/fileio/FileManager.h"
#include "../rm/manager.h"
#include "handle.h"

class IndexScan
{
	public:
		IndexHead head;
		LeafNode cur;
		int slotid;
		IndexHandle *handle;
		int cmpop;
		ValueList val;
		IndexScan(){}
		~IndexScan(){}
		bool getnext()
		{
			slotid++;
			if(slotid>=(int)cur.rid.size())
			{
				if(cur.next==0)return false;
				int xx=cur.next;
				if(!handle->readleaf(cur,xx))return false;
				slotid=0;
			}
			return true;
		}
		bool checkvalid()
		{
			if(cur.rid[slotid]==lazydelete)return false;
			return CompOp::judge(cmpop,cur.value[slotid],val);
		}
		bool openScan(IndexHandle *_handle,int CmpOp,ValueList _val)
		{
			handle=_handle;
			head=handle->head;
			val=_val;
			cmpop=CmpOp;
			if(CmpOp!=0&&!typefit(val,head.atrlist))return false;
			if(cmpop==CompOp::Greater||cmpop==CompOp::GreaterOrEqual||cmpop==CompOp::Equal)
			{
				int pageid;
				slotid=0;
				if(!handle->findLeaf(val,pageid))return false;
				if(!handle->readleaf(cur,pageid))return false;
				if(cmpop==CompOp::Equal)
				{
					if(!handle->exist(val))return false;
				}
				while (!checkvalid())
				{
					if(!getnext())return false;
				}
			}
			else
			{
				if(!handle->findFirstLeaf(cur))return false;
				slotid=0;
				while (!checkvalid())
				{
					if(!getnext())return false;
				}
			}
			return true;
		}
		Rid getEntry()
		{
			return cur.rid[slotid];
		}
		bool nextEntry()
		{
			while (getnext())
			{
				if(checkvalid())
				{
					return true;
				}
				else
				{
					if(cmpop==CompOp::NonEqual);
					else
					{
					//	/*tmp*/cur.value[slotid].print();
						return false;
					}
				}
			}
			return false;
		}
};

#endif
