#include "catalog.h"


RelCatalog::RelCatalog(Status &status) : HeapFile(RELCATNAME, status)
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






}


const Status RelCatalog::addInfo(RelDesc & record)
{
    RID rid;
    InsertFileScan*  ifs;
    Status status;







}

const Status RelCatalog::removeInfo(const string & relation)
{
    Status status;
    RID rid;
    HeapFileScan*  hfs;

    if (relation.empty()) return BADCATPARM;







}


RelCatalog::~RelCatalog()
{
    // nothing needed
}


AttrCatalog::AttrCatalog(Status &status) : HeapFile(ATTRCATNAME, status)
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






}


const Status AttrCatalog::addInfo(AttrDesc & record)
{
    RID rid;
    InsertFileScan*  ifs;
    Status status;






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







}


const Status AttrCatalog::getRelInfo(const string & relation, 
				     int &attrCnt,
				     AttrDesc *&attrs)
{
    Status status;
    RID rid;
    Record rec;
    HeapFileScan*  hfs;

    if (relation.empty()) return BADCATPARM;









}


AttrCatalog::~AttrCatalog()
{
    // nothing needed
}
