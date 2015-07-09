#include "catalog.h"
#include "query.h"


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[], //select 
		       const attrInfo *attr,  //where
		       const Operator op, 
		       const char *attrValue) 
{
	 // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;
    Status  status;
    AttrDesc  ad ,rd;
    AttrDesc  temp[projCnt];
    int reclen=0;
   
   for(int i=0;i<projCnt;i++)
   {    
	    if((status=attrCat->getInfo(projNames[i].relName, projNames[i].attrName,ad))!=OK)
	    	return status;
    	reclen+=ad.attrLen;
    	temp[i]=ad;
   }
  if(attr==NULL)
 	{	 
	   rd.attrOffset=0;
	   rd.attrLen=0;   
	  strcpy( rd.relName,projNames[0].relName); 
	  rd. attrType= STRING;
      attrValue=NULL;    
  	}
  	else
    { 	  
  		if((status=attrCat->getInfo(attr->relName, attr->attrName,rd))!=OK)
	    	return status;	
  	} 		
 if((status= ScanSelect( result,projCnt, temp, &rd, op, attrValue,reclen))!=OK)
	return status;
    return OK;
}

const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],    
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;
   Status  status;
   InsertFileScan *ifs;
   HeapFileScan  *hfs;
   Record  rec,recd;
	RID    rid;  	
	 int tempi;
    float tempf;
     int len=0;
	char data[ reclen];
   hfs=new  HeapFileScan(attrDesc->relName,status);
   if(status!=OK)
	  	return status;
   ifs=new  InsertFileScan(result,status);
   if(status!=OK)
  	return status;
   switch( attrDesc->attrType)
    {
    case INTEGER:
     tempi=atoi(filter);
 	 if(( status=hfs->startScan(attrDesc->attrOffset,attrDesc->attrLen,INTEGER,(char*)&tempi,op))!=OK)
     	return status;
      break;
    case FLOAT:
      tempf=atof((char*)filter);
      if(( status=hfs->startScan(attrDesc->attrOffset, attrDesc->attrLen ,FLOAT,(char*)&tempf,op))!=OK)
      	 return status;
      break;
    default:
     if(( status=hfs->startScan(attrDesc->attrOffset,attrDesc->attrLen ,STRING,(char*)filter,op))!=OK)
     	return  status;
      break;
    }  

      while ((status=hfs->scanNext(rid)) != FILEEOF)
   {
	    if((status=hfs->getRecord(rec))!=OK)
	      	return status;
	     char recdata[rec.length];
	     memcpy(&recdata, rec.data,rec.length );	 
	    // int len=0;
	     for(int i=0;i<projCnt;i++)
	     {  
	        memcpy(&data[len],&recdata[projNames[i].attrOffset],projNames[i].attrLen);
	        len+=projNames[i].attrLen;
       }
          recd.data=data;
	      recd .length=reclen;
	      if((status=ifs->insertRecord(recd, rid))!=OK)
	          return status;    
	      len=0;
 } 
	  	delete hfs;
	  	delete ifs;
		return OK;
}
