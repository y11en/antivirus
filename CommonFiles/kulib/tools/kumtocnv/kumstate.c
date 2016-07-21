

#include "kum.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static const char* parse_state( const char *s, int32_t state[ 0x100 ], uint8_t *pflags )
{
    const char* tmp;
    uint32_t start, end, i;
    int32_t entry;

    for( i = 0; i < 0x100; ++i ) 
	{
        state[ i ] = MBCS_ENTRY_FINAL( 0, MBCS_STATE_ILLEGAL, 0xffff );
    }

    s = skip_ws( s );

    if( strncmp( "initial", s, 7 ) == 0 ) 
	{
        *pflags = KUM_STATE_FLAG_DIRECT;
        s = skip_ws( s + 7 );
        if( *s != ',') 
		{
            return s;
        }
		++s;
    }
    else if( *pflags == 0 && strncmp( "surrogates", s, 10 ) == 0 )
	{
        *pflags = KUM_STATE_FLAG_SURROGATES;
        s = skip_ws( s + 10 );
        if( *s != ',' ) 
		{
            return s;
		}
		++s;
	}
	else if( *s == '\0' ) 
	{
        return NULL;
    }

    for(;;) 
	{
        s = skip_ws( s );
        start = strtoul( s, (char**) &tmp, 16 );
        if( s == tmp || 0xff < start ) 
		{
            return s;
        }
        s = skip_ws( tmp );

        if( *s == '-' )
		{
            s = skip_ws( s + 1 );
            end = strtoul( s, (char**) &tmp, 16 );
            if( s == tmp || end < start || 0xff < end )
			{
                return s;
            }
            s = skip_ws( tmp );
        } 
		else
		{
            end = start;
        }

        if( *s != ':' && *s != '.' ) 
		{
            entry = MBCS_ENTRY_FINAL( 0, MBCS_STATE_VALID_16, 0 );
        } 
		else
		{
            entry = MBCS_ENTRY_TRANSITION( 0, 0 );
            if( *s == ':' )
			{
                s = skip_ws( s + 1 );
                i = strtoul( s, (char**) &tmp, 16 );
                if( s != tmp )
				{
                    if( i > 0x7f )
					{
                        return s;
                    }
                    s = skip_ws( tmp );
                    entry = MBCS_ENTRY_SET_STATE( entry, i );
                }
            }

            if( *s == '.' )
			{
                entry = MBCS_ENTRY_SET_FINAL( entry );

                s = skip_ws( s + 1 );
                if( *s == 'u' )
				{
                    entry = MBCS_ENTRY_FINAL_SET_ACTION_VALUE( entry, MBCS_STATE_UNASSIGNED, 0xfffe );
                    s = skip_ws( s + 1 );
                }
				else if( *s == 'p' )
				{
                    if( *pflags != KUM_STATE_FLAG_DIRECT )
					{
                        entry = MBCS_ENTRY_FINAL_SET_ACTION( entry, MBCS_STATE_VALID_16_PAIR );
                    }
					else
					{
                        entry = MBCS_ENTRY_FINAL_SET_ACTION( entry, MBCS_STATE_VALID_16 );
                    }
                    s = skip_ws( s + 1 );
                }
				else if( *s == 's' )
				{
                    entry = MBCS_ENTRY_FINAL_SET_ACTION( entry, MBCS_STATE_CHANGE_ONLY );
                    s = skip_ws( s + 1 );
                }
				else if( *s == 'i' )
				{
                    entry = MBCS_ENTRY_FINAL_SET_ACTION_VALUE( entry, MBCS_STATE_ILLEGAL, 0xffff );
                    s = skip_ws( s + 1 );
                }
				else
				{
                    entry = MBCS_ENTRY_FINAL_SET_ACTION( entry, MBCS_STATE_VALID_16 );
                }
            }

        }

        if( MBCS_ENTRY_FINAL_ACTION( entry ) == MBCS_STATE_VALID_16 ) 
		{
            switch( *pflags )
			{
            case KUM_STATE_FLAG_DIRECT:
                entry = MBCS_ENTRY_FINAL_SET_ACTION_VALUE( entry, MBCS_STATE_VALID_DIRECT_16, 0xfffe );
                break;
            case KUM_STATE_FLAG_SURROGATES:
                entry = MBCS_ENTRY_FINAL_SET_ACTION_VALUE( entry, MBCS_STATE_VALID_16_PAIR, 0 );
                break;
            }
        }

        for( i = start; i <= end; ++i ) 
		{
            state[ i ] = entry;
        }

        if( *s == ',' )
		{
            ++s;
        }
		else
		{
            return *s == '\0' ? NULL : s;
        }
    }
}

void kum_add_state( kum_states* states, const char* s )
{
	const char* end;

    if( states->states_count == MBCS_MAX_STATE_COUNT )
	{
        fprintf( stderr, "kum error: too many states (maximum %d)\n", MBCS_MAX_STATE_COUNT );
        exit( 1 );
    }

	end = parse_state( s, states->states[ states->states_count ], 
			&states->flags[ states->states_count ] );

	if( end != NULL )
	{
        fprintf( stderr, "kum error: invalid state definition \'%s\'\n", s );
        exit( 1 );
	}

	++states->states_count;
}

