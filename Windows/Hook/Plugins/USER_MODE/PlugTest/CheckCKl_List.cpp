#include "kl_list.h"
#include "vector"

struct SItem
{
    SItem(ULONG val)
    {
        ID = val;
    }
    QUEUE_ENTRY;
    ULONG   ID;

    bool operator <  ( SItem Item )
    {
        return  ( ID < Item.ID );
    }
    bool operator >  ( SItem Item )
    {
        return  ( ID > Item.ID );
    }
    bool operator == ( SItem Item )
    {
        return  ( ID == Item.ID );
    }
    bool operator <= ( SItem Item )
    {
        return  ( ID <= Item.ID );
    }
    bool operator >= ( SItem Item )
    {
        return  ( ID >= Item.ID );
    }
};

void    PrintItems(CKl_List<SItem>* List)
{
    SItem*  Item = NULL;

    printf ("print list\n");
    while ( Item = List->Next(Item) )
    {
        printf ("%d ", Item->ID );
    }    

    printf ("\n");

    Item = NULL;
    while ( Item = List->Prev(Item) )
    {
        printf ("%d ", Item->ID );
    }

    printf("\n\n");
}

void
PrintItem(SItem*    Item)
{
    if ( Item )
    {
        printf("%d", Item->ID );
    }
    else
    {
        printf("Null");
    }
}

bool
CheckFind_PTR()
{
    printf("--------- check Find_PTR ----------\n");
    CKl_List<SItem> pTestList;
    
    SItem i0(0); SItem i1(1); SItem i2(2);SItem i3(3); SItem i4(100); SItem i5(200);
    
    SItem*  Item;

    pTestList.InsertTailNoLock(&i0);
    pTestList.InsertTailNoLock(&i1);
    pTestList.InsertTailNoLock(&i2);
    pTestList.InsertTailNoLock(&i3);
    pTestList.InsertTailNoLock(&i4);
    pTestList.InsertTailNoLock(&i5);
    
    // must be found
    Item = pTestList.Find_PTR(FIELD_OFFSET(SItem, ID), (void*)0);
    printf("%s\n", ( Item && Item->ID == 0) ? "ok" : "failed");

    Item = pTestList.Find_PTR(FIELD_OFFSET(SItem, ID), (void*)1);
    printf("%s\n", ( Item && Item->ID == 1) ? "ok" : "failed");

    Item = pTestList.Find_PTR(FIELD_OFFSET(SItem, ID), (void*)200);
    printf("%s\n", ( Item && Item->ID == 200) ? "ok" : "failed");

    Item = pTestList.Find_PTR(FIELD_OFFSET(SItem, ID), (void*)100);
    printf("%s\n", ( Item && Item->ID == 100) ? "ok" : "failed");

    // must not be found
    Item = pTestList.Find_PTR(FIELD_OFFSET(SItem, ID), (void*)6);
    printf("%s\n", ( Item && Item->ID == 6) ? "failed" : "ok");

    Item = pTestList.Find_PTR(FIELD_OFFSET(SItem, ID), (void*)99);
    printf("%s\n", ( Item && Item->ID == 99) ? "failed" : "ok");
    
    return true;
}

bool
CheckHeadTail()
{
    CKl_List<SItem> pTestList;
    
    SItem i0(10); SItem i1(20); SItem i2(30);SItem i3(40); SItem i4(100); SItem i5(200);
    
    SItem*  Item;

    pTestList.InsertTailNoLock(&i0);
    pTestList.InsertTailNoLock(&i1);
    pTestList.InsertTailNoLock(&i2);
    pTestList.InsertTailNoLock(&i3);
    pTestList.InsertTailNoLock(&i4);
    pTestList.InsertTailNoLock(&i5);
    
    printf ("------- check Head ----------\n");    
    Item = pTestList.Head();        
    printf ( "%s\n", ( *Item == SItem ( 10 ) ) ? "ok" : "failed" );

    printf ("------- check Tail ----------\n");
    Item = pTestList.Tail();        
    printf ( "%s\n", ( *Item == SItem ( 200 ) ) ? "ok" : "failed" );
    return true;
}

