#define SFDB_SIGNATURE 0x42444653 // 'BDFS' => SFDB

typedef struct tag_SFDB_HDR {
	tDWORD dwSignature;			// 'SFDB'
	tVERSION vDBVersion;
	tBYTE  byFileCheckSum[16];	// MD5 checksum to check file integrity
	tDWORD dwIndexBits;			// number of bits used as index part;
	tDWORD dwIndexMask;			// mask to extract cluster index; 
	tDWORD dwClusters;			// number of clusters in database
	
	tDWORD dwHeaderSize;		// sizeof(SFDB_HDR) rounded to dwCusterSize
	tDWORD dwFileSize;			// total size of file
	
	tDWORD dwClusterSize;		// equal to system memory page size
	tDWORD dwClusterHeaderSize;	// cluster header size in bytes
	
	tDWORD dwRecordIDAlg;		// algorithm used to identify record (to generate IDs)
	tDWORD dwRecordIDSize;		// ID size in bytes used to identify record
	tDWORD dwRecordDataSize;	// record data size in bytes
	tDWORD dwRecordSize;		// record size in bytes
	tDWORD dwRecordsPerCluster;	// number of records per custer
	tDWORD dwActiveClients;		// number of clients currently connected to database
	tDWORD dwSerializeOptions;  
	
} SFDB_HDR, *PSFDB_HDR;

typedef struct tag_SFDB_CLUSTER_HDR {
	tDWORD dwRecords;			// number of records in cluster
} SFDB_CLUSTER_HDR, *PSFDB_CLUSTER_HDR;

typedef struct tag_SFDB_RECORD_SESSION_DATA {
	tDWORD dwUsageCount;			// number of records in cluster
} SFDB_RECORD_SESSION_DATA, *PSFDB_RECORD_SESSION_DATA;

typedef struct tag_SFDB_RECORD_INTERNAL_DATA {
	tDWORD dwLastUsedDate;			// number of records in cluster
} SFDB_RECORD_INTERNAL_DATA, *PSFDB_RECORD_INTERNAL_DATA;

