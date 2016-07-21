////  this file is copy of RECORDS.H file from Prague sources

#ifndef _RECORDS_H_D61649F2_5780_40ff_9DAC_5411486143D4
#define _RECORDS_H_D61649F2_5780_40ff_9DAC_5411486143D4

#pragma pack (1)

struct R_File
{
    unsigned char Len1;
    unsigned short Off1;                   //                      If Len1==0  LinkIdx
    unsigned short ControlWord;
    unsigned char Type;
    // dos xms part
    unsigned long Sum1;
    unsigned char Len2;
    unsigned short Off2;
    unsigned long Sum2;
    unsigned short LinkIdx;                //      ---             Idx
    unsigned long NameIdx;                //      ---             Idx             PTR?
    //      cure part
    unsigned char CureMethod;             // 8th bit set means Page=File
    unsigned short CureData1;
    unsigned short CureData2;
    unsigned short CureData3;
    unsigned short CureData4;
    unsigned short CureData5;
};

struct R_Sector
{
    unsigned char Len1;
    unsigned short Off1;                   //                      If Len1==0  LinkIdx
    unsigned short ControlWord;
    // dos xms part
    unsigned long Sum1;
    unsigned char Len2;
    unsigned short Off2;
    unsigned long Sum2;
    unsigned short LinkIdx;                //      ---             Idx
    unsigned long NameIdx;                //      ---             Idx             PTR?
    //      cure part
    unsigned char CureObjAndMethod1;
    unsigned short CureAddr11;
    unsigned short CureAddr12;
    unsigned char CureData1;
    unsigned char CureObjAndMethod2;
    unsigned short CureAddr21;
    unsigned short CureAddr22;
    unsigned char CureData2;
};

struct R_Memory
{
    unsigned char Method;
    unsigned short Offset;
    unsigned short Segment;
    unsigned char Len1;
    unsigned long Sum1;
    unsigned char ControlByte;
    unsigned short LinkIdx;                //      ---             Idx
    unsigned long NameIdx;                //      ---             Idx             PTR?
    //      cure part
    unsigned short CureOffset;
    unsigned char CureLength;
    unsigned char CureData[5];
};

struct R_CA
{
    unsigned char Len1;
    unsigned short Off1;                   //                      If Len1==0  LinkIdx
    unsigned short ControlWord;
    unsigned char Type;
    // dos xms part
    unsigned long Sum1;
    unsigned char Len2;                   //                      If !=0 then false alarm.
    unsigned short Off2;
    unsigned long Sum2;
    unsigned short LinkIdx;                //      ---             Idx
    unsigned long NameIdx;                //      ---             Idx             PTR?
};

struct R_Unpack
{
    unsigned char Len1;
    unsigned short Off1;                   //                      If Len1==0  LinkIdx
    unsigned short ControlWord;
    unsigned char Type;
    // dos xms part
    unsigned long Sum1;
    unsigned char Len2;
    unsigned short Off2;
    unsigned long Sum2;
    unsigned short LinkIdx;                //      ---             Idx
    unsigned long NameIdx;                //      ---             Idx             PTR?
};

struct R_Extract
{
    unsigned char Len1;
    unsigned short Off1;                   //                      If Len1==0  LinkIdx
    unsigned short ControlWord;
    unsigned char Type;
    // dos xms part
    unsigned long Sum1;
    unsigned char Len2;
    unsigned short Off2;
    unsigned long Sum2;
    unsigned short LinkIdx;                //      ---             Idx
    unsigned long NameIdx;                //      ---             Idx             PTR?
};


#pragma pack()

#endif  // #ifndef _RECORDS_H_D61649F2_5780_40ff_9DAC_5411486143D4