bool
CheckNextPrev()
{
    CKl_List<SItem> pTestList;
    
    SItem i0(10); SItem i1(20); SItem i2(30);SItem i3(40); SItem i4(100); SItem i5(200);

    pTestList.InsertTailNoLock(&i0);
    pTestList.InsertTailNoLock(&i1);
    pTestList.InsertTailNoLock(&i2);
    pTestList.InsertTailNoLock(&i3);
    pTestList.InsertTailNoLock(&i4);
    pTestList.InsertTailNoLock(&i5);

    printf ("------- check Next ----------\n");    
    printf ( "%s\n", ( pTestList.Next( NULL ) == &i0 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Next( &i0 )  == &i1 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Next( &i1 )  == &i2 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Next( &i2 )  == &i3 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Next( &i3 )  == &i4 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Next( &i4 )  == &i5 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Next( &i5 )  == NULL ) ? "ok" : "failed" );

    printf ("------- check Prev ----------\n");    
    printf ( "%s\n", ( pTestList.Prev( NULL ) == &i5 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Prev( &i5 )  == &i4 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Prev( &i4 )  == &i3 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Prev( &i3 )  == &i2 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Prev( &i2 )  == &i1 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Prev( &i1 )  == &i0 ) ? "ok" : "failed" );
    printf ( "%s\n", ( pTestList.Prev( &i0 )  == NULL ) ? "ok" : "failed" );

    return true;
}

bool
CheckConsistency( CKl_List<SItem>* pTestList, SItem* p, SItem* ii, SItem* n)
{
    return (
            pTestList->Next( ii ) == n && pTestList->Prev( ii  ) == p &&
            pTestList->Next( p ) == ii && pTestList->Prev( n ) == ii 
            );
}

