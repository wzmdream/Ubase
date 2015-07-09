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
  numBufs = bufs;

  bufTable = new BufDesc[bufs];  // allocate array of buffer frame descriptors
  for (int i = 0; i < bufs; i++) bufTable[i].frameNo = i;

  bufPool = new Page[bufs];	 // allocate the actual buffer pool
  memset(bufPool, 0, bufs * sizeof(Page));  // zero out all buffer pool frames

  int htsize = ((((int) (bufs * 1.2))*2)/2)+1; // all to make sure an odd number
  hashTable = new BufHashTbl (htsize); // allocate the buffer hash table

  //thread all pages in the pool together on the pinned list;
  // onto a two-way linked list with a sentinal node
  rearUPL = &bufTable[0]; // set rear pointer
  frontUPL = new BufDesc; // allocate a sentinal node

  // first set forward links
  for (int i=0; i < bufs-1; i++)
  {
	bufTable[i].next = &bufTable[i+1];   // forward link
  }
  bufTable[bufs-1].next = frontUPL;
  frontUPL->next = rearUPL;

  // then set reverse links
  for (int i=1; i < bufs; i++)
  {
	bufTable[i].prior = &bufTable[i-1];   // reverse link
  }
  bufTable[0].prior = frontUPL;
  frontUPL->prior = &bufTable[bufs-1];
  ASSERT (rearUPL->next != NULL);
  ASSERT (rearUPL->prior == frontUPL);
  ASSERT (frontUPL->next == rearUPL);
}

BufMgr::~BufMgr() {

  // flush out all unwritten pages
  for (int i = 0; i < numBufs; i++) 
    {
      BufDesc* tmpbuf = &bufTable[i];
      if (tmpbuf->valid == true && tmpbuf->dirty == true) {

#ifdef DEBUGBUF
	cout << "flushing page " << tmpbuf->pageNo
	     << " from frame " << i << endl;
#endif

	tmpbuf->file->writePage(tmpbuf->pageNo, &(bufPool[i]));
      }
    }

  delete [] bufTable;
  delete [] bufPool;
  delete hashTable; // invoke the hash table destructor to clean things up
  delete frontUPL;  // delete the sentinal node
}

void BufMgr::BufDump()
{
   int i; 
   BufDesc  *p;
   p = &bufTable[0];
   int cntAct = 0;

   printf ("\nfrontUPL = %x, rearUPL = %x\n", (unsigned int) frontUPL, (unsigned int) rearUPL);
   printf("frameNo\tfile\tpageNo\tpinCnt\tdirty\tvalid\n");
   for (i=0; i<numBufs; i++,p++)
   {
	printf("%d\t%x\t",p->frameNo, (unsigned int) p->file);
	printf("%d\t%d\t%d\t%d\n", p->pageNo, p->pinCnt,p->dirty,p->valid);
	if (p->pinCnt > 0) cntAct++;
   }
   // now count on the number of pages on the free list going forward
   p = rearUPL;
   int cntFwd = 0;
   while (p != frontUPL)
   {
	cntFwd++;
	p = p->next;
	if (p->pinCnt > 0) 
	 printf("ERROR. frame %d is on unpinned list but has a pinCnt of %d\n",
		p->frameNo, p->pinCnt);
   }
   // now count on the number of pages on the free list going backward
   p = frontUPL->prior;
   int cntBwd = 0;
   while (p != frontUPL) // stop when it wraps
   {
	cntBwd++;
	p = p->prior;
   }

   printf("# of pages actually pinned = %d, forward=%d, backward=%d\n",
	cntAct, cntFwd, cntBwd);

  ASSERT (rearUPL->next != NULL);
  ASSERT (rearUPL->prior == frontUPL);
  ASSERT (frontUPL->next == rearUPL);

}


