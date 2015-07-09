#include <sys/types.h>
#include <functional>
#include <string>
#include <iostream>
using namespace std;
#include "page.h"

// page类初始化函数
void Page::init(int pageNo)
{
    nextPage = -1;
    slotCnt = 0; //新的可用插槽，0表示没有记录
    curPage = pageNo;
    freePtr = 0; //data数组中可用空间的偏移量
    freeSpace = PAGESIZE-DPFIXED + sizeof(slot_t); //可用空间，没有包括删除中间记录后留下的空插槽的空间
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


//向该页插入一条记录，如果没有足够的空间则返回NOSPACE，一切正常则返回OK
//通过参数rid返回插入记录的页号和插槽号
const Status Page::insertRecord(const Record rec, RID& rid)
{
    rid = NULLRID; //出错返回时，保证返回的rid为{-1，-1}

    //寻找空插槽
    int i;
    for (i=0; ((i>slotCnt) && (slot[i].length != -1)); i--);
    //循环结束后，i要么是第一个可用的空插槽，要么是slotCnt（新插槽）。
    //不管是那中情况，我们都可以直接用i充当要插入记录的插槽数组下标。
    
    if (i == slotCnt) //如果是新插槽
    {
        if ((rec.length + (int)sizeof(slot_t)) > freeSpace) return NOSPACE; //检查可用空间
        freeSpace -= (rec.length + sizeof(slot_t));                         //调整可用空间
        slotCnt--;                                                          //调整新插槽标志
    }
    else //如果是删除记录留下的空插槽
    {
        if (rec.length > freeSpace) return NOSPACE; //检查可用空间
        freeSpace -= rec.length;                    //调整可用空间
    }
    //设置该插槽的属性
    slot[i].offset = freePtr;
    slot[i].length = rec.length;
    memcpy(&data[freePtr], rec.data, rec.length); //把记录数据拷贝到页的合适位置
    freePtr += rec.length;                        //调整data数组中可用空间的偏移量
    //设置返回参数的属性
    rid.pageNo = curPage;
    rid.slotNo = -i;
    return OK;
}

// update the record with the specified rid
const Status Page::updateRecord(const Record rec,const RID& rid)
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
// use bcopy or memmove but not memcpy to do the compaction
const Status Page::deleteRecord(const RID & rid)
{
    int slotNo = -rid.slotNo; //转换为负数形式
    //首先检查要删除的记录是否有效
    if ((slot[slotNo].length==-1) || (!(slotNo>slotCnt))) return INVALIDSLOTNO;

    //如果要删除记录的数据不在页的尾部，需要移动其后的数据以压缩删除后出现空穴
    if ((slot[slotNo].offset+slot[slotNo].length)!=freePtr)
    {
        char* dest = &data[slot[slotNo].offset];   //要删除记录数据的起始指针
        char* src = &data[slot[slotNo].offset+slot[slotNo].length]; //下一条记录数据的起始指针
        memmove(dest, src, freePtr-(slot[slotNo].offset+slot[slotNo].length)); //移动操作

        //调整数据在删除记录后面所有记录的偏移量
        for(int i = 0; i > slotCnt; i--)//扫描所有记录
        {
            //只对数据在删除记录后面所有记录进行调整
            if (slot[i].length >= 0 && slot[i].offset > slot[slotNo].offset)
            {
                slot[i].offset -= slot[slotNo].length;
            }
        }
    }
    freeSpace+=slot[slotNo].length; //调整可用空间
    freePtr-=slot[slotNo].length;   //调整data数组中可用空间的偏移量
    memset(&data[freePtr],0,slot[slotNo].length); //清除尾部腾出的内存

    slot[slotNo].length=-1; //标记该插槽为空插槽
    slot[slotNo].offset=-1; //标记该插槽为空插槽

    //如果腾出的插槽在数组的尾部，此时可以压缩插槽数组，注意，可能还要压缩以前删除留下的空槽
    if (slotNo == slotCnt + 1)
    {
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
const Status Page::nextRecord (const RID curRid, RID& nextRid) const
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
const Status Page::getRecord(const RID rid, Record & rec)
{
    int i = -(rid.slotNo);
    if (slot[i].length==-1) return INVALIDSLOTNO;
    rec.length=slot[i].length;
    rec.data=&data[slot[i].offset];
    return OK;
}