bool
CheckInsert()
{   
    CKl_List<SItem> pTestList;
    
    SItem i0(10); SItem i1(20); SItem i2(30);SItem i3(40); SItem i4(100); SItem i5(200);
    
    SItem ii0(1);
    SItem ii1(15);
    SItem ii2(25);
    SItem ii3(300);
    
    SItem*  Item, *n,*p;

    pTestList.InsertTailNoLock(&i0);
    pTestList.InsertTailNoLock(&i1);
    pTestList.InsertTailNoLock(&i2);
    pTestList.InsertTailNoLock(&i3);
    pTestList.InsertTailNoLock(&i4);
    pTestList.InsertTailNoLock(&i5);

    printf ("------- check Insert Before ----------\n");    
    
    pTestList.InsertBefore(&ii0, &i0);
    printf("%s\n", CheckConsistency(&pTestList, NULL, &ii0, &i0 ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    pTestList.InsertBefore(&ii0, &i1);
    printf("%s\n", CheckConsistency(&pTestList, &i0, &ii0, &i1 ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    // Если BeforeItem == NULL, то вставка в голову    
    pTestList.InsertBefore(&ii0, NULL);
    printf("%s\n", CheckConsistency(&pTestList, NULL, &ii0, &i0 ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    printf ("------- check Insert After  ----------\n");
    
    pTestList.InsertAfter(&ii0, &i0);
    printf("%s\n", CheckConsistency(&pTestList, &i0, &ii0, &i1 ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    
    pTestList.InsertAfter(&ii0, &i5);
    printf("%s\n", CheckConsistency(&pTestList, &i5, &ii0, NULL ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );
    
    // Если AfterItem == NULL, то вставка в хвост
    pTestList.InsertAfter(&ii0, NULL);
    printf("%s\n", CheckConsistency(&pTestList, &i5, &ii0, NULL ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    return true;
}

bool
CheckInsert1()
{
    CKl_List<SItem> pTestList;
    
    SItem i0(10); SItem i1(20); SItem i2(30);SItem i3(40); SItem i4(100); SItem i5(200);
    
    SItem ii0(1);
    SItem ii1(15);
    SItem ii2(25);
    SItem ii3(300);
    
    SItem*  Item, *n,*p;

    pTestList.InsertTailNoLock(&i0);
    pTestList.InsertTailNoLock(&i1);
    pTestList.InsertTailNoLock(&i2);
    pTestList.InsertTailNoLock(&i3);
    pTestList.InsertTailNoLock(&i4);
    pTestList.InsertTailNoLock(&i5);

    printf ("------- check Insert Head  ----------\n");

    pTestList.InsertHeadNoLock( &ii0 );
    printf("%s\n", CheckConsistency(&pTestList, NULL, &ii0, &i0 ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    printf ("------- check Insert Tail  ----------\n");
    pTestList.InsertTailNoLock( &ii0 );
    printf("%s\n", CheckConsistency(&pTestList, &i5, &ii0, NULL ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    return true;
}

bool
CheckInsertByOrder()
{
    CKl_List<SItem> pTestList;
    
    SItem i0(10); SItem i1(20); SItem i2(30);SItem i3(40); SItem i4(100); SItem i5(200);
    
    SItem ii0(1);
    SItem ii1(15);
    SItem ii2(25);
    SItem ii3(300);
    
    SItem*  Item, *n,*p;

    pTestList.InsertByOrderNoLock(&i0);
    pTestList.InsertByOrderNoLock(&i1);
    pTestList.InsertByOrderNoLock(&i2);
    pTestList.InsertByOrderNoLock(&i3);
    pTestList.InsertByOrderNoLock(&i4);
    pTestList.InsertByOrderNoLock(&i5);

    printf ("------- check InsertByOrderNoLock  ----------\n");
    
    pTestList.InsertByOrderNoLock( &ii0 );    
    printf("%s\n", CheckConsistency(&pTestList, &i0, &ii0, NULL ) ? "ok" : "failed" );
    pTestList.Remove( &ii0 );

    pTestList.InsertByOrderNoLock( &ii1 );    
    printf("%s\n", CheckConsistency(&pTestList, &i1, &ii1, &i0 ) ? "ok" : "failed" );
    pTestList.Remove( &ii1 );

    pTestList.InsertByOrderNoLock( &ii2 );    
    printf("%s\n", CheckConsistency(&pTestList, &i2, &ii2, &i1 ) ? "ok" : "failed" );
    pTestList.Remove( &ii2 );

    pTestList.InsertByOrderNoLock( &ii3 );    
    printf("%s\n", CheckConsistency(&pTestList, NULL, &ii3, &i5 ) ? "ok" : "failed" );
    pTestList.Remove( &ii3 );
    
    return true;
}

bool
CheckRemove()
{
    CKl_List<SItem> pTestList;
    
    SItem i0(10); SItem i1(20); SItem i2(30);SItem i3(40); SItem i4(100); SItem i5(200);
    
    SItem ii0(1);
    SItem ii1(15);
    SItem ii2(25);
    SItem ii3(300);
    
    SItem*  Item, *n,*p;

    pTestList.InsertTailNoLock(&i0);
    pTestList.InsertTailNoLock(&i1);
    pTestList.InsertTailNoLock(&i2);
    pTestList.InsertTailNoLock(&i3);
    pTestList.InsertTailNoLock(&i4);
    pTestList.InsertTailNoLock(&i5);

    printf ("------- check Remove  ----------\n");

    Item = pTestList.RemoveHead();
    printf("%s\n", *Item == SItem( 10 ) && CheckConsistency(&pTestList, NULL, &i1, &i2 ) ? "ok" : "failed" );
    pTestList.InsertHead( Item );

    Item = pTestList.RemoveTail();
    printf("%s\n", *Item == SItem( 200 ) && CheckConsistency(&pTestList, &i3, &i4, NULL ) ? "ok" : "failed" );
    pTestList.InsertTail( Item );

    Item = pTestList.Remove(&i2);
    printf("%s\n", *Item == SItem( 30 ) && CheckConsistency(&pTestList, &i1, &i3, &i4 ) ? "ok" : "failed" );

    
    return true;
}

void TestInitCKl_List(std::vector<TestFunc>& Test)
{
    printf ("*********************************************************************\n");
    printf ("*******************Check_CKl_List************************************\n");
    printf ("*********************************************************************\n");
    
    Test.push_back( &CheckFind_PTR              );
    Test.push_back( &CheckHeadTail          );
    Test.push_back( &CheckNextPrev          );
    Test.push_back( &CheckInsert            );
    Test.push_back( &CheckInsert1           );
    Test.push_back( &CheckInsertByOrder     );
    Test.push_back( &CheckRemove            );
}