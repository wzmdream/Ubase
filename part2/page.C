#include <sys/types.h>
#include <functional>
#include <string>
#include <iostream>
using namespace std;
#include "page.h"

// page class initializer
void Page::init(int pageNo)
{
    nextPage = -1;
    slotCnt = 0; // no slots in use
    curPage = pageNo;
    freePtr=0; // offset of free space in data array
    freeSpace=PAGESIZE-DPFIXED + sizeof(slot_t); // amount of space available
}

// dump page utlity
void Page::dumpPage() const
{
    int i;
    cout << "curPage = " << curPage <<", nextPage = " << nextPage
         << "\nfreePtr = " << freePtr << ",  freeSpace = " << freeSpace
         << ", slotCnt = " << slotCnt << endl;

    for (i=0;i>slotCnt;i--)
    {
        cout << "slot[" << i << "].offset = " << slot[i].offset 
             << ", slot[" << i << "].length = " << slot[i].length << endl;
        if (slot[i].length==-1)
            cout << "This record has been deleted!" << endl;
        else
        {
            for (int j=slot[i].offset;j<slot[i].offset+slot[i].length;j++)
                printf("%c", data[j]);
            printf("\n");
        }
    }

}

const Status Page::setNextPage(int pageNo)
{
    nextPage = pageNo;
    return OK;
}

const Status Page::getNextPage(int& pageNo) const
{
    pageNo = nextPage;
    return OK;
}

const short Page::getFreeSpace() const
{
    return freeSpace;
}
    
const short Page::getSlotCnt() const
{
    return slotCnt;
}

const short Page::getFreePtr() const
{
    return freePtr;
}

const short Page::getOffset(const int i) const
{
    return slot[i].offset;
}

const short Page::getLength(const int i) const
{
    return slot[i].length;
}

// Add a new record to the page. Returns OK if everything went OK
// otherwise, returns NOSPACE if sufficient space does not exist
// RID of the new record is returned via rid parameter

const Status Page::insertRecord(const Record & rec, RID& rid)
{       
     int i=0;
	if(slotCnt==0)                        //������ʱ����û������ ����freeSpace=4080
	 {  
        if ( freeSpace<rec.length+4) //�жϿռ�Ĵ�С�Ƿ��ܲ���������¼������
        	{   
        		 rid.pageNo=-1;
        		 rid.slotNo=-1;
        		 return  NOSPACE ;
        	}
      memcpy(&data[ freePtr], rec.data, rec.length);    //д������
      slot[slotCnt].offset=freePtr;                                //�޸���Ӧ������            
	  slot[slotCnt].length=rec.length;
      freePtr= freePtr+rec.length;
      freeSpace= freeSpace- rec.length-4;
      rid.pageNo=curPage;
      rid.slotNo=  slotCnt;
      slotCnt --; 
      return OK;
	      
     } 
		
	  for (i=0;i>slotCnt;i--)                                  //�ж��Ƿ��пղ�
	 {
	  	if(slot[i].length==-1 &&  slot[i].offset==-1)
	  	{	
  		if ( freeSpace<rec.length) 
  			{
                  rid.pageNo=-1;
        		 rid.slotNo=-1;
        		 return  NOSPACE ;
   	     	 }
   	    memcpy(&data[freePtr], rec.data, rec.length);     //д������ Ҳ������bcopy����
	    slot[i].offset= freePtr;                                      //�޸���Ӧ������        
	    slot[i].length=rec.length;
	    freePtr=freePtr+rec.length;
		 freeSpace=freeSpace-rec.length;
		 rid.slotNo=-i;
		 rid.pageNo=curPage;
           return OK;  
          } 
               
	 }  	
	    if ( freeSpace<rec.length+4)    //û�пղܣ�������һ�����
	     	{
			      rid.pageNo=-1;
		          rid.slotNo=-1;
		          return  NOSPACE ;
		   }
		memcpy(&data[freePtr], rec.data, rec.length);    //д������ Ҳ������bcopy����
	    slot[slotCnt ].offset=freePtr;           //�޸���Ӧ������     
	    slot[slotCnt ].length=rec.length;
	    freePtr=freePtr+rec.length;
	    freeSpace=freeSpace-rec.length-4    ;	
	  	rid.slotNo=-slotCnt ;
	    rid.pageNo=curPage;
        slotCnt --; 
	    return OK;
	}

