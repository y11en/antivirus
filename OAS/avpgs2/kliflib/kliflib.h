/*
 * Copyright (c) 1997-2007, Kaspersky Lab
 * All Rights Reserved.
 * 
 * This is PROPRIETARY SOURCE CODE of Kaspersky Lab.
 * The contents of this file may not be disclosed to third parties,
 * copied or duplicated in any form, in whole or in part, without
 * the prior written permission of Kaspersky Lab
 */

#ifndef INCLUDE_KLIFLIB_H
#define INCLUDE_KLIFLIB_H

#define KLIF_ACTION_OPEN	1
#define KLIF_ACTION_CLOSE	2
#define KLIF_ACTION_EXEC	3

/*! Allow access to the file */
#define KLIF_MODULE_ACTION_PASS			1

/*! Deny access to the file */
#define KLIF_MODULE_ACTION_DENY			2

/*! Allow access to the file, and add the file to the kernel cache */
#define KLIF_MODULE_ACTION_PASSCACHE	3


/*!
 * \brief A file access event callback function
 *
 * \param filename [in] The full path to the file accessed.
 * \param objectid [in] The object identifier which must be used in klif_report_file() 
 *                 to identify this file
 * \param uid [in] The user ID of the process trying to access the file.
 * \param pid [in] The process ID trying to access the file.
 * \param action [in] The type of file access; one of actions KLIF_ACTION_OPEN,
 *                    KLIF_ACTION_CLOSE, KLIF_ACTION_EXEC
 *
 * This callback is called in context of thread/process which called klif_start_interceptor.
 * It should not block, as no more callbacks will be generated until this one returns. The best
 * implementation would just add the file to the internal queue, and return. The other thread should
 * pick up files from the queue, check them, and call klif_report_file() for each file.
 *
 * Remember that original process accessing the file is forced to sleep, and will not wake up until
 * klif_report_file() is called for this file.
 */
typedef void (*klif_callback) ( const char * filename, long objectid, long uid, long pid, int action, void *context );


/*!
 * \brief Initializates the driver interface
 *
 * \param key [in] Pointer to the 8-byte array containing the addRemovePid key.
 *            Might be NULL, in this case the key will be generated automatically.
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function initializates the KLIF driver. At this moment the kernel module 
 * should be already loaded. The call generates the addRemovePid key, connects to
 * the KLIF driver, and sets the key. Only the processes run by root could call 
 * this function.
 *
 * Note that this function does not start the file interceptor. 
 *
 * This function is not reentrant, and must be called as soon as possible after 
 * the program starts.
 */
int		klif_init( const char * securitykey );


/*!
 * \brief Starts the file interceptor
 *
 * \param pfn [in] Pointer to the ballbck function. Cannot be NULL.
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function starts the file interceptor process. It runs in a forever loop, which 
 * receives the file access events from the kernel module, and calls the provided callback.
 * The callback must return a KLIF_MODULE_ACTION_* value which are forwarded to the kernel 
 * module. This function does not return until klif_stop_interceptor() is called - either 
 * from the callback, or from separate thread. Returns 0 if stopped by klif_stop_interceptor(), 
 * or other error otherwise.
 *
 * This function must be called from the same thread/process which called klif_init(), and is
 * not reentrant.
 */
int		klif_start_interceptor( klif_callback pfn, void *context );


/*!
 * \brief Sends the needed file action to the kernel module
 * \param key [in] Security key. See klif_register_pid()
 * \param objectid [in] The object identifier from klif_callback() to identify this file
 * \param action [in] Allow or block access to this file. Possible values are ::KLIF_MODULE_ACTION_PASS
 *               to allow access to the file, ::KLIF_MODULE_ACTION_PASSCACHE to allow access to the file,
 *               and store it in the kernel cache (so subsequent attempts to open this file will not result
 *               in generating a callback unless the cache is clean or expires), and ::KLIF_MODULE_ACTION_DENY 
 *               to deny access to the file.
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function reports what to do with the intercepted file access after the application is woken up. 
 * You could either allow access, or deny it by providing appropriate \a action value. 
 *
 * This function could be called from different thread or process, and is thread-safe.
 */
int		klif_set_file_action( const char * securitykey, long objectid, int action );


/*!
 * \brief Restarts the timeout for the scanned file
 * \param key [in] Security key. See klif_register_pid()
 * \param objectid [in] The object identifier from klif_callback() to identify this file
 * \param timeout [in] New timeout for the specified file.
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function sets the new timeout for the currently scanned file. After the time has been expired, 
 * the file will not be hold, but will be released (or denied access) by the kernel module according to 
 * klif_report_file_preset(). Note that new timeout replaces the old timeout, not adds it to the 
 * existing one.
 *
 * This function could be called from different thread or process, and is thread-safe.
 */
