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

//---------------------------------------------------------------------------
// BufMgr类的构造函数
//---------------------------------------------------------------------------

BufMgr::BufMgr(const int bufs)
{
    numBufs = bufs;
    bufTable = new BufDesc[bufs];  // 分配缓存描述器数组
    for (int i = 0; i < bufs; i++) bufTable[i].frameNo = i;  //初始化帧号

    bufPool = new Page[bufs];	 // 分配缓存池
    //memset(bufPool, 0, bufs * sizeof(Page));  //初始化分配缓存池
    bzero(bufPool, bufs * sizeof(Page));        //初始化分配缓存池

    //计算大于bufs*1.2的最小素数
    int htsize = 3;
    int range;
    range = (int)ceil(bufs*1.2);
    do
    {
        if (range < htsize) break;
        int divisor = 3;
        do
        {
            if ((divisor*divisor)>htsize || (htsize % divisor)==0) break;
            divisor+=2;
        }while(true);
        if (((divisor*divisor)>htsize) && (htsize > range)) break;
        htsize+=2;
    }
    while(true);
    //计算大于bufs*1.2的最小素数结束


    hashTable = new BufHashTbl (htsize); // 分配缓存散列表

    // 利用标志节点将可替代缓存描述节点链接成双向环形链表
    rearUPL = &bufTable[0]; // 设置rearUPL指针
    frontUPL = new BufDesc; // 分配标志节点

    // 设置正向链接
    for (int i=0; i < bufs-1; i++)
    {
        bufTable[i].next = &bufTable[i+1];
    }
    bufTable[bufs-1].next = frontUPL;
    frontUPL->next = rearUPL;

    // 设置反向链接
    for (int i=1; i < bufs; i++)
    {
        bufTable[i].prior = &bufTable[i-1];
    }
    bufTable[0].prior = frontUPL;
    frontUPL->prior = &bufTable[bufs-1];

    //以下断言应当成立
    ASSERT (rearUPL->next != NULL);
    ASSERT (rearUPL->prior == frontUPL);
    ASSERT (frontUPL->next == rearUPL);
}


//-----------------------------------------------------------------------
// BufMgr类的析构函数
//-----------------------------------------------------------------------

BufMgr::~BufMgr() {

    // 将所有脏页写回磁盘
    for (int i = 0; i < numBufs; i++) 
    {
        BufDesc* tmpbuf = &bufTable[i];
        if (tmpbuf->valid == true && tmpbuf->dirty == true)
        {
	          tmpbuf->file->writePage(tmpbuf->pageNo, &(bufPool[i]));
        }
    }

    delete [] bufTable; // 释放缓存描述器
    delete [] bufPool;  // 释放缓存池
    delete hashTable;   // 释放散列表
    delete frontUPL;    // 释放标志节点
}

void BufMgr::BufDump()
{
    BufDesc  *p;          //声明BufDesc类型的指针
    p = &bufTable[0];     //将缓存描述器的第一个节点的地址赋给p
    int cntAct = 0;       //记录订住（pinCnt > 0）的缓存帧的数量

    //输出标志节点和尾节点的地址
    printf ("\nfrontUPL = %x, rearUPL = %x\n", (unsigned int) frontUPL, (unsigned int) rearUPL);

    //输出表头，各列用制表符（tab）分割，格式为：
    //frameNo		pageNo	pinCnt	dirty	valid
    printf("frameNo\tfileName\tpageNo\tpinCnt\tdirty\tvalid\n");

    //循环输出每个缓存描述器的分项值，共5列100行

    for (int i=0; i<numBufs; i++,p++)
    {
        printf("%d\t%s\t",p->frameNo, p->file->getFileName());
        printf("%d\t%d\t%d\t%d\n", p->pageNo, p->pinCnt,p->dirty,p->valid);
        if (p->pinCnt > 0) cntAct++;
    }

    //正向遍历可替换帧链表，计算可替换帧的数量
    p = rearUPL;
    int cntFwd = 0;

    //printf("frameNo\tfileName\tpageNo\tpinCnt\tdirty\tvalid\n");
    while (p != frontUPL)
    {
        //printf("%d\t%x\t",p->frameNo, (unsigned int) p->file);
        //printf("%d\t%d\t%d\t%d\n", p->pageNo, p->pinCnt,p->dirty,p->valid);

        cntFwd++;
        p = p->next;
        if (p->pinCnt > 0)
        printf("ERROR. frame %d is on unpinned list but has a pinCnt of %d\n",
                p->frameNo, p->pinCnt);
    }

    //反向遍历可替换帧链表，计算可替换帧的数量
    p = frontUPL->prior;
    int cntBwd = 0;
    while (p != frontUPL) //绕回一圈时结束循环
    {
        cntBwd++;
        p = p->prior;
    }

    printf("订住的帧数为： %d, 可替换帧数（正向遍历）为：%d, 可替换帧数（反向遍历）为：%d\n", cntAct, cntFwd, cntBwd);

    ASSERT (rearUPL->next != NULL);         //我们肯定rearUPL->next不为NULL
    ASSERT (rearUPL->prior == frontUPL);    //我们肯定rearUPL->prior和frontUPL相等
    ASSERT (frontUPL->next == rearUPL);     //我们肯定frontUPL->next和rearUPL相等
}
//-------------------------------------------------------------------------
// 缓存分配函数
// 如果分配成功，用引用参数frame返回分配的缓存帧号
//-------------------------------------------------------------------------

