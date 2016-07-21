#ifndef _HIPS_TOOLS_H_
#define _HIPS_TOOLS_H_

//////////////////////////////////////////////////////
//	access flags
//////////////////////////////////////////////////////
#define HIPS_FLAG_ALLOW		0	//	allow permission
#define HIPS_FLAG_ASK		1	//	ask permission
#define HIPS_FLAG_DENY		3	//	deny permission

#define HIPS_FLAG_INHERIT	2	//	inherit permission
								//	(user only, for driver must be converted to aks\allow\deny)
//----------------------------------------------------
#define HIPS_POS_WRITE		0	//	write (file, reg) AM position
#define HIPS_POS_PERMIS		0	//	permission AM position
#define HIPS_POS_READ		1	//	read (file, reg) AM position
#define HIPS_POS_ENUM		2	//	enum (file, reg) AM position
#define HIPS_POS_DELETE		3	//	delete (file, reg) AM position
#define HIPS_POS_KLFLTDEV	0	//	device (for klfltdev) AM position
//----------------------------------------------------
#define HIPS_LOG_OFF		0
#define HIPS_LOG_ON			1
//----------------------------------------------------
#define HIPS_RULE_TYPE_GLB_DNY		1
#define HIPS_RULE_TYPE_LCL_ALW		2
#define HIPS_RULE_TYPE_LCL_DNY		3
//////////////////////////////////////////////////////
//	service macros
//////////////////////////////////////////////////////

#define HIPS_GET_BLOCK(val, pos) \
			( (val >> (pos*4)) & 15 )
			//( (val & (7 << (3*pos))) >> (pos*3) )
//----------------------------------------------------
//	get AM in POS
#define HIPS_GET_AM(val, pos) \
			((HIPS_GET_BLOCK(val, pos)) & 3)
//----------------------------------------------------
#define HIPS_SET_BLOCK(am, is_log, is_inh) \
			((is_inh << 3) | (is_log << 2) | am)

//////////////////////////////////////////////////////
//	users macros
//////////////////////////////////////////////////////

//----------------------------------------------------
//	setting(add to VAL) AM and LOG to POS
#define HIPS_SET_BLOCK_POS(am, is_log, is_inh, pos, val) \
			(val | \
			(HIPS_SET_BLOCK(am, is_log, is_inh) << (pos*4)))
//
#define HIPS_SET_BLOCK_POS_EX(block, pos, val) \
			(val | \
			(block << (pos*4)))
//----------------------------------------------------
//	get state of LOG in POS
#define HIPS_GET_INH(val, pos) \
			((HIPS_GET_BLOCK(val, pos)) >> 3)
//----------------------------------------------------
//	get state of LOG in POS
#define HIPS_GET_LOG(val, pos) \
			(((HIPS_GET_BLOCK(val, pos)) >> 2) & 1)
			//(HIPS_GET_BLOCK(val, pos)) >> 2
//----------------------------------------------------
//	checking AM in POS
#define HIPS_IS_AM_SET(val, pos, flag) \
			((HIPS_GET_AM(val, pos)) == flag)
//----------------------------------------------------


#endif
