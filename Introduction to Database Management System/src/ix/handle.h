#ifndef INDEX_HANDLE
#define INDEX_HANDLE

#include "../pf/bufmanager/BufPageManager.h"
#include "../pf/fileio/FileManager.h"
#include "../rm/manager.h"
#include "manager.h"

const int fatpos=2047;

class IndexHandle
{
	public:
		int fileid;
		FileManager *fm;
		BufPageManager *bfm;
		IndexHead head;
		int nextpos;
		vector<int>leafoffset;
		vector<int>nmloffset;
		IndexHandle(FileManager *_fm,BufPageManager *_bfm,int _fileid)
		{
			fileid=_fileid;
			fm=_fm;
			bfm=_bfm;
			int bufid,tmp=3;
			BufType b=bfm->getPage(fileid,0,bufid);
			head.root=b[0];
			head.lastpage=b[1];
			head.atrlist.clear();
			int atrlistsize=b[2];
			for (int i=0;i<atrlistsize;i++)
			{
				head.atrlist.push_back(AttrType(b[tmp+1],b[tmp+2],(b[tmp+3]-1)*4));
				tmp+=3;
			}
			head.valuesize=head.atrlist.length;
			tmp=1;
			nmloffset.clear();
			while (tmp<2047)
			{
				nmloffset.push_back(tmp);
				tmp+=head.valuesize;
				tmp++;
			}
			head.nmlpagesize=nmloffset.size()-1;
			tmp=1;
			leafoffset.clear();
			while (tmp<2047)
			{
				leafoffset.push_back(tmp);
				nextpos=tmp;
				tmp+=2;
				tmp+=head.valuesize;
			}
			head.leafpagesize=leafoffset.size()-1;
			bfm->release(bufid);
			if(is_debug)
			{
				cout<<"Index Handle Ready: valuesize="<<head.valuesize<<" nextpos="<<nextpos<<" leafsize="<<head.leafpagesize<<" nmlsize="<<head.nmlpagesize<<endl;
				cout<<"root="<<head.root<<" lastpage="<<head.lastpage<<endl;
			}
			if(head.leafpagesize<3||head.nmlpagesize<3)cout<<"Warning: Too Small Page Size for Index !!!"<<endl;
		}
		void close()
		{
			if(fileid==-1)return;
			int bufid; 
			BufType b=bfm->getPage(fileid,0,bufid);
			int tmp=3;
			b[0]=head.root;
			b[1]=head.lastpage;
			b[2]=head.atrlist.size;
			b[3]=head.atrlist.length;
			for (int i=0;i<head.atrlist.size;i++)
			{
				tmp++;
				b[tmp]=head.atrlist.list[i].type;
				tmp++;
				b[tmp]=head.atrlist.list[i].offset;
				tmp++;
				b[tmp]=head.atrlist.list[i].length;
			}
			bfm->markDirty(bufid);
			fileid=-1;
			nextpos=-1;
		}
		~IndexHandle()
		{
			close();
		}
		bool writenormal(NormalNode& lnode)
		{
			int pageid=lnode.pageid;
			int bufid;
			BufType b=bfm->getPage(fileid,pageid,bufid);
			b[0]=lnode.size;
			b[fatpos]=lnode.fat;
			for (int i=0;i<(int)b[0];i++)
			{
				if(!valuetobuf(b+nmloffset[i],lnode.value[i],head.atrlist))
				{
					bfm->release(bufid);
					return false;
				}
				b[nmloffset[i+1]-1]=lnode.child[i];
			}
			bfm->markDirty(bufid);
			return true;
		}
		bool writeleaf(LeafNode& lnode)
		{
			int pageid=lnode.pageid;
			int bufid;
			BufType b=bfm->getPage(fileid,pageid,bufid);
			b[0]=lnode.size+10000;
			b[nextpos]=lnode.next;
			b[fatpos]=lnode.fat;
			for (int i=0;i<lnode.size;i++)
			{
				if(!valuetobuf(b+leafoffset[i],lnode.value[i],head.atrlist))
				{
					bfm->release(bufid);
					return false;
				}
				b[leafoffset[i+1]-2]=lnode.rid[i].pageID;
				b[leafoffset[i+1]-1]=lnode.rid[i].slotID;
			}
			bfm->markDirty(bufid);
			return true;
		}
		bool readnormal(NormalNode& lnode,int pageid)
		{
			ValueList val;
			lnode.clear();
			lnode.pageid=pageid;
			int bufid;
			BufType b=bfm->getPage(fileid,pageid,bufid);
			lnode.size=b[0];
			lnode.fat=b[fatpos];
			for (int i=0;i<lnode.size;i++)
			{
				if(!buftovalue(b+nmloffset[i],val,head.atrlist))
				{
					bfm->release(bufid);
					return false;
				}
				lnode.value.push_back(val);
				lnode.child.push_back(b[nmloffset[i+1]-1]);
			}
			return true;
		}
		bool readleaf(LeafNode& lnode,int pageid)
		{
			ValueList val;
			lnode.clear();
			lnode.pageid=pageid;
			int bufid;
			BufType b=bfm->getPage(fileid,pageid,bufid);
			lnode.size=b[0]-10000;
			lnode.next=b[nextpos];
			lnode.fat=b[fatpos];
			for (int i=0;i<lnode.size;i++)
			{
				if(!buftovalue(b+leafoffset[i],val,head.atrlist))
				{
					bfm->release(bufid);
					return false;
				}
				lnode.value.push_back(val);
				lnode.rid.push_back(Rid(b[leafoffset[i+1]-2],b[leafoffset[i+1]-1]));
			}
			return true;
		}
		bool isLeaf(int pageid)
		{
			int bufid;
			BufType b=bfm->getPage(fileid,pageid,bufid);
			if(b[0]>=10000)return true;
			return false;
		}
		void changefatleaf(int pageid,int fatid)
		{
			LeafNode cur;
			readleaf(cur,pageid);
			cur.fat=fatid;
			writeleaf(cur);
		}
		void changefat(int pageid,int fatid)
		{
			NormalNode nml;
			readnormal(nml,pageid);
			nml.fat=fatid;
			writenormal(nml);
		}
		void debugPrint(int pageid)
		{
			if(isLeaf(pageid))
			{
				cout<<"Leaf Node "<<pageid<<":"<<endl;
				LeafNode cur;
				if(!readleaf(cur,pageid))return;
				for (int i=0;i<cur.size;i++)
				{
					cout<<"Value: ";
					cur.value[i].print();
					cout<<endl;
					cout<<"Rid: ( "<<cur.rid[i].pageID<<" , "<<cur.rid[i].slotID<<" )"<<endl;
				}
			}
			else
			{
				cout<<"Normal Node "<<pageid<<":"<<endl;
				NormalNode cur;
				if(!readnormal(cur,pageid))return;
				for (int i=0;i<cur.size;i++)
				{
					cout<<"Value: "<<endl;
					cur.value[i].print();
					cout<<endl;
					cout<<"Child: "<<cur.child[i]<<endl;
				}
				for (int i=0;i<cur.size;i++)
				{
					debugPrint(cur.child[i]);
				}
			}
		}
		void debug()
		{
			if(head.root==0)
			{
				cout<<"No nodes in this Index"<<endl;
			}
			else debugPrint(head.root);
		}
		bool overflownode(int pageid,ValueList val,ValueList val2,int lchild,int rchild,int fr)
		{
			if(pageid==0)
			{
				//root depart
				head.lastpage++;
				NormalNode newroot;
				newroot.size=2;
				newroot.fat=0;
				newroot.pageid=head.lastpage;
				newroot.value.push_back(val);
				newroot.value.push_back(val2);
				newroot.child.push_back(lchild);
				newroot.child.push_back(rchild);
				if(isLeaf(lchild))
				{
					changefatleaf(lchild,head.lastpage);
					changefatleaf(rchild,head.lastpage);
				}
				else
				{
					changefat(lchild,head.lastpage);
					changefat(rchild,head.lastpage);
				}
				writenormal(newroot);
				return true;
			}
			//updatecurnode
			NormalNode nml;
			if(!readnormal(nml,pageid))return false;
			int slotid=0;
			for (int i=0;i<nml.size;i++)
			{
				if(nml.child[i]==fr)
				{
					nml.value[i]=val2;
					break;
				}
				slotid=i+1;
			}
			nml.child[slotid]=rchild;
			nml.size++;
			nml.child.resize(nml.size);
			nml.value.resize(nml.size);
			for (int i=nml.size-1;i>slotid;i--)
			{
				nml.child[i]=nml.child[i-1];
				nml.value[i]=nml.value[i-1];
			}
			nml.child[slotid]=lchild;
			nml.value[slotid]=val;
			//check if overflow
			if(nml.size<head.nmlpagesize)
			{
				if(!writenormal(nml))return false;
				return true;
			}
			NormalNode leftnode,rightnode;
			int leftsiz=nml.size/2,rightsiz=nml.size-leftsiz;
			//build leftnode rightnode
			leftnode.size=leftsiz;
			leftnode.fat=nml.fat;
			head.lastpage++;
			leftnode.pageid=head.lastpage;
			for (int i=0;i<leftsiz;i++)
			{
				leftnode.value.push_back(nml.value[i]);
				leftnode.child.push_back(nml.child[i]);
				if(isLeaf(nml.child[i]))changefatleaf(nml.child[i],leftnode.pageid);
				else changefat(nml.child[i],leftnode.pageid);
			}
			//write leftnode rightnode
			rightnode.size=rightsiz;
			rightnode.fat=nml.fat;
			head.lastpage++;
			rightnode.pageid=head.lastpage;
			for (int i=leftsiz;i<nml.size;i++)
			{
				rightnode.value.push_back(nml.value[i]);
				rightnode.child.push_back(nml.child[i]);
				if(isLeaf(nml.child[i]))changefatleaf(nml.child[i],rightnode.pageid);
				else changefat(nml.child[i],rightnode.pageid);
			}
			if(!writenormal(leftnode))return false;
			if(!writenormal(rightnode))return false;
			//overflownode father
			if(!overflownode(nml.fat,leftnode.value[leftnode.size-1],rightnode.value[rightnode.size-1],leftnode.pageid,rightnode.pageid,nml.pageid))return false;
			return true;
		}
		bool insertEntry(ValueList val,Rid rid)
		{
			if(head.root==0)
			{
				head.lastpage=2;
				head.root=1;
				LeafNode lf;
				NormalNode nl;
				lf.pageid=2;
				lf.next=0;
				lf.fat=1;
				lf.size=1;
				lf.value.push_back(val);
				lf.rid.push_back(rid);
				nl.pageid=1;
				nl.size=1;
				nl.fat=0;
				nl.value.push_back(val);
				nl.child.push_back(2);
				if(!writeleaf(lf))return false;
				if(!writenormal(nl))return false;
				return true;
			}
			int pageid;
			if(!findLeaf(val,pageid))return false;
			//insert pageid val
			LeafNode curleaf;
			if(!readleaf(curleaf,pageid))return false;
			if(curleaf.size<head.leafpagesize)
			{
				//normal insert
				int slotid=0;
				for (int i=0;i<curleaf.size;i++)
				{
					if(CompOp::judge(CompOp::GreaterOrEqual,curleaf.value[i],val))break;
					slotid=i+1;
				}
				if(slotid==curleaf.size)
				{
					//trace back to root
					curleaf.size++;
					curleaf.value.push_back(val);
					curleaf.rid.push_back(rid);
					if(!writeleaf(curleaf))return false;
					NormalNode nml;
					int fatid=curleaf.fat;
					while (fatid!=0)
					{
						if(!readnormal(nml,fatid))return false;
						nml.value[nml.size-1]=val;
						fatid=nml.fat;
						writenormal(nml);
					}
				}
				else
				{
					curleaf.size++;
					curleaf.value.resize(curleaf.size);
					curleaf.rid.resize(curleaf.size);
					for (int i=curleaf.size-1;i>slotid;i--)
					{
						curleaf.value[i]=curleaf.value[i-1];
						curleaf.rid[i]=curleaf.rid[i-1];
					}
					curleaf.value[slotid]=val;
					curleaf.rid[slotid]=rid;
					if(!writeleaf(curleaf))return false;
				}
			}
			else
			{
				//page overflow
				int slotid=0;
				for (int i=0;i<curleaf.size;i++)
				{
					if(CompOp::judge(CompOp::GreaterOrEqual,curleaf.value[i],val))break;
					slotid=i+1;
				}
				curleaf.size++;
				curleaf.value.resize(curleaf.size);
				curleaf.rid.resize(curleaf.size);
				for (int i=curleaf.size-1;i>slotid;i--)
				{
					curleaf.value[i]=curleaf.value[i-1];
					curleaf.rid[i]=curleaf.rid[i-1];
				}
				curleaf.value[slotid]=val;
				curleaf.rid[slotid]=rid;
				int lefsize,rigsize;
				lefsize=curleaf.size/2;
				rigsize=curleaf.size-lefsize;
				LeafNode leftleaf,rightleaf;
				//buildup leftleaf,rightleaf
				leftleaf.size=lefsize;
				rightleaf.size=rigsize;
				head.lastpage++;
				leftleaf.pageid=head.lastpage;
				head.lastpage++;
				rightleaf.pageid=head.lastpage;
				leftleaf.next=rightleaf.pageid;
				rightleaf.next=curleaf.pageid;
				leftleaf.fat=curleaf.fat;
				rightleaf.fat=curleaf.fat;
				for (int i=0;i<lefsize;i++)
				{
					leftleaf.value.push_back(curleaf.value[i]);
					leftleaf.rid.push_back(curleaf.rid[i]);
				}
				for (int i=lefsize;i<curleaf.size;i++)
				{
					rightleaf.value.push_back(curleaf.value[i]);
					rightleaf.rid.push_back(curleaf.rid[i]);
				}
				//write leftleaf,rightleaf
				if(!writeleaf(leftleaf))return false;
				if(!writeleaf(rightleaf))return false;
				//overflownode father
				if(!overflownode(curleaf.fat,leftleaf.value[leftleaf.size-1],rightleaf.value[rightleaf.size-1],leftleaf.pageid,rightleaf.pageid,curleaf.pageid))return false;
			}
			return true;
		}
		bool findLeaf(ValueList val,int& res)
		{
			if(head.root==0)return false;
			int pageid=head.root;
			while (!isLeaf(pageid))
			{
				NormalNode cur;
				if(!readnormal(cur,pageid))return false;
				for (int i=0;i<cur.size;i++)
				{
					pageid=cur.child[i];
					if(CompOp::judge(CompOp::GreaterOrEqual,cur.value[i],val))break;
				}
			}
			res=pageid;
			return true;
		}
		bool findFirstLeaf(LeafNode& cur)
		{
			if(head.root==0)return false;
			int pageid=head.root;
			while (!isLeaf(pageid))
			{
				NormalNode cur;
				if(!readnormal(cur,pageid))return false;
				pageid=cur.child[0];
			}
			if(!readleaf(cur,pageid))return false;
			return true;
		}
		bool deleteEntry(int pageid,int slotid)
		{
			LeafNode cur;
			if(!readleaf(cur,pageid))return false;
			cur.rid[slotid]=lazydelete;
			writeleaf(cur);
			return true;
		}
		bool deleteByRid(Rid rid)
		{
			LeafNode cur;
			if(!findFirstLeaf(cur))return false;
			int pageid=cur.pageid;
			while (pageid!=0)
			{
				if(!readleaf(cur,pageid))return false;
				for (int i=0;i<(int)cur.size;i++)
				{
					if(cur.rid[i]==rid)
					{
						cur.rid[i]=lazydelete;
						writeleaf(cur);
						return true;
					}
				}
				pageid=cur.next;
			}return false;
		}
		bool exist(ValueList val)
		{
		//	cout<<"ifexist "<<endl;
		//	val.print();
			if(head.root==0)return false;
			int pageid=0;
			if(!findLeaf(val,pageid))return false;
			LeafNode cur;
			while (pageid!=0)
			{
				if(!readleaf(cur,pageid))return false;
				for (int i=0;i<(int)cur.size;i++)
				{
					if(CompOp::judge(CompOp::Equal,cur.value[i],val))return true;
					if(CompOp::judge(CompOp::Greater,cur.value[i],val))return false;
				}
				pageid=cur.next;
			}return false;
		}
		bool checkDuplicate()
		{
			ValueList lastval;
			int pageid,slotid=0;
			bool notfound=false;
			LeafNode cur;
			if(!findFirstLeaf(cur))return false;
			while (cur.rid[slotid]==lazydelete)
			{
				slotid++;
				if(slotid==cur.size)
				{
					pageid=cur.next;
					if(pageid==0)
					{
						notfound=true;
						break;
					}
					if(!readleaf(cur,pageid))return false;
					slotid=0;
				}
			}
			lastval=cur.value[slotid];
			while (!notfound)
			{
				slotid++;
				if(slotid==cur.size)
				{
					pageid=cur.next;
					if(pageid==0)
					{
						break;
					}
					if(!readleaf(cur,pageid))return false;
					slotid=0;
				}
				while (cur.rid[slotid]==lazydelete)
				{
					slotid++;
					if(slotid==cur.size)
					{
						pageid=cur.next;
						if(pageid==0)
						{
							notfound=true;
							break;
						}
						if(!readleaf(cur,pageid))return false;
						slotid=0;
					}
				}
				if(!notfound)
				{
					ValueList val;
					val=cur.value[slotid];
					if(CompOp::judge(CompOp::Equal,lastval,val))return true;
					lastval=val;
				}
			}
			return false;
		}
};

IndexHandle* IndexManager::openIndex(string curpath,const string& filename,string indexNo)
{
	string indexname=curpath+"/"+filename+".idx."+indexNo;
	IndexHandle *ixh=nullptr;
	if(fname.find(indexname)==fname.end())
	{
		cout<<"Error: index not found when open index handle"<<endl;
		return ixh;
	}
	int idx=fname[indexname];
	ixh=new IndexHandle(this->fm,this->bfm,idx);
	return ixh;
}

void IndexManager::closeIndex(IndexHandle *handle)
{
	handle->close();
}
#endif
