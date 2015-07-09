#include <unistd.h>
#include <fcntl.h>
#include "catalog.h"
#include "utility.h"

//
// Loads a file of (binary) tuples from a standard file into the relation.
// Any indices on the relation are updated appropriately.
//
// Returns:
// 	OK on success
// 	an error code otherwise
//

const Status UT_Load(const string & relation, const string & fileName)
{
    Status status;
    RelDesc rd;
    AttrDesc *attrs;
    int attrCnt;

    if (relation.empty() || fileName.empty() || relation == string(RELCATNAME) || relation == string(ATTRCATNAME))
        return BADCATPARM;

    // open Unix data file
    int fd;
    if ((fd = open(fileName.c_str(), O_RDONLY, 0)) < 0)
        return UNIXERR;












    // close Unix file
    if (close(fd) < 0)
        return UNIXERR;
    else
        return OK;
}
