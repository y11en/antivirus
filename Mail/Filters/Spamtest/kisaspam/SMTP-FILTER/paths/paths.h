/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: paths.h                                                               *
 * Created: Tue Apr 30 00:09:28 2002                                           *
 * Desc: Config implementation of file names and so on.                        *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  paths.h
 * \brief Config implementation of file names and so on.
 */

#ifndef __paths_h__
#define __paths_h__

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * All this functions returns pointer to internal storage,
     * so if you want to use results of two functions at same
     * time, you must store values, obtained as results, in 
     * you buffers. Otherwise, older path will be overwriten.
     */

    /* 
     * set paths 
     */
    int         path_set_main_root(const char *root);

    int         path_set_bin_root(const char *root);

    int         path_set_config_root(const char *root);
    int         path_set_conf_bin_root(const char *root);
    int         path_set_conf_src_root(const char *root);
    int         path_set_conf_tmp_root(const char *root);
    
    int         path_set_conf_tmp_subdirname(const char *subdirname);

    int         path_set_cfdata_root(const char *root);
    int         path_set_cfdata_mainset_root(const char *root);

#ifdef AP_LICENSE
    const char *get_userdb_path();
#endif

    /* 
     * bin (utilities and scripts)
     */
    const char *full_path_compiler();
    const char *full_path_cfbuilder();
    const char *full_path_compiler_script();

    const char *full_path_informer_profile_compiler();
    const char *full_path_informer_cfdata_compiler();

    const char *full_path_main_root();
    
    const char *full_path_config_file();

    /*
     * Configuration (source and binary)
     */
    const char *full_path_config_root();
    const char *full_path_config_bin_root();
    const char *full_path_config_src_root();
    const char *full_path_config_tmp_root();

    const char *full_path_config_tmp_subdir();

    const char *full_path_config_tmp_subdir_samples();
    const char *full_path_config_tmp_subdir_profiles();
    const char *full_path_config_tmp_subdir_iplists();
    const char *full_path_config_tmp_subdir_emails();
    const char *full_path_config_tmp_subdir_dnsblacklists();

    const char *full_path_profile_db();
    const char *full_path_list_db();
    const char *full_path_config();
    const char *full_name_cfilter();

    const char *full_path_profiles();
    const char *full_path_emails();
    const char *full_path_iplists();
    const char *full_path_dnslists();
    const char *full_path_main();
    const char *full_path_samples();

    const char *full_tmp_path_profiles();
    const char *full_tmp_path_emails();
    const char *full_tmp_path_iplists();
    const char *full_tmp_path_dnslists();
    const char *full_tmp_path_main();
    const char *full_tmp_path_samples();

    const char *full_path_profile(const char *profile_name);
    const char *full_path_email_list(const char *list_name);
    const char *full_path_ip_list(const char *list_name);
    const char *full_path_dns_list(const char *list_name);
    const char *full_path_sample(const char *file_name);

    const char *full_path_hidden_profile(const char *profile_name);


    const char *full_tmp_path_profile(const char *profile_name);
    const char *full_tmp_path_email_list(const char *list_name);
    const char *full_tmp_path_ip_list(const char *list_name);
    const char *full_tmp_path_dns_list(const char *list_name);
    const char *full_tmp_path_sample(const char *file_name);

    const char *full_path_cfg_lock();

    /*
     * Updates
     */
    const char *full_path_cfdata_root();
    const char *full_path_cfdata_mainset_root();
    const char *full_path_catlist();
    const char *full_name_mainset();

    /*
     * Misc. 
     */
#define PATH_VALIDATE_NORMAL  0x00000000U
#define PATH_VALIDATE_DIR     0x00000001U
int path_validate(const char* path, unsigned int mode, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif /* __paths_h__ */

/*
 * <eof paths.h>
 */
