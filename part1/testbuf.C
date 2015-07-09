#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include "page.h"
#include "buf.h"
#include "db.h"

//应当返回OK的函数调用该宏
#define CALL(c)\
{    Status s; \
     if ((s = c) != OK) { \
        cerr << "错误发生在测试程序的第" << __LINE__ << "行" << endl; \
        cerr << "此时调用的函数为：" << #c << endl;\
        error.print(s); \
        cerr << "很遗憾，没有通过测试！" <<endl; \
        exit(1); \
      } \
}
//应当返回错误的函数调用该宏
#define FAIL(c)\
{\
	Status s;\
	if ((s = c) == OK)\
	{\
		cerr << "在测试程序的第" << __LINE__ << "行" << endl;\
		cerr << "调用函数" #c << "应当发生错误却返回了OK" << endl;\
		cerr << "很遗憾，不能通过测试！" <<endl;\
		exit(1);\
	}\
}
BufMgr* bufMgr; //声明缓存管理器对象

//#define LOVE 1
//#define HATE 0

int main()
{
    struct stat statusBuf;
    Error   error;
    DB*     db;  
    db = new DB;
    File*   file1;
    File*   file2;
    File*   file3;
    File*   file4;
    int     i;
    const int   num = 100;  //缓存池大小为100帧
    int         j[num];    
    bufMgr = new BufMgr(num); // 创建缓存管理器对象
/*
    File*   file[1024];
    char    filename[10];
    for (int i=5; i<1030; i++)
    {
       sprintf(filename, "test.%d",i);
       cout << filename << endl;
       CALL(db->createFile(filename));
       db->openFile(filename, file[i]);
    }
*/
    //检查文件是否已经存在，如果已经存在，删除之
    lstat("test.1", &statusBuf);
    if (errno == ENOENT)
        errno = 0;
    else 
        (void)db->destroyFile("test.1");

    lstat("test.2", &statusBuf);
    if (errno == ENOENT)
        errno = 0;
    else 
        (void)db->destroyFile("test.2");

    lstat("test.3", &statusBuf);
    if (errno == ENOENT)
        errno = 0;
    else
       (void)db->destroyFile("test.3");

    lstat("test.4", &statusBuf);
    if (errno == ENOENT)
        errno = 0;
    else
        (void)db->destroyFile("test.4");
    //创建4个关系文件
    CALL(db->createFile("test.1"));
    ASSERT(db->createFile("test.1") == FILEEXISTS);
    CALL(db->createFile("test.2"));
    CALL(db->createFile("test.3"));
    CALL(db->createFile("test.4"));
    //exit(0);
    //打开4个关系文件
    //CALL(db.openFile("test.1", file1));
    db->openFile("test.1", file1);
    CALL(db->openFile("test.2", file2));
    CALL(db->openFile("test.3", file3));
    CALL(db->openFile("test.4", file4));
    //exit(0);
    // test buffer manager
    Page* page;
    Page* page2;
    Page* page3;
    char  cmp[PAGESIZE];
    int pageno, pageno2, pageno3;

    cout << "为文件test.1分配100个数据页..." << endl;
    cout << "修改缓存中的100帧..." << endl;
    cout << "解除缓存中100帧的摁钉..." << endl;
    for (i = 0; i < num; i++)
    {      
        CALL(bufMgr->allocPage(file1, j[i], page));
        //cout<<(char*)page<<"      "<<j[i]<<endl;
        sprintf((char*)page, "test.1 Page %d %7.1f", j[i], (float)j[i]);
        //cout<<(char*)page<<"      "<<j[i]<<endl;
        CALL(bufMgr->unPinPage(file1, j[i], true));
    }

    //bufMgr->BufDump();
    cout <<"恭喜：测试1顺利通过"<<endl<<endl;

    cout << "顺序读文件test.1的100个数据页..." << endl;
    cout << "比较读入的内容..." << endl;
    cout << "解除缓存中100帧的摁钉..." << endl;
    for (i = 0; i < num; i++)
    {
        CALL(bufMgr->readPage(file1, j[i], page));
       // cout<<(char*)page<<endl;
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", j[i], (float)j[i]);
       // cout<<(char*)cmp<<endl;
        ASSERT(memcmp(page, &cmp, strlen((char*)&cmp)) == 0);
        CALL(bufMgr->unPinPage(file1, j[i], true));
    }
    //bufMgr->BufDump();
    cout<< "恭喜：测试2顺利通过"<<endl<<endl;

    cout << "为文件test.2、test.3分配33个数据页..." << endl;
    cout << "随即读test.1文件33页并比较读入的内容..." << endl;
    cout << "顺序读test.2、test.3文件33页并比较读入的内容..." << endl;
    cout << "解除缓存中test.1文件33帧的摁钉..." << endl;
    cout << "解除缓存中test.2、test.3文件33帧的摁钉..." << endl;
    for (i = 0; i < num/3; i++) 
    {
        CALL(bufMgr->allocPage(file2, pageno2, page2));
        sprintf((char*)page2, "test.2 Page %d %7.1f", pageno2, (float)pageno2);
       
        CALL(bufMgr->allocPage(file3, pageno3, page3));
        sprintf((char*)page3, "test.3 Page %d %7.1f", pageno3, (float)pageno3);
         //cout<<"当前页是333:"<<(char*)page3<<" "<<endl;
        pageno = j[random() % num];
        CALL(bufMgr->readPage(file1, pageno, page));
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", pageno, (float)pageno);
        //cout<<"当前页是111:"<<(char*)page<<"     "<<i<<endl;
       
        //cout<<"cmp:"<<(char*)cmp<<"     "<<i<<endl;
        ASSERT(memcmp(page, &cmp, strlen((char*)&cmp)) == 0);
        
        CALL(bufMgr->readPage(file2, pageno2, page2));
        sprintf((char*)&cmp, "test.2 Page %d %7.1f", pageno2, (float)pageno2);
         //cout<<"当前页是2:"<<(char*)page2<<"     "<<i<<endl;
      //bufMgr->BufDump();
        ASSERT(memcmp(page2, &cmp, strlen((char*)&cmp)) == 0);
        //cout<<"当前cmp是2:"<<(char*)&cmp<<"     "<<i<<endl;
        CALL(bufMgr->readPage(file3, pageno3, page3));
        sprintf((char*)&cmp, "test.3 Page %d %7.1f", pageno3, (float)pageno3);
        ASSERT(memcmp(page3, &cmp, strlen((char*)&cmp)) == 0);
         //cout<<(char*)page3<<endl;
        CALL(bufMgr->unPinPage(file1, pageno, true));
    }
    //bufMgr->BufDump();

    for (i = 0; i < num/3; i++)
    {
        CALL(bufMgr->unPinPage(file2, i+1, true));
        CALL(bufMgr->unPinPage(file2, i+1, true));
        CALL(bufMgr->unPinPage(file3, i+1, true));
        CALL(bufMgr->unPinPage(file3, i+1, true));
    }

    //bufMgr->BufDump();
    //exit(0);

    cout << "恭喜：测试3顺利通过" << endl<<endl;



    //bufMgr->printSelf();

    cout << "顺序读test.1文件33页并比较读入的内容..." << endl;
    cout << "解除缓存中test.1文件33帧的摁钉..." << endl;

    for (i = 1; i < num/3; i++)
    {
        CALL(bufMgr->readPage(file1, i, page2));
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page2, &cmp, strlen((char*)&cmp)) == 0);
        CALL(bufMgr->unPinPage(file1, i, false));
    }
    //bufMgr->BufDump();
    //exit(0);

    cout << "恭喜：测试4顺利通过" <<endl<<endl;

    cout << "顺序读test.2文件33页并比较读入的内容..." << endl;
    cout << "解除缓存中test.2文件33帧的摁钉..." << endl;

    for (i = 1; i < num/3; i++)
    {
        CALL(bufMgr->readPage(file2, i, page2));
        sprintf((char*)&cmp, "test.2 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page2, &cmp, strlen((char*)&cmp)) == 0);
        //cout << (char*)page2 << endl;
        CALL(bufMgr->unPinPage(file2, i, false));
    }
    cout << "恭喜：测试5顺利通过" <<endl<<endl;


    cout << "顺序读test.3文件33页并比较读入的内容..." << endl;
    cout << "解除缓存中test.3文件33帧的摁钉..." << endl;

    for (i = 1; i < num/3; i++)
    {
        CALL(bufMgr->readPage(file3, i, page3));
        sprintf((char*)&cmp, "test.3 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page3, &cmp, strlen((char*)&cmp)) == 0);
        //cout << (char*)page3 << endl;
        CALL(bufMgr->unPinPage(file3, i, false));
    }

    cout << "恭喜：测试6顺利通过" <<endl<<endl;

    cout << "读test.4文件的第一数据页"<< endl;
    cout << "应当提示错误信息，因为test.4尚未分配数据页"<<endl;

    Status status;
    FAIL(status = bufMgr->readPage(file4, 1, page));
    error.print(status);
    //bufMgr->BufDump();
    cout << "恭喜：测试7顺利通过" <<endl<<endl;
 
    cout << "为test.4文件分配第一数据页"<< endl;
    cout << "解除缓存中test.4文件第一数据页的摁钉..." << endl;
    cout << "第二次应当提示错误信息，因为缓存中test.4文件第一数据页的摁钉数已经为0"<<endl;
    CALL(bufMgr->allocPage(file4, i, page));
    CALL(bufMgr->unPinPage(file4, i, true));
    FAIL(status = bufMgr->unPinPage(file4, i, false));
    error.print(status);
    //bufMgr->BufDump();
    cout << "恭喜：测试8顺利通过" <<endl<<endl;
    
    cout << "为test.4文件再分配100个数据页"<< endl;
    for (i = 0; i < num; i++)
    {
        CALL(bufMgr->allocPage(file4, j[i], page));
        sprintf((char*)page, "test.4 Page %d %7.1f", j[i], (float)j[i]);
    }
    cout << "在缓存中100个帧均被钉住的情况下，为test.4文件再分配1个数据页，应当出错"<< endl;
    int tmp;
    FAIL(status = bufMgr->allocPage(file4, tmp, page));
    error.print(status);

    cout << "恭喜：测试9顺利通过" <<endl<<endl;

    //bufMgr->BufDump();

    //bufMgr->printSelf();

    for (i = 0; i < num; i++)
        CALL(bufMgr->unPinPage(file4, i+2, true));
    
    cout << "顺序读test.1文件100页并比较读入的内容..." << endl;
 

    for (i = 1; i < num; i++)
    {
        CALL(bufMgr->readPage(file1, i, page));
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page, &cmp, strlen((char*)&cmp)) == 0);
        //cout << (char*)page << endl;
    }
    
    cout << "恭喜：测试10顺利通过" <<endl<<endl;

    cout << "未解除缓存中test.1文件100帧摁钉的情况下调用flushFile，应当出错..." << endl;
    FAIL(status = bufMgr->flushFile(file1));
    error.print(status);

    cout << "恭喜：测试11顺利通过"<<endl<<endl;

    for (i = 1; i < num; i++) 
        CALL(bufMgr->unPinPage(file1, i, true));

    CALL(bufMgr->flushFile(file1));


    CALL(db->closeFile(file1));
    CALL(db->closeFile(file2));
    CALL(db->closeFile(file3));
    CALL(db->closeFile(file4));

    CALL(db->destroyFile("test.1"));
    CALL(db->destroyFile("test.2"));
    CALL(db->destroyFile("test.3"));
    CALL(db->destroyFile("test.4"));
    
    delete bufMgr;

    cout << endl << "恭喜：所有测试顺利通过." << endl;
    return (1);
}