static int32_t calc_state_offsets( kum_states* states )
{
    int32_t entry;
	int32_t sum;
	int state, cell, count;
    int all_states_ready;

	all_states_ready = 0;
	for( count = states->states_count; !all_states_ready && count > 0; --count )
	{
        all_states_ready = 1;

        for( state = states->states_count - 1; state >= 0; --state )
		{
            if( !( states->flags[ state ] & KUM_STATE_FLAG_READY ) )
			{
                sum = 0;

				for( cell = 0; cell < 0x100; ++cell )
				{
					entry = states->states[ state ][ cell ];
					if( MBCS_ENTRY_IS_FINAL( entry ) )
					{
						switch( MBCS_ENTRY_FINAL_ACTION( entry ) )
						{
						case MBCS_STATE_VALID_16:
							states->states[ state ][ cell ] = MBCS_ENTRY_FINAL_SET_VALUE( entry, sum );
							sum += 1;
							break;
						case MBCS_STATE_VALID_16_PAIR:
							states->states[ state ][ cell ] = MBCS_ENTRY_FINAL_SET_VALUE( entry, sum );
							sum += 2;
							break;
						}
					}
				}

                for( cell = 0; cell < 0x100; ++cell )
				{
                    entry = states->states[ state ][ cell ];
                    if( MBCS_ENTRY_IS_TRANSITION( entry ) )
					{
                        if(states->flags[ MBCS_ENTRY_TRANSITION_STATE( entry ) ] & KUM_STATE_FLAG_READY )
						{
                            states->states[ state ][ cell ] = MBCS_ENTRY_TRANSITION_SET_OFFSET( entry, sum );
                            sum += states->offsets[ MBCS_ENTRY_TRANSITION_STATE( entry ) ];
                        } else {
                            sum = -1;
                            break;
                        }
                    }
                }

                if( sum != -1 )
				{
                    states->offsets[state] = sum;
                    states->flags[state] |= KUM_STATE_FLAG_READY;
                }
				else
				{
	                all_states_ready = 0;
				}
            }
        }
    }

    if( !all_states_ready )
	{
        fprintf( stderr, "kum error: state table has circular references\n" );
        exit( 1 );
    }

    sum = states->offsets[ 0 ];
    for( state = 1; state < states->states_count; ++state ) 
	{
        if( ( states->flags[ state ] & 0xf ) == KUM_STATE_FLAG_DIRECT )
		{
            int32_t sum2 = sum;
            sum += states->offsets[ state ];
            for( cell = 0; cell < 0x100; ++cell )
			{
                entry=states->states[ state ][ cell ];
                if( MBCS_ENTRY_IS_TRANSITION( entry ) )
				{
                    states->states[ state ][ cell ] = MBCS_ENTRY_TRANSITION_ADD_OFFSET( entry, sum2 );
                }
            }
        }
    }

    /* round to even number for 32-bit alignment */
    return states->to_unicode_units_count = ( sum + 1 ) & ~1;
}

void kum_process_states( kum_file* kum )
{
	kum_states* states = &kum->states;

	if( states->states_count == 0 )
	{
		switch( kum->conv_type )
		{
		case KUM_SBCS:
			kum->conv_type = KUM_MBCS;
			kum_add_state( states, "0-ff" );
			break;

		case KUM_DBCS:
			kum->conv_type = KUM_MBCS;
            kum_add_state( states, "0-3f:3, 40:2, 41-fe:1, ff:3" );
            kum_add_state( states, "41-fe" );
            kum_add_state( states, "40" );
            kum_add_state( states, "" );
			break;

		case KUM_MBCS:
			fprintf( stderr, "kum error: missing state table information for MBCS\n");
			exit( 1 );
			break;

        default:
            fprintf( stderr, "kum error: unknown charset type\n" );
            exit( 1 );
            break;
		}
	}

    if( kum->max_char_len < kum->min_char_len ) 
	{
        fprintf( stderr, "ucm error: max_char_len < min_char_len\n" );
        exit( 1 );
    }
	

	calc_state_offsets( states );	
}


int32_t kum_count_chars( kum_states* states, const uint8_t* bytes, int32_t len )
{
    uint32_t offset = 0;
    int32_t i, entry, count = 0;
    uint8_t state = 0;

    for( i = 0; i < len; ++i ) 
	{
        entry = states->states[ state ][ bytes[ i ] ];
        if( MBCS_ENTRY_IS_TRANSITION( entry ) ) 
		{
			state = (uint8_t) MBCS_ENTRY_TRANSITION_STATE( entry );
			offset += MBCS_ENTRY_TRANSITION_OFFSET( entry );
        }
		else
		{
            switch( MBCS_ENTRY_FINAL_ACTION( entry ) )
			{
            case MBCS_STATE_ILLEGAL:
                fprintf( stderr, "kum error: byte sequence ends in illegal state\n" );
                return -1;
            case MBCS_STATE_CHANGE_ONLY:
                fprintf( stderr, "kum error: byte sequence ends in state-change-only\n" );
                return -1;
            case MBCS_STATE_UNASSIGNED:
            case MBCS_STATE_FALLBACK_DIRECT_16:
            case MBCS_STATE_VALID_DIRECT_16:
            case MBCS_STATE_FALLBACK_DIRECT_20:
            case MBCS_STATE_VALID_DIRECT_20:
            case MBCS_STATE_VALID_16:
            case MBCS_STATE_VALID_16_PAIR:
                ++count;
                state = (uint8_t) MBCS_ENTRY_FINAL_STATE( entry );
                offset = 0;
                break;
            default:
                fprintf(stderr, "kum error: byte sequence reached reserved action code, entry: 0x%02lx\n", (unsigned long)entry);
                return -1;
            }
        }
    }

    if( offset != 0 )
	{
        fprintf(stderr, "kum error: byte sequence too short, ends in non-final state %hu\n", state);
        return -1;
    }

    return count;
}

















