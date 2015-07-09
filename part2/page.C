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
	if(slotCnt==0)                        //当插入时里面没有数据 ，即freeSpace=4080
	 {  
        if ( freeSpace<rec.length+4) //判断空间的大小是否能插下这条记录的数据
        	{   
        		 rid.pageNo=-1;
        		 rid.slotNo=-1;
        		 return  NOSPACE ;
        	}
      memcpy(&data[ freePtr], rec.data, rec.length);    //写入数据
      slot[slotCnt].offset=freePtr;                                //修改相应的属性            
	  slot[slotCnt].length=rec.length;
      freePtr= freePtr+rec.length;
      freeSpace= freeSpace- rec.length-4;
      rid.pageNo=curPage;
      rid.slotNo=  slotCnt;
      slotCnt --; 
      return OK;
	      
     } 
		
	  for (i=0;i>slotCnt;i--)                                  //判断是否有空曹
	 {
	  	if(slot[i].length==-1 &&  slot[i].offset==-1)
	  	{	
  		if ( freeSpace<rec.length) 
  			{
                  rid.pageNo=-1;
        		 rid.slotNo=-1;
        		 return  NOSPACE ;
   	     	 }
   	    memcpy(&data[freePtr], rec.data, rec.length);     //写入数据 也可以用bcopy函数
	    slot[i].offset= freePtr;                                      //修改相应的属性        
	    slot[i].length=rec.length;
	    freePtr=freePtr+rec.length;
		 freeSpace=freeSpace-rec.length;
		 rid.slotNo=-i;
		 rid.pageNo=curPage;
           return OK;  
          } 
               
	 }  	
	    if ( freeSpace<rec.length+4)    //没有空曹，就增加一个插曹
	     	{
			      rid.pageNo=-1;
		          rid.slotNo=-1;
		          return  NOSPACE ;
		   }
		memcpy(&data[freePtr], rec.data, rec.length);    //写入数据 也可以用bcopy函数
	    slot[slotCnt ].offset=freePtr;           //修改相应的属性     
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
	    scr=slot[slotNo].offset+slot[slotNo].length;    //待覆盖的区域起始位置
	    dest=slot[slotNo].offset;    // 被覆盖的区域的起始位置
	    
//	     while(len>slot[slotNo].length)  //先覆盖
//	    	{
//	             memcpy( &data[dest],&data[scr],slot[slotNo].length);
//	    		   len=len-slot[slotNo].length;
//	    		   scr=scr+slot[slotNo].length;
//	    		   dest=dest+slot[slotNo].length;
//	    	}
//	       memcpy( &data[dest],&data[scr],len);
	    bcopy( &data[scr],&data[dest],len);   	//覆盖数据
	   
	   for(i=0;i>slotCnt;i--)
	   {
	      	if (slot[i].offset>slot[slotNo].offset)
		      slot[i].offset= slot[i].offset-slot[slotNo].length;  	
	    }
	   
	     	freePtr=freePtr-slot[slotNo].length;
		   freeSpace=freeSpace+slot[slotNo].length;
           slot[ slotNo ].length=-1;
			slot[ slotNo ].offset=-1;
			 
	  if( slotNo==slotCnt+1)      //最后一个插曹
	  {		
	   		for(i= slotNo;i<=0;i++)  //判断后面是否有连续空曹	   		
	   	{	
	   		if(slot[i].offset==-1 && slot[i].length==-1)   //压缩空曹
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
    if(freeSpace==4080)  //判断是不是空页
      return NORECORDS;
    for(i=0;i>slotCnt;i--)
     {
       if ( slot[i].offset!=-1 && slot[i].length!=-1 )  //取得第一个不是空曹的记录
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
	    if (-curRid.slotNo==slotCnt+1)  //判断是不是最后一条记录
	 	return ENDOFPAGE;
	 	for(i=-curRid.slotNo-1;i>slotCnt;i--)
	 	{
	 		if(slot[i].offset!=-1 &&  slot[i].length!=-1) //取得下一个不是空曹的记录
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
    if ((slot[slotNo].length==-1) || (slotNo<=slotCnt))   //页中没有该记录
    	 return INVALIDSLOTNO;
    	rec.length=slot[slotNo].length;              
    	rec.data=&data[offset];
    	return OK;
    	
}
