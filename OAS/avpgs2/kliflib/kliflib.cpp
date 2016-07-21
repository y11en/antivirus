/*
   Copyright (c) 1997-2006, Kaspersky Lab
   All Rights Reserved.
  
   This is PROPRIETARY SOURCE CODE of Kaspersky Lab.
   The contents of this file may not be disclosed to third parties,
   copied or duplicated in any form, in whole or in part, without
   the prior written permission of Kaspersky Lab
 */

#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/sys_domain.h>
#include <sys/event.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sysctl.h>

#include "kliflib.h"
#include "../kext/klif_interface.h"

// Socket used in user-kernel communication
static int klifsock = -1;

// security key for add/remove PID
static char securitykey[8];

// Flag "run/stop"
static int runflag = 0;



// Gets the security key to use in add/removePid functions
const char * klif_getsecuritykey()
{
	return securitykey;
}


// Initializates the driver interface
int	klif_init( const char * key )
{
	struct sockaddr_ctl	addr;
	struct ctl_info	ctl_info;
	
	// We cannot connect twice
	if ( klifsock != -1 )
	{
		errno = EALREADY;
		return -1;
	}
	
	// Do we have a security key? If not, generate it
	if ( !key )
	{
		// This generation is not really secure, but better than some 
		// partners could do. Note that ^ is XOR, not power of.
		int i;
		
		srand( time(0) ^ getpid() );
		
		for ( i = 0; i < 7; i++ )
			securitykey[i] = (char) (255.0 * (rand() / (RAND_MAX + 1.0))); // read "man rand" why
	}
	else
		memcpy( securitykey, key, 8 );
	
	// Create the socket
	klifsock = socket( PF_SYSTEM, SOCK_STREAM, SYSPROTO_CONTROL );

	if ( klifsock == -1 )
		return -1;

	// Obtain the kernel extension dynamically-generated NKE ID
	bzero( &ctl_info, sizeof(struct ctl_info) );
	strcpy( ctl_info.ctl_name, KLIF_INTERFACE_NAME );

	// Obtain the socket CTL ID
	if ( ioctl( klifsock, CTLIOCGINFO, &ctl_info ) == -1 )
		return -1;

	// Setup socket address
	bzero( &addr, sizeof(addr) );
	addr.sc_len = sizeof(addr);
	addr.sc_family = AF_SYSTEM;
	addr.ss_sysaddr = AF_SYS_CONTROL;
	addr.sc_id = ctl_info.ctl_id;
    addr.sc_unit = 0;
	
	// And connect it
	if ( connect( klifsock, (struct sockaddr *)&addr, sizeof(addr)) != 0 )
		return -1;
	
	// Now we could send the key
	if ( setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_SETKEY, securitykey, 8 ) != 0 )
	{
		errno = EAUTH;
		close( klifsock );
		klifsock = -1;
		return -1;
	}
	
	runflag = 0;
	return 0;
}


// Shuts down the driver interface
int	klif_shutdown()
{
	if ( klifsock == -1 )
	{
		errno = ENOTCONN;
		return -1;
	}
	
	klif_stop_interceptor();
	close( klifsock );

	klifsock = -1;
	return 0;
}


static void process_kevents( struct kevent * kevents, int active_kevents )
{
	for ( int i = 0; i < active_kevents; i++)
	{
		if ( kevents[i].flags & EV_ERROR )
		{
			fprintf( stdout, "event %d [%d] error detected\n", kevents[i].ident, i );
			continue;
		}
		
		if ( kevents[i].fflags & (NOTE_EXIT | NOTE_EXEC) )
			klif_unregister_pid( securitykey, kevents[i].ident );
	}
}


