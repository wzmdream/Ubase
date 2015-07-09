#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include "heapfile.h"

extern Status createHeapFile(string FileName);
extern Status destroyHeapFile(string FileName);

// globals
DB db;
BufMgr* bufMgr;

int main(int argc, char **argv)
{
    cout << "Testing the relation interface" << endl << endl;

    struct stat statusBuf;

    Error               error;
    HeapFile*           file1;
    HeapFileScan        *scan1, *scan2;
    InsertFileScan      *iScan;
    Status              status;
    int                 i,j;
    int                 num = 10000;
    RID                 newRid,rec2Rid;
    RID*  		ridArray;

    ridArray = new RID[num];

    typedef struct {
        int i;
        float f;
        char s[64];
    } RECORD;

    RECORD	rec1,rec2;
    int		rec1Len;
    Record	dbrec1,dbrec2;

    bufMgr = new BufMgr(100);

    cout << "Destroy any old copies of relation table1,test destroyHeapFile function" << endl;
    
    lstat("table1", &statusBuf);
    if (errno == ENOENT)
    {
        errno = 0;
        status = destroyHeapFile("table1");
        ASSERT(status != OK);
    }
    else
    {
        status = destroyHeapFile("table1");
        ASSERT(status == OK);
    }
    cout << "Test 1 passed!" << endl << endl;

    cout << "Create relation table1,test createHeapFile function" << endl;
    status = createHeapFile("table1");
    ASSERT(status == OK);
    lstat("table1", &statusBuf);
    ASSERT(statusBuf.st_size==12288);    
    cout << "Test 2 passed!" << endl << endl;

    // initialize all of rec1.s to keep purify happy
    memset(rec1.s, ' ', sizeof(rec1.s));

    cout << "Insert " << num << " records into table1" << endl;

    cout << "Start an insert scan which will have the side effect of opening table1" << endl;
    iScan = new InsertFileScan("table1", status);
    for(i = 0; i < num; i++) {
        sprintf(rec1.s, "This is record %05d", i);
        rec1.i = i;
        rec1.f = i;

        dbrec1.data = &rec1;
        dbrec1.length = sizeof(RECORD);
        status = iScan->insertRecord(dbrec1, newRid);

	// stash away rid and key of the record
	ridArray[i] = newRid;
	//printf("next rid (%d.%d)\n",ridArray[i].pageNo, ridArray[i].slotNo);

        if (status != OK) 
        {
            cout << "got error status return from insert record" << endl;
            cout << "inserted " << i << " records into file table1 before error " 
                 << endl;
            error.print(status);
            exit(1);
        }
    }
    delete iScan; // delete the scan, closing the file at the same time
    file1 = NULL;

    lstat("table1", &statusBuf);
    ASSERT(statusBuf.st_size==782336);
    cout << "inserted " << num << " records into file table1 successfully" << endl;
    cout << "Test 3 passed!" << endl << endl;

    // scan the file sequentially checking that each record was stored properly
    cout << "scan file table1" << endl;
    scan1 = new HeapFileScan("table1", status);
    ASSERT(status ==OK);

    scan1->startScan(0, 0, STRING, NULL, EQ);
    i = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        // reconstruct record i
    	sprintf(rec1.s, "This is record %05d", i);
    	rec1.i = i;
    	rec1.f = i;
    	status = scan1->getRecord(dbrec2);
    	ASSERT(status == OK);
        ASSERT(memcmp(&rec1, dbrec2.data, sizeof(RECORD)) == 0);
    	i++;
    }
    ASSERT(status == FILEEOF);
    cout << "scan table1 saw " << i << " records " << endl;
    ASSERT(i == num);
    // delete scan object
    scan1->endScan();
    delete scan1;
    scan1 = NULL;
    cout << "Test 4 passed!" << endl << endl;

    // pull every 7th record from the file directly w/o opening a scan
    // by using the file->getRecord() method
    cout << "pull every 7th record from file table1 using file->getRecord() " << endl;
    file1 = new HeapFile("table1", status); // open the file
    ASSERT(status == OK);
    // get every 7th record
    for (i=0;i<num;i=i+7)
    {
        // reconstruct record for comparison purposes
        sprintf(rec1.s, "This is record %05d", i);
        rec1.i = i;
        rec1.f = i;
        // now read the record
        // printf("getting record (%d.%d)\n",ridArray[i].pageNo, ridArray[i].slotNo);
        status = file1->getRecord(ridArray[i], dbrec2);
        if (status != OK)
        {
             error.print(status);
             exit(1);
        }
        // compare with what we should get back
        if (memcmp(&rec1, dbrec2.data, sizeof(RECORD)) != 0)
        {
            cout << "error reading record " << i << " back" << endl;
            exit(1);
        }
    }
    cout << "getRecord() tests passed successfully" << endl;
    delete file1; // close the file
    delete [] ridArray;
    cout << "Test 5 passed!" << endl << endl;
    // next scan the file deleting all the odd records
    cout << "scan table1 deleting all records whose i field is odd" << endl;

    scan1 = new HeapFileScan("table1", status);
    ASSERT(status == OK);
    scan1->startScan(0, 0, STRING, NULL, EQ);
    i = 0;
    int deleted = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        ASSERT(status == OK);
        if ((i % 2) != 0)
        {
            //printf("deleting record %d with rid(%d.%d)\n",i,rec2Rid.pageNo, rec2Rid.slotNo);
            status = scan1->deleteRecord();
            if (status != OK)  //&& ( status != NORECORDS))
            {
                cout << "error status return from deleteRecord" << endl;
                error.print(status);
                exit(1);
            }
            deleted++;
        }
        i++;
    }
    ASSERT(status == FILEEOF);
    cout << "deleted " << deleted << " records" << endl;
    if (deleted != num / 2)
    { 
        cout << "!!! should have deleted " << num / 2 << " records!" << endl;
        exit(1);
    }
    scan1->endScan();
    delete scan1;
    scan1 = NULL;
    cout << "Test 6 passed!" << endl << endl;

    cout << "scan file, counting number of remaining records" << endl;
    scan1 = new HeapFileScan("table1", status);
    ASSERT(status == OK);
    i = 0;
    scan1->startScan(0, 0, STRING, NULL, EQ);
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        if ( i > 0 ) // don't delete the first one
        {
            status = scan1->deleteRecord();
            if ((status != OK) && ( status != NORECORDS))
            {
                cout << "error status return from deleteRecord" << endl;
                error.print(status);
                exit(1);
            }
        }
        i++;
    }
    // subtract first record
    i--;
    ASSERT(status == FILEEOF);
    scan1->endScan(); 
    delete scan1;
    scan1 = NULL;

    scan1 = new HeapFileScan("table1", status);
    ASSERT(status == OK);
    scan1->startScan(0, 0, STRING, NULL, EQ);

    // delete the rest (should only be one)
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        status = scan1->deleteRecord();
        if ((status != OK) && ( status != NORECORDS))
        {
            cout << "error status return from deleteRecord" << endl;
            error.print(status);
            exit(1);
        }
        i++;
    }
    
    scan1->endScan();
    delete scan1;
    cout << "scan saw " << i << " records " << endl;
    if (i != (num+1) / 2)
    {
        cout << "!!! scan should have returned " << (num+1) / 2 << " records!" << endl;
        exit(1);
    }
    status = destroyHeapFile("table1");
    if (status != OK) 
    {
	cerr << "got error status return from  destroyHeapFile" << endl;
    	error.print(status);
        exit(1);
    }
    cout << "Test 7 passed!" << endl << endl;

    destroyHeapFile("table2");
    status = createHeapFile("table2");
    if (status != OK) 
    {
	cerr << "got error status return from  createHeapFile" << endl;
    	error.print(status);
        exit(1);
    }
    iScan = new InsertFileScan("table2", status);
    if (status != OK) 
    {
	cout << "got error status return from new insertFileScan call" << endl;
    	error.print(status);
        exit(1);
    }

    cout << "insert " << num << " variable-size records into table2" << endl;
    int smallest, largest;
    for(i = 0; i < num; i++) {
        rec1Len = 2 + rand() % (sizeof(rec1.s)-2);    // includes NULL!!
        //cout << "record length is " << rec1Len << endl;
        memset((void *)rec1.s, 32 + rec1Len, rec1Len - 1);
        rec1.s[rec1Len-1] = 0;
        rec1Len += sizeof(rec1.i) + sizeof(rec1.f);
        rec1.i = i;
        rec1.f = rec1Len;
        dbrec1.data = &rec1;
        dbrec1.length = rec1Len;

        status = iScan->insertRecord(dbrec1, newRid);
        if (status != OK) 
        {
            cout << "got error status return from insertrecord" << endl;
            error.print(status);
            exit(1);
        }
        if (i == 0 || rec1Len < smallest) smallest = rec1Len;
        if (i == 0 || rec1Len > largest) largest = rec1Len;
    }
    cout << "inserted " << num << " variable sized records successfully into table2" << endl;
    cout << "smallest record was " << smallest << " bytes, largest was " << largest << " bytes" << endl;
    
    delete iScan;
    file1 = NULL;
    cout << "Test 8 passed!" << endl << endl;

    cout << "scan table2" << endl;
    j = num/2;
    scan1 = new HeapFileScan("table2", status);
    ASSERT(status == OK);
    scan1->startScan(0, sizeof(int), INTEGER, (char*)&j, LT);
    i = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        status = scan1->getRecord(dbrec2);
        if (status != OK) break;
        memcpy(&rec2, dbrec2.data, dbrec2.length);
        if (rec2.i >= j || rec2.f != dbrec2.length || rec2.s[0] != 32+dbrec2.length-8)
            cout << "error reading record " << i << " back" << endl;
        i++;
    }
    ASSERT(status == FILEEOF);
    cout << "scan of table2 saw " << i << " records " << endl;
    if (i != num / 2)
    {
        cout << "!!! scan should have returned " << num / 2 << " records!" << endl;
        exit(1);
    }
    scan1->endScan();
    delete scan1;
    scan1 = NULL;
    cout << "Test 9 passed!" << endl << endl;

    cout << "Next attempt two concurrent scans on table2" << endl;
    int Ioffset = (char*)&rec1.i - (char*)&rec1;
    int Ivalue = num/2;
    int Foffset = (char*)&rec1.f - (char*)&rec1;
    float Fvalue = 0;

    scan1 = new HeapFileScan("table2", status);
    ASSERT(status == OK);
    status = scan1->startScan(Ioffset, sizeof(int), INTEGER,(char*)&Ivalue, LT);
    ASSERT(status == OK);

    scan2 = new HeapFileScan("table2", status);
    ASSERT(status == OK);
    status = scan2->startScan(Foffset, sizeof(float), FLOAT,(char*)&Fvalue, GTE);
    ASSERT(status == OK);
    int count = 0;
    for(i = 0; i < num; i++)
    {
        status = scan1->scanNext(rec2Rid);
        if (status == OK)
        {
            count++;
        }
        else if (status != FILEEOF)
        {
            error.print(status);
            exit(1);
        }
        status = scan2->scanNext(rec2Rid);
        if (status == OK)
        {
            count++;
        }
        else if (status != FILEEOF)
        {
            error.print(status);
            exit(1);
        }
    }
    cout << "scan table2 saw " << count << " records " << endl;
    if (count != num/2 + num)
    {
        cout << "!!! scan should have returned " << num/2 + num << " records!" << endl;
        exit(1);
    }
    if (scan1->scanNext(rec2Rid) != FILEEOF)
    {
        cout << "!!! scan past end of file did not return FILEEOF!" << endl;
        exit(1);
    }
    if (scan2->scanNext(rec2Rid) != FILEEOF)
    {
        cout << "!!! scan past end of file did not return FILEEOF!" << endl;
        exit(1);
    }
    delete scan1;
    delete scan2;
    scan1 = scan2 = NULL;

    cout << "Destroy table2" << endl;
    if ((status = destroyHeapFile("table2")) != OK) {
        cout << "got error status return from destroy file" << endl;
        error.print(status);
        exit(1);
    }
    cout << "Test 10 passed!" << endl << endl;

    destroyHeapFile("table3");
    status = createHeapFile("table3");

    if (status != OK) 
    {
	cerr << "got error status return from  createHeapFile" << endl;
    	error.print(status);
        exit(1);
    }

    iScan = new InsertFileScan("table3", status);
    if (status != OK) 
    {
	cerr << "got error status return from new insertFileScan" << endl;
    	error.print(status);
        exit(1);
    }
    cout << "inserting " << num << " records into table3" << endl;
    for(i = 0; i < num; i++) {
        sprintf(rec1.s, "This is record %05d", i);
        rec1.i = i;
        rec1.f = i;

        dbrec1.data = &rec1;
        dbrec1.length = sizeof(RECORD);
        status = iScan->insertRecord(dbrec1, newRid);
        if (status != OK) 
        {
            cout << "got error status return from insertrecord" << endl;
            error.print(status);
            exit(1);
        }
    }
    delete iScan;
    file1 = NULL;
    cout << "Test 11 passed!" << endl << endl;
    //bufMgr->clearBufStats();
    

    int numDeletes = 0;
    cout << endl;
    cout << "now scan table3, deleting records with keys between 1000 and 2000" << endl;

    scan1 = new HeapFileScan("table3", status);
    ASSERT(status == OK);
    status = scan1->startScan(0, 0, STRING, NULL, EQ);
    ASSERT(status == OK);
    i = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        // reconstruct record i
        sprintf(rec1.s, "This is record %05d", i);
        rec1.i = i;
        rec1.f = i;
        status = scan1->getRecord(dbrec2);
        if (status != OK) break;
        if (memcmp(&rec1, dbrec2.data, sizeof(RECORD)) != 0)
        { 
            cout << "error reading record " << i << " back" << endl;
            exit(1);
        }
        if ((i > 1000) && ( i <= 2000))
        {
            status = scan1->deleteRecord();
            if ((status != OK) && ( status != NORECORDS))
            {
                cout << "error status return from deleteRecord" << endl;
                error.print(status);
                exit(1);
            }
            else
            {
                numDeletes++;
            }
        }
    	i++;
    }
    ASSERT(status == FILEEOF);
    cout << "scan file1 saw " << i << " records " << endl;
    if (i != num)
    {
        cout << "Error. scan should have returned " << (int) num << " records!!" << endl;
        exit(1);
    }
    cout << "number of records deleted by scan " << numDeletes << endl;
    if (numDeletes != 1000)
    {
        cout << "Error. Should have deleted 1000 records!!!" << endl;
        exit(1);
    }
    delete scan1;

    // rescan.  should see 1000 fewer records
    scan1 = new HeapFileScan("table3", status);
    ASSERT(status == OK);
    scan1->startScan(0, 0, STRING, NULL, EQ);
    ASSERT(status == OK);
    i = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
	i++;
    }
    cout << "should have seen 1000 fewer records after deletions" << endl;
    cout << "saw " << i << " records" << endl;
    delete scan1;
    cout << "Test 12 passed!" << endl << endl;

    // perform filtered scan #1
    scan1 = new HeapFileScan("table3", status);
    ASSERT(status == OK);

    int filterVal1 = num * 3 / 4;
    cout << "Filtered scan matching i field GTE than " << filterVal1 << endl;
    status = scan1->startScan(0, sizeof(int), INTEGER, (char *) &filterVal1, GTE);
    ASSERT(status == OK);
    i = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        // reconstruct record i
        status = scan1->getRecord(dbrec2);
        if (status != OK) break;
        // verify that the scan predicate was actually met
        RECORD *currRec = (RECORD *) dbrec2.data;
        if (! (currRec->i >= filterVal1))
        {
            cerr << "!!! filtered scan returned record that doesn't satisfy predicate " << "i val is " << currRec->i << endl;
            exit(1);
        }            
        i++;
    }
    ASSERT(status == FILEEOF);
    cout << "scan file1 saw " << i << " records " << endl;
    if (i != num/4)
    {
        cout << "!!! scan should have returned " << num << " records!" << endl;
        exit(1);
    }
    delete scan1;
    cout << "Test 13 passed!" << endl << endl;

    // perform filtered scan #2
    scan1 = new HeapFileScan("table3", status);
    ASSERT(status == OK);
    float filterVal2 = num * 9 / 10;
    cout << "Filtered scan matching f field GT than " << filterVal2 << endl;
    status = scan1->startScan(sizeof(int), sizeof(float), FLOAT, (char *) &filterVal2, GT);
    ASSERT(status == OK);
    i = 0;
    while ((status = scan1->scanNext(rec2Rid)) != FILEEOF)
    {
        // reconstruct record i
        status = scan1->getRecord(dbrec2);
        if (status != OK) break;
        // verify that the scan predicate was actually met
        RECORD *currRec = (RECORD *) dbrec2.data;
        if (! (currRec->f > filterVal2))
        {
            cerr << "!!! filtered scan returned record that doesn't satisfy predicate "
                     << "f val is " << currRec->f << endl;
            exit(1);
        }
        i++;
    }
    if (status != FILEEOF) error.print(status);
    cout << "scan table3 saw " << i << " records " << endl;
    if (i != num/10-1)
    {
        cout << "!!! filtered scan 2 should have returned " << num/10-1 << " records!" << endl;
        exit(1);
    }
    delete scan1;
    cout << "Test 14passed!" << endl << endl;


    // open up the heapFile
    file1 = new HeapFile("table3", status);
    ASSERT(status == OK);
    delete file1;


    // cout << "BUFSTATS:" << bufMgr->getBufStats() << endl;
    // bufMgr->clearBufStats();

    cout << "Call startScan with wrong parameter" << endl;
    scan1 = new HeapFileScan("table3", status);
    ASSERT(status == OK);
    status = scan1->startScan(0, 20, INTEGER, "Hi" , EQ);
    if ( status == BADSCANPARM)
    {
        cout << "passed BADSCANPARAM test" << endl;
    }
    else
    {
        cout << "should have returned BADSCANPARM, actually returned: " << endl;
        error.print(status);
        exit(1);
    }
    cout << "Test 15 passed!" << endl << endl;

    // add insert for bigger than pagesized record
    cout << "Try to insert bigger record than page size" << endl;
    iScan = new InsertFileScan("table3", status);
    ASSERT(status == OK)
    char bigdata[8192];
    sprintf(bigdata, "big record");
    dbrec1.data = (void *) &bigdata;
    dbrec1.length = 8192;
    status = iScan->insertRecord(dbrec1, rec2Rid);
    if ((status == INVALIDRECLEN) || (status == NOSPACE))
    {
        cout << "passed large record insert test" << endl;
    }
    else
    {
        cout << "got error status return from insert record " << endl;
        error.print(status);
        cout << "expected INVALIDRECLEN or NOSPACE" << endl;
        exit(1);
    }
    delete iScan;

    delete scan1;
    cout << "Test 16 passed!" << endl << endl;

    // MORE ERROR HANDLING TESTS HERE
  
    // get rid of the file
    if ((status = destroyHeapFile("table3")) != OK) {
        cout << endl << "got error status return from destroy file" << endl;
        error.print(status);
        exit(1);
    }
    delete bufMgr;
    cout << "Passed all tests." << endl;
    return 1;

}
