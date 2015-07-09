#include <iostream>
#include <strings.h>
#include "page.h"

// page class constructor
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
      cout << "slot[" << i << "].offset = " << slot[i].offset 
	   << ", slot[" << i << "].length = " << slot[i].length << endl;
}

void Page::setNextPage(int pageNo)
{
    nextPage = pageNo;
}

const int Page::getNextPage() const
{
    return nextPage;
}

const short Page::getFreeSpace() const
{
  return freeSpace;
}
    
// Add a new record to the page. Returns OK if everything went OK
// otherwise, returns NOSPACE if sufficient space does not exist
// RID of the new record is returned via rid parameter
const Status Page::insertRecord(const Record & rec, RID& rid)
{
	// you will implement this method during part2
}

// delete a record from a page. Returns OK if everything went OK
// compacts remaining records but leaves hole in slot array
// use bcopy and not memcpy to do the compaction
const Status Page::deleteRecord(const RID & rid)
{
	// you will implement this method during part2


}

// update the record with the specified rid
const Status Page::updateRecord(const Record & rec,const RID& rid)
{
        // you will implement this method during part2


}

// returns RID of first record on page
const Status Page::firstRecord(RID& firstRid) const
{
	// you will implement this method during part2


}

// returns RID of next record on the page
// returns ENDOFPAGE if no more records exist on the page; otherwise OK
const Status Page::nextRecord (const RID &curRid, RID& nextRid) const
{
	// you will implement this method during part2

}

// returns length and pointer to record with RID rid
const Status Page::getRecord(const RID & rid, Record & rec)
{
	// you will implement this method during part2


}
