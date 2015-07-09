#ifndef HEAPFILE_H
#define HEAPFILE_H

#include <sys/types.h>
#include <functional>
#include <iostream>
#include <string.h>
using namespace std;

#include "page.h"
#include "buf.h"

extern DB db;

// define if debug output wanted
//#define DEBUGREL

// Some constant definitions
const unsigned MAXNAMESIZE = 50;

enum Datatype { STRING, INTEGER, FLOAT };    // attribute data types
enum Operator { LT, LTE, EQ, GTE, GT, NE };  // scan operators

struct pageEntry {
	int pageNo; // page number of the page
	int freeSpace; // number of free bytes on the page
};

const int DIRCNT = (PAGESIZE-2*sizeof(int)-MAXNAMESIZE)/sizeof(pageEntry);

class FileHdrPage
{
public:
  char 		fileName[MAXNAMESIZE];   // name of file
  int		recCnt;		// record count
  int		pageCnt;	// number of "in-use" entries in pages
  pageEntry	pages[DIRCNT];
};


// class definition of heapFile
class HeapFile {
protected:
   File* 	filePtr;        // underlying DB File object
   FileHdrPage*  headerPage;	// pinned file header page in buffer pool
   int		headerPageNo;	// page number of header page
   bool		hdrDirtyFlag;   // true if header page has been updated

   Page* 	curPage;	// data page currently pinned in buffer pool
   int   	curPageNo;	// page number of pinned page
   int		curIdx;		// index of current page in pages[] array
   bool  	curDirtyFlag;   // true if page has been updated
   RID   	curRec;         // rid of last record returned

public:

  // initialize
  HeapFile(const string & name, Status& returnStatus);

  // destructor
  ~HeapFile();

  // return number of records in file
  const int getRecCnt() const;

  // given a RID, read record from file, returning pointer and length
  const Status getRecord(const RID &rid, Record & rec);
};


class HeapFileScan : public HeapFile
{
public:

    HeapFileScan(const string & name, Status & status);

    // end filtered scan
    ~HeapFileScan();

    const Status startScan(const int offset, 
                           const int length,  
                           const Datatype type, 
                           const char* filter, 
                           const Operator op);

    const Status endScan(); // terminate the scan
    const Status markScan(); // save current position of scan
    const Status resetScan(); // reset scan to last marked location

    // return RID of next record that satisfies the scan 
    const Status scanNext(RID& outRid);

    // read current record, returning pointer and length
    const Status getRecord(Record & rec);

    // delete current record 
    const Status deleteRecord();

    // update current record
    const Status updateRecord(const Record & rec);

    // marks current page of scan dirty
    const Status markDirty();

private:
    int   offset;            // byte offset of filter attribute
    int   length;            // length of filter attribute
    Datatype type;           // datatype of filter attribute
    const char* filter;      // comparison value of filter
    Operator op;             // comparison operator of filter

    // The following variables are used to preserve the state
    // of the scan when the method markScan() is invoked.
    // A subsequent invocation of resetScan() will cause the
    // scan to be rolled back to the following
    int   markedPageNo;	// value of curPageNo when scan was "marked"
    RID   markedRec;    // value of curRec when scan was "marked"
    int	  markedIdx;    // value of curIdx when scan was "marked"

    const bool matchRec(const Record & rec) const;
};


class InsertFileScan : public HeapFile
{
public:

    InsertFileScan(const string & name, Status & status);

    // end filtered scan
    ~InsertFileScan();

    // insert record into file, returning its RID
    const Status insertRecord(const Record & rec, RID& outRid); 
};

#endif
