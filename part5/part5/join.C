#include "catalog.h"
#include "query.h"
#include "sort.h"

extern JoinType JoinMethod;

const int matchRec(const Record & outerRec,
		   const Record & innerRec,
		   const AttrDesc & attrDesc1,
		   const AttrDesc & attrDesc2);

/*
 * Joins two relations.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

// implementation of nested loops join goes here
const Status QU_NL_Join(const string & result, 
		     const int projCnt, 
		     const attrInfo projNames[],
		     const attrInfo *attr1, 
		     const Operator op, 
		     const attrInfo *attr2)
{
    Status status;
    AttrDesc attrecord,attres,ad;
    AttrDesc  temp[projCnt];
    cout << "Doing NL Join" << endl;
    InsertFileScan *ifs;
   HeapFileScan  *out_hfs;  
   HeapFileScan  *in_hfs;  
   Record  rec,record,recd;
   RID rid,Rid,tid;
   int tempi;
   float tempf;
   char  tempc;
    int len=0;
    int j=0,k=0;
   out_hfs=new  HeapFileScan(attr1->relName,status);
   if(status!=OK)
	  	return status;
   ifs=new  InsertFileScan(result,status);
	if(status!=OK)
  		return status;
	for(int i=0;i<projCnt;i++)
	{
		 if((status=attrCat->getInfo(projNames[i].relName, projNames[i].attrName,ad))!=OK)
	    	return status;
		len+=ad.attrLen;	
		temp[i]=ad;
	}
	char data[len];
  	if((status=attrCat->getInfo(attr1->relName, attr1->attrName,attres))!=OK)
	    return status;
//	    cout<<"attr1->relName:         "<<attr1->relName<<endl;
//	   	cout<<"attr1->attrName:         "<<attr1->attrName<<endl; 
//	   	cout<<"attr1->attrValue:         "<<attr1->attrValue<<endl;  
//	   	cout<<"attres.relName:         "<<attres.relName<<endl;    
//	   	cout<<"attres.attrName:         "<<attres.attrName<<endl; 
//	   	cout<<"attres.attrOffset:         "<<attres.attrOffset<<endl;   
//	   	cout<<"attres. attrType:         "<<attres. attrType<<endl;  
//	   	cout<<"attres.attrLen:         "<<attres.attrLen<<endl;  
	if((status=attrCat->getInfo(attr2->relName, attr2->attrName, attrecord))!=OK)
	      		return status;
//	   cout<<"-------------------------------------------------------------"<<endl;    		
//	     cout<<"attr2->relName:         "<<attr2->relName<<endl;
//	   	cout<<"attr2->attrName:         "<<attr2->attrName<<endl;    
//	   	cout<<"attrecord.relName:         "<<attrecord.relName<<endl;    
//	   	cout<<"attrecord.attrName:         "<<attrecord.attrName<<endl; 
//	   	cout<<"attrecord.attrOffset:         "<<attrecord.attrOffset<<endl;   
//	   	cout<<"attrecord. attrType:         "<<attrecord. attrType<<endl;  
//	   	cout<<"attrecord.attrLen:         "<<attrecord.attrLen<<endl;  
   if((status=out_hfs->startScan(0, 0,  STRING,NULL, EQ))!=OK)
   		return status;
	while((status=out_hfs->scanNext(rid))!= FILEEOF)
	 {      
	 	   k++;
	 	   in_hfs=new  HeapFileScan(attr2->relName,status);
            if(status!=OK)
	  			return status;
	 	   if((status=out_hfs->getRecord(rec))!=OK)
	      		return status;
	      	
	      	 switch(attres.attrType)
		    {
		    case INTEGER:
		      memcpy(&tempi, (char *)rec.data + attres.attrOffset, attres.attrLen);	
	        if((status=in_hfs->startScan( attrecord.attrOffset,attrecord.attrLen, INTEGER,(char *) &tempi ,op))!=OK)
  			    	return status;
  			    break; 	
		    case FLOAT:
	    	 memcpy(&tempf, (char *)rec.data + attres.attrOffset, attres.attrLen);	
		     if((status=in_hfs->startScan( attrecord.attrOffset,attrecord.attrLen, FLOAT,(char *) &tempf,op))!=OK)
  			    	return status;
		       break;
		    case STRING:
	     	 memcpy(&tempc, (char *)rec.data + attres.attrOffset, attres.attrLen);	
	         if((status=in_hfs->startScan( attrecord.attrOffset,attrecord.attrLen, STRING,(char *)&tempc,op))!=OK)
			    	return status;
  			     break;
		    }
			 
	 	 while((status=in_hfs->scanNext(Rid))!= FILEEOF)
	 	 {   
	 	 		 j++;
	 	 	  if((status=in_hfs->getRecord(record))!=OK)
	      		return status;
	      	 int length=0; 
 	    	 for(int i=0;i<projCnt;i++)
 	    	 {
 	    	 if( strcmp(projNames[i].relName,attr1->relName)==0)	
 	    	   memcpy(&data[ length],(char *)rec.data+temp[i].attrOffset,temp[i].attrLen);	 	    	   	    	      
 	    	   if( strcmp(projNames[i].relName,attr2->relName)==0)
 	    	   memcpy(&data[length],(char *)record.data+temp[i].attrOffset,temp[i].attrLen);        
 	    	   length=length+temp[i].attrLen;	
 	    	}	 	
	 	    	recd.data=&data;
			    recd .length=len;
			    if((status=ifs->insertRecord(recd, tid))!=OK)
			          return status;						      			       	 	
	 	}   
	 	
	      delete  in_hfs;
	  
	}
// 	 cout<<"-------------k"<<k<<endl; 
//	 cout<<"-------------j"<<j<<endl;    
    delete out_hfs; 
    delete  ifs;
   return OK;
}

// implementation of sort merge join goes here
const Status QU_SM_Join(const string & result, 
		     const int projCnt, 
		     const attrInfo projNames[],
		     const attrInfo *attr1, 
		     const Operator op, 
		     const attrInfo *attr2)
{
    Status status;
    cout << "Doing SM Join" << endl;
    // part 5

    return OK;
}


// implementation of hash join goes here
const Status QU_Hash_Join(const string & result, 
		     const int projCnt, 
		     const attrInfo projNames[],
		     const attrInfo *attr1, 
		     const Operator op, 
		     const attrInfo *attr2)
{
    Status status;
    cout << "Doing Hash Join" << endl;
    // part5

    return OK;
}

const Status QU_Join(const string & result, 
		     const int projCnt, 
		     const attrInfo projNames[],
		     const attrInfo *attr1, 
		     const Operator op, 
		     const attrInfo *attr2)
{

  if ((JoinMethod == NLJoin) || ((JoinMethod == HashJoin) && (op != EQ)))
  {
	return QU_NL_Join (result, projCnt, projNames, attr1, op, attr2);
  }
  else
  if (JoinMethod == SMJoin)
  {
	return QU_SM_Join (result, projCnt, projNames, attr1, op, attr2);
  }
  else return QU_Hash_Join (result, projCnt, projNames, attr1, op, attr2);
}



const int matchRec(const Record & outerRec,
		   const Record & innerRec,
		   const AttrDesc & attrDesc1,
		   const AttrDesc & attrDesc2)
{
  int tmpInt1, tmpInt2;
  float tmpFloat1, tmpFloat2;

  switch(attrDesc1.attrType)
    {
    case INTEGER:
      memcpy(&tmpInt1, (char *)outerRec.data + attrDesc1.attrOffset, sizeof(int));
      memcpy(&tmpInt2, (char *)innerRec.data + attrDesc2.attrOffset, sizeof(int));
      return tmpInt1 - tmpInt2;

    case FLOAT:
      memcpy(&tmpFloat1, (char *)outerRec.data + attrDesc1.attrOffset, sizeof(float));
      memcpy(&tmpFloat2, (char *)innerRec.data + attrDesc2.attrOffset, sizeof(float));
      return int(tmpFloat1 - tmpFloat2);

    case STRING:
      return strcmp((char *)outerRec.data + attrDesc1.attrOffset, 
		    (char *)innerRec.data + attrDesc2.attrOffset);
    }

  return 0;
}
