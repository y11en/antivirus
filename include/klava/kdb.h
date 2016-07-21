// klava/kdb.h
//
// KLAVA DB structures
//

#ifndef klava_kdb_h_INCLUDED
#define klava_kdb_h_INCLUDED

#include <kl_types.h>
#include <klava/kdb_ids.h>

////////////////////////////////////////////////////////////////
#include <kl_pushpack.h>

////////////////////////////////////////////////////////////////
// Verdict table (KDC)

typedef struct tagKLAV_KDC_Verdict
{
	uint32_t vd_type_flags;
	uint32_t vd_name_ref;
} KL_PACKED KLAV_KDC_Verdict;

////////////////////////////////////////////////////////////////
// Signature table (KDC)

typedef struct tagKLAV_KDC_Signature
{
	uint32_t sig_evid;
	uint32_t sig_data;
} KL_PACKED KLAV_KDC_Signature;

// Signature event table

typedef struct tagKLAV_Sig_Event_Record
{
	uint32_t sig_evid;        // event ID
	uint32_t sig_evid_next;   // idx of the next record for the same signature
} KL_PACKED KLAV_Sig_Event_Record;

////////////////////////////////////////////////////////////////
// Action table v2

// KDC format
typedef struct tagKLAV_KDC_Action_Record
{
	uint32_t m_evid;
	uint32_t m_strid;
	uint32_t m_type;
} KL_PACKED KLAV_KDC_Action_Record;

typedef struct tagKLAV_ACT2_Record
{
	uint32_t act_name_id;
	uint32_t act_event_cnt;  // event IDs follow
} KL_PACKED KLAV_ACT2_Record;

////////////////////////////////////////////////////////////////
// AVP3 ported records

// FILE/SECTOR/CA/UNPACK/EXTRACT detection record
typedef struct tagKLAV_R3_Detect
{
// part one: 'short'
	uint8_t  avp_len1;
	uint16_t avp_off1;
	uint16_t avp_cword;   // control word
// part two: 'cutted'
	uint8_t  avp_subtype; // subtype
	uint32_t avp_sum1;
	uint8_t  avp_len2;
	uint16_t avp_off2;
	uint32_t avp_sum2;
	uint32_t avp_name;    // index in RECORD_NAME section
	uint32_t avp_link;    // index in AVP3 link table
	uint32_t avp_cure;    // index in CURE table
} KL_PACKED KLAV_R3_Detect;

// SCAN1 part (6 bytes)
// Note: last record is 'terminating guard record' with MASK=CWORD=0
typedef uint32_t KLAV_R3_Scan1;

// SCAN2 part (24 bytes)
// Note: last record is 'terminating guard record' with NAME=0
typedef struct tagKLAV_R3_Scan2
{
	uint8_t  avp_len1;
	uint8_t  avp_type;
	uint16_t avp_link;		// index in AVP3 link table
	uint32_t avp_sum1;
} KL_PACKED KLAV_R3_Scan2;

// SCAN2 part (24 bytes)
// Note: last record is 'terminating guard record' with NAME=0
typedef struct tagKLAV_R3_Scan3
{
	uint16_t avp_off2;
	uint16_t avp_cure;		// index in CURE table
	uint32_t avp_len2 : 8;
	uint32_t avp_name : 24; // index in RECORD_NAME section (0 for temninating record)
	uint32_t avp_sum2;
} KL_PACKED KLAV_R3_Scan3;

// record count by subtypes
typedef struct tagKLAV_R3_ScanCnt
{
	uint32_t avp_rcnt [8]; // COUNT(1 << (0..7))
} KL_PACKED KLAV_R3_ScanCnt;

// FILE cure record
typedef struct tagKLAV_R3_Cure
{
	uint32_t avp_cure_link;
	uint8_t  avp_cure_method;
	uint8_t  avp_cure_method2;
	uint16_t avp_cure_data [5];
} KL_PACKED KLAV_R3_Cure;

typedef struct tagKLAV_R3_Jump
{
	uint8_t  avp_len1;
	uint16_t avp_cword;
	uint8_t  avp_jmp_method;
	uint32_t avp_bitmask1;
	uint32_t avp_bitmask2;
	uint32_t avp_sum1;
	uint32_t avp_jmp_link;
	uint16_t avp_jmp_data [3];
} KL_PACKED KLAV_R3_Jump;

