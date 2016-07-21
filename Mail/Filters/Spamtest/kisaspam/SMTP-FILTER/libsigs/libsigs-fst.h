/* -*- C -*-
 * File: libsigs-fst.h
 *
 * Created: Fri Jul 26 13:00:33 2005
 */

#ifndef __libsigs_fst_h
#define __libsigs_fst_h

#include "smtp-filter/libsigs/libsigs.h"

#include "lib/fstorage/fstorage.h"
#include "lib/fstorage/fstorage_id.h"

#ifdef __cplusplus
extern "C"
{
#endif

    md5sigarray *fstorage_loadmd5sig(fstorage *fs, fstorage_section_id sid);
    int fstorage_md5sigsave(fstorage *fs, md5sigarray *sa, fstorage_section_id sid);
    void fstorage_unloadmd5sig(md5sigarray *sa);


#define MAX_GSG_UPDATE_FILES 64

    int fstorage_updatemd5sig(fstorage *res_fs, fstorage *old_fs, size_t upd_count, fstorage **upd_fs);

    
#ifdef __cplusplus
}
#endif

#endif