const Status BufMgr::allocBuf(int& frame) 
{
    Status status;
    BufDesc* victim;

    if (frontUPL == rearUPL) return BUFFEREXCEEDED;  // 所有帧被钉住

    // 从未钉住帧的双向环形链表取出由rearUPL指向的节点
    victim = rearUPL;
    ASSERT(victim->pinCnt == 0);

    // 从双向环形链表剥离由rearUPL指向的节点
    rearUPL->next->prior = rearUPL->prior;
    rearUPL->prior->next = rearUPL->next;
    rearUPL = rearUPL->next;

    ASSERT (rearUPL->next != NULL);
    ASSERT (rearUPL->prior == frontUPL);
    ASSERT (frontUPL->next == rearUPL);

    frame = victim->frameNo;  // 返回帧号

    if (victim->valid == true)          // 该帧曾被使用
    {
        if (victim->dirty == true)      // 该帧的内容曾被修改 
        {    
            if ((status = victim->file->writePage(victim->pageNo, &(bufPool[victim->frameNo]))) != OK)
	        return status;
        } 
        if ((status = hashTable->remove(victim->file, victim->pageNo)) != OK) return status;
    }
    victim->Clear();
    return OK;
}

//-------------------------------------------------------------------------
// 释放未使用的缓存帧，该函数在缓存帧已经分配而其他操作发生错误时调用
//-------------------------------------------------------------------------
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

    if (!file) return BADFILE;

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
    return OK;
}
const Status BufMgr::unPinPage(const File* file, const int PageNo, const bool dirty, const bool loveIt)
{
    int index;

    // see if page is in the hash table
    if (hashTable->lookup(file, PageNo, index) == HASHNOTFOUND) return HASHNOTFOUND;

    if (bufTable[index].valid == false) return BADBUFFER;

    if (bufTable[index].pinCnt == 0) return PAGENOTPINNED;

    if (dirty == true) bufTable[index].dirty = dirty;

    bufTable[index].pinCnt--;

    if (bufTable[index].pinCnt == 0)
    {
        if (loveIt)
        {
            // page is LOVED, add it to the front of the unpinned list,simulates LRU
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
    return OK;
}
const Status BufMgr::flushFile(const File* file)
{

    Status status;

    for (int i = 0; i < numBufs; i++)
    {
        BufDesc* tmpbuf = &(bufTable[i]);
        if (tmpbuf->valid == true && tmpbuf->file == file)
        {

            if (tmpbuf->pinCnt > 0)
	        return PAGEPINNED;

            if (tmpbuf->dirty == true)
            {
	        if ((status = tmpbuf->file->writePage(tmpbuf->pageNo, &(bufPool[i]))) != OK)
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
    if (hashTable->lookup(file, pageNo, index) == HASHNOTFOUND) return HASHNOTFOUND;

    BufDesc* tmpbuf = &(bufTable[index]);

    if (tmpbuf->valid == false || tmpbuf->file != file || tmpbuf->pageNo != pageNo)  return BADBUFFER;
  
    if (tmpbuf->pinCnt > 0)  return PAGEPINNED;

    if((status = hashTable->remove(file, pageNo)) != OK)  return status;

    releaseBuf(index);
    tmpbuf->Clear();

    status = file->disposePage(pageNo);
    return status;
}


const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page)
{

    Status status;

    int index;

    status = allocBuf(index); // allocate a buffer
    if (status != OK || index >= numBufs) return status;

    if ((status = file->allocatePage(pageNo)) != OK)
    {
		releaseBuf(index);
		return status;
	}
    if ((status = hashTable->insert(file, pageNo, index)) != OK) 
    {
        releaseBuf(index);
        return status;
    }

    bufTable[index].Set(file, pageNo);

//  bufTable[index].dirty = true;      // Is this needed ?
    page = &(bufPool[index]);

    return OK;
}

void BufMgr::printSelf(void)
{

    BufDesc* tmpbuf;
  
    cout << endl << "Print buffer...\n";
    for (int i=0; i<numBufs; i++)
    {
        tmpbuf = &(bufTable[i]);
        cout << i << "\t" << (char*)(&bufPool[i]) << "\tpinCnt: " << tmpbuf->pinCnt;
    
        if (tmpbuf->valid == true) cout << "\tvalid\n";
        cout << endl;
    };
}
