#include <sys/types.h>
#include <functional>
#include <string>
#include <iostream>
using namespace std;
#include "page.h"

// page���ʼ������
void Page::init(int pageNo)
{
    nextPage = -1;
    slotCnt = 0; //�µĿ��ò�ۣ�0��ʾû�м�¼
    curPage = pageNo;
    freePtr = 0; //data�����п��ÿռ��ƫ����
    freeSpace = PAGESIZE-DPFIXED + sizeof(slot_t); //���ÿռ䣬û�а���ɾ���м��¼�����µĿղ�۵Ŀռ�
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


//���ҳ����һ����¼�����û���㹻�Ŀռ��򷵻�NOSPACE��һ�������򷵻�OK
//ͨ������rid���ز����¼��ҳ�źͲ�ۺ�
const Status Page::insertRecord(const Record rec, RID& rid)
{
    rid = NULLRID; //��������ʱ����֤���ص�ridΪ{-1��-1}

    //Ѱ�ҿղ��
    int i;
    for (i=0; ((i>slotCnt) && (slot[i].length != -1)); i--);
    //ѭ��������iҪô�ǵ�һ�����õĿղ�ۣ�Ҫô��slotCnt���²�ۣ���
    //������������������Ƕ�����ֱ����i�䵱Ҫ�����¼�Ĳ�������±ꡣ
    
    if (i == slotCnt) //������²��
    {
        if ((rec.length + (int)sizeof(slot_t)) > freeSpace) return NOSPACE; //�����ÿռ�
        freeSpace -= (rec.length + sizeof(slot_t));                         //�������ÿռ�
        slotCnt--;                                                          //�����²�۱�־
    }
    else //�����ɾ����¼���µĿղ��
    {
        if (rec.length > freeSpace) return NOSPACE; //�����ÿռ�
        freeSpace -= rec.length;                    //�������ÿռ�
    }
    //���øò�۵�����
    slot[i].offset = freePtr;
    slot[i].length = rec.length;
    memcpy(&data[freePtr], rec.data, rec.length); //�Ѽ�¼���ݿ�����ҳ�ĺ���λ��
    freePtr += rec.length;                        //����data�����п��ÿռ��ƫ����
    //���÷��ز���������
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
    int slotNo = -rid.slotNo; //ת��Ϊ������ʽ
    //���ȼ��Ҫɾ���ļ�¼�Ƿ���Ч
    if ((slot[slotNo].length==-1) || (!(slotNo>slotCnt))) return INVALIDSLOTNO;

    //���Ҫɾ����¼�����ݲ���ҳ��β������Ҫ�ƶ�����������ѹ��ɾ������ֿ�Ѩ
    if ((slot[slotNo].offset+slot[slotNo].length)!=freePtr)
    {
        char* dest = &data[slot[slotNo].offset];   //Ҫɾ����¼���ݵ���ʼָ��
        char* src = &data[slot[slotNo].offset+slot[slotNo].length]; //��һ����¼���ݵ���ʼָ��
        memmove(dest, src, freePtr-(slot[slotNo].offset+slot[slotNo].length)); //�ƶ�����

        //����������ɾ����¼�������м�¼��ƫ����
        for(int i = 0; i > slotCnt; i--)//ɨ�����м�¼
        {
            //ֻ��������ɾ����¼�������м�¼���е���
            if (slot[i].length >= 0 && slot[i].offset > slot[slotNo].offset)
            {
                slot[i].offset -= slot[slotNo].length;
            }
        }
    }
    freeSpace+=slot[slotNo].length; //�������ÿռ�
    freePtr-=slot[slotNo].length;   //����data�����п��ÿռ��ƫ����
    memset(&data[freePtr],0,slot[slotNo].length); //���β���ڳ����ڴ�

    slot[slotNo].length=-1; //��Ǹò��Ϊ�ղ��
    slot[slotNo].offset=-1; //��Ǹò��Ϊ�ղ��

    //����ڳ��Ĳ���������β������ʱ����ѹ��������飬ע�⣬���ܻ�Ҫѹ����ǰɾ�����µĿղ�
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