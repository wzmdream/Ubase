#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
	Status   status;
	RID    rid;
   AttrDesc  ad;
   HeapFileScan *hfs;
   RelDesc  record;
   Record  rec;
  hfs = new HeapFileScan(relation,status);
  if (status != OK)
  	 return status;
  if((status=	relCat->getInfo( relation, record))!=OK)
  		return status;
   if((status=attrCat->getInfo(relation, attrName,ad))!=OK)
  	 	return status;
    switch(ad.attrType)
    {
	    case INTEGER:
	      int tempi;
	       tempi=atoi((char *)attrValue);
	      if(( status=hfs->startScan(ad.attrOffset, ad.attrLen ,type, (char*)&tempi,op))!=OK)
	       	 	 return status;
	      break;
	    case FLOAT:
	      float tempf;
	       tempf=atof((char *)attrValue);
	       if(( status=hfs->startScan(ad.attrOffset, ad.attrLen ,type,(char *)&tempf,op))!=OK)
	         	 	  return status;
	      break;
	    default:
	    	   if(( status=hfs->startScan(ad.attrOffset, ad.attrLen ,type,attrValue,op))!=OK)
	         	 	  return status;
	      break;
    }
  while ((status=hfs->scanNext(rid)) != FILEEOF)
   {
    if((status= hfs->getRecord(rec))!=OK)
      	return status;
    if((status = hfs->deleteRecord())!=OK)
     		return status;
  }
 delete  hfs;
return OK;

}


