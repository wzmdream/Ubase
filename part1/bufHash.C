#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"

// buffer pool hash table implementation
//kjgjhgjh
int BufHashTbl::hash(const File* file, const int pageNo)
{
  int tmp, value;
  tmp = (int)file;  // cast of pointer to the file object to an integer
  value = (tmp + pageNo) % HTSIZE;
  return value;
}


BufHashTbl::BufHashTbl(int htSize)
{
  HTSIZE = htSize;
  // allocate an array of pointers to hashBuckets
  ht = new hashBucket* [htSize];
  for(int i=0; i < HTSIZE; i++)
    ht[i] = NULL;
}


BufHashTbl::~BufHashTbl()
{
  for(int i = 0; i < HTSIZE; i++) {
    hashBucket* tmpBuf = ht[i];
    while (ht[i]) {
      tmpBuf = ht[i];
      ht[i] = ht[i]->next;
      delete tmpBuf;
    }
  }
  delete [] ht;
}


//---------------------------------------------------------------
// insert entry into hash table mapping (file,pageNo) to frameNo;
// returns OK if OK, HASHTBLERROR if an error occurred
//---------------------------------------------------------------

Status BufHashTbl::insert(const File* file, const int pageNo, const int frameNo) 
{  
	int index=hash(file,pageNo);          //通过哈希算法来确定桶号
	hashBucket*tmpcBuc=ht[index];    //定义一个临时变量的tmpcBuc，它 的指针与ht的指针一样
	while(tmpcBuc)                             //判断哈希桶的溢出桶是不是最后一个桶
	{
	  if(tmpcBuc->pageNo==pageNo && tmpcBuc->file== file) return HASHTBLERROR;//插入的时候看看以前有没有插入的数据
			tmpcBuc=tmpcBuc->next; //往下查找
	}
	 tmpcBuc=new hashBucket;//当tmpcBuc是最后一个桶就跳出循环为它分配一个新的散列桶
	tmpcBuc->pageNo=pageNo;
	  tmpcBuc->file= (File*)file;
	tmpcBuc-> frameNo=frameNo;
	 tmpcBuc->next=ht[index];//把新的散列桶连接到原来哈希桶的第一个位子
	 ht[index]=tmpcBuc;
	 return OK;
 
  
}




//-------------------------------------------------------------------	     
// Check if (file,pageNo) is currently in the buffer pool (ie. in
// the hash table).  If so, return corresponding frameNo. else return 
// HASHNOTFOUND
//-------------------------------------------------------------------

Status BufHashTbl::lookup(const File* file, const int pageNo, int & frameNo) 
 { 
	 int index=hash(file,pageNo);            //映射到散列桶的号
	hashBucket*tmpBuc=ht[index];        //分配一个临时的指针跟ht[index]相同
	while(tmpBuc)
	{ 
		if(tmpBuc->pageNo==pageNo  &&   (File*) file==tmpBuc->file)    //找到了页号和文件的号
	    
	     {
             frameNo=tmpBuc-> frameNo ;        //把tmpBuc-> frameNo的针号给  frameNo返回
           return OK; 
	 	}
	   else 
	   	   tmpBuc=tmpBuc->next;                          //继续查找下一个
	 }
	return HASHNOTFOUND;
	
 }
//-------------------------------------------------------------------
// delete entry (file,pageNo) from hash table. REturn OK if page was
// found.  Else return HASHTBLERROR
//-------------------------------------------------------------------

Status BufHashTbl::remove(const File* file, const int pageNo) 
 { 
    int index = hash(file,pageNo);
    hashBucket* tmpBuf = ht[index];//分配一个临时的指针跟ht[index]相同
    hashBucket* preBuf = ht[index];//分配一个临时的指针跟ht[index]相同
    //其实在刚开始的时候ht[index]与tmpBuf和preBuf都是指向同一个哈希桶
    while(tmpBuf)
      {
         if(tmpBuf->pageNo == pageNo && tmpBuf->file == file)
              {
                 if(tmpBuf == ht[index]) //只有在第一次查找ht[index]对应的那个桶时查找了所需要的文件和页的时候，通过改变指针来删除这个桶
                     ht[index] = tmpBuf->next;
                else
                     preBuf->next = tmpBuf->next; //通过改变指针，来删除桶
                     delete tmpBuf;
                     return OK;
              }
       else
              {
                  preBuf = tmpBuf;    //preBuf来存tmpBuf的地址，然后tmpBuf指向下一个，以后preBuf总在tmpBuf之后
                  tmpBuf = tmpBuf->next;
               }
    }
      return HASHTBLERROR;

 }
