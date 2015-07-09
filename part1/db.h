/*****************************************************
* 产品名称: UBASE                                    *
* 大学名称: 新疆农业大学                             *
* 学院名称: 计算机与信息工程学院                     *
* 原始作者: 张太红                                   *
* 修订日期: 2012-02-20                               *
* (c) Copyright 2006-2012. All rights reserved.      *
******************************************************
* 文件名称: db.h                                     *
* 版本编号: V1.00                                    *
* 功能描述: UBASE IO层头文件（接口)                  *
******************************************************
* 学生学号: XXXXXXXXX                                *
* 学生姓名：YYYYYYYYY                                *
* 修订标记：对此文件没有修订                         *
* 修订日期：yyyy-mm-dd                               *
* 修订内容：无                                       *
*****************************************************/
#ifndef DB_H
#define DB_H

#include <sys/types.h>
#include <functional>
#include "error.h"
#include <string.h>
using namespace std;

// define if debug output wanted

//#define DEBUGIO
//#define DEBUGFREE

// db对象提前申明
class DB;

// 文件对象
class File {
   friend class DB;
   friend class OpenFileHashTbl;

   public:
   const Status allocatePage(int& pageNo);                          // 分配一个新页
   const Status disposePage(const int pageNo);                      // 释放一页
   const Status readPage(const int pageNo, Page* pagePtr) const;    // 从文件中将一页读入内存
   const Status writePage(const int pageNo, const Page* pagePtr);   // 将内存中的一页写入文件
   const Status getFirstPage(int& pageNo) const;                    // 返回第一页的页号

   const char* getFileName() //返回文件名称
   {
      return fileName.c_str();
   }

   bool operator == (const File & other) const    //文件对象的相等比较操作符的实现
   {
      return fileName == other.fileName;
   }

   private: 
   File(const string &fname);       // 构造函数
   ~File();                         // 析构函数

   static const Status create(const string &fileName);
   static const Status destroy(const string &fileName);

   const Status open();
   const Status close();

   #ifdef DEBUGFREE
      void listFree();                 // 列出文件中所有空白页，用于调试
   #endif

   string fileName;                    // 文件名称
   int openCnt;                        // 文件打开的次数
   int unixFile;                       // 文件的unix系统编号
};

class BufMgr;
extern BufMgr* bufMgr;

// 散列表每个散列节点的结构（用于管理打开文件）
struct fileHashBucket
{
   string          fname;        // 文件名
   File*           file;         // 指向文件对象的指针
   fileHashBucket* next;	 // 指向散列表中溢出节点的指针
	
};

// 管理打开文件的散列表对象
class OpenFileHashTbl
{
   private:
   int HTSIZE;                  // 散列表桶数
   fileHashBucket**  ht;        // 散列表，用指针数组实现
   int	 hash(string fileName);  // 散列函数，返回值在0和HTSIZE-1之间，其实就是用来计算散列表数组下标的

   public:
   OpenFileHashTbl();           // 构造函数
   ~OpenFileHashTbl();          // 析构函数
	
   // 当一个文件首次打开时，向散列表插入一个节点，如果一切正常则返回OK，否则返回HASHTBLERROR
   Status insert(const string fileName, File* file);

   // 在散列表中查找名为fileName的文件是否存在，如果存在则返回OK并通过参数file返回该文件对象指针，否则返回HASHNOTFOUND
   Status find(const string fileName, File*& file);

   // 当一个文件被彻底关闭时，从散列表删除该文件节点，如果一切正常则返回OK，否则返回HASHTBLERROR
   Status erase(const string fileName);
};

class DB {
   public:
   DB();                                                          // 构造函数 
   ~DB();                                                         // 析构函数

   const Status createFile(const string & fileName) ;             // 创建文件

   const Status destroyFile(const string & fileName) ;            // 删除文件

   const Status openFile(const string & fileName, File* & file);  // 打开文件

   const Status closeFile(File* file);                            // 关闭文件

   private:
   OpenFileHashTbl   openFiles;                                   // 管理打开文件的散列表
};


// structure of DB (header) page

typedef struct {
   int nextFree;                         // 指向第一个空白页
   int firstPage;                        // 指向第一个数据页
   int numPages;                         // 文件总页数
}DBPage;

#endif
