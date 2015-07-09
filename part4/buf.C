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
// BufMgr��Ĺ��캯��
//---------------------------------------------------------------------------

BufMgr::BufMgr(const int bufs)
{
    numBufs = bufs;

    bufTable = new BufDesc[bufs];  // ���仺������������
    for (int i = 0; i < bufs; i++) bufTable[i].frameNo = i;  //��ʼ��֡��

    bufPool = new Page[bufs];	 // ���仺���
    //memset(bufPool, 0, bufs * sizeof(Page));  //��ʼ�����仺���
    bzero(bufPool, bufs * sizeof(Page));        //��ʼ�����仺���

    //�������bufs*1.2����С����
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
    //�������bufs*1.2����С��������


    hashTable = new BufHashTbl (htsize); // ���仺��ɢ�б�

    // ���ñ�־�ڵ㽫��������������ڵ����ӳ�˫��������

    rearUPL = &bufTable[0]; // ����rearUPLָ��
    frontUPL = new BufDesc; // �����־�ڵ�

    // ������������
    for (int i=0; i < bufs-1; i++)
    {
        bufTable[i].next = &bufTable[i+1];
    }
    bufTable[bufs-1].next = frontUPL;
    frontUPL->next = rearUPL;

    // ���÷�������
    for (int i=1; i < bufs; i++)
    {
        bufTable[i].prior = &bufTable[i-1];
    }
    bufTable[0].prior = frontUPL;
    frontUPL->prior = &bufTable[bufs-1];

    //���¶���Ӧ������
    ASSERT (rearUPL->next != NULL);
    ASSERT (rearUPL->prior == frontUPL);
    ASSERT (frontUPL->next == rearUPL);
}


//-----------------------------------------------------------------------
// BufMgr�����������
//-----------------------------------------------------------------------

BufMgr::~BufMgr() {

    // ��������ҳд�ش���
    for (int i = 0; i < numBufs; i++) 
    {
        BufDesc* tmpbuf = &bufTable[i];
        if (tmpbuf->valid == true && tmpbuf->dirty == true)
        {
	    tmpbuf->file->writePage(tmpbuf->pageNo, &(bufPool[i]));
        }
    }

    delete [] bufTable; // �ͷŻ���������
    delete [] bufPool;  // �ͷŻ����
    delete hashTable;   // �ͷ�ɢ�б�
    delete frontUPL;    // �ͷű�־�ڵ�
}

void BufMgr::BufDump()
{
    BufDesc  *p;          //����BufDesc���͵�ָ��
    p = &bufTable[0];     //�������������ĵ�һ���ڵ�ĵ�ַ����p
    int cntAct = 0;       //��¼��ס��pinCnt > 0���Ļ���֡������

    //�����־�ڵ��β�ڵ�ĵ�ַ
    printf ("\nfrontUPL = %x, rearUPL = %x\n", (unsigned int) frontUPL, (unsigned int) rearUPL);

    //�����ͷ���������Ʊ�����tab���ָ��ʽΪ��
    //frameNo		pageNo	pinCnt	dirty	valid
//    printf("frameNo\tfile\tpageNo\tpinCnt\tdirty\tvalid\n");

    //ѭ�����ÿ�������������ķ���ֵ����5��100��
/*
    for (int i=0; i<numBufs; i++,p++)
    {
        printf("%d\t%x\t",p->frameNo, (unsigned int) p->file);
        printf("%d\t%d\t%d\t%d\n", p->pageNo, p->pinCnt,p->dirty,p->valid);
        if (p->pinCnt > 0) cntAct++;
    }
*/
    //����������滻֡������������滻֡������
    p = rearUPL;
    int cntFwd = 0;

    printf("frameNo\tfile\tpageNo\tpinCnt\tdirty\tvalid\n");
    while (p != frontUPL)
    {
        printf("%d\t%x\t",p->frameNo, (unsigned int) p->file);
        printf("%d\t%d\t%d\t%d\n", p->pageNo, p->pinCnt,p->dirty,p->valid);

        cntFwd++;
        p = p->next;
        if (p->pinCnt > 0)
        printf("ERROR. frame %d is on unpinned list but has a pinCnt of %d\n",
                p->frameNo, p->pinCnt);
    }

    //����������滻֡������������滻֡������
    p = frontUPL->prior;
    int cntBwd = 0;
    while (p != frontUPL) //�ƻ�һȦʱ����ѭ��
    {
        cntBwd++;
        p = p->prior;
    }

    printf("��ס��֡��Ϊ�� %d, ���滻֡�������������Ϊ��%d, ���滻֡�������������Ϊ��%d\n", cntAct, cntFwd, cntBwd);

    ASSERT (rearUPL->next != NULL);         //���ǿ϶�rearUPL->next��ΪNULL
    ASSERT (rearUPL->prior == frontUPL);    //���ǿ϶�rearUPL->prior��frontUPL���
    ASSERT (frontUPL->next == rearUPL);     //���ǿ϶�frontUPL->next��rearUPL���
}


//-------------------------------------------------------------------------
// ������亯��
// �������ɹ��������ò���frame���ط���Ļ���֡��
//-------------------------------------------------------------------------

const Status BufMgr::allocBuf(int& frame) 
{
    Status status;
    BufDesc* victim;

    if (frontUPL == rearUPL) return BUFFEREXCEEDED;  // ����֡����ס

    // ��δ��ס֡��˫��������ȡ����rearUPLָ��Ľڵ�
    victim = rearUPL;
    ASSERT(victim->pinCnt == 0);

    // ��˫��������������rearUPLָ��Ľڵ�
    rearUPL->next->prior = rearUPL->prior;
    rearUPL->prior->next = rearUPL->next;
    rearUPL = rearUPL->next;

    ASSERT (rearUPL->next != NULL);
    ASSERT (rearUPL->prior == frontUPL);
    ASSERT (frontUPL->next == rearUPL);

    frame = victim->frameNo;  // ����֡��

    if (victim->valid == true)          // ��֡����ʹ��
    {
        if (victim->dirty == true)      // ��֡�����������޸� 
        {    
            if ((status = victim->file->writePage(victim->pageNo, &(bufPool[victim->frameNo]))) != OK)
	        return status;
        } 
        if ((status = hashTable->remove(victim->file, victim->pageNo)) != OK) return status;
    }
    victim->Clear();
    return OK;
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


const Status BufMgr::unPinPage(const File* file, const int PageNo, 
			       const bool dirty, const bool loveIt)
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

    if (hashTable->lookup(file, pageNo, index) != HASHNOTFOUND)
    {

        BufDesc* tmpbuf = &(bufTable[index]);

        if (tmpbuf->valid == false || tmpbuf->file != file || tmpbuf->pageNo != pageNo)
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
    { 
        return HASHNOTFOUND;
    }

    status = file->disposePage(pageNo);
    return status;
}


const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page)
{

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