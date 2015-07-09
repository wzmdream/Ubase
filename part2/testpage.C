#include <stdio.h>
#include <iostream>
using namespace std;
#include "page.h"
#define CALL(c)    { Status s; \
                     if ((s = c) != OK) { \
                       cerr << "At line " << __LINE__ << ":" << endl << "  "; \
                       error.print(s); \
                       cerr << "TEST DID NOT PASS" <<endl; \
                       exit(1); \
                     } \
                   }

#define FAIL(c)  { Status s; \
                   if ((s = c) == OK) { \
                     cerr << "At line " << __LINE__ << ":" << endl << "  "; \
                     cerr << "This call should fail: " #c << endl; \
                     cerr << "TEST DID NOT PASS" <<endl; \
                     exit(1); \
                     } \
                     }

int main()
{
    Error   error;
    Page*   page1;

    char    rec1[33];
    char    rec2[58];
    char    rec3[71];
    char    rec4[100];
    char    rec5[4000];
    char    rec3_new[71];

    Record record1, record2, record3, record, record4, record5;
    Record record3_new;
    Status	status;
    RID		rid1, rid2, rid3, rid4, rid5, tmpRid1, tmpRid2;
    int 	i,j;	

    record1.data = &rec1;
    record1.length = sizeof(rec1);
    record2.data = &rec2;
    record2.length = sizeof(rec2);
    record3.data = &rec3;
    record3.length = sizeof(rec3);
    record4.data = &rec4;
    record4.length = sizeof(rec4);
    record5.data = &rec5;
    record5.length = sizeof(rec5);
    record3_new.data = &rec3_new;
    record3_new.length = sizeof(rec3_new);

    for (i=0;i<33;i++) rec1[i]='a';
    for (i=0;i<58;i++) rec2[i]='b';
    for (i=0;i<71;i++) rec3[i]='c';
    for (i=0;i<100;i++) rec4[i]='d';
    for (i=0;i<4000;i++) rec5[i]='e';
    for (i=0;i<71;i++) rec3_new[i]='X';

    // create data page
    page1 = new Page;
    page1->init(1);
    ASSERT(page1->getFreePtr() == 0);
    ASSERT(page1->getFreeSpace() == 4080);
    ASSERT(page1->getSlotCnt() == 0)
    //page1->dumpPage();


    // insert 3 records
    printf ("inserting 3 records of lengths 33, 58, 71\n");
    CALL(page1->insertRecord(record1, rid1));
    ASSERT(page1->getFreePtr() == 33);
    ASSERT(page1->getFreeSpace() == 4043);
    ASSERT(page1->getSlotCnt() == -1);
    ASSERT(rid1.pageNo==1);
    ASSERT(rid1.slotNo==0);
    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    CALL(page1->getRecord(rid1,record));
    ASSERT(memcmp(record.data,rec1,record.length)==0);
    //page1->dumpPage();

    CALL(page1->insertRecord(record2, rid2));
    ASSERT(page1->getFreePtr() == 91);
    ASSERT(page1->getFreeSpace() == 3981);
    ASSERT(page1->getSlotCnt() == -2);
    ASSERT(rid2.pageNo==1);
    ASSERT(rid2.slotNo==1);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==33);
    ASSERT(page1->getLength(-1)==58);

    CALL(page1->getRecord(rid2,record));
    ASSERT(memcmp(record.data,rec2,record.length)==0);
    //page1->dumpPage();

    CALL(page1->insertRecord(record3, rid3));
    ASSERT(page1->getFreePtr() == 162);
    ASSERT(page1->getFreeSpace() == 3906);
    ASSERT(page1->getSlotCnt() == -3);
    ASSERT(rid3.pageNo==1);
    ASSERT(rid3.slotNo==2);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==33);
    ASSERT(page1->getLength(-1)==58);
    ASSERT(page1->getOffset(-2)==91);
    ASSERT(page1->getLength(-2)==71);

    CALL(page1->getRecord(rid3,record));
    ASSERT(memcmp(record.data,rec3,record.length)==0);

    //page1->dumpPage();

    cout << "Test 1 Passed!" << endl;

    printf("\nNow, scan the records on the page\n");

    // try scanning records on the page
    CALL(page1->firstRecord(tmpRid1));
    ASSERT(tmpRid1.pageNo==1);
    ASSERT(tmpRid1.slotNo==0);

    for ( i = 0; i < 3; i++)
    {
        // get a record
        CALL(page1->getRecord(tmpRid1, record));
        cout << "Record " << i << ":";
        for (j=0;j<record.length;j++) 
            printf("%c", ((char *)record.data)[j]); printf("\n");

        //get rid of next record
        status = page1->nextRecord(tmpRid1, tmpRid2);
        if ( i == 2)
        {
            ASSERT( status == ENDOFPAGE );
            break;
        }
        ASSERT( status == OK);
        tmpRid1 = tmpRid2;
    }
    cout << "Test 2 Passed!" << endl;

    // delete the middle record
    printf("\ndelete middle record with rid2=%d.%d\n", rid2.pageNo, rid2.slotNo);
    CALL(page1->deleteRecord(rid2));

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==-1);
    ASSERT(page1->getLength(-1)==-1);
    ASSERT(page1->getOffset(-2)==33);
    ASSERT(page1->getLength(-2)==71);

    ASSERT(page1->getFreePtr() == 104);
    ASSERT(page1->getFreeSpace() == 3964);
    ASSERT(page1->getSlotCnt() == -3);
    CALL(page1->getRecord(rid3,record));
    ASSERT(memcmp(record.data,rec3,record.length)==0);

    page1->dumpPage();

    cout << "Test 3 Passed!" << endl;

    // insert record 4 with length 100
    printf("\ninsert the record 4 with length 100\n");
    CALL(page1->insertRecord(record4, rid4));
    ASSERT(page1->getFreePtr() == 204);
    ASSERT(page1->getFreeSpace() == 3864);
    ASSERT(page1->getSlotCnt() == -3);
    ASSERT(rid4.pageNo==1);
    ASSERT(rid4.slotNo==1);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==104);
    ASSERT(page1->getLength(-1)==100);
    ASSERT(page1->getOffset(-2)==33);
    ASSERT(page1->getLength(-2)==71);

    CALL(page1->getRecord(rid4,record));
    ASSERT(memcmp(record.data,rec4,record.length)==0);
    //printf("rid4 = %d.%d\n",rid4.pageNo, rid4.slotNo);
    page1->dumpPage();

    cout << "Test 4 Passed!" << endl;


    // try to insert a very large record
    printf("\ntry to insert a record that should not fit in the space left\n");
    FAIL(status = page1->insertRecord(record5, rid5));
    ASSERT(page1->getFreePtr() == 204);
    ASSERT(page1->getFreeSpace() == 3864);
    ASSERT(page1->getSlotCnt() == -3);
    ASSERT(rid5.pageNo==-1);
    ASSERT(rid5.slotNo==-1);
    printf("status=%d, rid returned = %d.%d\n",status, rid5.pageNo, rid5.slotNo);
    ASSERT(status == NOSPACE) 
    cout << "correctly identified that there isn't enough space" << endl;
    //page1->dumpPage();

    // get the amount of space left on the page
    int freeSpace = page1->getFreeSpace();
    printf("%d bytes left on the page\n",freeSpace);

    cout << "Test 5 Passed!" << endl;

    // shorten the record to fit and try again
    record5.length = freeSpace - sizeof(slot_t);
    printf("\ntrying again with a large record that should fit exactly \n");
    CALL(status = page1->insertRecord(record5, rid5));
    ASSERT(page1->getFreePtr() == 4064);
    ASSERT(page1->getFreeSpace() == 0);
    ASSERT(page1->getSlotCnt() == -4);
    ASSERT(rid5.pageNo==1);
    ASSERT(rid5.slotNo==3);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==104);
    ASSERT(page1->getLength(-1)==100);
    ASSERT(page1->getOffset(-2)==33);
    ASSERT(page1->getLength(-2)==71);
    ASSERT(page1->getOffset(-3)==204);
    ASSERT(page1->getLength(-3)==record5.length);

    CALL(page1->getRecord(rid5,record));
    ASSERT(memcmp(record.data,rec5,record.length)==0);
    printf("status=%d, rid5 returned = %d.%d\n",
	status, rid5.pageNo, rid5.slotNo);
    //page1->dumpPage();

    cout << "Accturelly, we have 4 records now, since we have deleted one" << endl;
    cout << "Test 6 Passed!" << endl;

    cout << endl << "Delete all the records. next the end one" << endl;
    printf("delete rid3=%d.%d\n", rid3.pageNo, rid3.slotNo);
    CALL(page1->deleteRecord(rid3));
    ASSERT(page1->getFreePtr() == 3993);
    ASSERT(page1->getFreeSpace() == 71);
    ASSERT(page1->getSlotCnt() == -4);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==33);
    ASSERT(page1->getLength(-1)==100);
    ASSERT(page1->getOffset(-2)==-1);
    ASSERT(page1->getLength(-2)==-1);
    ASSERT(page1->getOffset(-3)==133);
    ASSERT(page1->getLength(-3)==record5.length);

    //page1->dumpPage();

    // delete rec4
    printf("delete rid4=%d.%d\n", rid4.pageNo, rid4.slotNo);
    CALL(page1->deleteRecord(rid4));
    ASSERT(page1->getFreePtr() == 3893);
    ASSERT(page1->getFreeSpace() == 171);
    ASSERT(page1->getSlotCnt() == -4);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==-1);
    ASSERT(page1->getLength(-1)==-1);
    ASSERT(page1->getOffset(-2)==-1);
    ASSERT(page1->getLength(-2)==-1);
    ASSERT(page1->getOffset(-3)==33);
    ASSERT(page1->getLength(-3)==record5.length);

    //page1->dumpPage();

    printf("delete rid5=%d.%d\n", rid5.pageNo, rid5.slotNo);
    CALL(page1->deleteRecord(rid5));
    ASSERT(page1->getFreePtr() == 33);
    ASSERT(page1->getFreeSpace() == 4043);
    ASSERT(page1->getSlotCnt() == -1)

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==-1);
    ASSERT(page1->getLength(-1)==-1);
    ASSERT(page1->getOffset(-2)==-1);
    ASSERT(page1->getLength(-2)==-1);
    ASSERT(page1->getOffset(-3)==-1);
    ASSERT(page1->getLength(-3)==-1);

    //page1->dumpPage();

    // then the first one
    printf("delete rid1=%d.%d\n", rid1.pageNo, rid1.slotNo);
    CALL(page1->deleteRecord(rid1));
    ASSERT(page1->getFreePtr() == 0);
    ASSERT(page1->getFreeSpace() == 4080);
    ASSERT(page1->getSlotCnt() == 0);

    ASSERT(page1->getOffset(0)==-1);
    ASSERT(page1->getLength(0)==-1);
    ASSERT(page1->getOffset(-1)==-1);
    ASSERT(page1->getLength(-1)==-1);
    ASSERT(page1->getOffset(-2)==-1);
    ASSERT(page1->getLength(-2)==-1);
    ASSERT(page1->getOffset(-3)==-1);
    ASSERT(page1->getLength(-3)==-1);

    //page1->dumpPage();

    
    FAIL(status = page1->deleteRecord(rid1));
    ASSERT(status==INVALIDSLOTNO);
    FAIL(status = page1->firstRecord(rid1));
    ASSERT(status==NORECORDS);
    FAIL(status = page1->getRecord(rid1,record1));
    ASSERT(status==INVALIDSLOTNO);

    cout << "Test 7 Passed!" << endl;
    
    cout << endl << "Test function firstRecord" << endl;
    CALL(page1->insertRecord(record1, rid1));
    CALL(page1->insertRecord(record2, rid2));
    CALL(page1->insertRecord(record3, rid3));
    CALL(page1->insertRecord(record4, rid4));
    CALL(page1->firstRecord(tmpRid1));
    ASSERT(tmpRid1.pageNo==1);
    ASSERT(tmpRid1.slotNo==0);
    CALL(page1->deleteRecord(rid1));
    CALL(page1->firstRecord(tmpRid1));
    ASSERT(tmpRid1.pageNo==1);
    ASSERT(tmpRid1.slotNo==1);
    CALL(page1->deleteRecord(rid2));
    CALL(page1->firstRecord(tmpRid1));
    ASSERT(tmpRid1.pageNo==1);
    ASSERT(tmpRid1.slotNo==2);
    CALL(page1->deleteRecord(rid3));
    CALL(page1->firstRecord(tmpRid1));
    ASSERT(tmpRid1.pageNo==1);
    ASSERT(tmpRid1.slotNo==3);
    CALL(page1->insertRecord(record1, rid1));
    CALL(page1->firstRecord(tmpRid1));
    ASSERT(tmpRid1.pageNo==1);
    ASSERT(tmpRid1.slotNo==0);
    page1->dumpPage();
    CALL(page1->deleteRecord(rid4));
    CALL(page1->deleteRecord(rid1));
    ASSERT(page1->firstRecord(tmpRid1)==NORECORDS);
    
    cout << "Test 8 Passed!" << endl;
    
    cout << endl << "Insert two records again" << endl;
    CALL(page1->insertRecord(record1, rid1));
    ASSERT(page1->getFreePtr() == 33);
    ASSERT(page1->getFreeSpace() == 4043);
    ASSERT(page1->getSlotCnt() == -1);
    ASSERT(rid1.pageNo==1);
    ASSERT(rid1.slotNo==0);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);


    CALL(page1->insertRecord(record3, rid3));
    ASSERT(page1->getFreePtr() == 104);
    ASSERT(page1->getFreeSpace() == 3968);
    ASSERT(page1->getSlotCnt() == -2);
    ASSERT(rid3.pageNo==1);
    ASSERT(rid3.slotNo==1);

    ASSERT(page1->getOffset(0)==0);
    ASSERT(page1->getLength(0)==33);
    ASSERT(page1->getOffset(-1)==33);
    ASSERT(page1->getLength(-1)==71);

    page1->dumpPage();
    cout << "Test 9 Passed!" << endl;

    cout << endl << "Update the second record" << endl;
    CALL(page1->updateRecord(record3_new,rid3));
    CALL(page1->getRecord(rid3,record));
    ASSERT(memcmp(record.data,rec3_new,record.length)==0);
    page1->dumpPage();

    cout << "Test 10 Passed!" << endl;

    delete page1;

    cout << endl << "Pass All Tests!" << endl;
    return (1);
}
