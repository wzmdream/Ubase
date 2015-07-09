#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"

#define ASSERT(c)  { if (!(c)) { \
		       cerr << "At line " << __LINE__ << ":" << endl << "  "; \
                       cerr << "This condition should hold: " #c << endl; \
                       exit(1); \
		     } \
                   }

//----------------------------------------
// Constructor of the class BufMgr
//----------------------------------------

BufMgr::BufMgr(const int bufs)
{ 
	 int b;
	 int t = 1;
	 int i;
	 clockHand=0;
	 numBufs=bufs;
	 bufStats.pinnedFrame=0;                                                //��ʼ��������ж�ס��֡��
	 bufPool= new Page[bufs];                                               //���仺��ش�С
	 memset(bufPool, 0,  numBufs * sizeof(Page));                 // ��ʼ�������(����ȫ����Ϊ0)
	 bufTable = new BufDesc[bufs];                                       //���仺��״̬������
	 for (int i = 0; i < numBufs; i++)
	  {
	 	 bufTable[i].frameNo = i;                                              //��ʼ��֡���
	 	 
	 	}
	 int k = (int)(1.2 *  numBufs);                                            //���һ������k
	 if(k % 2 == 0)
	 k = k - 1;
	 while(t)
	   { 
		 	k += 2;
		   for(i = 3; i <k/2; i=i+2)
		   if(k % i == 0)
		    break;
		    
		   if (i ==k/2)
			  t=0; 
	  }
	      b=k;
	 hashTable= new BufHashTbl(b);                                           
 } 
 
BufMgr::~BufMgr() 
{ 
	   for(int i = 0; i < numBufs; i++)
	   if(bufTable[i].dirty == true)                                                //��������ҳ��д�ش���
	   	{  
		     bufTable[i].file->writePage(bufTable[i].pageNo,&bufPool [i]);
		   	 bufTable[i].dirty=false;
	    }
	    
	  delete [] bufTable;
	  delete [] bufPool;
	  delete  hashTable;
 }
void BufMgr::BufDump() 
{
   



}


const Status BufMgr::allocBuf(int& frame) 
{  
	Status a;                                            //����һ��״̬����Ϊ����ֵ
	if(bufStats.pinnedFrame == numBufs)                  //�������еĻ���ص��붼����ס�ͷ��ش���
		return BUFFEREXCEEDED;

	while(true)
	{
		if(bufTable[clockHand].valid == false)                 //�Ƿ�Ϊ��Чҳ
        {
				bufTable[clockHand].Clear();                         //��ʼ���ڵ�
			    frame = bufTable[clockHand].frameNo;
				break;
         }
         
  	 if (bufTable[clockHand].refbit == true)                   //�Ƿ񱻷��ʹ�
  		{
  			 bufTable[clockHand].refbit = false;
	  		 advanceClock();                                        //��ʱ����һ��
	  		 continue;                                             	//�������ѭ�����������´�ѭ��
  		}  
  		
	    if(bufTable[clockHand].pinCnt!=0)                            //�Ƿ񱻶�ס
	    {
			advanceClock();
		  	continue;
	    }
	   
	   if(bufTable[clockHand].dirty == true)                        //�Ƿ�Ϊ��ҳ
  	   { 
		 	   if((a=bufTable[clockHand].file->writePage(bufTable[clockHand].pageNo,&bufPool[clockHand]))!=OK)  //�������ľ�д�ش���
		 		    return a;
  	    }
 	   if ((a=hashTable->remove(bufTable[clockHand].file,bufTable[clockHand].pageNo))!=OK)   //ɾ������ӳ���ϵ
         	 return a;
  		bufTable[clockHand].Clear();
    	frame=bufTable[clockHand].frameNo;                         //��bufTable[clockHand]��Ÿ�frame����
    	break;                                                                         //����ѭ��
	}
	 return OK;
}

 // release an unused buffer frame - used in error conditions
const void BufMgr::releaseBuf(int frame) 
{
    bufTable[frame].Clear();  
}
	
