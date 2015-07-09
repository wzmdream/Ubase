#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"

//缓存池散列表实现

//******************************************************************************
// 散列函数
// 参数：
//     1、const File* file，文件对象指针
//     2、const int pageNo，页号
// 返回值：
//     value，整型，范围0到HTSIZE-1
//******************************************************************************

int BufHashTbl::hash(const File* file, const int pageNo)
{
    int tmp, value;
    tmp = (int)file;  //文件对象指针强制转换为整型
    value = (tmp + pageNo) % HTSIZE;
    return value;
}


//******************************************************************************
// 构造函数，创建并初始化散列表
// 参数：int htSize，散列表初级大小
//******************************************************************************

BufHashTbl::BufHashTbl(const int htSize)
{
    HTSIZE = htSize;
    // 分配指向散列节点（hashBuckets）的指针数组
    ht = new hashBucket* [htSize];

    //初始化每个指针
    for(int i=0; i < HTSIZE; i++)
        ht[i] = NULL;

}

//******************************************************************************
// 析构函数
//******************************************************************************

BufHashTbl::~BufHashTbl()
{
    //释放所有散列节点的散列表(ht)
    //因为ht为指针数组，我们用delete [] ht来释放它
    for(int i = 0; i < HTSIZE; i++)
    {
        hashBucket* tmpBuf = ht[i];
        while (ht[i])
        {
            tmpBuf = ht[i];
            ht[i] = ht[i]->next;
            delete tmpBuf;//释放散列节点
        }
    }
    delete [] ht;//释放指向散列节点（hashBuckets）的指针数组

}

//******************************************************************************
// 向散列表插入映射(file,pageNo)到frameNo的散列节点
// 成功返回OK，发生错误返回HASHTBLERROR
//******************************************************************************

Status BufHashTbl::insert(const File* file, const int pageNo, const int frameNo) 
{
    //计算散列表数组的下标
    int index = hash(file,pageNo);

    //查找节点是否存在
    hashBucket* tmpBuc = ht[index];
    while (tmpBuc)
    {
        //如果已经存在这个节点
        if ((tmpBuc->file == file) && (tmpBuc->pageNo == pageNo)) return HASHTBLERROR;

        //遍历溢出链的下一个节点
        tmpBuc = tmpBuc->next;
    }

    //创建一个新的散列节点
    tmpBuc = new hashBucket;
    if (!tmpBuc) return HASHTBLERROR;

    //设置散列节点的属性
    tmpBuc->file = (File*)file;
    tmpBuc->pageNo = pageNo;
    tmpBuc->frameNo = frameNo;

    //将散列节点插入到溢出链的头部
    tmpBuc->next = ht[index];
    ht[index] = tmpBuc;

    return OK;
}

//******************************************************************************
// 检查(file,pageNo)目前是否在缓存池(即是否在散列表)
// 如果存在，利用引用参数frameNo返回对应的帧号，否则返回HASHNOTFOUND
//******************************************************************************

Status BufHashTbl::lookup(const File* file, const int pageNo, int& frameNo) 
{
    int index = hash(file,pageNo); //计算散列表数组的下标
    hashBucket* tmpBuc = ht[index];

    //遍历溢出链节点
    while (tmpBuc)
    {
        if ((tmpBuc->file == file) && (tmpBuc->pageNo == pageNo))
        {
            frameNo=tmpBuc->frameNo; // 返回帧号
            return OK;
        }
        tmpBuc = tmpBuc->next;
    }
    return HASHNOTFOUND;
}

//******************************************************************************
// 从散列表删除(file,pageNo)节点
// 如果该节点存在，将其删除并返回OK，否则返回HASHTBLERROR
//******************************************************************************

Status BufHashTbl::remove(const File* file, const int pageNo) 
{
    int index = hash(file,pageNo);   //计算散列表数组的下标

    hashBucket* tmpBuc = ht[index];  //溢出链上的第一个节点

    hashBucket* prevBuc = ht[index]; //溢出链上当前节点的前一个节点

    //遍历溢出链节点
    while (tmpBuc)
    {
        if ((tmpBuc->file == file) && (tmpBuc->pageNo == pageNo)) //找到匹配节点
        {
            if(tmpBuc == ht[index]) //如果是溢出链上的第一个节点
            {
                ht[index] = tmpBuc->next;
            }
            else //如果不是溢出链上的第一个节点
            {
                prevBuc->next = tmpBuc->next; //修改前一个节点的next指针
            }
            delete tmpBuc; //删除找到的匹配节点
            return OK;
        }
        else
        {
            prevBuc = tmpBuc;//当前节点变为前一个节点
            tmpBuc = tmpBuc->next; //当前节点移向下一个节点
        }
    }
    return HASHTBLERROR;
}
