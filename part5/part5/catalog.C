#include "catalog.h"


RelCatalog::RelCatalog(Status &status) :
	 HeapFile(RELCATNAME, status)
{
// nothing needed
}


const Status RelCatalog::getInfo(const string & relation, RelDesc &record)
{
  if (relation.empty())
    return BADCATPARM;

  Status status;
  Record rec;
  RID rid;

  HeapFileScan * scan;

  scan = new HeapFileScan(RELCATNAME,status);
  if (status != OK) return status;

  scan->startScan(0,sizeof(record.relName),STRING,relation.c_str(),EQ);
  if (status != OK) return status;

  status=scan->scanNext(rid);
  if (status == FILEEOF) return  RELNOTFOUND;
  if ((status != FILEEOF) && (status != OK)) return status;

  status=scan->getRecord(rec);
  if (status != OK) return status;

  memcpy(&record,rec.data,rec.length);
  
  scan->endScan();
  delete scan;
  scan = NULL;
  return OK;
}


const Status RelCatalog::addInfo(RelDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;
  Record Relrec;

  Relrec.data = &record;
  Relrec.length=sizeof(RelDesc);
  ifs = new InsertFileScan(RELCATNAME,status);
  if (status != OK) return status;
  
  status = ifs->insertRecord(Relrec,rid);
  if (status != OK) return status; 

  delete ifs;

  return OK;
}

const Status RelCatalog::removeInfo(const string & relation)
{
  Status status;
  RID rid;
  HeapFileScan*  hfs;

  if (relation.empty()) return BADCATPARM;

  hfs = new HeapFileScan(RELCATNAME,status);
  if (status != OK) return status;

  hfs->startScan(0,relation.length(),STRING,relation.c_str(),EQ);
  if (status != OK) return status;

  status=hfs->scanNext(rid);
  if (status != OK) return status;

  status = hfs->deleteRecord();
  if (status != OK) return status;

  hfs->endScan();
  delete hfs;

  return OK;
}


RelCatalog::~RelCatalog()
{
// nothing needed
}


AttrCatalog::AttrCatalog(Status &status) :
	 HeapFile(ATTRCATNAME, status)
{
// nothing needed
}


const Status AttrCatalog::getInfo(const string & relation, 
				  const string & attrName,
				  AttrDesc &record)
{

  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;


  hfs = new HeapFileScan(ATTRCATNAME,status);
  if (status != OK) return status;

  hfs->startScan(0,sizeof(record.relName),STRING,relation.c_str(),EQ);
  if (status != OK) return status;

  while ((status=hfs->scanNext(rid)) != FILEEOF)
  {
    if (status != OK) return status;
    status=hfs->getRecord(rec);
    if (status != OK) return status;
    if (strncmp((char*)rec.data + sizeof(record.relName),attrName.c_str(),sizeof(record.attrName))==0)
    {
        memcpy(&record,rec.data,rec.length);
        break;
    }
  }

  hfs->endScan();
  delete hfs;

  return OK;
}


const Status AttrCatalog::addInfo(AttrDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status status;

  Record Attrrec;

  Attrrec.data = &record;
  Attrrec.length=sizeof(AttrDesc);
  ifs = new InsertFileScan(ATTRCATNAME,status);
  if (status != OK) return status;

  status = ifs->insertRecord(Attrrec,rid);
  if (status != OK) return status;

  delete ifs;

  return OK;

}


const Status AttrCatalog::removeInfo(const string & relation, 
			       const string & attrName)
{
  Status status;
  Record rec;
  RID rid;
  AttrDesc record;
  HeapFileScan*  hfs;

  if (relation.empty() || attrName.empty()) return BADCATPARM;

  hfs = new HeapFileScan(ATTRCATNAME,status);
  if (status != OK) return status;

  hfs->startScan(0,sizeof(record.relName),STRING,relation.c_str(),EQ);
  if (status != OK) return status;
  while ((status=hfs->scanNext(rid)) != FILEEOF)
  {
    if (status != OK) return status;
    status=hfs->getRecord(rec);
    if (status != OK) return status;
    
    if (strncmp((char*)rec.data + sizeof(record.relName),attrName.c_str(),sizeof(record.attrName))==0)
    {
      status = hfs->deleteRecord();
      if (status != OK) return status;
      break;
    }
  }

  hfs->endScan();
  delete hfs;

  return OK;
}


const Status AttrCatalog::getRelInfo(const string & relation, 
				     int &attrCnt,
				     AttrDesc *&attrs)
{
  Status status;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;
  RelDesc rel;
  if (relation.empty()) return BADCATPARM;

  status = relCat->getInfo(relation, rel);
  //if (status == FILEEOF) return  RELNOTFOUND;
  //if ((status != FILEEOF) && (status != OK)) return status;

  if (status != OK) return status;

  hfs = new HeapFileScan(ATTRCATNAME,status);

  hfs->startScan(0,sizeof(attrs->relName),STRING,relation.c_str(),EQ);

  if (status != OK) return status;
  attrCnt = 0;
  attrs = new AttrDesc[10];
  while ((status=hfs->scanNext(rid)) != FILEEOF)
  {
    if (status != OK) return status;

    status=hfs->getRecord(rec);
    if (status != OK) return status;

    memcpy(&attrs[attrCnt],rec.data,rec.length);
    attrCnt++;
  }
  hfs->endScan();
  delete hfs;

  return OK;

}
AttrCatalog::~AttrCatalog()
{
// nothing needed
}