const Status BufMgr::allocBuf(int& frame) 
{
  Status status;
  BufDesc* victim;

  if (frontUPL == rearUPL) return BUFFEREXCEEDED;  // all pages are pinned

  // take a buffer from the rear of the unpinned list
  victim = rearUPL;
  ASSERT(victim->pinCnt == 0);

  // remove entry from linked list
  rearUPL->next->prior = rearUPL->prior;
  rearUPL->prior->next = rearUPL->next;
  rearUPL = rearUPL->next;
  ASSERT (rearUPL->next != NULL);
  ASSERT (rearUPL->prior == frontUPL);
  ASSERT (frontUPL->next == rearUPL);
  frame = victim->frameNo;  // return frame

  if (victim->valid == false)          // page frame was not in use
  {
        victim->Clear();
	return OK;
  }
  else 
  {
      // else page frame was in use
      if (victim->dirty == true) 
      {    
	// page has unwritten data on it

#ifdef DEBUGBUF
	cout << "kicking out page " << victim->pageNo
             << " from frame " << victim->frameNo << endl;
#endif

	if ((status = victim->file->writePage(victim->pageNo, &(bufPool[victim->frameNo]))) != OK)
	  	return status;
	victim->dirty = false;
      } 
      if ((status = hashTable->remove(victim->file, victim->pageNo)) != OK) return status;
      victim->Clear();
      return OK;
  }
}

// release an unused buffer frame - used in error conditions
const void BufMgr::releaseBuf(int frame) 
{
    bufTable[frame].next = rearUPL;
    bufTable[frame].prior = rearUPL->prior;
    rearUPL->prior->next = &bufTable[frame];
    rearUPL->prior = &bufTable[frame];
    rearUPL  = &bufTable[frame];
    ASSERT (rearUPL->next != NULL);
    ASSERT (rearUPL->prior == frontUPL);
    ASSERT (frontUPL->next == rearUPL);
}
	
const Status BufMgr::readPage(File* file, const int PageNo, Page*& page)
{
  int index;
  Status status;

  if (!file)
    return BADFILE;

  if (hashTable->lookup(file, PageNo, index) == HASHNOTFOUND) 
  {
    // page was not found in the hashtable
    if ((status = allocBuf(index)) != OK) return status;
    if ((status = file->readPage(PageNo, &(bufPool[index]))) != OK)
    {
	 releaseBuf(index); // release allocated frame 
         return status;
    }

    if ((status = hashTable->insert(file, PageNo, index)) != OK)
    {
	 releaseBuf(index); // release allocated frame 
         return status;
    }
    bufTable[index].Set(file, PageNo);
  } 
  else 
  {
    // page was found in the buffer pool
    if (bufTable[index].valid == false) return BADBUFFER;
    if (bufTable[index].pinCnt == 0)
    {
	// page is not pinned which means that it on the doubly linked 
	// list of unpinned pages.  remove it from the list
	// check to see if removing the rear node on the unpinned list
	// if so, fix up rearUPL appropriately before doing the remove
	if (rearUPL == &bufTable[index]) rearUPL = rearUPL->next;
	bufTable[index].prior->next = bufTable[index].next;
	bufTable[index].next->prior = bufTable[index].prior;
	bufTable[index].next = bufTable[index].prior = NULL;
  	ASSERT (rearUPL->next != NULL);
  	ASSERT (rearUPL->prior == frontUPL);
  	ASSERT (frontUPL->next == rearUPL);
    }
    bufTable[index].pinCnt++;
  }

  // return pointer to the page
  page = &(bufPool[index]);

#ifdef DEBUGBUF
    cout << "Reading PageNo " << PageNo
         << " from Buffer " << index  
         << " pinCnt is " << bufTable[index].pinCnt 
	 << "\t" << (char*)page << "\n";
#endif DEBUGBUF

  return OK;
}


