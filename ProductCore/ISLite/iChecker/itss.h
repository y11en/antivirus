typedef struct _ITSFileHeader
{
    tDWORD uMagic;          // Should be set to MAGIC_ITS_FILE;
    tDWORD uFormatVersion;  // Version number for file format
    tDWORD cbHeaderSize;    // Size of this header in bytes.
    tDWORD fFlags;          // State flag bits for the ITS file.
    tDWORD dwStamp;         // Content Version; changed each write.
    LCID     lcid;            // Locale Id for file.
    CLSID    clsidFreeList;   // Class id for the free list manager.
    CLSID    clsidPathMgr;    // Class id for the path manager manager.
    tQWORD   offFreeListData; // Offset in bytes to free list data
    tQWORD   cbFreeListData; // Size of free list data in bytes
    tQWORD   offPathMgrData;  // Offset in bytes to Path Manager data
    tQWORD   cbPathMgrData;  // Size of Path Manager data in bytes
    tQWORD   offPathMgrOrigin;// Coordinate origin offset wrapped around path manager offsets
	
} ITSFileHeader;

#define ITSFILE_HEADER_MAGIC 'I' | ('T' << 8) | ('S' << 16) | ('F' << 24) 

typedef struct _FreeListHeader
{
    tDWORD cItemsMax;
    tDWORD cItemsActual;
    tQWORD cbSpace;
    tQWORD cbLost;
	
} FreeListHeader;

typedef struct _freeitem
{
    tQWORD ullOffset; // Start of free block
    tQWORD ullCB;     // Size  of free block in bytes
    
} FREEITEM;


typedef struct _DatabaseHeader
{
    tDWORD uiMagic;           // ID value "ITSP" 
    tDWORD uiVersion;         // Revision # for this structure
    tDWORD cbHeader;          // sizeof(DatabaseHeader);
    tDWORD cCacheBlocksMax;   // Number of cache blocks allowed 
    tDWORD cbDirectoryBlock;  // Size of a directory block
    tDWORD cEntryAccessShift; // Base 2 log of Gap in entry access vector
    tDWORD cDirectoryLevels;  // Nesting depth from root to leaf
    tDWORD iRootDirectory;    // The top most internal directory
    tDWORD iLeafFirst;        // Lexically first leaf block
    tDWORD iLeafLast;         // Lexically last  leaf block
    tDWORD iBlockFirstFree;   // First block in unused block chain
    tDWORD cBlocks;           // Number of directory blocks in use
    LCID  lcid;              // Locale id for sorting and comparision rules)
    CLSID clsidEntryHandler; // Interface which understands node entries
    tDWORD  cbPrefix;          // Size of fixed portion of data base
    tDWORD iOrdinalMapRoot;   // Ordinal map root when they don't fit in a block
    tDWORD iOrdinalMapFirst;  // First and last Ordinal map for leaf blocks. 
    tDWORD iOrdinalMapLast;   // These are linked like the leaf blocks.
	
    // Note instance data for the Entry handler immediately follows the 
    // database header. That data is counted by cbPrefix.
	
} DatabaseHeader;

#define ITSDB_HEADER_MAGIC 'I' | ('T' << 8) | ('S' << 16) | ('P' << 24) 

typedef struct _NodeHeader
{
    tDWORD uiMagic;  // A type tag to mark leafs and internal nodes.
    tDWORD cbSlack;  // Trailing free space in the block.
	
} NodeHeader;

#define ITSBLOCK_LEAF_MAGIC ('P' | ('M' << 8) | ('G' << 16) | ('L' << 24)) //for Leaf blocks.
#define ITSBLOCK_INTERNAL_MAGIC ('P' | ('M' << 8) | ('G' << 16) | ('I' << 24)) //for Internal blocks.
#define ITSBLOCK_UNUSED_MAGIC 'P' | ('M' << 8) | ('G' << 16) | ('U' << 24) //for unused blocks


typedef struct _LeafChainLinks
{
    tDWORD iLeafSerial;
    tDWORD iLeafPrevious;  
    tDWORD iLeafNext;
		
} LeafChainLinks;

typedef struct _sHeaderInfo
{
    tDWORD  dwVerInfo;
    tDWORD  cRecs;
    tDWORD  cbRecSize;
    tDWORD  cbSize;
    tQWORD  uliVSpaceSize;
    tQWORD  uliTxSpaceSize;
    tDWORD  ulBlockSize;
    tDWORD  unused;
} sHeader;

typedef struct _sResetRecordV1
{
    tQWORD  uliVOffset;
    tQWORD  uliXOffset;
} sResetRecV1;

typedef struct _sResetRecord
{
    tQWORD  uliXOffset;
} sResetRec;

typedef struct _LZX_Control_Data
{
    tDWORD  cdwControlData;
    tDWORD dwLZXMagic;
    tDWORD dwVersion;
    tDWORD dwMulResetBlock;
    tDWORD dwMulWindowSize;
    tDWORD dwMulSecondPartition;
    tDWORD dwOptions;
	
} LZX_Control_Data, *PLZX_Control_Data;