int		klif_set_file_timeout( const char * securitykey, long objectid, unsigned int timeout );


/*!
 * \brief Presets the action in case timeout happens.
 * \param key [in] Security key. See klif_register_pid()
 * \param objectid [in] The object identifier from klif_callback() to identify this file
 * \param defaction [in] Allow or block access to this file. Possible values are ::KLIF_MODULE_ACTION_PASS
 *               to allow access to the file, ::KLIF_MODULE_ACTION_PASSCACHE to allow access to the file,
 *               and store it in the kernel cache (so subsequent attempts to open this file will not result
 *               in generating a callback unless the cache is clean or expires), and ::KLIF_MODULE_ACTION_DENY 
 *               to deny access to the file.
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function sets the default behavior if file scan results in timeout or the engine crash. By default 
 * in this case the ::KLIF_MODULE_ACTION_PASS is default. However if the file is still scanned, and already
 * contains some infected files, it could be marked as blocked even though it is still scanned.
 *
 * This function could be called from different thread or process, and is thread-safe.
 */
int		klif_set_file_default_action( const char * securitykey, long objectid, int defaction );


/*!
 * \brief Clears the kernel cache
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function clears the kernel cache of clean files added using ::KLIF_MODULE_ACTION_PASSCACHE option.
 * Could only be called when the file interceptor is active - klif_start_interceptor() is called. The cache
 * is cleaned automatically upon every klif_start_interceptor() call.
 *
 * This function must be called from the same thread/process which called klif_init(), and is
 * not reentrant.
 */
int		klif_clear_cache();


/*!
 * \brief Stops the file interceptor
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function stops the file interceptor process. It exits immediately (i.e. it does not 
 * wait until klif_start_interceptor() returns). This function could be called from different 
 * thread, and is thread-safe.
 */
int		klif_stop_interceptor();


/*!
 * \brief Shuts down the driver interface
 *
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function shuts down the driver interface, terminating the connection to kernel module.
 * It automatically calls klif_stop_interceptor() if needed.
 *  
 * This function must be called from the same thread/process which called klif_init(), and is
 * not reentrant.
 */
int		klif_shutdown();


/*!
 * \brief Gets the security key to use in add/removePid functions
 *
 * \return A pointer to the current 8-byte security key 
 *
 * This function returns the automatically generated, or previously set security key, which 
 * should be used in klif_register_pid() and klif_unregister_pid(). This allows to call 
 * those functions from different threads, or even from different processes, while 
 * maintaining security (i.e. only the process which knows the security key could register 
 * or unregister the PID, and only the process which is connected to the kernel module 
 * has access to the key).
 *
 * This function must be called from the same thread/process which called klif_init(), 
 * and is not reentrant.
 */
const char * klif_getsecuritykey();


/*!
 * \brief Adds the PID to the "exclusion" list
 * \param key [in] Security key which must be obtained by klif_setpidkey() or set in klif_init()
 *                 Cannot be NULL, must be 8 bytes long.
 * \param pid [in] A PID to add to the exclusion list
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function adds the specified process ID to the "excluded processes" list. The file 
 * operations done by excluded processes are not intercepted nor forwarded to the driver.
 * Any process which wants to work with infected files (like qurantining them) must be 
 * added to the excluded list to be able to access the files. The anti-virus engine process
 * must be added there as well.
 *
 * The driver does not remove the PIDs of processes which exit or crash automatically, so 
 * do not forget to call klif_unregister_pid(). The internal PID buffer could store 100 PIDs,
 * and when it is full, no PID could be added.
 *
 * This function may be called from any thread/process of the system, and is thread-safe.
 */
int		klif_register_pid( const char * key, long pid );


/*!
 * \brief Removes the PID from the "exclusion" list
 * \param key [in] Security key which must be obtained by klif_setpidkey() or set in klif_init()
 *                 Cannot be NULL, must be 8 bytes long.
 * \param pid [in] A PID to remove from the exclusion list
 * \return 0 for success, nonzero value means error, and errno is set accordingly.
 *
 * This function removes the specified process ID from the "excluded processes" list.
  * This function may be called from any thread/process of the system, and is thread-safe.
 */
int		klif_unregister_pid( const char * key, long pid );

#endif /* INCLUDE_KLIFLIB_H */