// Starts the file interceptor
int	klif_start_interceptor( klif_callback pfn, void *context )
{
	char buf[2048];
	int offset = 0;
	struct kevent kevents[MAXIMUM_EXCLUDED_PIDS];
	int active_kevents = 0;

	// Must be connected
	if ( klifsock == -1 )
	{
		errno = ENOTCONN;
		return -1;
	}

	// Init the kqueue events
	for ( int i = 0; i < MAXIMUM_EXCLUDED_PIDS; i++ )
	{
		EV_SET( &kevents[i], 0, 0, EV_DISABLE, 0, 0, 0 );
	}

	// Start the intercepting
	if ( setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_ENABLEFI, 0, 0 ) != 0 )
	{
		errno = ENOTSUP;
		return -1;
	}

	runflag = 1;

	// kqueue socket
	int kqueuesock = kqueue();

	// Run the forewer loop
	while ( runflag )
	{
		// Wait for available data
        fd_set readfds;
        struct timeval tv;
		struct timespec kqtv;
		
        FD_ZERO ( &readfds );
        FD_SET( klifsock, &readfds );
        FD_SET( kqueuesock, &readfds );        

        tv.tv_usec = 0;
        tv.tv_sec = 1;
        kqtv.tv_nsec = kqtv.tv_sec = 0;

		// Check events if applicable
		if ( active_kevents > 0 )
		{
			int nfound = kevent( kqueuesock, 0, 0, kevents, active_kevents, &kqtv );

			if ( nfound == -1 )
			{
				fprintf( stderr, "kevent failed: %d\n", errno );
				errno = EOVERFLOW;
				goto error_exit;
			}
			
			if ( nfound > 0 )
				process_kevents( kevents, active_kevents );
		}

		// wait for data
		if ( select( klifsock + 1, &readfds, 0, 0, &tv) < 0 )
        {
        	if ( errno == EINTR )
            	continue;

			fprintf( stderr, "select failed: %d\n", errno );
			setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_DISABLEFI, 0, 0 );
			errno = EOVERFLOW;
			return -1;
		}
		
		// Nothing to read? ok; check the runflag and listen again
		if ( !FD_ISSET( klifsock, &readfds) )
			continue;

		// Read the records
		int len = read( klifsock, buf + offset, sizeof(buf) - offset - 1 );
		
		if ( len <= 0 )
		{
			fprintf( stderr, "read failed: %d (%d)\n", len, errno );
			setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_DISABLEFI, 0, 0 );
			errno = EOVERFLOW;
			return -1;
		}
		
		offset += len;
		
		// We should have at least klif_event_header_t, and if we have if check for the whole msg
		while ( offset >= sizeof( klif_event_header_t ) )
		{
			// See what we have in the received buffer
			klif_event_header_t * hdr = (klif_event_header_t *) buf;
		
			// Is there enough data?
			if ( offset < hdr->length )
				break;
		
			if ( hdr->type == KLIF_EVENT_TYPE_FILE )
			{
				klif_file_event_t * fhdr = (klif_file_event_t*) (buf + sizeof(*hdr));
				char * filename = buf + sizeof(*hdr) + sizeof(*fhdr);
				
				int action = KLIF_ACTION_OPEN;
			 
				switch ( fhdr->action )
				{
					case 'O':
						action = KLIF_ACTION_OPEN;
						break;
						
					case 'C':
						action = KLIF_ACTION_CLOSE;
						break;
						
					case 'E':
						action = KLIF_ACTION_EXEC;
						break;
				}
										
				// Call the callback
				(*pfn) ( filename, fhdr->id, fhdr->uid, fhdr->pid, action, context );
			}
			else if ( hdr->type == KLIF_EVENT_TYPE_PIDS )
			{
				klif_pid_event_t * phdr = (klif_pid_event_t*) (buf + sizeof(*hdr));
				long * pids = &phdr->pid;
				
/*				printf("Recevied pid action %s for %d pids [ ", 
					phdr->action == KLIF_EVENT_PID_ADD ? "add" : "remove",
					phdr->count );
				for ( int i = 0; i < phdr->count; i++ )
					printf ("%d ", pids[i]);
				printf("]\n");
*/				
				// iterate through the pids list
				for ( int i = 0; i < phdr->count; i++ )
				{
					// Adding a new pid at the end of queue
					if ( phdr->action == KLIF_EVENT_PID_ADD )
					{
						if ( active_kevents == (MAXIMUM_EXCLUDED_PIDS - 1) )
						{
							fprintf( stderr, "Could not add pid to monitoring queue: no room. Recompilation needed\n" );
							goto error_exit;
						}
						
						EV_SET( &kevents[active_kevents], pids[i], EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT|NOTE_EXEC, 0, 0 );
						active_kevents++;
						
						// Reinit the kqueue.
						if ( kevent( kqueuesock, kevents, active_kevents, kevents, active_kevents, &kqtv ) == -1 )
						{
							if ( errno != EBADF && errno != ESRCH )
							{
								fprintf( stderr, "kevent after adding pid failed: %d\n", errno );
								errno = EOVERFLOW;
								goto error_exit;
							}
							else // EBADF/ESRCH are possible if the process just exited
								process_kevents( kevents, active_kevents );
						}
					}
					else // remove
					{
						// Find the pid in the queue
						int pididx;

						for ( pididx = 0; pididx < active_kevents; pididx++ )
						{
							if ( kevents[pididx].ident == pids[i] )
								break;
						}
					
						if ( pididx != active_kevents )
						{
							// Disable it, and call kevent to reset it
							EV_SET( &kevents[pididx], pids[i], EVFILT_PROC, EV_DELETE | EV_DISABLE, NOTE_EXIT|NOTE_EXEC, 0, 0 );
							kevent( kqueuesock, &kevents[pididx], 1, 0, 0, &kqtv );
							
							// Remove it. If it is not the latest one, replace it by the latest one
							if ( pididx != (active_kevents-1) )
							{
								// Copy the last event to removed
								kevents[pididx].ident = kevents[active_kevents-1].ident;
								kevents[pididx].filter = kevents[active_kevents-1].filter;
								kevents[pididx].flags = kevents[active_kevents-1].flags;
								kevents[pididx].fflags = kevents[active_kevents-1].fflags;
								kevents[pididx].data = kevents[active_kevents-1].data;
							}
							
							// And drop the counter
							active_kevents--;
						}
					}
				}
			}
			else
			{
				fprintf(stderr, "Fatal error: recevied unknown event type %d", hdr->type );
				abort();
			}
			
			// Store the length; it will be changed after the memmove
			unsigned int length = hdr->length;
			
			// and move the rest of the buffer back
			memmove( buf, buf + length, length );
			offset -= length;
		}
	}

	close( kqueuesock );
	return 0;

