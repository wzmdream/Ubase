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
	int index=hash(file,pageNo);          //ͨ����ϣ�㷨��ȷ��Ͱ��
	hashBucket*tmpcBuc=ht[index];    //����һ����ʱ������tmpcBuc���� ��ָ����ht��ָ��һ��
	while(tmpcBuc)                             //�жϹ�ϣͰ�����Ͱ�ǲ������һ��Ͱ
	{
	  if(tmpcBuc->pageNo==pageNo && tmpcBuc->file== file) return HASHTBLERROR;//�����ʱ�򿴿���ǰ��û�в��������
			tmpcBuc=tmpcBuc->next; //���²���
	}
	 tmpcBuc=new hashBucket;//��tmpcBuc�����һ��Ͱ������ѭ��Ϊ������һ���µ�ɢ��Ͱ
	tmpcBuc->pageNo=pageNo;
	  tmpcBuc->file= (File*)file;
	tmpcBuc-> frameNo=frameNo;
	 tmpcBuc->next=ht[index];//���µ�ɢ��Ͱ���ӵ�ԭ����ϣͰ�ĵ�һ��λ��
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
	 int index=hash(file,pageNo);            //ӳ�䵽ɢ��Ͱ�ĺ�
	hashBucket*tmpBuc=ht[index];        //����һ����ʱ��ָ���ht[index]��ͬ
	while(tmpBuc)
	{ 
		if(tmpBuc->pageNo==pageNo  &&   (File*) file==tmpBuc->file)    //�ҵ���ҳ�ź��ļ��ĺ�
	    
	     {
             frameNo=tmpBuc-> frameNo ;        //��tmpBuc-> frameNo����Ÿ�  frameNo����
           return OK; 
	 	}
	   else 
	   	   tmpBuc=tmpBuc->next;                          //����������һ��
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
    hashBucket* tmpBuf = ht[index];//����һ����ʱ��ָ���ht[index]��ͬ
    hashBucket* preBuf = ht[index];//����һ����ʱ��ָ���ht[index]��ͬ
    //��ʵ�ڸտ�ʼ��ʱ��ht[index]��tmpBuf��preBuf����ָ��ͬһ����ϣͰ
    while(tmpBuf)
      {
         if(tmpBuf->pageNo == pageNo && tmpBuf->file == file)
              {
                 if(tmpBuf == ht[index]) //ֻ���ڵ�һ�β���ht[index]��Ӧ���Ǹ�Ͱʱ����������Ҫ���ļ���ҳ��ʱ��ͨ���ı�ָ����ɾ�����Ͱ
                     ht[index] = tmpBuf->next;
                else
                     preBuf->next = tmpBuf->next; //ͨ���ı�ָ�룬��ɾ��Ͱ
                     delete tmpBuf;
                     return OK;
              }
       else
              {
                  preBuf = tmpBuf;    //preBuf����tmpBuf�ĵ�ַ��Ȼ��tmpBufָ����һ�����Ժ�preBuf����tmpBuf֮��
                  tmpBuf = tmpBuf->next;
               }
    }
      return HASHTBLERROR;

 }