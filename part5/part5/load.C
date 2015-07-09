#include <unistd.h>
#include <fcntl.h>
#include "catalog.h"
#include "utility.h"


//
// Loads a file of (binary) tuples from a standard file into the relation.
// Any indices on the relation are updated appropriately.
//
// Returns:
// 	OK on success
// 	an error code otherwise
//

const Status UT_Load(const string & relation, const string & fileName)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;

  if (relation.empty() || fileName.empty() || relation == string(RELCATNAME)
      || relation == string(ATTRCATNAME))
    return BADCATPARM;

  // open Unix data file
  int fd;
  if ((fd = open(fileName.c_str(), O_RDONLY, 0)) < 0)
    return UNIXERR;

  //check relcat to make sure the relation exists
  status = relCat->getInfo(relation,rd);
  if (status != OK) return status;

  //check attrcat to make sure the relation attributes entry exists
  status = attrCat->getRelInfo(relation,attrCnt,attrs);
  if (status!=OK) return status;

  //get record length  
  int reclen = 0;
  for(int i=0; i<attrCnt;i++)
    reclen += attrs[i].attrLen;

  InsertFileScan *iScan;
  RID rid;
  Record rec;
  rec.length=reclen;
  iScan = new InsertFileScan(relation,status);
  if (status != OK) return status;

  //buffer temporarily hold the record read from file

  char buffer[reclen];

  while(read(fd,buffer,reclen))
  {
    rec.data = &buffer;
    status = iScan->insertRecord(rec,rid);
    if (status != OK) return status;
  }
  delete iScan;

  // close Unix file
  if (close(fd) < 0)
     return UNIXERR;
  else
     return OK;
}
