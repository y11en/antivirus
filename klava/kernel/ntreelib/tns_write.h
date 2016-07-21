// tns_write.h
//
//

#ifndef tns_write_h_INCLUDED
#define tns_write_h_INCLUDED

#include "tns_mgr.h"

////////////////////////////////////////////////////////////////

struct TNS_Writer
{
	virtual uint32_t    KLAV_CALL size () = 0;
	virtual klav_bool_t KLAV_CALL append (const void *data, uint32_t size) = 0;
	virtual klav_bool_t KLAV_CALL read  (uint32_t pos, void *data, uint32_t size) = 0;
	virtual klav_bool_t KLAV_CALL write (uint32_t pos, const void *data, uint32_t size) = 0;
};

struct TNS_Write_Options
{
	uint32_t m_hash_level;	// write to header
	uint32_t m_hash_size;   // in Kb, e.g. 16,32,64..
	uint32_t m_flags;
};

#define TREE_WRITE_F_BIG_ENDIAN 0x01

struct TNS_Id_Subst
{
	virtual uint32_t KLAV_CALL getIdSubst(uint32_t id) = 0;
};

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
void KLAV_CALL KLAV_Tree_Write (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			TNS_Writer * writer,
			TNS_Write_Options * options,
			TNS_Id_Subst* idSubst
		);

////////////////////////////////////////////////////////////////

#endif // tns_write_h_INCLUDED

