#include <stdio.h>
#include "catalog.h"
#include "utility.h"


#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))


//
// Compute widths of columns in attribute array.
//

const Status UT_computeWidth(const int attrCnt, 
			     const AttrDesc attrs[], 
			     int *&attrWidth)
{
  attrWidth = new int [attrCnt];
  if (!attrWidth)
    return INSUFMEM;

  for(int i = 0; i < attrCnt; i++) {
    int namelen = strlen(attrs[i].attrName);
    switch(attrs[i].attrType) {
    case INTEGER:
    case FLOAT:
      attrWidth[i] = MIN(MAX(namelen, 5), 7);
      break;
    case STRING:
      attrWidth[i] = MIN(MAX(namelen, attrs[i].attrLen), 30);
      break;
    }
  }

  return OK;
}


//
// Prints values of attributes stored in buffer pointed to
// by recPtr. The desired width of columns is in attrWidth.
//

void UT_printRec(const int attrCnt, const AttrDesc attrs[], int *attrWidth,
		 const Record & rec)
{
  for(int i = 0; i < attrCnt; i++)
  {
    //char *attr = (char *)rec.data + attrs[i].attrOffset;
    //char temp[attrs[i].attrLen];
    switch(attrs[i].attrType)
    {
    case INTEGER:
      int tempi;
      memcpy(&tempi, (char *)rec.data + attrs[i].attrOffset, sizeof(int));
      printf("%-*d  ", attrWidth[i], tempi);
      break;
    case FLOAT:
      float tempf;
      memcpy(&tempf, (char *)rec.data + attrs[i].attrOffset, sizeof(float));
      printf("%-*.2f  ", attrWidth[i], tempf);
      break;
    default:
      string str1((char *)rec.data + attrs[i].attrOffset,attrs[i].attrLen);
      //memcpy(temp, (char *)rec.data + attrs[i].attrOffset,attrs[i].attrLen);
      //temp[attrs[i].attrLen+1] = '\0';
      printf("%-*.*s  ", attrWidth[i], attrWidth[i], str1.c_str());
      break;
    }
  }

  printf("\n");
}


//
// Prints the contents of the specified relation.
//
// Returns:
// 	OK on success
// 	an error code otherwise
//

const Status UT_Print(string relation)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;

  if (relation.empty()) relation = RELCATNAME;

  int *attrWidth;
  RID rid;
  Record rec;
  HeapFileScan *hfs;

  //check relcat to make sure the relation exists
  status = relCat->getInfo(relation,rd);
  if (status == FILEEOF) return RELNOTFOUND;
  if ((status != OK) && (status != FILEEOF)) return status;

  //check attrcat to make sure the relation attributes entry exists
  status = attrCat->getRelInfo(relation,attrCnt,attrs);
  if (status!=OK) return status;

  status = UT_computeWidth(attrCnt,attrs,attrWidth);
  if (status!=OK) return status;

  //print header of the relation
  cout << endl;
  for (int i=0; i<attrCnt;i++)
  {
    printf("%-*.*s  ", attrWidth[i], attrWidth[i], attrs[i].attrName);
  }
  printf("\n");
  for (int i=0; i<attrCnt;i++)
  {
    string str1(attrWidth[i] , '-' );    
    printf("%-*.*s  ", attrWidth[i], attrWidth[i], str1.c_str());
  }
  printf("\n");


  hfs = new HeapFileScan(relation,status);
  if (status != OK) return status;

  hfs->startScan(0,0,STRING,NULL,EQ);
  if (status != OK) return status;

  //print tuples
  int j = 0;
  while ((status=hfs->scanNext(rid)) != FILEEOF)
  {
    if (status != OK) return status;
    status=hfs->getRecord(rec);

    if (status != OK) return status;
    UT_printRec(attrCnt, attrs, attrWidth,rec);
    j=j++;
  }
  cout << endl;
  cout << j << " record(s) selected!" << endl;

  hfs->endScan();
  delete hfs;
  delete [] attrs;

  return OK;
}
