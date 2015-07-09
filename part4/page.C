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
        cout << "slot[" << i << "].offset = " << slot[i].offset << ", slot[" << i << "].length = " << slot[i].length << endl;
        if (slot[i].length==-1)
        {
            cout << "This record has been deleted!" << endl;
        }
        else
        {
            for (int j=slot[i].offset;j<slot[i].offset+slot[i].length;j++)
            {
                printf("%c", data[j]);
            }
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
    RID tmpRid;
    rid = NULLRID;
    int i=0;
    // look for an empty slot
    while (i > slotCnt)
    {
        if (slot[i].length == -1) break;
        else i--;
    }
    // at this point we have either found an empty slot
    // or i will be equal to slotCnt.  In either case,
    // we can just use i as the slot index

    // adjust free space
    if (i == slotCnt)
    {
        // using a new slot
        if ((rec.length + (int)sizeof(slot_t)) > freeSpace) return NOSPACE; // checking if sufficient space exists
        freeSpace -= (rec.length + sizeof(slot_t)); // adjust free space
        slotCnt--;
    }
    else
    {
        // reusing an existing slot
        if (rec.length > freeSpace) return NOSPACE; // checking if sufficient space exists
        freeSpace -= rec.length; // adjust free space
    }

    // use existing value of slotCnt as the index into slot array
    // use before incrementing because constructor sets the initial
    // value to 0
    slot[i].offset = freePtr;
    slot[i].length = rec.length;

    memcpy(&data[freePtr], rec.data, rec.length); // copy data on to the data page
    freePtr += rec.length; // adjust freePtr

    tmpRid.pageNo = curPage;
    tmpRid.slotNo = -i; // make a positive slot number
    rid = tmpRid;

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

// delete a record from a page. Returns OK if everything went OK
// compacts remaining records but leaves hole in slot array
// use bcopy and not memcpy to do the compaction
const Status Page::deleteRecord(const RID & rid)
{
    int slotNo = -rid.slotNo; //convert to negative format
    //first check the record validation
    if ((slot[slotNo].length==-1) || (slotNo<=slotCnt)) return INVALIDSLOTNO;

    if ((slot[slotNo].offset+slot[slotNo].length)==freePtr)
    {
        //the record will be deleted is physically the last record
        //in this case the records do not need to be compacted
        freeSpace+=slot[slotNo].length;
        freePtr-=slot[slotNo].length;
        slot[slotNo].length=-1; // mark the slot free
        slot[slotNo].offset=-1; // mark the slot free
    }
    else
    {
        //the record being deleted has one or more records after it.
        //This case requires compaction.
        int thisOffset=slot[slotNo].offset; // offset of record being deleted
        char* recPtr = &data[thisOffset];  // get a pointer to the record

        int nextOffset=slot[slotNo].offset+slot[slotNo].length; // offset of next record
        char* nextRec = &data[nextOffset]; // pointer to the next record

        int cnt=freePtr-nextOffset; // calculate number of bytes to move
        
        bcopy(nextRec, recPtr, cnt); // shift bytes to the left 

        // now need to adjust offsets of all valid slots to the
        // 'right' of slot being removed by size of the hole

        for(int i = 0; i > slotCnt; i--)
        {
            if (slot[i].length >= 0 && slot[i].offset > slot[slotNo].offset)
            {
                slot[i].offset -= slot[slotNo].length;
            }
        }
        freeSpace+=slot[slotNo].length;
        freePtr-=slot[slotNo].length;
        slot[slotNo].length=-1; // mark the slot free
        slot[slotNo].offset=-1; // mark the slot free

    }
    if (slotNo == slotCnt + 1)
    {
        // Slot being freed is at end of slot array. In this
        // case we can compact the slot array. Note that we
        // should even compact slots that might have been
        // emptied previously.
        do
        {
            slotCnt++;
            freeSpace += sizeof(slot_t);
        }
        while (slotCnt < 0 && slot[slotCnt + 1].length == -1);
    }
    return OK;
}

// returns RID of first record on page
const Status Page::firstRecord(RID& firstRid) const
{
    if (slotCnt==0) return NORECORDS;
    for (int i=0;i>slotCnt;i--)
    {
        if (slot[i].length!=-1)
        {
            firstRid.pageNo=curPage;
            firstRid.slotNo=abs(i);
            break;
        }
    }
    return OK;
}

// returns RID of next record on the page
// returns ENDOFPAGE if no more records exist on the page; otherwise OK
const Status Page::nextRecord (const RID &curRid, RID& nextRid) const
{
    for (int i= -(curRid.slotNo+1);i>slotCnt;i--)
    {
        if (slot[i].length!=-1)
        {
            nextRid.pageNo=curPage;
            nextRid.slotNo=abs(i);
            return OK;
        }
    }
    return ENDOFPAGE;
}



// returns length and pointer to record with RID rid
const Status Page::getRecord(const RID & rid, Record & rec)
{
    int i = -(rid.slotNo);
    if (slot[i].length==-1) return INVALIDSLOTNO;
    rec.length=slot[i].length;
    rec.data=&data[slot[i].offset];
    return OK;
}
