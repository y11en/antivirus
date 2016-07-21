#if defined(_MSC_VER) && _MSC_VER >= 1400 

#include <assert.h>
#include <errno.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#define _VALIDATE_RETURN_ERRCODE( expr, errorcode )                            \
	{                                                                          \
		assert((expr));									                       \
		if ( !(expr) )		                                                   \
		{                                                                      \
			errno = errorcode;                                                 \
			return ( errorcode );                                              \
		}                                                                      \
	}

#define _ERRCHECK(e) { errno_t err = e; assert(err == 0); }

//////////////////////////////////////////////////////////////////////////

/* support HPFS file system */
#define _HPFS_ 1

static wchar_t * __cdecl _wgetpath (const wchar_t *src, wchar_t *dst, size_t maxlen)
{
	const wchar_t *save_src;

	/*
	* strip off leading semi colons
	*/
	while ( *src == L';' )
		src++;

	/*
	* Save original src pointer
	*/
	save_src = src;

	/*
	* Decrement maxlen to allow for the terminating L'\0'
	*/
	if ( --maxlen == 0 )
	{
		errno = ERANGE;
		goto appendnull;
	}


	/*
	* Get the next path in src string
	*/
	while (*src && (*src != L';')) {

#if defined (_HPFS_)

		/*
		* Check for quote char
		*/
		if (*src != L'"') {

			*dst++ = *src++;

			if ( --maxlen == 0 ) {
				save_src = src; /* ensure NULL return */
				errno = ERANGE;
				goto appendnull;
			}

		}
		else {

			/*
			* Found a quote.  Copy all chars until we hit the
			* final quote or the end of the string.
			*/
			src++;                  /* skip over opening quote */

			while (*src && (*src != L'"')) {

				*dst++ = *src++;

				if ( --maxlen == 0 ) {
					save_src = src; /* ensure NULL return */
					errno = ERANGE;
					goto appendnull;
				}
			}

			if (*src)
				src++;          /* skip over closing quote */

		}

#else  /* defined (_HPFS_) */

		*dst++ = *src++;

		if ( --maxlen == 0 ) {
			save_src = src; /* ensure NULL return */
			errno = ERANGE;
			goto appendnull;
		}

#endif  /* defined (_HPFS_) */

	}

	/*
	* If we copied something and stopped because of a L';',
	* skip the L';' before returning
	*/
	while ( *src == L';' )
		src++;

	/*
	* Store a terminating null
	*/
appendnull:

	*dst = L'\0';

	return((save_src != src) ? (wchar_t *)src : NULL);
}

//////////////////////////////////////////////////////////////////////////


errno_t __cdecl _wsearchenv_s_fixed (
        const wchar_t *fname,
        const wchar_t *env_var,
        wchar_t *path,
        size_t sz
        )
{
        register wchar_t *p;
        register int c;
        wchar_t *envbuf = NULL;
        wchar_t *env_p, *save_env_p;
        size_t len;
        wchar_t pathbuf[_MAX_PATH + 4];
        wchar_t * pbuf = NULL;
        size_t fnamelen, buflen;
        errno_t save_errno;
        int ret;
        errno_t retvalue = 0;

        _VALIDATE_RETURN_ERRCODE( (path != NULL), EINVAL);
        _VALIDATE_RETURN_ERRCODE( (sz > 0), EINVAL);
        if (fname == NULL)
        {
            *path = L'\0';
            _VALIDATE_RETURN_ERRCODE( (fname != NULL), EINVAL);
        }

        /* special case: fname is an empty string: just return an empty path, errno is set to ENOENT */
        if (fname[0] == 0)
        {
            *path = L'\0';
            errno = ENOENT;
            retvalue = errno;
            goto cleanup;
        }

        save_errno = errno;
        ret = _waccess_s(fname, 0);
        errno = save_errno;

        if (ret == 0 ) {

            /* exists, convert it to a fully qualified pathname and
               return */
            if ( _wfullpath(path, fname, sz) == NULL )
            {
                *path = L'\0'; /* fullpath will set errno in this case */
                retvalue = errno;
                goto cleanup;
            }

            retvalue = 0;
            goto cleanup;
        }

        if (_wdupenv_s(&envbuf, NULL, env_var) != 0 || envbuf == NULL) {
            /* no such environment var. and not in cwd, so return empty
               string, set errno to ENOENT */
            *path = L'\0';
            errno = ENOENT;
            retvalue = errno;
            goto cleanup;
        }

        env_p = envbuf;
        fnamelen = wcslen(fname);
        pbuf = pathbuf;
        buflen = _countof(pathbuf);

        // BUG FIX is (+ wcslen(env_p))
		// buffer must be big enough to fit name AND FOLDER PATH ALSO!!!
		if(fnamelen + wcslen(env_p) >= buflen)
        {
            /* +2 for the trailing '\' we may need to add & the '\0' */
            buflen = wcslen(env_p) + fnamelen + 2;
            pbuf = (wchar_t *)calloc( buflen, sizeof(wchar_t));
            if(!pbuf)
            {
                 *path = L'\0';
                 errno = ENOMEM;
                 retvalue = errno;
                 goto cleanup;
            }
        }

        save_errno = errno;

        while(env_p)
        {
            save_env_p = env_p;
            env_p = _wgetpath(env_p, pbuf, buflen - fnamelen - 1);

            if( env_p == NULL && pbuf == pathbuf && errno == ERANGE)
            {
                buflen = wcslen(save_env_p) + fnamelen + 2;
                pbuf = (wchar_t *)calloc( buflen, sizeof(wchar_t));
                if(!pbuf)
                {
                     *path = L'\0';
                     errno = ENOMEM;
                     retvalue = errno;
                     goto cleanup;
                }
                env_p = _wgetpath(save_env_p, pbuf, buflen - fnamelen);
            }

            if(env_p == NULL || *pbuf == L'\0')
                break;

            /* path now holds nonempty pathname from env_p, concatenate
               the file name and go */
            /* If we reached here, we know that buflen is enough to hold
            the concatenation. If not, the getpath would have failed */

            len = wcslen(pbuf);
            p = pbuf + len;
            if ( ((c = *(p - 1)) != L'/') && (c != L'\\') &&
                 (c != L':') )
            {
                /* add a trailing '\' */
                *p++ = L'\\';
                len++;
            }
            /* p now points to character following trailing '/', '\'
               or ':' */

            _ERRCHECK(wcscpy_s(p, buflen - (p - pbuf), fname));

            if ( _waccess_s(pbuf, 0) == 0 ) {
                /* found a match, copy the full pathname into the caller's
                   buffer */
                if(len + fnamelen >= sz) {
                    *path = L'\0';

                    if(pbuf != pathbuf)
					{
                        free(pbuf);
						pbuf = NULL; // BUG FIX!!! (second free on cleanup)
					}

                    errno = ERANGE;
                    retvalue = errno;
                    goto cleanup;
                }

                errno = save_errno;

                _ERRCHECK(wcscpy_s(path, sz, pbuf));

                if(pbuf != pathbuf)
				{
                    free(pbuf);
					pbuf = NULL; // BUG FIX!!! (second free on cleanup)
				}

                retvalue = 0;
                goto cleanup;
            }

        }
        /* if we get here, we never found it, return empty string */
        *path = L'\0';
        errno = ENOENT;
        retvalue = errno;

cleanup:
        if(pbuf != pathbuf)
            free(pbuf);

        free(envbuf);

        return retvalue;
}

#endif