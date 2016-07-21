#ifndef __get_opt_h
#define __get_opt_h

#if defined( __cplusplus )
extern "C" {
#endif

// divides whole command line to exe name and command line
//   cmd      -- initial command line
//   exe_name -- buffer to receive exe name
//   cmd_line -- buffer to receive stripped command line
void prepare_cmd_line( const char* cmd, char* exe_name, char* cmd_line );

// searches command line for the option
// returns 1 if "option" is presented in the command line
//   cmd    -- command line
//   option -- key value 
//             if zero - searches for "default" option (without "-" sign and key)
//   value  -- buffer to receive value if present (optional)
//   trim   -- if non zero cuts out found option from command line
int get_option( char* cmd, char option, char* value, int trim );

// searches command line for the next option
// returns length ot the found option
//   cmd    -- command line
//   option -- int to receive key value
//             if value placed to the *option is zero - "default" option found (without "-" sign and key)
//   value  -- buffer to receive value if present (optional)
//   trim   -- if non zero cuts out found option from command line
int get_next_option( char* cmd, int* option, char* value, int trim );
	
#if defined( __cplusplus )
}
#endif

#endif
