#include "comdefs.h"

#ifdef WIN32

    /// take day of week
    static bool takeWeekDay(const char *&s, int &dayNumber)
    {
        static const char *daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
        for(unsigned i = 0; i < sizeof(daysOfWeek) / sizeof(daysOfWeek[0]); ++i)
        {
            if(strlen(s) < strlen(daysOfWeek[i]))
                continue;
        
            if(memcmp(daysOfWeek[i], s, strlen(daysOfWeek[i])) == 0)
            {
                dayNumber = i;
                s += strlen(daysOfWeek[i]);
                return true;
            }
        }
    
        // day of week matches no pattern
        return false;
    }

    /// take month
    static bool takeMonth(const char *&s, int &monthNumber)
    {
        static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
        for(unsigned i = 0; i < sizeof(months) / sizeof(months[0]); ++i)
        {
            if(strlen(s) < strlen(months[i]))
                continue;
        
            if(memcmp(months[i], s, strlen(months[i])) == 0)
            {
                monthNumber = i;
                s += strlen(months[i]);
                return true;
            }
        }
    
        // month matches no pattern
        return false;
    }

    /// take digits from date string
    static bool takeDigits(const char *&s, int &number, const unsigned maxdigits)
    {
        if(!*s)
            return false;

        std::string temp;

        // copy up to maxdigits characters
        for(unsigned i = 0; i < maxdigits; ++i)
        {
            temp.append(1, s[0]);
            s++;
            if(!*s  || !isdigit(*s))
                break;
        }
    
        // try to convert 
        return (1 == sscanf(temp.c_str(), "%d", &number));
    }


    const char *strptime(const char *s, const char *format, struct tm *tm)
    {
        if(!s || !format)
            return 0;

        memset(tm, 0, sizeof(struct tm));

        const char * _s = s;
        const char * _f = format;

        // skip spaces
        while(*_s && (' ' == *_s || '\t' == *_s))
            ++_s;
        while(*_f && (' ' == *_f || '\t' == *_f))
            ++_f;

        // proceed format line
        for(;;) 
        {
            // take the next tag of format line 
            switch(*_f)
            {
            case '%':

                // proceeding format line character
                if(*(_f++))
                {
                    switch(*_f)
                    {
                    case 'd':
                        // processing day of month
                        {
                            int monthDay = 0;
                            if(!takeDigits(_s, monthDay, 2))
                                return 0;
                        
                            tm->tm_mday = monthDay;
                        }
                        break; 
                    
                    case 'm':
                        // processing month digit
                        {
                            int month = 0;
                            if(!takeDigits(_s, month, 2))
                                return 0;
                        
                            tm->tm_mon = month - 1;
                        } 
                        break; 
                    
                    case 'Y':
                        // processing year digit
                        {
                            int year = 0;
                            if(!takeDigits(_s, year, 4) )
                                return 0;
                        
                            tm->tm_year = year - 1900;
                        } 
                        break; 

                    case 'H':
                        // processing hour digit
                        {
                            int hour = 0;
                            if(!takeDigits(_s, hour, 2))
                                return 0;
                        
                            tm->tm_hour = hour;
                        } 
                        break; 

                    case 'M':
                        // processing minutes digit
                        {
                            int minute = 0;
                            if(!takeDigits(_s, minute, 2))
                                return 0;
                        
                            tm->tm_min = minute;
                        } 
                        break;

                    case 'S':
                        // processing second digit
                        {
                            int second = 0;
                            if(!takeDigits(_s, second, 2))
                                return 0;

                            tm->tm_sec = second;
                        }
                        break;

                    case 'a':
                        // processing day of week name
                        {
                            int dayWeek = 0;
                            if(!takeWeekDay(_s, dayWeek))
                                return 0;

                            tm->tm_wday = dayWeek;
                        }
                        break;

                    case 'b':
                        // processing month name
                        {
                            int month = 0;
                            if(!takeMonth(_s, month))
                                return 0;

                            tm->tm_mon = month;
                        }
                        break;

                    case 'T':
                        // processing hour/minute/second
                        {
                            int hour = 0;
                            if(!takeDigits(_s, hour, 2))
                                return 0;

                            tm->tm_hour = hour;
                        }

                        // wrong delimiter
                        if(*_s != ':')
                            return 0;

                        ++_s;
                        {
                            int minute = 0;
                            if(!takeDigits(_s, minute, 2))
                                return 0;

                            tm->tm_min = minute;
                        }

                        // wrong delimiter
                        if(*_s != ':')
                            return 0;

                        ++_s;
                        {
                            int second = 0;
                            if(!takeDigits(_s, second, 2))
                                return 0;

                            tm->tm_sec = second;
                        }
                        break;

                    default:
                        // unknown or unsupported format
                        return 0;
                    }
                }
                ++_f;
                break;
            
            case '\0':
                // end of format line reached, everything Ok
                return s;
          
            default:
                // extra format character (delimiter)

                // error: string delimiter does not match
                if(*_s != *_f)
                    return 0;

                // syncing string with format line
                ++_f;
                ++_s;
                break;
            }
        }
        return 0;
    }

#endif  // WIN32
