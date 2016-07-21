#include <stdio.h>
#include <prague.h>
#include <iface/l_time.h>

static char *weekdays[] =
{
    "Sunday",
    "Monday",
    "Thursday",
    "Wednesday",
    "Tuesday",
    "Friday",
    "Saturday"
};

static char *weekdaysrus[] =
{
    "Понедельник",
    "Вторник",
    "Среда",
    "Четверг",
    "Пятница",
    "Суббота",
    "Воскресенье",
};

void printdate(tDT *dt)
{
    tDWORD year ;
    tDWORD month ;
    tDWORD day ;
    tDWORD hour ;
    tDWORD minute ;
    tDWORD second ;
    tDWORD ns ;
    tUINT dayno;
    tUINT weekday;

    DTGet(dt,&year,&month,&day,&hour,&minute,&second,&ns);

    if ( (year-1) )
    {
        //printf("%4d\\%02d\\%02d",year,month,day);
        printf("%02d.%02d.%4d",day,month,year);
        dayno = DTDayNo(dt);
        printf(" (day:%3d, week:%2d, week:%2d)",dayno,DTWeekNo(dt,0),DTWeekNo(dt,1));
    }

    if ( hour || minute || second || ns)
    {
        if ( (year-1) )
        {
            printf(" ");
        }
        printf("%02d:%02d:%02d",hour,minute,second);
    }

    if ( ns )
    {
        printf(".%08d",ns);
    }

    if ( year-1 )
    {
        weekday = DTWeekDay(dt,1);
        printf(", %s",weekdays[weekday]);
        weekday = DTWeekDay(dt,0);
        printf(", %s",weekdaysrus[weekday]);
    }
    printf("\n");
}

void main()
{
    tDT date;
    tDT date1;
    tDWORD year = 1980;
    tDWORD month = 1;
    tDWORD day = 1;
    tDWORD hour = 0;
    tDWORD minute = 0;
    tDWORD second = 0;
    tDWORD ns = 0;
    tDWORD m;
    tDWORD d;
    tDWORD y;
    int errcount;

    DTSet(&date,year,month,day,hour,minute,second,ns);
    DTCpy(&date1, &date);
    //printdate(&date);



    for ( d = 0;d <3000;d+=10)
    {
        tDWORD y,m,d;

        DTDateShift(&date,0,0,10);
        printdate(&date);

        DTDiffGet(&date,&date1,&y,NULL,NULL,NULL,NULL,NULL);
        printf("  Difference: %d years\n",y);
        DTDiffGet(&date,&date1,NULL,&m,NULL,NULL,NULL,NULL);
        printf("  Difference: %d months\n",m);
        DTDiffGet(&date,&date1,NULL,NULL,&d,NULL,NULL,NULL);
        printf("  Difference: %d days\n",d);
        DTDiffGet(&date,&date1,&y,NULL,&d,NULL,NULL,NULL);
        printf("  Difference: %d years, %d days\n",y,d);
        DTDiffGet(&date,&date1,NULL,&m,&d,NULL,NULL,NULL);
        printf("  Difference: %d months %d days\n",m,d);
        DTDiffGet(&date,&date1,&y,&m,&d,NULL,NULL,NULL);
        printf("  Difference: %d years %d months %d days\n",y,m,d);
    }

    return ;
    for ( ;; )
    {
        tINT y,m,d;

        printdate(&date);
        printf(":");
        if (!scanf("%d %d %d",&y,&m,&d)) break;
        printf("Adding %d years, %d months, %d days\n",y,m,d);
        DTDateShift(&date,y,m,d);
    }


    errcount = 0;

    return ;
    for ( y = 1601; y <= 2000;y++)
    {
        int num;
        num = 0;
        printf("\n\f\n%d\n",y);
        for ( m = 1;m <= 12 ; m++)
        {
            for ( d = 1;d <=31 ;d++ )
            {
                year = y;
                month = m;
                day = d;
                hour = 0;
                minute = 0;
                second = 0;
                ns = 0;
                if ( DTSet(&date,year,month,day,hour,minute,second,ns) == errOK )
                {
                    //printf("   SET: %4d\\%02d\\%02d %02d:%02d:%02d.%08d\n",year,month,day,hour,minute,second,ns);
                    DTGet(&date,&year,&month,&day,&hour,&minute,&second,&ns);
                    //printf("%3d %8d ",num, (num*1000)/(m*1000));
                    if ( year != y || month != m || day != d)
                    {
                        printf("!   ");
                        printf("GET: %4d\\%02d\\%02d ",year,month,day,hour,minute,second,ns);
                        printf("SET: %4d\\%02d\\%02d\n",y,m,d,hour,minute,second,ns);
                        errcount++;
                    }
                    else
                    {
                        //printf("    ");
                        //printf("GET: %4d\\%02d\\%02d %02d:%02d:%02d.%08d\n",year,month,day,hour,minute,second,ns);
                    }
                    num++;
                }
            }
        }
    }
    printf("%d errors\n",errcount);
}
