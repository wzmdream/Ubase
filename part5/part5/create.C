#include "catalog.h"


const Status RelCatalog::createRel(const string & relation, 
				   const int attrCnt,
				   const attrInfo attrList[])
{
  Status status;
  RelDesc rd;
  AttrDesc ad;


  if (relation.empty() || attrCnt < 1)
    return BADCATPARM;

  if (relation.length() >= sizeof rd.relName)
    return NAMETOOLONG;

  status = getInfo(relation,rd);
  if(status==OK)  return RELEXISTS;
  if((status!=OK) && (status!=RELNOTFOUND)) return status;

  //check duplicated attributes
  bool duplicated = false;
  for(int i=0; i<attrCnt-1; i++)
  {
    for(int j=i+1;j<attrCnt;j++)
    {
      if (strncmp(attrList[i].attrName,attrList[j].attrName,MAXNAME)==0)
      {
        duplicated=true;
        break;
      }
    }
  }
  if (duplicated==true)
    return DUPLATTR;

  //add catalog entry into relcat
  strcpy(rd.relName,relation.c_str());
  rd.attrCnt = attrCnt;
  status = addInfo(rd);
  if (status!=OK) return status;

  //add catalog entrys into attrcat
  int Offset = 0;
  for (int i=0; i<attrCnt;i++)
  {
    strcpy(ad.relName,attrList[i].relName);  // relation name
    strcpy(ad.attrName,attrList[i].attrName); // attribute name
    ad.attrOffset = Offset; // attribute offset
    ad.attrType = attrList[i].attrType; // attribute type
    ad.attrLen = attrList[i].attrLen; // attribute lengt
    status = attrCat->addInfo(ad);
    Offset +=attrList[i].attrLen;
    if (status!=OK) return status;
  }

  //create heap file
  status = createHeapFile(relation);
  if (status!=OK) return status;
  if(relation!="Tmp_Ubase_Result")
  {
    cout << endl;
    cout << "table " << relation << " created!" << endl;
  }
  return OK;
}