const Status BufMgr::readPage(File* file, const int PageNo, Page*& page)
  {  
	  	Status  a;
	    int framNo;
	  if((a=hashTable->lookup(file,PageNo,framNo))!=OK)  //ͨ����ϣ�����������ҳ�Ƿ��ڻ������
	     {
		    	if((a=allocBuf(framNo))!=OK)                            //����һ��������
					return  a;
					
				if( ( a= file->readPage(PageNo,&bufPool[framNo] ))!=OK) 
				{
						releaseBuf(framNo) ;
					return a;
				}
				
				 if((a=hashTable->insert(file,PageNo,framNo))!=OK)            //ȡ��ӳ���ϵ
				{
						releaseBuf(framNo) ;
					return a;
				}
				bufTable[framNo].Set(file,PageNo)	;
				bufStats.pinnedFrame++;                                       //����ص��ܵ���������1                 
				page=&bufPool[framNo] ;
				return OK;   
   	  }
  	else
	  	{
		      bufTable[framNo].refbit=true; 
		      if(bufTable[framNo].pinCnt==0)
		      bufStats.pinnedFrame++;                                                    //����ص��ܵ���������1
		   		bufTable[framNo].pinCnt++; //��ҳ����������1
		  	 	page=&bufPool[framNo] ;  
		       return OK;   
	    }
  
}

   const Status BufMgr::unPinPage(const File* file, const int PageNo, const bool dirty)
  {
	     int frame;                                                           //����һ��������
	    Status status;
	    status = hashTable->lookup(file,PageNo,frame);  //ͨ����ϣ������������Ӧ���ļ���ҳ���Ƿ��ڻ������
		if(status != OK)
	       return HASHNOTFOUND;
	       
	    if(bufTable[frame].pinCnt == 0)
	       return PAGENOTPINNED;
	       
	     if(dirty  == true)
		    bufTable[frame].dirty = dirty;
	     bufTable[frame].pinCnt-- ;
	     
	     if(bufTable[frame].pinCnt == 0) //���� bufTable[frame].pinCnt-- Ϊ0ʱ��˵����ҳû�б���ס����ô����ص��ܵı���ס����͸ü�1
	        bufStats.pinnedFrame-- ;
	       return OK;
	       
	 }
	 
const Status BufMgr::flushFile(const File* file) 
{
		int i;
		Status a;
		for(i=0;i<numBufs;i++)    //ɨ����ļ������е�ҳ
		if(bufTable[i].file==file)
		 {
			 	if(bufTable[i].pinCnt!=0)
			 		return PAGEPINNED;
			 	if(bufTable[i].dirty==true)             //�ж�����dirtyλ
			 	  a=bufTable[i].file->writePage(bufTable[i].pageNo,&bufPool[i]);  // д�ش���
			 	if(a!= OK)
		            return a;
		       else 
		          bufTable[i].dirty=false;
		      if((a=hashTable->remove(file,bufTable[i].pageNo))!=OK)      //ɾ��ӳ���ϵ
		          	return a;
		      bufTable[i].Clear();                                                                //��ʼ���ڵ�
		       	  continue;         //��������ѭ������������һ��ѭ�� ʵ���Ͼ���ɨ����ļ�����һ��ҳ
	       }
	            return OK;
}

const Status BufMgr::disposePage(File* file, const int pageNo) 
	{   
		Status a ;
	    int index;                                                                  //����һ��������
	    if((a= hashTable->lookup(file,pageNo,index))!=OK)  //ͨ����ϣ������������Ӧ���ļ���ҳ���Ƿ��ڻ���ؼ��粻�ھͷ���a
			return a;
		bufTable[index].Clear();   
		
	   if(bufTable[index].pinCnt!=0)
			 return PAGEPINNED;
	   return OK;
	
	}
	 

const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page) 
	{	
		int apageNo;                                            //ҳ��
		Status status ;
		int frame;
		if((status=file->allocatePage(apageNo))!=OK) return status;  //ͨ��allocatePage(apageNo))����apageNo
			
	  if ((status =allocBuf(frame))!=OK) return status;
	  		
	 	if ((status=hashTable->insert(file,apageNo,frame))!=OK)  return status; //����ӳ���ϵ
	   
	    bufTable[frame].Set(file,apageNo);	
	  	pageNo=apageNo;                               //��apageNo��pageNo����ȥ
	  	page=&bufPool[frame];
	  	bufStats.pinnedFrame++;                      //����ص��ܵ���������1
	  	return OK;
   }
   
void BufMgr::printSelf(void) {

  BufDesc* tmpbuf;
  
  cout << endl << "Print buffer...\n";
  for (int i=0; i<numBufs; i++) {
    tmpbuf = &(bufTable[i]);
    cout << i << "\t" << (char*)(&bufPool[i]) 
      << "\tpinCnt: " << tmpbuf->pinCnt;
    
    if (tmpbuf->valid == true)
      cout << "\tvalid\n";
    cout << endl;
  };
}