#define  LZX_MAGIC = 'L' | ('Z' << 8 ) | ('X' << 16) | ('C' << 24)

typedef struct _LockStorageHeader
{
    tDWORD SigApplication;
    tDWORD SigLockStorage;
    tDWORD VerNoLockStorage;
    tDWORD cbLockStorageHeader;
    tDWORD cSubAllocSlots; 
    tDWORD cbApplicationHeader;
    CLSID clsidFreeListMgr;
	
} LockStorageHeader;

typedef struct _SubAllocDesc 
{
    tQWORD ullcbOffset;       
    tQWORD ullcbData;
	
} SubAllocDesc;

enum {
	SLOT_KEY_SEG_CACHE  = 1,
	SLOT_ORDMAP_SEG_CACHE   = 2,
	SLOT_KEY_HANDLER_IMAGE  = 3,
	SLOT_DATA_HANDLER_IMAGE = 4,
};

typedef struct _BTreeControl
{
    tQWORD iRootDirectory;
    tQWORD iLeafFirst;
    tQWORD iLeafLast;
    tDWORD  cbInfo;
    tDWORD  fAlignment;
    tDWORD  cbDirectoryBlock;
    tDWORD  cEntryAccessShift;
    tBOOL   fDirty;
    tDWORD  cDirectoryLevels;
    tDWORD  iSetSerial;
    tQWORD cEntryCount;
	
} BTreeControl, *PBTreeControl;



typedef struct _AppDatabaseHeader
{
    tDWORD  ulVersion;
    tDWORD  cbHeader;
	
    BTreeControl BTCKeys;
    BTreeControl BTCOrdinals;
	
    tDWORD cbKeyTreeCacheLimit;
    tDWORD cbOrdTreeCacheLimit;
	
    tBOOL fDefaultDataHandling;
    tBOOL fOrdinalMapObsolete;
} AppDatabaseHeader, *PAppDatabaseHeader;

typedef struct _CTRL_DATA_IITAssociationOrderedList
{
    tDWORD ulSignature;
    tDWORD ulVersion;
    tDWORD cbStructure;
	
    tDWORD KeySetID;
    tDWORD OrdMapSetID;
    tDWORD cbBTreeBlockKeySet;
    tDWORD cbBTreeBlockOrdMapSet;
    tDWORD cbBTreeBlockCacheLimitKeySet;
    tDWORD cbBTreeBlockCacheLimitOrdMapSet;
    tDWORD cbBTreeBlockInitialSlackSpaceKeySet;
    tDWORD cbBTreeBlockInitialSlackSpaceOrdMapSet;
	
} CTRL_DATA_IITAssociationOrderedList, *PCTRL_DATA_IITAssociationOrderedList;



typedef struct _ITSFileHeaderInfo
{
    tDWORD     uMagic;
    tDWORD     uFormatVersion;
    tDWORD     cbHeaderSize;
    tDWORD     fFlags;
    tQWORD     offPathMgrOrigin;
    tDWORD    dwStamp;
    LCID     lcid;
	
} ITSFileHeaderInfo, *PITSFileHeaderInfo;

typedef struct _ITSPathControlHeader
{
    CTRL_DATA_IITAssociationOrderedList CDAOL;
	
    ITSFileHeaderInfo ITSFHI;
	
} ITSPathControlHeader, *PITSPathControlHeader;



typedef struct _CMHeader
{
    tDWORD  ulCMSignature;
    tDWORD  ulVersion;
    tDWORD  cbBlockSize;
    tDWORD  cbCacheSize;
    tQWORD ullFirstFreeBlock;
    tQWORD ullBlocksAllocated;
	
} CMHeader, *PCMHeader;

typedef struct _BTNodeHeader
{
    tDWORD   uiMagic;
    tDWORD   cbSlack;
    tQWORD ullBlockID;
	
} BTNodeHeader, *PBTNodeHeader;

#define ITSNODE_LEAF_MAGIC  'A' | ('O' << 8) | ('L' << 16) | ('L' << 24) // Leaf Node Signature
#define ITSNODE_INTERNAL_MAGIC 'A' | ('O' << 8) | ('L' << 16) | ('I' << 24) // Internal Node Signature


typedef struct _BTLeafChainLinks
{
    tQWORD idLeafPrevious;
    tQWORD idLeafNext;
    tQWORD cEntriesPrev;
    tDWORD iLeafSerial;
	
} BTLeafChainLinks, *PBTLeafChainLinks;

typedef struct _tag_ITSS41Data_
{
	DWORD offContent;
	DWORD cbContent;
	DWORD offControlData;
	DWORD cbControlData;
	DWORD offResetTable;
	DWORD cbResetTable;
}ITSSData;


void DumpPaths(tBYTE* pData, tDWORD dwSize, ITSSData* pITSSData);
void CheckNameSpace(tUINT Counter, tBYTE* pPointer, tBYTE* pEnd, ITSSData* pData);
tDWORD DecodePacked32Value(tBYTE **ppb);

