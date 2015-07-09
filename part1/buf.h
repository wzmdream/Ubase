#ifndef BUF_H
#define BUF_H

#include "db.h"

// define if debug output wanted
//#define DEBUGBUF

// declarations for buffer pool hash table
struct hashBucket
{
    File* file;        // pointer a file object (more on this below)
    int	pageNo;        // page number within a file
    int	frameNo;       // frame number of page in the buffer pool
    hashBucket* next;  // next node in the hash table
};
// 缓存池统计结构的声名（爱憎算法和时钟算法都可以使用） 

struct BufStats 
{ 
	int pinnedFrame; // 缓存池中钉住的帧数,当钉住的帧数等于缓存池的总帧数时,表示缓存池已满(及pinnedFrame==numBufs时,再分配新帧应当返回BUFFEREXCEEDED)
	int hit; // readPage()方法命中次数,要读入的页已经在缓存池中 
	int miss; // readPage()方法未命中次数，要读入的页不在缓存池中 
};

// hash table to keep track of pages in the buffer pool
class BufHashTbl
{
    private:

    int HTSIZE;
    hashBucket**  ht; // actual hash table
    int	 hash(const File* file, const int pageNo); // returns value between 0 and HTSIZE-1

    public:

    BufHashTbl(const int htSize);  // constructor
    ~BufHashTbl(); // destructor
	
    // insert entry into hash table mapping (file,pageNo) to frameNo;
    // returns 0 if OK, HASHTBLERROR if an error occurred
    Status insert(const File* file, const int pageNo, const int frameNo);

    // Check if (file,pageNo) is currently in the buffer pool (ie. in
    // the hash table).  If so, return corresponding frameNo. else return 
    // HASHNOTFOUND
    Status lookup(const File* file, const int pageNo, int & frameNo);

    // delete entry (file,pageNo) from hash table. REturn OK if page was
    // found.  Else return HASHTBLERROR
    Status remove(const File* file, const int pageNo);  
};


class BufMgr;  //forward declaration of BufMgr class 

// class for maintaining information about buffer pool frames
class BufDesc {
    friend class BufMgr;
    private:

    File*    file;     // pointer to file object
    int      pageNo;   // page within file
    int	     frameNo;  // frame # of frame
    int      pinCnt;   // number of times this page has been pinned
    bool     dirty;    // true if dirty;  false otherwise
    bool     valid;    // true if page is valid
    bool     refbit; // 访问标记，每当调用缓存管理器的readPage()方法访问某个 缓存帧后，该帧对应的访问标记将设置为true 

    void Clear()
    {  // initialize buffer frame for a new user
        pinCnt = 0;
        file = NULL;
        pageNo = -1;
        dirty = false;
        valid = false;
        refbit = false;
    };

    BufDesc()
    {
        Clear();
    }

    void Set(File* filePtr, const int pageNum)
    { 
        file = filePtr;
        pageNo = pageNum;
        pinCnt = 1;
        dirty = false;
        valid = true;
        refbit = true;
    }

};

class BufMgr 
{
    private:

    int clockHand; // Clock hand for clock algorithm 
    int         numBufs;    // Number of pages in buffer pool
    BufHashTbl* hashTable;  // hash table mapping (File, page) to frame
    BufDesc*    bufTable;   // vector of status info, 1 per page
    BufStats bufStats;

    const Status allocBuf(int & frame);   // allocate a free frame.  

    const void releaseBuf(int frame);     // return unused frame to end of list

    public:

    Page*	         bufPool;   // actual buffer pool

    BufMgr(const int bufs);
    ~BufMgr();
    //时钟前进一步 
    void advanceClock() 
    { 
    	clockHand = (clockHand + 1) % numBufs; 
    } 


    void BufDump();

    const Status readPage(File* file, const int PageNo, Page*& page);

    const Status unPinPage(const File* file, const int PageNo, const bool dirty);

    const Status allocPage(File* file, int& PageNo, Page*& page); // allocates a new, empty page 

    const Status flushFile(const File* file); // writing out all dirty pages of the file

    const Status disposePage(File* file, const int PageNo); // dispose of page in file

    void  printSelf();
    void listUnpinned();
};

#endif
