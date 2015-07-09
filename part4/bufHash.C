#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"

//�����ɢ�б�ʵ��

//******************************************************************************
// ɢ�к���
// ������
//     1��const File* file���ļ�����ָ��
//     2��const int pageNo��ҳ��
// ����ֵ��
//     value�����ͣ���Χ0��HTSIZE-1
//******************************************************************************

int BufHashTbl::hash(const File* file, const int pageNo)
{
    int tmp, value;
    tmp = (int)file;  //�ļ�����ָ��ǿ��ת��Ϊ����
    value = (tmp + pageNo) % HTSIZE;
    return value;
}


//******************************************************************************
// ���캯������������ʼ��ɢ�б�
// ������int htSize��ɢ�б�������С
//******************************************************************************

BufHashTbl::BufHashTbl(int htSize)
{
    HTSIZE = htSize;

    // ����ָ��ɢ�нڵ㣨hashBuckets����ָ������
    ht = new hashBucket* [htSize];

    //��ʼ��ÿ��ָ��
    for(int i=0; i < HTSIZE; i++)
        ht[i] = NULL;
}


//******************************************************************************
// ��������
//******************************************************************************

BufHashTbl::~BufHashTbl()
{
    //�ͷ�����ɢ�нڵ��ɢ�б�(ht)
    //��ΪhtΪָ�����飬������delete [] ht���ͷ���
    for(int i = 0; i < HTSIZE; i++)
    {
        hashBucket* tmpBuf = ht[i];
        while (ht[i])
        {
            tmpBuf = ht[i];
            ht[i] = ht[i]->next;
            delete tmpBuf;
        }
    }
    delete [] ht;
}

//******************************************************************************
// ��ɢ�б�����ӳ��(file,pageNo)��frameNo��ɢ�нڵ�
// �ɹ�����OK���������󷵻�HASHTBLERROR
//******************************************************************************

Status BufHashTbl::insert(const File* file, const int pageNo, const int frameNo) 
{
    //����ɢ�б�������±�
    int index = hash(file,pageNo);

    //���ҽڵ��Ƿ����
    hashBucket* tmpBuc = ht[index];
    while (tmpBuc)
    {
        //����Ѿ���������ڵ�
        if ((tmpBuc->file == file) && (tmpBuc->pageNo == pageNo)) return HASHTBLERROR;

        //�������������һ���ڵ�
        tmpBuc = tmpBuc->next;
    }

    //����һ���µ�ɢ�нڵ�
    tmpBuc = new hashBucket;
    if (!tmpBuc) return HASHTBLERROR;

    //����ɢ�нڵ������
    tmpBuc->file = (File*)file;
    tmpBuc->pageNo = pageNo;
    tmpBuc->frameNo = frameNo;

    //��ɢ�нڵ���뵽�������ͷ��
    tmpBuc->next = ht[index];
    ht[index] = tmpBuc;

    return OK;
}

//******************************************************************************
// ���(file,pageNo)Ŀǰ�Ƿ��ڻ����(���Ƿ���ɢ�б�)
// ������ڣ��������ò���frameNo���ض�Ӧ��֡�ţ����򷵻�HASHNOTFOUND
//******************************************************************************

Status BufHashTbl::lookup(const File* file, const int pageNo, int& frameNo) 
{
    int index = hash(file,pageNo); //����ɢ�б�������±�
    hashBucket* tmpBuc = ht[index];

    //����������ڵ�
    while (tmpBuc)
    {
        if ((tmpBuc->file == file) && (tmpBuc->pageNo == pageNo))
        {
            frameNo=tmpBuc->frameNo; // ����֡��
            return OK;
        }
        tmpBuc = tmpBuc->next;
    }
    return HASHNOTFOUND;
}

//******************************************************************************
// ��ɢ�б�ɾ��(file,pageNo)�ڵ�
// ����ýڵ���ڣ�����ɾ��������OK�����򷵻�HASHTBLERROR
//******************************************************************************

Status BufHashTbl::remove(const File* file, const int pageNo) 
{
    int index = hash(file,pageNo);   //����ɢ�б�������±�

    hashBucket* tmpBuc = ht[index];  //������ϵĵ�һ���ڵ�

    hashBucket* prevBuc = ht[index]; //������ϵ�ǰ�ڵ��ǰһ���ڵ�

    ////����������ڵ�
    while (tmpBuc)
    {
        if ((tmpBuc->file == file) && (tmpBuc->pageNo == pageNo)) //�ҵ�ƥ��ڵ�
        {
            if(tmpBuc == ht[index]) //�����������ϵĵ�һ���ڵ�
            {
                ht[index] = tmpBuc->next;
            }
            else //�������������ϵĵ�һ���ڵ�
            {
                prevBuc->next = tmpBuc->next; //�޸�ǰһ���ڵ��nextָ��
            }
            delete tmpBuc; //ɾ���ҵ���ƥ��ڵ�
            return OK;
        }
        else
        {
            prevBuc = tmpBuc;//��ǰ�ڵ��Ϊǰһ���ڵ�
            tmpBuc = tmpBuc->next; //��ǰ�ڵ�������һ���ڵ�
        }
    }
    return HASHTBLERROR;
}