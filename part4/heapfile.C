#include "heapfile.h"
#include "error.h"
Error err;
// routine to create a heapfile
const Status createHeapFile(const string fileName)
{
    File* 		file;
    Status 		status;
    FileHdrPage*	hdrPage;
    int			hdrPageNo;
    int			newPageNo;
    Page*		newPage;
    int 		i;

    //create new file,the file contains only one page(file header)
    status = db.createFile(fileName);
    if (status!=OK) return status;

    //open the file just created
    status = db.openFile(fileName,file);
    if (status!=OK) return status;
  
    //produce the FileHdrPage,
    //acturally,this is the second page of physical file
    //and the page number in the physical file shoud be 1 
    status = bufMgr->allocPage(file,hdrPageNo,newPage);
    if (status!=OK) return status;

    //cast from Page* to FileHdrPage*
    hdrPage=reinterpret_cast<FileHdrPage*>(newPage);
    
    //initialize data member
    memcpy(hdrPage->fileName,&fileName,fileName.length());
    hdrPage->recCnt=0;
    hdrPage->pageCnt=0;
    for (i=0; i<DIRCNT;i++){
        hdrPage->pages[i].pageNo=-1;
        hdrPage->pages[i].freeSpace=-1;
    }

    //allocate first data page,acturally,this is the third page of physical file
    status = bufMgr->allocPage(file,newPageNo,newPage);
    if (status!=OK) return status;
    newPage->init(newPageNo);
    hdrPage->pageCnt=1;
    hdrPage->pages[0].pageNo=newPageNo;
    hdrPage->pages[0].freeSpace=newPage->getFreeSpace();

    status = bufMgr->unPinPage(file,hdrPageNo,true,false); 
    if (status!=OK) return status;

    status = bufMgr->unPinPage(file,newPageNo,true,false);
    if (status!=OK) return status;

/*
    status = bufMgr->flushFile(file);
    if (status!=OK)
        cout << "flush file error after file created when running createHeapFile" << endl;
*/
    status = db.closeFile(file);
    if (status!=OK)
        cout << "close file error after file created when running createHeapFile" << endl;

    return OK;
}

// routine to destroy a heapfile
const Status destroyHeapFile(const string fileName)
{
    return (db.destroyFile (fileName));
}

// constructor opens the underlying file
HeapFile::HeapFile(const string & fileName, Status& returnStatus)
{
    Status 	status;
    Page*	pagePtr;
    //cout << "running constructor on file " << fileName << endl;

    status = db.openFile(fileName,filePtr);
    if (status!=OK)
    {
        returnStatus=status;
        err.print(status);
        exit(1);
    }
    status = filePtr->getFirstPage(headerPageNo);
    if (status!=OK)
    {
        returnStatus=status;
        err.print(status);
        exit(1);
    }
    status = bufMgr->readPage(filePtr, headerPageNo, pagePtr);
    if (status!=OK)
    {
        returnStatus=status;
        err.print(status);
        exit(1);
    }
    headerPage = reinterpret_cast<FileHdrPage*>(pagePtr);
    headerPageNo = 1;
    hdrDirtyFlag = false;

    curPageNo = headerPage->pages[0].pageNo;

    status = bufMgr->readPage(filePtr, curPageNo, pagePtr);
    if (status!=OK)
    {
        returnStatus=status;
        err.print(status);
        exit(1);
    }

    curPage = pagePtr;
    curIdx = 0;
    curDirtyFlag = false;
    curRec = NULLRID;
    returnStatus=status;
}

// the destructor closes the file
HeapFile::~HeapFile()
{
    Status status;
    //unpin header page of the scan
    status = bufMgr->unPinPage(filePtr,headerPageNo,hdrDirtyFlag,false);
    if (status!=OK)
        cout << "Destructor encounters error while unpin the header page" << endl;

    // unpin last page of the scan
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
        if (status != OK)
            cout << "Destructor encounters error while unpin the last page" << endl;
    }
    status = db.closeFile(filePtr);
    if (status!=OK)
        cout << "Destructor encounters error while close the file" << endl;
}

// Return number of records in heap file

const int HeapFile::getRecCnt() const
{
    return headerPage->recCnt;
}

// retrieve an arbitrary record from a file.
// if record is not on the currently pinned page, the current page
// is unpinned and the required page is read into the buffer pool
// and pinned.  returns a pointer to the record via the rec parameter

