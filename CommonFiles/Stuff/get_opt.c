#include <ctype.h>
#include <string.h>

// ---
void prepare_cmd_line( const char* cmd, char* exe_name, char* cmd_line ) {
	const char* exe;
	if ( *cmd == '"' || *cmd == '\'' ) {
		int j = *cmd++;
		exe = cmd;
		while( *cmd && *cmd!=j )    
			cmd++;  
		if ( exe_name ) {
			memcpy( exe_name, exe, cmd-exe );
			exe_name[cmd-exe] = 0;
		}
		cmd++;
	}
	else {
		exe = cmd;
		while( *cmd && *cmd!=' ' )  
			cmd++;
		if ( exe_name ) {
			memcpy( exe_name, exe, cmd-exe );
			exe_name[cmd-exe] = 0;
		}
	}
	while( *cmd && *cmd==' ' )  
		cmd++;
	strcpy( cmd_line, cmd );
}




// ---
int find_key_and_val( const char* src, int* key, int* start, int* stop ) {
		
	int symb;
	int curr = 0;
	int quota_mark = 0;
	int start_ind = -1;
	int stop_ind = -1;
	int ready_for_key = 1;
	int quota_began = 0;

	while( src[curr] && (src[curr]==' ') )
		curr++;

	if ( (src[curr] == '-') || (src[curr] == '/') ) {
		*key = src[++curr];
		if ( *key ) {
			curr++;
			if ( src[curr] == ' ' ) {
				if ( start && stop )
					curr++;
			}
			else
				ready_for_key = 0;
		}
		else {
      if ( start )
        *start = curr;
      if ( stop )
        *stop = curr;
      return curr;
		}
	}
	else
		*key = 0;

	if ( src[curr] == ' ' )
		start_ind = stop_ind = curr;

	else {
		if ( (src[curr] == '"') || (src[curr] == '\'') ) { // start of quoted string
			ready_for_key = 0;
			quota_mark = src[curr++];
			quota_began = 1;
		}
		start_ind = curr;
		/*
		if ( !start || !stop )
			stop_ind = curr;
		*/
	}
		
	for( symb=src[curr]; symb; symb=src[++curr] ) {

		if ( quota_mark ) {
			if ( symb == quota_mark ) { // end of quoted string
				quota_mark = 0;
				stop_ind = curr + !quota_began;
				ready_for_key = 1;
			}
			continue;
		}
		else if ( (symb == '"') || (symb == '\'') ) {
			quota_mark = symb;
			continue;
		}

		else if ( ready_for_key && ((symb == '-') || (symb == '/')) ) {
			if ( stop_ind == -1 )
				stop_ind = curr;
			break;
		}

		else if ( symb == ' ' ) {
			ready_for_key = 1;
			if ( stop_ind == -1 )
				stop_ind = curr;
		}

		else if ( stop_ind != -1 )
			break;

		else
			ready_for_key = 0;
	}

	if ( start && stop ) {
		if ( stop_ind == -1 )
			stop_ind = curr;
		
		while( (stop_ind > start_ind) && (src[stop_ind-1] == ' ') )
			stop_ind--;

		*start = start_ind;
		*stop = stop_ind;
	}
	
	return curr;
}




// ---
int get_option( char* cmd, char option, char* value, int trim ) {

	int key = 0;
	int start_ind = 0;
	int stop_ind = 0;
	int ln = (int)strlen( cmd );
	int opt_len;
	int* start = value ? &start_ind : 0;
	int* stop = value ? &stop_ind : 0;
	
	if ( option )
		option = tolower( option );

	if ( value )
		*value = 0;

	while( ln && (ln >= (opt_len=find_key_and_val(cmd,&key,start,stop))) && opt_len ) {
		if ( option == tolower(key) ) {
			if ( value ) {
				if ( stop_ind > start_ind )
					memcpy( value, cmd+start_ind, stop_ind-start_ind );
				value[stop_ind-start_ind] = 0;
			}
			if ( trim )
				memmove( cmd, cmd+opt_len, ln-opt_len+1 );
			return 1;
		}
		else {
			cmd += opt_len;
			ln -= opt_len;
		}
	}

	return 0;
}




// ---
int get_next_option( char* cmd, int* option, char* value, int trim ) {
	
	int key = 0;
	int start_ind = 0;
	int stop_ind = 0;
	int opt_len = find_key_and_val( cmd, &key, &start_ind, &stop_ind );
	
	if ( opt_len ) {
		if ( value ) {
			if ( stop_ind > start_ind )
				memcpy( value, cmd+start_ind, stop_ind-start_ind );
			value[stop_ind-start_ind] = 0;
		}
		
		if ( trim ) {
			int len = (int)strlen( cmd );
			memmove( cmd, cmd+opt_len, len-opt_len+1 );
		}
	}
	else if ( value )
		*value = 0;

	if ( option )
		*option = key;

	return opt_len;
}


