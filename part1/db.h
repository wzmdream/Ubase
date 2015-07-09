/*****************************************************
* ��Ʒ����: UBASE                                    *
* ��ѧ����: �½�ũҵ��ѧ                             *
* ѧԺ����: ���������Ϣ����ѧԺ                     *
* ԭʼ����: ��̫��                                   *
* �޶�����: 2012-02-20                               *
* (c) Copyright 2006-2012. All rights reserved.      *
******************************************************
* �ļ�����: db.h                                     *
* �汾���: V1.00                                    *
* ��������: UBASE IO��ͷ�ļ����ӿ�)                  *
******************************************************
* ѧ��ѧ��: XXXXXXXXX                                *
* ѧ��������YYYYYYYYY                                *
* �޶���ǣ��Դ��ļ�û���޶�                         *
* �޶����ڣ�yyyy-mm-dd                               *
* �޶����ݣ���                                       *
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

// db������ǰ����
class DB;

// �ļ�����
class File {
   friend class DB;
   friend class OpenFileHashTbl;

   public:
   const Status allocatePage(int& pageNo);                          // ����һ����ҳ
   const Status disposePage(const int pageNo);                      // �ͷ�һҳ
   const Status readPage(const int pageNo, Page* pagePtr) const;    // ���ļ��н�һҳ�����ڴ�
   const Status writePage(const int pageNo, const Page* pagePtr);   // ���ڴ��е�һҳд���ļ�
   const Status getFirstPage(int& pageNo) const;                    // ���ص�һҳ��ҳ��

   const char* getFileName() //�����ļ�����
   {
      return fileName.c_str();
   }

   bool operator == (const File & other) const    //�ļ��������ȱȽϲ�������ʵ��
   {
      return fileName == other.fileName;
   }

   private: 
   File(const string &fname);       // ���캯��
   ~File();                         // ��������

   static const Status create(const string &fileName);
   static const Status destroy(const string &fileName);

   const Status open();
   const Status close();

   #ifdef DEBUGFREE
      void listFree();                 // �г��ļ������пհ�ҳ�����ڵ���
   #endif

   string fileName;                    // �ļ�����
   int openCnt;                        // �ļ��򿪵Ĵ���
   int unixFile;                       // �ļ���unixϵͳ���
};

class BufMgr;
extern BufMgr* bufMgr;

// ɢ�б�ÿ��ɢ�нڵ�Ľṹ�����ڹ�����ļ���
struct fileHashBucket
{
   string          fname;        // �ļ���
   File*           file;         // ָ���ļ������ָ��
   fileHashBucket* next;	 // ָ��ɢ�б�������ڵ��ָ��
	
};

// ������ļ���ɢ�б����
class OpenFileHashTbl
{
   private:
   int HTSIZE;                  // ɢ�б�Ͱ��
   fileHashBucket**  ht;        // ɢ�б���ָ������ʵ��
   int	 hash(string fileName);  // ɢ�к���������ֵ��0��HTSIZE-1֮�䣬��ʵ������������ɢ�б������±��

   public:
   OpenFileHashTbl();           // ���캯��
   ~OpenFileHashTbl();          // ��������
	
   // ��һ���ļ��״δ�ʱ����ɢ�б����һ���ڵ㣬���һ�������򷵻�OK�����򷵻�HASHTBLERROR
   Status insert(const string fileName, File* file);

   // ��ɢ�б��в�����ΪfileName���ļ��Ƿ���ڣ���������򷵻�OK��ͨ������file���ظ��ļ�����ָ�룬���򷵻�HASHNOTFOUND
   Status find(const string fileName, File*& file);

   // ��һ���ļ������׹ر�ʱ����ɢ�б�ɾ�����ļ��ڵ㣬���һ�������򷵻�OK�����򷵻�HASHTBLERROR
   Status erase(const string fileName);
};

class DB {
   public:
   DB();                                                          // ���캯�� 
   ~DB();                                                         // ��������

   const Status createFile(const string & fileName) ;             // �����ļ�

   const Status destroyFile(const string & fileName) ;            // ɾ���ļ�

   const Status openFile(const string & fileName, File* & file);  // ���ļ�

   const Status closeFile(File* file);                            // �ر��ļ�

   private:
   OpenFileHashTbl   openFiles;                                   // ������ļ���ɢ�б�
};


// structure of DB (header) page

typedef struct {
   int nextFree;                         // ָ���һ���հ�ҳ
   int firstPage;                        // ָ���һ������ҳ
   int numPages;                         // �ļ���ҳ��
}DBPage;

#endif
