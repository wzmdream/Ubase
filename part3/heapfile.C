#include "heapfile.h"
#include "error.h"

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
    int len ;
    if((status=db.createFile(fileName))!=OK)                                  //创建一个文件 
	   return status;
	if((status=db.openFile(fileName,file))!=OK)
	 	return status;
    if((status=bufMgr->allocPage(file,hdrPageNo,newPage))!=OK)  //在缓冲池中分配一个页 
	    return status;
    hdrPage=( FileHdrPage*)newPage;
    len=(int)fileName.length( );
    for(i=0;i<len;i++)
    hdrPage->fileName[i]=fileName[i];
   cout<<hdrPage->fileName<<endl;
	hdrPage->recCnt=0;
    hdrPage->pageCnt=0;
     cout << " createHeapFile, hdrPageNo is " <<hdrPageNo << endl;
    for(i=0;i<DIRCNT;i++)
    {
    	hdrPage->pages[i].pageNo=-1;
        hdrPage->pages[i].freeSpace=-1;
      }
    if((status=bufMgr->allocPage(file,newPageNo,newPage))!=OK)
	return status;
	newPage->init(newPageNo);
	hdrPage->pages[hdrPage->pageCnt].pageNo=newPageNo;
	hdrPage->pages[hdrPage->pageCnt].freeSpace=newPage->getFreeSpace();
	hdrPage->pageCnt++;
	     cout << " createHeapFile, newPageNo is " << 	newPageNo<< endl;
	if((status=bufMgr->unPinPage( file,hdrPageNo, true ,true ))!=OK)
     	return status;
    if((status=bufMgr->unPinPage( file,newPageNo, true ,true ))!=OK)
	   return status;
	   
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
	    int pageNo;
	    int No;
	   cout << "running constructor on file " << fileName << endl;
	   if((status=db.openFile(fileName,filePtr))!=OK)
		 returnStatus= status;
	   if((status= filePtr->getFirstPage( pageNo) )!=OK)
       returnStatus= status;
	   if((status= bufMgr->readPage(filePtr, pageNo,pagePtr))!=OK)
	   returnStatus=status;
	   headerPage=( FileHdrPage*)pagePtr;
	   headerPageNo=pageNo;
	   hdrDirtyFlag=true;
	     cout << " HeapFile, headerPageNo is " <<headerPageNo << endl;
	   No=headerPage->pages[0]. pageNo;
	   if((status= bufMgr->readPage(filePtr, No,pagePtr))!=OK)
	   returnStatus= status;
	   curPage=pagePtr;
	   curPageNo=No;
	   curIdx=0;
	   curDirtyFlag=true;
	   cout << " HeapFile, curPageNo is " <<curPageNo << endl;
	   curRec=NULLRID;
	   returnStatus= OK;

}

// the destructor closes the file
HeapFile::~HeapFile()
{
   // Status status;
    //cout << "invoking heapfile destructor on file " << headerPage->fileName << endl;
    bufMgr-> unPinPage(filePtr,  headerPageNo,  hdrDirtyFlag, true); 
    bufMgr-> unPinPage(filePtr,curPageNo, curDirtyFlag,false);
    bufMgr->flushFile(filePtr);
    db.closeFile(filePtr);
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
	if (rid. pageNo==curPageNo)
		{ 
			if((status=curPage->getRecord(rid,rec))!=OK)
		 	  return status;
		 	  
		}
	else
	 {
	 	 if((status=bufMgr->unPinPage( filePtr,curPageNo,curDirtyFlag ,false ))!=OK)
		   return status;
		 if((status=bufMgr->readPage(filePtr, rid. pageNo,curPage))!=OK)
		   return status;
		 curPageNo=rid. pageNo;     
		 curDirtyFlag=true;  
		 if((status=curPage->getRecord(rid,rec))!=OK)
		 	return status;  	
		}	
      curRec=rid;
	  return OK;

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
    Page*	pagePtr;
    // start by making sure the first page of the file is in the buffer pool
    // if not, unpin the currently pinned page and then read the 
    // first page of the file into the buffer pool
    // your code to do the above goes here
 	if(curPageNo!= headerPage->pages[0].pageNo)
 	   {
		   	if((status=bufMgr->unPinPage( filePtr,curPageNo, curDirtyFlag ,false ))!=OK)
			   return status;   
	       if((status=bufMgr->readPage(filePtr,headerPage->pages[0].pageNo,pagePtr))!=OK)
			    return status;
    	}	
	  curPage=pagePtr;
	  curPageNo=headerPage->pages[0].pageNo;
       
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
    Status status;
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
        curPage = NULL;
        curPageNo = 0;
	curDirtyFlag = false;
    }
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
if(markedPageNo!=curPageNo)
	{
		  if((status=bufMgr->unPinPage( filePtr,curPageNo,curDirtyFlag ,false ))!=OK)
			   return status; 
		   if((status=bufMgr->readPage(filePtr,markedPageNo,curPage))!=OK)
		     return status;		
		    curPageNo  =markedPageNo ;	
	}
     curRec = markedRec ;
     curIdx  =markedIdx ;
     curDirtyFlag=true;
     return OK;

}


