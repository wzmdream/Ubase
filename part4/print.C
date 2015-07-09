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
      attrWidth[i] = MIN(MAX(namelen, attrs[i].attrLen), 20);
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
  for(int i = 0; i < attrCnt; i++) {
    char *attr = (char *)rec.data + attrs[i].attrOffset;
    switch(attrs[i].attrType) {
    case INTEGER:
      int tempi;
      memcpy(&tempi, attr, sizeof(int));
      printf("%-*d  ", attrWidth[i], tempi);
      break;
    case FLOAT:
      float tempf;
      memcpy(&tempf, attr, sizeof(float));
      printf("%-*.2f  ", attrWidth[i], tempf);
      break;
    default:
      printf("%-*.*s  ", attrWidth[i], attrWidth[i], attr);
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
















}