error_exit:
	setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_DISABLEFI, 0, 0 );
	close( kqueuesock );	
	return -1;
}


int	klif_stop_interceptor()
{
	// Must be connected
	if ( klifsock == -1 )
	{
		errno = ENOTCONN;
		return -1;
	}

	// Stop the intercepting
	if ( setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_DISABLEFI, 0, 0 ) != 0 )
	{
		errno = ENOTSUP;
		return -1;
	}

	runflag = 0;
	return 0;
}


// Adds the PID to the "exclusion" list
int	klif_register_pid( const char * key, long pid )
{
	klif_sysctl_message_t sysctlmsg;
	
	// Prepare the sysctl message
	memcpy( sysctlmsg.securitykey, key, 8 );
	sysctlmsg.action = KLIF_SYSCTL_PID_ADD;
	sysctlmsg.subaction = 0;
	sysctlmsg.value = pid;
	
	return sysctlbyname( KLIF_SYSCTL_NAME, 0, 0, &sysctlmsg, sizeof( sysctlmsg ) );
}


// Removes the PID from the "exclusion" list
int klif_unregister_pid( const char * key, long pid )
{
	klif_sysctl_message_t sysctlmsg;
	
	// Prepare the sysctl message
	memcpy( sysctlmsg.securitykey, key, 8 );
	sysctlmsg.action = KLIF_SYSCTL_PID_REMOVE;
	sysctlmsg.subaction = 0;
	sysctlmsg.value = pid;
	
	return sysctlbyname( KLIF_SYSCTL_NAME, 0, 0, &sysctlmsg, sizeof( sysctlmsg ) );
}


// Sends the needed file action to the kernel module
int klif_set_file_action( const char * securitykey, long objectid, int action )
{
	klif_sysctl_message_t sysctlmsg;
	
	// Prepare the sysctl message
	memcpy( sysctlmsg.securitykey, securitykey, 8 );
	sysctlmsg.action = KLIF_SYSCTL_QUEUE_SET;
	sysctlmsg.value = objectid;
	
	if ( action == KLIF_MODULE_ACTION_DENY )
		sysctlmsg.subaction = KLIF_SYSCTL_ACTION_DENY;
	else if ( action == KLIF_MODULE_ACTION_PASSCACHE )
		sysctlmsg.subaction = KLIF_SYSCTL_ACTION_PASSCACHE;
	else if ( action == KLIF_MODULE_ACTION_PASS )
		sysctlmsg.subaction = KLIF_SYSCTL_ACTION_PASS;
	else
	{
		errno = EINVAL;
		return -1;
	}
	
	return sysctlbyname( KLIF_SYSCTL_NAME, 0, 0, &sysctlmsg, sizeof( sysctlmsg ) );
}


// Presets the action in case timeout happens.
int klif_set_file_default_action( const char * securitykey, long objectid, int defaction )
{
	klif_sysctl_message_t sysctlmsg;
	
	// Prepare the sysctl message
	memcpy( sysctlmsg.securitykey, securitykey, 8 );
	sysctlmsg.action = KLIF_SYSCTL_QUEUE_PRESET;
	sysctlmsg.value = objectid;
	
	if ( defaction == KLIF_MODULE_ACTION_DENY )
		sysctlmsg.subaction = KLIF_SYSCTL_ACTION_DENY;
	else if ( defaction == KLIF_MODULE_ACTION_PASSCACHE )
		sysctlmsg.subaction = KLIF_SYSCTL_ACTION_PASSCACHE;
	else if ( defaction == KLIF_MODULE_ACTION_PASS )
		sysctlmsg.subaction = KLIF_SYSCTL_ACTION_PASS;
	else
	{
		errno = EINVAL;
		return -1;
	}
	
	return sysctlbyname( KLIF_SYSCTL_NAME, 0, 0, &sysctlmsg, sizeof( sysctlmsg ) );
}


// Restarts the timeout for the scanned file
int klif_set_file_timeout( const char * securitykey, long objectid, unsigned int timeout )
{
	klif_sysctl_message_t sysctlmsg;

	memcpy( sysctlmsg.securitykey, securitykey, 8 );
	sysctlmsg.action = KLIF_SYSCTL_QUEUE_EXTENDSCAN;
	sysctlmsg.value = objectid;
	sysctlmsg.subaction = timeout > 32767 ? 32767 : timeout;
	
	return sysctlbyname( KLIF_SYSCTL_NAME, 0, 0, &sysctlmsg, sizeof( sysctlmsg ) );
}


// brief Clears the kernel cache
int klif_clear_cache()
{
	// Must be connected
	if ( klifsock == -1 )
	{
		errno = ENOTCONN;
		return -1;
	}

	// Stop the intercepting
	if ( setsockopt( klifsock, SYSPROTO_CONTROL, KLIF_SOCKOPT_CLEARCACHE, 0, 0 ) != 0 )
	{
		errno = ENOTSUP;
		return -1;
	}

	return 0;
}