const Status HeapFileScan::scanNext(RID& outRid)
{
    Status 	status = OK;
    RID		nextRid;
    RID		tmpRid;
    Record      rec;
    int i;
   cout << "in ScanNext(), curPageNo is " << curPageNo << endl;
   if(curRec.pageNo!=curPageNo)
  	 return BADPAGENO;
   if((status=curPage->getRecord(curRec,  rec))!=OK)
	 return status;
   if( matchRec(rec)==true)
	 {    
		    curRec = tmpRid;
		    outRid= curRec;
		  //  cout << "in ScanNext(), curPageNo is " << curPageNo << endl;
		    return OK;
	   }
    for(i=0;i<headerPage->pageCnt;i++)
    {	 
       if((status=bufMgr->unPinPage( filePtr,curPageNo, curDirtyFlag ,false ))!=OK)
	    return status; 
       if((status==bufMgr->readPage(filePtr,headerPage->pages[i].pageNo,curPage))!=OK)
    	return status;
       curPageNo=headerPage->pages[i].pageNo;
       curDirtyFlag=true;
       curIdx=i;	
     //  cout << "in ScanNext(), curPageNo is " << curPageNo << endl;
       if((status=curPage->firstRecord(tmpRid))!=OK)
         return status;
        while(status==OK)
        {
	         if((status=curPage->getRecord(tmpRid,  rec))!=OK)
	        	return status;
	         if( matchRec(rec)==true)
	         {
			       curRec = tmpRid;
			       outRid= curRec;
			       return OK;
			       
	          }
	          
	       if(( status=curPage->nextRecord(tmpRid,nextRid))!=OK)
	          break;
	       tmpRid=nextRid;
     }
     
       }
  if(i==headerPage->pageCnt)
   return  FILEEOF ;
     
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
  if(curRec.pageNo!=curPageNo)
  	 return BADPAGENO;
  if((status=curPage-> deleteRecord(curRec))!=OK)
  	 return status;  
  headerPage->pages[curIdx].freeSpace=curPage->getFreeSpace(); 
  headerPage->recCnt--;	 
  hdrDirtyFlag=true;
  curDirtyFlag=true;
   return OK;

}

// update current record
const Status HeapFileScan::updateRecord(const Record & rec)
{
    Status status;
    if((status=curPage-> updateRecord( rec,curRec))!=OK)
      return status;
   hdrDirtyFlag=true;
   curDirtyFlag=true;
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
    Status status;
    // unpin last page of the scan
    if (curPage != NULL)
    {
        status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false);
        curPage = NULL;
	curRec = NULLRID;
        curPageNo = 0;
        if (status != OK) cerr << "error in unpin of data page\n";
    }
}

// Insert a record into the file
const Status InsertFileScan::insertRecord(const Record & rec, RID& outRid)
{
    Status	status;
    Page*	page;
    RID		rid;
     int   pageNo;
     int i;
//  	status=curPage->insertRecord(rec,  rid);
//    while(status!=OK)
//    {
//          if(( status = bufMgr->unPinPage(filePtr, curPageNo, false, false))!=OK)
//          	return status;
//  	    if((status=bufMgr->allocPage(filePtr, pageNo,curPage))!=OK)
//            return status;	
//  	    curPageNo= pageNo;   
//  	    curPage->init( curPageNo);
//        headerPage->pages[  headerPage->pageCnt++].pageNo= pageNo;
//        headerPage->pages[  headerPage->pageCnt++].freeSpace= curPage->getFreeSpace();	
//  	    headerPage->recCnt++;	
//  	    status=curPage->insertRecord(rec,  rid);
//     }
//    	
//    	return OK;	
 if(curPage->insertRecord(rec, rid)==OK) //判断插入记录是否成功 
      {   
          hdrDirtyFlag=true;
          curDirtyFlag=true;
          headerPage->recCnt++;
          headerPage->pages[curIdx].freeSpace=curPage->getFreeSpace(); 
          outRid=rid;
          curRec=rid;
         //    cout<< curRec.pageNo<<endl;
          return OK;
       }

	for(i=0;i<(headerPage->pageCnt);i++)
   	{
   		 if(( status = bufMgr->unPinPage(filePtr, curPageNo, curDirtyFlag, false))!=OK)
          	return status;
          curPageNo=headerPage->pages[i].pageNo;  	
         if((status=bufMgr-> readPage(filePtr,curPageNo,curPage))!=OK)
          	return status;
         if(curPage->insertRecord(rec, rid)==OK) //判断插入记录是否成功 
          	{   	          
      			 hdrDirtyFlag=true;
       			 curDirtyFlag=true;
       			 curIdx=i;	
       			 headerPage->recCnt++;
       			 headerPage->pages[curIdx].freeSpace=curPage->getFreeSpace(); 
        		outRid=rid;
        		curRec=rid;
        		//  cout<< curRec.pageNo<<endl;
        		return OK;
            }
     }

     if(i==headerPage->pageCnt)  //假如在整个目录页都没有找到
    {     
	        if((status=bufMgr->allocPage(filePtr,pageNo,page))!=OK)
	            return status;	
	            
	        if((status=curPage->setNextPage(pageNo))!=OK)
	         	return status;
	         	
	        if(( status = bufMgr->unPinPage(filePtr, curPageNo  , curDirtyFlag, false))!=OK)
	          	return status;
	          	
	       	curPageNo=pageNo;
	       	curPage=page;
	        curPage->init( curPageNo);
	        headerPage->pages[headerPage->pageCnt].pageNo=curPageNo;
	        headerPage->pageCnt++;
	        curIdx= headerPage->pageCnt;	
	        if((status=curPage->insertRecord(rec, rid))!=OK) //判断插入记录是否成功 
	          return status;   
	          
		    hdrDirtyFlag=true;
		    curDirtyFlag=true;
		    curIdx=i;	
		    headerPage->recCnt++;
		    headerPage->pages[curIdx].freeSpace=curPage->getFreeSpace(); 
		    outRid=rid;
		    curRec=rid;
		 // cout<< curRec.pageNo<<endl;
    }
    return OK;
     
  }
    
    
    
    
    
   
      

