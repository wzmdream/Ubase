#include <sys/types.h>
#include <functional>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "error.h"
#include "utility.h"
#include "catalog.h"

// define if debug output wanted


//
// Retrieves and prints information from the catalogs about the for the
// user. If no relation is given (relation is NULL), then it lists all
// the relations in the database, along with the width in bytes of the
// relation, the number of attributes in the relation, and the number of
// attributes that are indexed.  If a relation is given, then it lists
// all of the attributes of the relation, as well as its type, length,
// and offset, whether it's indexed or not, and its index number.
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::help(const string & relation)
{
  Status status;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;

  if (relation.empty()) //no relation specified
  {
    status = UT_Print(RELCATNAME);
    if (status != OK) return status;
  }
  else // relation specified
  {
    //check relcat to make sure the relation exists
    status = relCat->getInfo(relation,rd);
    if (status == FILEEOF) return RELNOTFOUND;
    if ((status != OK) && (status != FILEEOF)) return status;

    //check attrcat to make sure the relation attributes entry exists
    status = attrCat->getRelInfo(relation,attrCnt,attrs);
    if (status!=OK) return status;

    //print header of the relation
    printf("%-*.*s  ", 20, 20, "Table Name");
    printf("%-*.*s  ", 20, 20 ,"Attribute Name");
    printf("%-*.*s  ", 6, 6, "Offset");
    printf("%-*.*s  ", 6, 6 ,"Type");
    printf("%-*.*s  ", 6, 6, "Length");
    printf("\n");
    printf("%-*.*s  ", 20, 20, "--------------------");
    printf("%-*.*s  ", 20, 20, "--------------------");
    printf("%-*.*s  ", 6, 6, "------");
    printf("%-*.*s  ", 6, 6, "------");
    printf("%-*.*s  ", 6, 6, "------");
    printf("\n");

    //print tuples
    char * att_type;
    for(int i = 0; i < attrCnt; i++)
    {
      printf("%-*.*s  ", 20, 20, attrs[i].relName);
      printf("%-*.*s  ", 20, 20, attrs[i].attrName);
      printf("%-*d  ",  6, attrs[i].attrOffset);
      switch (attrs[i].attrType)
      {
        case 0 : 
          att_type="string";
          break;
        case 1 :
          att_type="int";
          break;
        case 2 :
          att_type="float";
          break;
      }
      printf("%-*.*s  ",6,  6, att_type);
      printf("%-*d  ",  6, attrs[i].attrLen);
      printf("\n");
/*
      cout << setw(32) << attrs[i].relName;
      cout << attrs[i].attrName;
      cout << attrs[i].attrOffset;
      cout << attrs[i].attrType;
      cout << attrs[i].attrLen;
      cout << endl;
*/
    }
    delete [] attrs;
  }

  return OK;
}