// KDB IDs
// Section types
#define KDBID_RECORD_NAME MAKE_KDBID('R','N','A','M')
#define KDBID_RECORD_INFO MAKE_KDBID('R','I','N','F')

// AVP3 record type IDs
enum
{
	KLAV_R3_TYPE_FILE    = 'F',
	KLAV_R3_TYPE_SECTOR  = 'S',
	KLAV_R3_TYPE_CA      = 'C',
	KLAV_R3_TYPE_FA      = 'X',
	KLAV_R3_TYPE_EXTRACT = 'E',
	KLAV_R3_TYPE_UNPACK  = 'U',
	KLAV_R3_TYPE_JUMP    = 'J',
	KLAV_R3_TYPE_CURE    = 'Q',
};

// AVP3 record info class IDs
enum
{
	KLAV_R3_INFO_RECORD  = 'R',  // source record information
	KLAV_R3_INFO_SCAN_1  = '1',  // scan data 1 (KLAV_R3_Scan1)
	KLAV_R3_INFO_SCAN_2  = '2',  // scan data 2 (KLAV_R3_Scan2)
	KLAV_R3_INFO_SCAN_3  = '3',  // scan data 3 (KLAV_R3_Scan3)
	KLAV_R3_INFO_SCAN_ST = 'T',  // record count by subtypes
	KLAV_R3_INFO_IREF    = 'I',  // info reference
};

// common KDBID prefix
#define KDBID_R3  MAKE_KDBID('A','3', 0, 0)

// Section types (R3 record bodies)
#define KDBID_R3_FILE    MAKE_KDBID('A','3','R','F')
#define KDBID_R3_SECTOR  MAKE_KDBID('A','3','R','S')
#define KDBID_R3_CA      MAKE_KDBID('A','3','R','C')
#define KDBID_R3_FA      MAKE_KDBID('A','3','R','X')
#define KDBID_R3_EXTRACT MAKE_KDBID('A','3','R','E')
#define KDBID_R3_UNPACK  MAKE_KDBID('A','3','R','U')
#define KDBID_R3_JUMP    MAKE_KDBID('A','3','R','J')
#define KDBID_R3_CURE    MAKE_KDBID('A','3','R','Q')

// Section types (R3 satellites - INFO REFs)
#define KDBID_R3_IREF_FILE    MAKE_KDBID('A','3','I','F')
#define KDBID_R3_IREF_SECTOR  MAKE_KDBID('A','3','I','S')
#define KDBID_R3_IREF_CA      MAKE_KDBID('A','3','I','C')
#define KDBID_R3_IREF_FA      MAKE_KDBID('A','3','I','X')
#define KDBID_R3_IREF_EXTRACT MAKE_KDBID('A','3','I','E')
#define KDBID_R3_IREF_UNPACK  MAKE_KDBID('A','3','I','U')
#define KDBID_R3_IREF_JUMP    MAKE_KDBID('A','3','I','J')


#include <kl_poppack.h>
////////////////////////////////////////////////////////////////

#ifdef __cplusplus

struct KLAV_R3_Set
{
	const KLAV_R3_Scan1 * m_recs1;
	const KLAV_R3_Scan2 * m_recs2;
	const KLAV_R3_Scan3 * m_recs3;
	uint32_t              m_rcnt;
	const uint32_t  *     m_info_refs;  // INFO record references
	KLAV_R3_Set *         m_recs_next;  // NULL by default
};

struct KLAV_R3_Data
{
	KLAV_R3_Set         m_recs_file[8];
	KLAV_R3_Set         m_recs_sector;
	KLAV_R3_Set         m_recs_ca;
	KLAV_R3_Set         m_recs_fa;
	KLAV_R3_Set         m_recs_unpack;
	KLAV_R3_Set         m_recs_extract;

	const KLAV_R3_Jump* m_recs_jump;
	const KLAV_R3_Cure* m_recs_cure;

	const void**        m_proc_table;
	const void*         m_recs_names;
};

#endif // __cplusplus

////////////////////////////////////////////////////////////////

#endif // klava_kdb_h_INCLUDED

