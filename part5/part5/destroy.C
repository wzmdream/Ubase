#include "catalog.h"
#include "error.h"


//
// Destroys a relation. It performs the following steps:
//
// 	removes the catalog entry for the relation
// 	destroys the heap file containing the tuples in the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::destroyRel(const string & relation)
{
  Status status;

  if (relation.empty() || 
      relation == string(RELCATNAME) || 
      relation == string(ATTRCATNAME))
    return BADCATPARM;

  //destroys the heap file containing the tuples in the relation
  status = destroyHeapFile(relation);
  if (status != OK) return status;

  //get relation name(also file name) from catalog(relcat)
  RelDesc record;

  status = getInfo(relation,record);
  if (status == FILEEOF) return RELNOTFOUND;
  if ((status != OK) && (status != FILEEOF)) return status;

  //remove the catalog entry of the relation from attrcat
  status = attrCat->dropRelation(relation);
  if (status != OK) return status;

  //remove the catalog entry of the relation from relcat
  status = removeInfo(relation);
  if (status != OK) return status;
  if(relation!="Tmp_Ubase_Result")
  {
    cout << endl;
    cout << "relation " << relation << " dropped successfully!" << endl;
  }

  return OK;
}


//
// Drops a relation. It performs the following steps:
//
// 	removes the catalog entries for the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status AttrCatalog::dropRelation(const string & relation)
{
  Status status;
  AttrDesc *attrs;
  int attrCnt;

  if (relation.empty())
    return BADCATPARM;

  //get attributes infomation of the relation
  status = getRelInfo(relation,attrCnt,attrs);
  if (status != OK) return status;

  //removes the catalog entries in attrcat of the relation
  for (int i=0; i < attrCnt; i++)
  {
    status = removeInfo(relation,attrs[i].attrName);
    if (status != OK) return status;
  }
  delete [] attrs;
  return OK;
}