const Status HeapFile::getRecord(const RID &  rid, Record & rec)
{
    Status status;

    // cout<< "getRecord. record (" << rid.pageNo << "." << rid.slotNo << ")" << endl;
    if (rid.pageNo==curPageNo)
    {
        status=curPage->getRecord(rid, rec);
        return status;
    }
    else
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
        if(status!=OK) return status;

        curPageNo=rid.pageNo;
        status = bufMgr->readPage(filePtr, curPageNo, curPage);
        if(status!=OK) return status;

        curDirtyFlag = false;
        status=curPage->getRecord(rid, rec);
        curRec=rid;
        return status;
    }
}

HeapFileScan::HeapFileScan(const string & name,
			   Status & status) : HeapFile(name, status)
{
    filter = NULL;
}

const Status HeapFileScan::startScan(const int offset_,
				     const int length_,
				     const Datatype type_, 
				     const char* filter_,
				     const Operator op_)
{
    Status status;

    // start by making sure the first page of the file is in the buffer pool
    // if not, unpin the currently pinned page and then read the 
    // first page of the file into the buffer pool

    // your code to do the above goes here

    if (curPageNo!=headerPage->pages[0].pageNo)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
        if (status!=OK) return status;
        curPageNo=headerPage->pages[0].pageNo;
        status = bufMgr->readPage(filePtr, curPageNo, curPage);
        if (status!=OK) return status;
        curDirtyFlag = false;
        curRec=NULLRID;
        curIdx=0;
    }
    // then check the rest of the paramters
    if (!filter_) {                        // no filtering requested
        filter = NULL;
        return OK;
    }
   
    if ((offset_ < 0 || length_ < 1) ||
        (type_ != STRING && type_ != INTEGER && type_ != FLOAT) ||
        (type_ == INTEGER && length_ != sizeof(int)
         || type_ == FLOAT && length_ != sizeof(float)) ||
        (op_ != LT && op_ != LTE && op_ != EQ && op_ != GTE && op_ != GT && op_ != NE))
    {
        return BADSCANPARM;
    }

    offset = offset_;
    length = length_;
    type = type_;
    filter = filter_;
    op = op_;

    return OK;
}

const Status HeapFileScan::endScan()
{
    // nothing really to do here as we want to leave to leave the last page pinned
    return OK;
}

HeapFileScan::~HeapFileScan()
{
    // Nothing really to do here,
    // because the destructor of parent class i.e. HeapFile::~HeapFile()
    // will do everything for us
}

const Status HeapFileScan::markScan()
{
    // make a snapshot of the state of the scan
    markedPageNo = curPageNo;
    markedRec = curRec;
    markedIdx = curIdx;
    return OK;
}

const Status HeapFileScan::resetScan()
{
    Status status;
    if (markedPageNo == curPageNo) return OK;

    status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
    if (status!=OK) return status;

    status = bufMgr->readPage(filePtr, markedPageNo, curPage);
    if (status!=OK) return status;

    curPageNo=markedPageNo;
    curDirtyFlag = false;
    curRec=markedRec;
    curIdx=markedIdx;

    return OK;
}


const Status HeapFileScan::scanNext(RID& outRid)
{
    Status 	status = OK;
    RID		nextRid;
    RID		tmpRid;
    Record      rec;

    // cout << "in ScanNext(), curPageNo is " << curPageNo << endl;
    while(true)
    {
        if (curRec.slotNo==-1){
            status=curPage->firstRecord(tmpRid);    
            if ((status==NORECORDS) && (headerPage->pages[curIdx+1].pageNo==-1)){
                 return FILEEOF;
            }
            if ((status==NORECORDS) && (headerPage->pages[curIdx+1].pageNo!=-1)){
                status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
                if (status!=OK) return status;

                curPageNo=headerPage->pages[curIdx+1].pageNo;
                status = bufMgr->readPage(filePtr, curPageNo, curPage);
                if (status!=OK) return status;
                curIdx++;
                curRec=NULLRID;
                continue;
            }

            if ((status!=OK) && (status!=NORECORDS)) return status;
            curRec=tmpRid;
        }
        else{
            status=curPage->nextRecord(curRec,nextRid);
            if (status==OK){
                tmpRid=nextRid;
                curRec=tmpRid;
            }
            if((status==ENDOFPAGE) && (headerPage->pages[curIdx+1].pageNo==-1))
                return FILEEOF;

            if((status==ENDOFPAGE) && (headerPage->pages[curIdx+1].pageNo!=-1)){
                status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
                if (status!=OK) return status;

                curPageNo=headerPage->pages[curIdx+1].pageNo;
                status = bufMgr->readPage(filePtr, curPageNo, curPage);
                if (status!=OK) return status;
                curIdx++;
                curRec=NULLRID;
                continue;
            }
            if ((status!=OK) && (status!=ENDOFPAGE))
                return status;
        }
        status=getRecord(rec);
        if (status!=OK) return status;

        if(matchRec(rec)==true){
            outRid=curRec;
            break;
        }
    }
    return OK;
}


