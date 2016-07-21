/*
 * Copyright (c) 1997-2007, Kaspersky Lab
 * All Rights Reserved.
 * 
 * This is PROPRIETARY SOURCE CODE of Kaspersky Lab.
 * The contents of this file may not be disclosed to third parties,
 * copied or duplicated in any form, in whole or in part, without
 * the prior written permission of Kaspersky Lab
 */

#ifndef INCLUDE_KLIF_INTERFACE
#define INCLUDE_KLIF_INTERFACE


/* Maximum number of excluded PIDs */
#define MAXIMUM_EXCLUDED_PIDS	50

/*
 * KLIF socket options
 */
 
/* Kernel control socket interface name */
#define KLIF_INTERFACE_NAME		"com.kaspersky.kext.klif"
#define KLIF_SYSCTL_NAME		"kern.com_kaspersky_kext_klif"

/* Clears the kernel cache */
#define KLIF_SOCKOPT_CLEARCACHE	1

/* Sets the kext debug level. The argument is 32-bit int */
#define KLIF_SOCKOPT_DEBUGLEVEL	2

/* Enables file interceptor. No arguments */
#define KLIF_SOCKOPT_ENABLEFI	3

/* Disables file interceptor. No arguments */
#define KLIF_SOCKOPT_DISABLEFI	4

/* Sets the key for add/remove PID function */
#define KLIF_SOCKOPT_SETKEY		5


/*
 * KLIF sysctl values
 */
 
/* Add the PID to the list of excluded; subaction is 0, value contains PID  */
#define KLIF_SYSCTL_PID_ADD				0

/* Remove the PID from the list of excluded; subaction is 0, value contains PID  */
#define KLIF_SYSCTL_PID_REMOVE			1

/* Sets the file status; value is queue ID, subaction is status */
#define KLIF_SYSCTL_QUEUE_SET			2

/* Pre-set the file status to be used on timeout; value is queue ID, subaction is status */
#define KLIF_SYSCTL_QUEUE_PRESET		3

/* Extends the scan by delaying the timeout for the specified number of seconds; value is queue ID, subaction is delay in seconds */
#define KLIF_SYSCTL_QUEUE_EXTENDSCAN	4

/* Subaction for reported or prereported actions */
#define KLIF_SYSCTL_ACTION_PASS			1
#define KLIF_SYSCTL_ACTION_DENY			2
#define KLIF_SYSCTL_ACTION_PASSCACHE	3


/* 
 * Structures used in socket communications between kernel and userspace
 */
 
// Kernel event type
#define KLIF_EVENT_TYPE_FILE		1 
#define KLIF_EVENT_TYPE_PIDS		2
 
// KLIF_EVENT_TYPE_PIDS action type
#define KLIF_EVENT_PID_ADD			1 
#define KLIF_EVENT_PID_REMOVE		2

#pragma pack(1)

// Sysctl message structure
typedef struct
{
	char 	securitykey[8];
	short	action;
	short	subaction;	
	long 	value;
} klif_sysctl_message_t;


// The generic event header, followed by appropriate type
typedef struct
{
	unsigned short	length;	// total message length
	unsigned short	type;	// KLIF_EVENT_TYPE_*
} klif_event_header_t;


// The file event structure. Will be followed by the filename (including the 0 terminator)
typedef struct
{
	unsigned int 	id;
	long 			uid;
	long 			pid;
	char 			action;
	unsigned short	length; // the length of following filename
} klif_file_event_t;

// The pids event structure. Might be followed by pid(s) list if there are more than one. 
// The first pid is included.
typedef struct
{
	unsigned short 	count;	// how many pids are there; at least one
	unsigned short	action;	// KLIF_EVENT_PID_*
	long 			pid;	// the first one; more might be here
} klif_pid_event_t;

#pragma pack()

#endif /* INCLUDE_KLIF_INTERFACE */
