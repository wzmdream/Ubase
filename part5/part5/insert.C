#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
	 Status  status;
	 Record    record;
	  RID rid;
	 AttrDesc   *attrs ;
     int  AttrCnt  ;
      InsertFileScan *hfs;
      int length=0;
      hfs=new   InsertFileScan(relation,status);
      if(status!=OK)
      	return status;
      if((status=attrCat->getRelInfo(relation,  AttrCnt , attrs))!=OK)
	 	return status;
   for(int i=0;i<AttrCnt;i++)
    {
  	  length+=attrs[i].attrLen;
    }
    char data[length];
    if(AttrCnt ==attrCnt)
	 {         
	 	        for(int i=0;i<attrCnt;i++)   
	 	        for(int j=0;j<attrCnt;j++)
			    {
			         if (strcmp(attrs[i].attrName,attrList[j].attrName)==0)	
	         	    switch(attrs[i].attrType)
					    {
					    case INTEGER:
					    int tempi;
					     tempi=atoi((char *)attrList[j].attrValue);
					     memcpy(&data[ attrs[i].attrOffset],&tempi,attrs[i]. attrLen );	  
					      break;
					    case FLOAT:
					      float tempf;
					      tempf=atof((char *)attrList[j].attrValue);
					       memcpy(&data[ attrs[i].attrOffset],&tempf,attrs[i]. attrLen );	  
					      break;
					    default:
					     memcpy(&data[ attrs[i].attrOffset],(char *)attrList[j].attrValue,attrs[i]. attrLen );
					      break;
					    }  
			     }		
	                 record.data=data;
	                 record.length=length;
			      	if((status=hfs->insertRecord(record, rid))!=OK)
						return status;      
	 }		 
	 else
	 	return ATTRIBUTELACK;
delete hfs;
delete [] attrs;
return OK;

}