// returns pointer to the current record.  page is left pinned
// and the scan logic is required to unpin the page 

const Status HeapFileScan::getRecord(Record & rec)
{
    return curPage->getRecord(curRec, rec);
}

// delete record from file. 
const Status HeapFileScan::deleteRecord()
{
    Status status;

    if (curRec.pageNo!=curPageNo) return BADPAGENO;

    status=curPage->deleteRecord(curRec);
    if (status!=OK) return status;

    status=markDirty();
    if (status!=OK) return status;

    headerPage->recCnt++;
    hdrDirtyFlag=true;

    return OK;
}

// update current record
const Status HeapFileScan::updateRecord(const Record & rec)
{

    Status status;

    if (curRec.pageNo!=curPageNo) return BADPAGENO;

    status=curPage->updateRecord(rec,curRec);
    if (status!=OK) return status;

    status=markDirty();
    if (status!=OK) return status;

    return OK;
}


// mark current page of scan dirty

const Status HeapFileScan::markDirty()
{
    curDirtyFlag = true;
    return OK;
}

const bool HeapFileScan::matchRec(const Record & rec) const
{
    // no filtering requested
    if (!filter) return true;

    // see if offset + length is beyond end of record
    // maybe this should be an error???
    if ((offset + length -1 ) >= rec.length)
	return false;

    float diff = 0;                       // < 0 if attr < fltr
    switch(type) {

    case INTEGER:
        int iattr, ifltr;                 // word-alignment problem possible
        memcpy(&iattr,
               (char *)rec.data + offset,
               length);
        memcpy(&ifltr,
               filter,
               length);
        diff = iattr - ifltr;
        break;

    case FLOAT:
        float fattr, ffltr;               // word-alignment problem possible
        memcpy(&fattr,
               (char *)rec.data + offset,
               length);
        memcpy(&ffltr,
               filter,
               length);
        diff = fattr - ffltr;
        break;

    case STRING:
        diff = strncmp((char *)rec.data + offset,
                       filter,
                       length);
        break;
    }

    switch(op) {
    case LT:  if (diff < 0.0) return true; break;
    case LTE: if (diff <= 0.0) return true; break;
    case EQ:  if (diff == 0.0) return true; break;
    case GTE: if (diff >= 0.0) return true; break;
    case GT:  if (diff > 0.0) return true; break;
    case NE:  if (diff != 0.0) return true; break;
    }

    return false;
}

InsertFileScan::InsertFileScan(const string & name,
                               Status & status) : HeapFile(name, status)
{
    //cout << "doing insertfile scan constuctor " << endl;
    // the heapfile constructor will make sure that the header page 
    // and the first page of the file are in the buffer pool
    curRec = NULLRID;
}

InsertFileScan::~InsertFileScan()
{
    // Nothing really to do here,
    // because the destructor of parent class i.e. HeapFile::~HeapFile()
    // will do everything for us
}

// Insert a record into the file
const Status InsertFileScan::insertRecord(const Record & rec, RID& outRid)
{
    Status	status;
    RID		rid;
    int		prevPageNo;
    Page*	prevPage;
    while(headerPage->pages[curIdx].freeSpace<(rec.length+(int)sizeof(slot_t)))
    {
        prevPageNo = curPageNo;
        prevPage = curPage;
        if(headerPage->pages[curIdx+1].pageNo==-1)
        {
            status = bufMgr->allocPage(filePtr,curPageNo,curPage);
            if (status!=OK) return status;

            prevPage->setNextPage(curPageNo);
            status = bufMgr->unPinPage(filePtr,prevPageNo, true, false);
            if (status!=OK) return status;


            curPage->init(curPageNo);
            curRec=NULLRID;
            curIdx++;
            headerPage->pageCnt++;
            headerPage->pages[curIdx].pageNo=curPageNo;
            headerPage->pages[curIdx].freeSpace=curPage->getFreeSpace();
            hdrDirtyFlag=true;
            break;
        }
        else
        {
            status = bufMgr->unPinPage(filePtr,curPageNo, curDirtyFlag, false);
            if (status!=OK) return status;
            curPageNo=headerPage->pages[curIdx+1].pageNo;
            status = bufMgr->readPage(filePtr, curPageNo, curPage);
            if (status!=OK) return status;
            curRec=NULLRID;
            curIdx++;
        }
    }
    status = curPage->insertRecord(rec,rid);
    if (status!=OK) return status;
    headerPage->pages[curIdx].freeSpace=curPage->getFreeSpace();
    curRec=rid;
    curDirtyFlag=true;
    headerPage->recCnt++;
    hdrDirtyFlag=true;
    outRid=rid;

    return OK;
}
