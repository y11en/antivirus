#ifndef _ANTIPHISHING_DB_
#define _ANTIPHISHING_DB_

#define ANTIPHISHING_DB_SIGNATURE "APDB"

typedef struct tag_ANTIPHISHING_DB {
	unsigned char signature[4];
	unsigned long header_size;
	unsigned long total_db_size;
	long          timestamp; // time_t
	unsigned long hashes_count;
	unsigned long hashes_offset;
	unsigned long hashes_size;
	unsigned long wildcards_count;
	unsigned long wildcards_offset;
	unsigned long wildcards_size;
} tANTIPHISHING_DB;

#endif // _ANTIPHISHING_DB_