// update the record with the specified rid
const Status Page::updateRecord(const Record & rec,const RID& rid)
{
    int slotNo = -rid.slotNo; //convert to negative format

    //first check the record validation
    if ((slot[slotNo].length==-1) || (slotNo<=slotCnt)) return INVALIDSLOTNO;

    int offset = slot[slotNo].offset;

    memcpy(&data[offset], rec.data, rec.length); // copy data on to the data page

    return OK;
}
const Status Page::deleteRecord(const RID & rid)
{    
	    int i,len;
	    int scr,dest;
		int slotNo = -rid.slotNo; 
	    if ((slot[slotNo].length==-1) || (slotNo<=slotCnt)) 
	    	return INVALIDSLOTNO; 	
	   	len=freePtr-slot[slotNo].offset-slot[slotNo].length;
	    scr=slot[slotNo].offset+slot[slotNo].length;    //�����ǵ�������ʼλ��
	    dest=slot[slotNo].offset;    // �����ǵ��������ʼλ��
	    
//	     while(len>slot[slotNo].length)  //�ȸ���
//	    	{
//	             memcpy( &data[dest],&data[scr],slot[slotNo].length);
//	    		   len=len-slot[slotNo].length;
//	    		   scr=scr+slot[slotNo].length;
//	    		   dest=dest+slot[slotNo].length;
//	    	}
//	       memcpy( &data[dest],&data[scr],len);
	    bcopy( &data[scr],&data[dest],len);   	//��������
	   
	   for(i=0;i>slotCnt;i--)
	   {
	      	if (slot[i].offset>slot[slotNo].offset)
		      slot[i].offset= slot[i].offset-slot[slotNo].length;  	
	    }
	   
	     	freePtr=freePtr-slot[slotNo].length;
		   freeSpace=freeSpace+slot[slotNo].length;
           slot[ slotNo ].length=-1;
			slot[ slotNo ].offset=-1;
			 
	  if( slotNo==slotCnt+1)      //���һ�����
	  {		
	   		for(i= slotNo;i<=0;i++)  //�жϺ����Ƿ��������ղ�	   		
	   	{	
	   		if(slot[i].offset==-1 && slot[i].length==-1)   //ѹ���ղ�
	   		{
	   			freeSpace=freeSpace+4;
	   			slotCnt++;					
	   		}
	   		else 
	   			break;
	   		}
	   		
		}
	   	   	     return OK;

}
//// returns RID of first record on page
const Status Page::firstRecord(RID& firstRid) const
{
     int i;
	RID rid;
    if(freeSpace==4080)  //�ж��ǲ��ǿ�ҳ
      return NORECORDS;
    for(i=0;i>slotCnt;i--)
     {
       if ( slot[i].offset!=-1 && slot[i].length!=-1 )  //ȡ�õ�һ�����ǿղܵļ�¼
      	{
      	    rid.slotNo=-i;                                         
      	    rid.pageNo=curPage;
      	    break;
      	 }
      	 
      }
    firstRid=rid;
    return OK;
    
   }

// returns RID of next record on the page
// returns ENDOFPAGE if no more records exist on the page; otherwise OK
const Status Page::nextRecord (const RID &curRid, RID& nextRid) const
{    
       int i;
	    if (-curRid.slotNo==slotCnt+1)  //�ж��ǲ������һ����¼
	 	return ENDOFPAGE;
	 	for(i=-curRid.slotNo-1;i>slotCnt;i--)
	 	{
	 		if(slot[i].offset!=-1 &&  slot[i].length!=-1) //ȡ����һ�����ǿղܵļ�¼
	        {  
	     	   nextRid.slotNo=-i;
              nextRid.pageNo=curRid.pageNo;
              break;
            }
         
         }
          return OK;

}

// returns length and pointer to record with RID rid
const Status Page::getRecord(const RID & rid, Record & rec)
{
	
    int slotNo = -rid.slotNo; 
    int offset = slot[slotNo].offset;
    if ((slot[slotNo].length==-1) || (slotNo<=slotCnt))   //ҳ��û�иü�¼
    	 return INVALIDSLOTNO;
    	rec.length=slot[slotNo].length;              
    	rec.data=&data[offset];
    	return OK;
    	
}