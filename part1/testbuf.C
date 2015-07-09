#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include "page.h"
#include "buf.h"
#include "db.h"

//Ӧ������OK�ĺ������øú�
#define CALL(c)\
{    Status s; \
     if ((s = c) != OK) { \
        cerr << "�������ڲ��Գ���ĵ�" << __LINE__ << "��" << endl; \
        cerr << "��ʱ���õĺ���Ϊ��" << #c << endl;\
        error.print(s); \
        cerr << "���ź���û��ͨ�����ԣ�" <<endl; \
        exit(1); \
      } \
}
//Ӧ�����ش���ĺ������øú�
#define FAIL(c)\
{\
	Status s;\
	if ((s = c) == OK)\
	{\
		cerr << "�ڲ��Գ���ĵ�" << __LINE__ << "��" << endl;\
		cerr << "���ú���" #c << "Ӧ����������ȴ������OK" << endl;\
		cerr << "���ź�������ͨ�����ԣ�" <<endl;\
		exit(1);\
	}\
}
BufMgr* bufMgr; //�����������������

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
    const int   num = 100;  //����ش�СΪ100֡
    int         j[num];    
    bufMgr = new BufMgr(num); // �����������������
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
    //����ļ��Ƿ��Ѿ����ڣ�����Ѿ����ڣ�ɾ��֮
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
    //����4����ϵ�ļ�
    CALL(db->createFile("test.1"));
    ASSERT(db->createFile("test.1") == FILEEXISTS);
    CALL(db->createFile("test.2"));
    CALL(db->createFile("test.3"));
    CALL(db->createFile("test.4"));
    //exit(0);
    //��4����ϵ�ļ�
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

    cout << "Ϊ�ļ�test.1����100������ҳ..." << endl;
    cout << "�޸Ļ����е�100֡..." << endl;
    cout << "���������100֡������..." << endl;
    for (i = 0; i < num; i++)
    {      
        CALL(bufMgr->allocPage(file1, j[i], page));
        //cout<<(char*)page<<"      "<<j[i]<<endl;
        sprintf((char*)page, "test.1 Page %d %7.1f", j[i], (float)j[i]);
        //cout<<(char*)page<<"      "<<j[i]<<endl;
        CALL(bufMgr->unPinPage(file1, j[i], true));
    }

    //bufMgr->BufDump();
    cout <<"��ϲ������1˳��ͨ��"<<endl<<endl;

    cout << "˳����ļ�test.1��100������ҳ..." << endl;
    cout << "�Ƚ϶��������..." << endl;
    cout << "���������100֡������..." << endl;
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
    cout<< "��ϲ������2˳��ͨ��"<<endl<<endl;

    cout << "Ϊ�ļ�test.2��test.3����33������ҳ..." << endl;
    cout << "�漴��test.1�ļ�33ҳ���Ƚ϶��������..." << endl;
    cout << "˳���test.2��test.3�ļ�33ҳ���Ƚ϶��������..." << endl;
    cout << "���������test.1�ļ�33֡������..." << endl;
    cout << "���������test.2��test.3�ļ�33֡������..." << endl;
    for (i = 0; i < num/3; i++) 
    {
        CALL(bufMgr->allocPage(file2, pageno2, page2));
        sprintf((char*)page2, "test.2 Page %d %7.1f", pageno2, (float)pageno2);
       
        CALL(bufMgr->allocPage(file3, pageno3, page3));
        sprintf((char*)page3, "test.3 Page %d %7.1f", pageno3, (float)pageno3);
         //cout<<"��ǰҳ��333:"<<(char*)page3<<" "<<endl;
        pageno = j[random() % num];
        CALL(bufMgr->readPage(file1, pageno, page));
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", pageno, (float)pageno);
        //cout<<"��ǰҳ��111:"<<(char*)page<<"     "<<i<<endl;
       
        //cout<<"cmp:"<<(char*)cmp<<"     "<<i<<endl;
        ASSERT(memcmp(page, &cmp, strlen((char*)&cmp)) == 0);
        
        CALL(bufMgr->readPage(file2, pageno2, page2));
        sprintf((char*)&cmp, "test.2 Page %d %7.1f", pageno2, (float)pageno2);
         //cout<<"��ǰҳ��2:"<<(char*)page2<<"     "<<i<<endl;
      //bufMgr->BufDump();
        ASSERT(memcmp(page2, &cmp, strlen((char*)&cmp)) == 0);
        //cout<<"��ǰcmp��2:"<<(char*)&cmp<<"     "<<i<<endl;
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

    cout << "��ϲ������3˳��ͨ��" << endl<<endl;



    //bufMgr->printSelf();

    cout << "˳���test.1�ļ�33ҳ���Ƚ϶��������..." << endl;
    cout << "���������test.1�ļ�33֡������..." << endl;

    for (i = 1; i < num/3; i++)
    {
        CALL(bufMgr->readPage(file1, i, page2));
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page2, &cmp, strlen((char*)&cmp)) == 0);
        CALL(bufMgr->unPinPage(file1, i, false));
    }
    //bufMgr->BufDump();
    //exit(0);

    cout << "��ϲ������4˳��ͨ��" <<endl<<endl;

    cout << "˳���test.2�ļ�33ҳ���Ƚ϶��������..." << endl;
    cout << "���������test.2�ļ�33֡������..." << endl;

    for (i = 1; i < num/3; i++)
    {
        CALL(bufMgr->readPage(file2, i, page2));
        sprintf((char*)&cmp, "test.2 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page2, &cmp, strlen((char*)&cmp)) == 0);
        //cout << (char*)page2 << endl;
        CALL(bufMgr->unPinPage(file2, i, false));
    }
    cout << "��ϲ������5˳��ͨ��" <<endl<<endl;


    cout << "˳���test.3�ļ�33ҳ���Ƚ϶��������..." << endl;
    cout << "���������test.3�ļ�33֡������..." << endl;

    for (i = 1; i < num/3; i++)
    {
        CALL(bufMgr->readPage(file3, i, page3));
        sprintf((char*)&cmp, "test.3 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page3, &cmp, strlen((char*)&cmp)) == 0);
        //cout << (char*)page3 << endl;
        CALL(bufMgr->unPinPage(file3, i, false));
    }

    cout << "��ϲ������6˳��ͨ��" <<endl<<endl;

    cout << "��test.4�ļ��ĵ�һ����ҳ"<< endl;
    cout << "Ӧ����ʾ������Ϣ����Ϊtest.4��δ��������ҳ"<<endl;

    Status status;
    FAIL(status = bufMgr->readPage(file4, 1, page));
    error.print(status);
    //bufMgr->BufDump();
    cout << "��ϲ������7˳��ͨ��" <<endl<<endl;
 
    cout << "Ϊtest.4�ļ������һ����ҳ"<< endl;
    cout << "���������test.4�ļ���һ����ҳ������..." << endl;
    cout << "�ڶ���Ӧ����ʾ������Ϣ����Ϊ������test.4�ļ���һ����ҳ���������Ѿ�Ϊ0"<<endl;
    CALL(bufMgr->allocPage(file4, i, page));
    CALL(bufMgr->unPinPage(file4, i, true));
    FAIL(status = bufMgr->unPinPage(file4, i, false));
    error.print(status);
    //bufMgr->BufDump();
    cout << "��ϲ������8˳��ͨ��" <<endl<<endl;
    
    cout << "Ϊtest.4�ļ��ٷ���100������ҳ"<< endl;
    for (i = 0; i < num; i++)
    {
        CALL(bufMgr->allocPage(file4, j[i], page));
        sprintf((char*)page, "test.4 Page %d %7.1f", j[i], (float)j[i]);
    }
    cout << "�ڻ�����100��֡������ס������£�Ϊtest.4�ļ��ٷ���1������ҳ��Ӧ������"<< endl;
    int tmp;
    FAIL(status = bufMgr->allocPage(file4, tmp, page));
    error.print(status);

    cout << "��ϲ������9˳��ͨ��" <<endl<<endl;

    //bufMgr->BufDump();

    //bufMgr->printSelf();

    for (i = 0; i < num; i++)
        CALL(bufMgr->unPinPage(file4, i+2, true));
    
    cout << "˳���test.1�ļ�100ҳ���Ƚ϶��������..." << endl;
 

    for (i = 1; i < num; i++)
    {
        CALL(bufMgr->readPage(file1, i, page));
        sprintf((char*)&cmp, "test.1 Page %d %7.1f", i, (float)i);
        ASSERT(memcmp(page, &cmp, strlen((char*)&cmp)) == 0);
        //cout << (char*)page << endl;
    }
    
    cout << "��ϲ������10˳��ͨ��" <<endl<<endl;

    cout << "δ���������test.1�ļ�100֡����������µ���flushFile��Ӧ������..." << endl;
    FAIL(status = bufMgr->flushFile(file1));
    error.print(status);

    cout << "��ϲ������11˳��ͨ��"<<endl<<endl;

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

    cout << endl << "��ϲ�����в���˳��ͨ��." << endl;
    return (1);
}