const Status BufMgr::unPinPage(const File* file, const int PageNo, 
			       const bool dirty, const bool loveIt)
{
  int index;

  // see if page is in the hash table
  if (hashTable->lookup(file, PageNo, index) == HASHNOTFOUND)
    return HASHNOTFOUND;

  if (bufTable[index].valid == false) return BADBUFFER;
  if (bufTable[index].pinCnt == 0) return PAGENOTPINNED;
  if (dirty == true) bufTable[index].dirty = dirty;
  bufTable[index].pinCnt--;
  if (bufTable[index].pinCnt == 0)
  {
      if (loveIt)
      {
	// page is LOVED, add it to the front of the unpinned list
	// simulates LRU
	bufTable[index].next = frontUPL;
	bufTable[index].prior = frontUPL->prior;
	frontUPL->prior->next = &bufTable[index];
	frontUPL->prior = &bufTable[index];
	// in the case that the list was empty need to reset rear pointer
	if (frontUPL == rearUPL) rearUPL = &bufTable[index];
  	ASSERT (rearUPL->next != NULL);
  	ASSERT (rearUPL->prior == frontUPL);
  	ASSERT (frontUPL->next == rearUPL);
      }
      else
      {
	// page is NOT LOVED, so add it to the rear of the unpinned list
	// so it will be allocated first - simulates MRU
	// add to rear of list
	bufTable[index].next = rearUPL;
	bufTable[index].prior = rearUPL->prior;
	rearUPL->prior->next = &bufTable[index];
	rearUPL->prior = &bufTable[index];
	rearUPL  = &bufTable[index];
  	ASSERT (rearUPL->next != NULL);
  	ASSERT (rearUPL->prior == frontUPL);
  	ASSERT (frontUPL->next == rearUPL);
      }
  }

#ifdef DEBUGBUF
  cout << "Unpinning pageNo " << PageNo << "\tpinCnt is "
       << bufTable[index].pinCnt << "\n";
#endif DEBUGBUF

  return OK;
}


const Status BufMgr::flushFile(const File* file) {

  Status status;

  for (int i = 0; i < numBufs; i++) {
    BufDesc* tmpbuf = &(bufTable[i]);
    if (tmpbuf->valid == true && tmpbuf->file == file) {

      if (tmpbuf->pinCnt > 0)
	  return PAGEPINNED;

      if (tmpbuf->dirty == true) {
#ifdef DEBUGBUF
	cout << "flushing page " << tmpbuf->pageNo
             << " from frame " << i << endl;
#endif
	if ((status = tmpbuf->file->writePage(tmpbuf->pageNo,
					      &(bufPool[i]))) != OK)
	  return status;

	tmpbuf->dirty = false;
      }

      hashTable->remove(file,tmpbuf->pageNo);

      tmpbuf->file = NULL;
      tmpbuf->pageNo = -1;
      tmpbuf->valid = false;
    }

    else if (tmpbuf->valid == false && tmpbuf->file == file)
      return BADBUFFER;
  }
  
  return OK;
}


const Status BufMgr::disposePage(File* file, const int pageNo) {

  Status status;
  int index;

  if (hashTable->lookup(file, pageNo, index) != HASHNOTFOUND)
    {

      BufDesc* tmpbuf = &(bufTable[index]);

      if (tmpbuf->valid == false || tmpbuf->file != file
	  || tmpbuf->pageNo != pageNo)
	return BADBUFFER;
  
      if (tmpbuf->pinCnt > 0)
	return PAGEPINNED;

      if((status = hashTable->remove(file, pageNo)) != OK)
	return status;

      tmpbuf->file = NULL;
      tmpbuf->pageNo = -1;
      tmpbuf->dirty = false;
      tmpbuf->valid = false;
    }

  else 
    return HASHNOTFOUND;

  status = file->disposePage(pageNo);
  return status;
}


const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page) {

  Status status;
  if ((status = file->allocatePage(pageNo)) != OK) return status;

  int index;

  status = allocBuf(index); // allocate a buffer
  if (status != OK || index >= numBufs) return status;

  if ((status = hashTable->insert(file, pageNo, index)) != OK) 
  {
      releaseBuf(index);
      return status;
  }

  bufTable[index].Set(file, pageNo);

//  bufTable[index].dirty = true;      // Is this needed ?
  page = &(bufPool[index]);

#ifdef DEBUGBUF
  cout << "Allocating pageNo " << pageNo   
       << " to Buffer " << index  
       << "\tpinCnt is " << bufTable[index].pinCnt << "\n";
#endif DEBUGBUF

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
