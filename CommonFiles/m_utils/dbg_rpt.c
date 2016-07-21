
char dbg_string[] = "check esp register failed";

int (*g_check_func)( const char* msg );

// ---
void set_dbg_report_func( int (*check_func_param)(const char*) ) {
	g_check_func = check_func_param;
}



int dbg_report( char* msg ) {
	if ( g_check_func && g_check_func(msg) )
		return 1;
	else
		return 0;
}