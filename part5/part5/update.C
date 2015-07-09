#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Update(const string & relation,
                       const int attrCnt,
                       const attrInfo attrList[],
                       const string & attrName,
                       const Operator op,
                       const Datatype type,
                       const char *attrValue)

{	
	
//typedef struct {
//  char relName[MAXNAME];                // relation name
//  char attrName[MAXNAME];               // attribute name
//  int attrOffset;                       // attribute offset
//  int attrType;                         // attribute type
//  int attrLen;                          // attribute length
//} AttrDesc;
//	typedef struct {
//  char relName[MAXNAME];                // relation name
//  char attrName[MAXNAME];               // attribute name
//  int  attrType;                        // INTEGER, FLOAT, or STRING
//  int  attrLen;                         // length of attribute in bytes
//  void *attrValue;                      // ptr to binary value
//} attrInfo; 
	 Status status;
	 HeapFileScan  *hfs;  
	  InsertFileScan *ifs;
	 AttrDesc   rd,temp[attrCnt];
	 int   tempi,tempa;
	 float tempf,tempb;
	 RID rid,Rid;
	 Record  rec,recd;
    int i=0;
    if (relation.empty())
    	 return BADCATPARM;
    ifs=new  InsertFileScan(relation,status);
 	  if(status!=OK)
  		return status;
     hfs=new  HeapFileScan(relation,status);
     if(status!=OK)
	  	return status;
//    	 cout<< "------------attrCnt       "<< attrCnt<<endl;
//    	 cout<<" ------------- relation       "<< relation<<endl;  	 
//    	 for(int i=0;i<attrCnt;i++)
//    	 {
//    	 	cout<<i<<"           "<<" attrList->relName               "<< attrList[i].relName<<endl;
//    	 	cout<<i<<"           "<<" attrList->attrName              "<< attrList[i].attrName<<endl;
//    	 	cout<<i<<"            "<<"attrList->attrType                 "<< attrList[i].attrType<<endl;
//    	 	cout<<i<<"            "<<"attrList->attrLen               "<< attrList[i].attrLen<<endl;
//    	 	cout<<i<<"            "<<"attrList-attrValue              "<< attrList[i].attrValue<<endl;	 	
//    	}
//    	 cout<<"  ------------ attrName           "<< attrName<<endl;
//	if((status=QU_Insert(relation, attrCnt, attrList))!=OK)
//		return status;
//	if((status=QU_Delete(relation,  attrName, op, type,  attrValue))!=OK)
//		return status;
if((status=attrCat->getInfo(relation, attrName,rd))!=OK)
	return status;
   	 	cout<<"           "<<" rd.relName               "<< rd.relName<<endl;
    	cout<<"           "<<" rd.attrName              "<< rd.attrName<<endl;
   	 	cout<<"            "<<"rd.attrType                 "<< rd.attrType<<endl;
   	 	cout<<"            "<<"rd.attrLen                  " << rd.attrLen<<endl;
   	 	cout<<"            "<<"ard.attrOffset            "<< rd.attrOffset<<endl;	 
  switch(rd.attrType)
    {
    case INTEGER:
     tempi=atoi(attrValue);
	 if((status=hfs->startScan(rd. attrOffset,rd. attrLen, INTEGER, (char*)&tempi,op))!=OK)
	    return OK;
      break;
    case FLOAT:
      tempf=atof(attrValue);
      if(( status=hfs->startScan(rd. attrOffset ,rd. attrLen ,FLOAT,(char*)&tempf,op))!=OK)
      	 return status;
      break;
    default:
     if(( status=hfs->startScan(rd. attrOffset,rd. attrLen ,STRING,(char*)attrValue,op))!=OK)
     	return  status;
      break;
    }  
  
      while ((status=hfs->scanNext(rid)) != FILEEOF)
	  {   
	  	      i=i+1;
		   if((status=hfs->getRecord(rec))!=OK)
	      	return status;
		  char data[rec.length];	
		  memcpy(&data, rec.data,rec.length );	 
		 if((status=hfs-> deleteRecord())!=OK)
		 	return status;
		 	
		// cout<<"---------------"<<rec.length<<endl;
		for(int j=0;j<attrCnt;j++)
		{
			if((status=attrCat->getInfo(attrList[j].relName, attrList[j].attrName,temp[j]))!=OK)
				return status;
//			  switch(temp[j].attrType)
//			    {
//			    case INTEGER:
//			     tempa=atoi((char *)attrList[j].attrValue);
//   				 memcpy(&data[ temp[j].attrOffset], &tempa,temp[j]. attrLen );   覆盖数据 
//				    return OK;
//			      break;
//			    case FLOAT:
//			     tempb=atof((char *)attrList[j].attrValue);
//				 memcpy(&data[ temp[j].attrOffset], &tempb,temp[j]. attrLen );   覆盖数据 
//			      break;
//			    default:
//			  	  memcpy(&data[ temp[j].attrOffset],(char *)attrList[j].attrValue,temp[j]. attrLen );   覆盖数据  
//			      break;
  			// memcpy(&data[ temp[j].attrOffset],(char *)attrList[j].attrValue,temp[j]. attrLen );   //覆盖数据   
  			  }  
		// recd.data=data;
   //   recd.length=rec.length;
   //   cout<<"1111111111111111"<<endl;
	//  if((status=ifs->insertRecord(recd, Rid))!=OK)
	       //   return status;  
	}
		//
	 cout<<" 总共找到的个数  --------------         "<<i<<endl;
	 delete hfs;
	 delete ifs;
	return OK;
}
