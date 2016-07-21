#include "stdafx.h"
#include "RPCGen.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include "Options.h"
#include "..\AVPPveId.h"
#include "..\PCGError.h"
#include <time.h>
#include <algorithm>
#include <vector>
#include <string>

bool TypeToRPC(char *rpctype, char *type, DWORD id, DWORD typeoftype)
{
    if (type[0] == 'h')
    {
        sprintf(rpctype,"rpc%s",&type[1]);
        return true;
    }
    else
    {
        strcpy(rpctype,type);
        return false;
    }
}

void CRPCGen::InterfaceToIDL()
{
    char idlfullname[MAX_PATH];
    strcpy(idlfullname, m_output_folder);
    
    char acffullname[MAX_PATH];
    strcpy(acffullname, m_output_folder);

    char clifullname[MAX_PATH];
    char srvfullname[MAX_PATH];
    strcpy(clifullname, m_output_folder);
    strcpy(srvfullname, m_output_folder);

    char name[MAX_PATH];
    char rpcname[MAX_PATH];
    char cliname[MAX_PATH];
    char srvname[MAX_PATH];
    m_iface.HeaderName(name,sizeof(name));

    if (name[0] == 'i' && name[1] == '_')
    {
        sprintf(rpcname, "rpc_%s", &name[2]);
        sprintf(cliname, "cli_%s", &name[2]);
        sprintf(srvname, "srv_%s", &name[2]);
    }
    else
    {
        sprintf(rpcname, "rpc_%s", name);
        sprintf(cliname, "cli_%s", name);
        sprintf(srvname, "srv_%s", name);
    }

    char idlname[MAX_PATH];
    strcpy(idlname, rpcname);
    strcat(idlname, ".idl");

    char acfname[MAX_PATH];
    strcpy(acfname, rpcname);
    strcat(acfname, ".acf");

    strcat(idlfullname, idlname);
    strcat(acffullname, acfname);

    strcat(cliname, ".cpp");
    strcat(srvname, ".cpp");

    strcat(clifullname, cliname);
    strcat(srvfullname, srvname);

    char ifacelowcase[256];
    strcpy(ifacelowcase,m_iface.Name());
    strlwr(ifacelowcase);

    char ifaceuprcase[256];
    strcpy(ifaceuprcase,m_iface.Name());
    strupr(ifaceuprcase);

    // --------------------- ACF --------------------------

    FILE * fo = fopen(acffullname,"w+t");

    fprintf(fo,

"[explicit_handle]\n"
"interface rpc_%s\n"
"{\n"
"\n"
"}\n"

    , ifacelowcase );

    fclose(fo);


    // --------------------- IDL --------------------------

    CPubMethodInfo pmi(m_iface);
    CObjTypeInfo oti(m_types);

    char thistype[256];
    m_iface.TypeName(&oti, thistype, sizeof(thistype));

    char thisrpctype[256];
    sprintf(thisrpctype, "rpc%s", &thistype[1]);

    int methods = 0;

    for ( pmi.GoFirst(); pmi; pmi.GoNext() ) 
    {
        methods ++;
    }

    std::vector<std::string> methodstrs(methods);

    std::vector<std::string> rpctypes;
    rpctypes.push_back(thisrpctype);

    std::vector<std::string> climethodstrs(methods);
    std::vector<std::string> climethodbodystrs(methods);

    std::vector<std::string> climethodvtblstrs(methods);

    std::vector<std::string> srvmethodstrs(methods);
    std::vector<std::string> srvmethodbodystrs(methods);

    int m = 0;
    for ( pmi.GoFirst(); pmi; pmi.GoNext(), ++m ) 
    {
        char tmp[4096];

        if (pmi.IsSeparator()) 
        {
            sprintf(tmp, "\t// -- %s\n", pmi.Name());
            methodstrs[m] += tmp;

            sprintf(tmp, "// -- %s\n", pmi.Name());
            climethodstrs[m] += tmp;

            sprintf(tmp, "\t// -- %s\n", pmi.Name());
            climethodvtblstrs[m] += tmp;

            sprintf(tmp, "// -- %s\n", pmi.Name());
            srvmethodstrs[m] += tmp;
            continue;
        }

        char rettype[256];
        char retrpctype[256];

        DWORD rettypeid = pmi.TypeID();
        DWORD rettypeof = pmi.TypeOfType();

        bool isrettype = false;
        bool isrethtype = false;
        bool isrethobject = false;

        if (rettypeof != VE_NTT_GLOBALTYPE || rettypeid != tid_ERROR && rettypeid != tid_VOID)
        {
            isrettype = true;
        }
        pmi.TypeName(&oti, rettype, sizeof(rettype));
        if ( TypeToRPC(retrpctype, rettype, rettypeid, rettypeof))
        {
            isrethtype = true;
            if (strcmp(rettype, "hOBJECT") == 0)
            {
                isrethobject = true;
            }
            else if (std::find(rpctypes.begin(), rpctypes.end(), retrpctype) == rpctypes.end())
            {
                rpctypes.push_back(retrpctype);
            }
        }

        sprintf(tmp, "\ttERROR %s_%s\t( %s _this", m_iface.Name(), pmi.Name(), thisrpctype);
        methodstrs[m] += tmp;

        sprintf(tmp, "tERROR pr_call Local_%s_%s( %s _this", m_iface.Name(), pmi.Name(), thistype);
        climethodstrs[m] += tmp;

        sprintf(tmp, "\tPR_PROXY_CALL( %s_%s( PR_THIS_PROXY", m_iface.Name(), pmi.Name() );
        climethodbodystrs[m] += tmp;

        sprintf(tmp, "\tLocal_%s_%s,\n", m_iface.Name(), pmi.Name() );
        climethodvtblstrs[m] += tmp;

        sprintf(tmp, "extern \"C\" tERROR %s_%s( handle_t IDL_handle, %s _this", m_iface.Name(), pmi.Name(), thisrpctype);
        srvmethodstrs[m] += tmp;

        sprintf(tmp, "\treturn CALL_%s_%s( PR_%s_STUB", m_iface.Name(), pmi.Name(), ifaceuprcase );
        srvmethodbodystrs[m] += tmp;

        if (isrettype)
        {
            sprintf(tmp, ", [mout] %s* result", retrpctype);
            methodstrs[m] += tmp;

            sprintf(tmp, ", %s* result", rettype);
            climethodstrs[m] += tmp;

            climethodbodystrs[m] += (isrethtype)? ", PR_OUT_LOCAL(result)" : ", result";

            sprintf(tmp, ", %s* result", retrpctype);
            srvmethodstrs[m] += tmp;

            if (isrethtype) sprintf(tmp, ", (%s*)PR_OUT_REMOTE(result)", rettype);
            else sprintf(tmp, ", result");

            srvmethodbodystrs[m] += tmp;
        }

        CMethodParamInfo mpi (pmi);

        for ( mpi.GoFirst(); mpi; mpi.GoNext() )
        {
            char paramtype[256];
            char paramrpctype[256];
            DWORD paramtypeid = mpi.TypeID();
            DWORD paramtypeof = mpi.TypeOfType();

            bool ishtype = false;
            bool ishobject = false;
            bool ispointer = false;
            bool isconst = false;

            char sizeparamname[256];

            mpi.TypeName(&oti, paramtype, sizeof(paramtype));
            if (TypeToRPC(paramrpctype, paramtype, paramtypeid, paramtypeof))
            {
                ishtype = true;
                if (strcmp(paramtype, "hOBJECT") == 0)
                {
                    ishobject = true;
                }
                else if (std::find(rpctypes.begin(), rpctypes.end(), paramrpctype) == rpctypes.end())
                {
                    rpctypes.push_back(paramrpctype);
                }
            }

            if ( paramtypeof == VE_NTT_GLOBALTYPE &&
                 ( paramtypeid == tid_STRING || 
                   paramtypeid == tid_WSTRING ||
                   paramtypeid == tid_PTR 
                 ) ||
                 mpi.IsPointer()
               )
            {
                ispointer = true;

                CMethodParamInfo mpisize(pmi);
                mpisize = mpi.Data();
                mpisize.GoNext();

                mpisize.Name(sizeparamname,sizeof(sizeparamname));

                sprintf(paramrpctype, "tCHAR*");
            }

            isconst = mpi.IsConst();

            methodstrs[m] += ", ";

            climethodstrs[m] += ", ";
            climethodbodystrs[m] += ", ";
            srvmethodstrs[m] += ", ";
            srvmethodbodystrs[m] += ", ";

            if ( ispointer)
            {
                if (isconst)
                {
                    methodstrs[m] += "[in";
                }
                else
                {
                    methodstrs[m] += "[mout";
                }
                sprintf(tmp, ",size_is(%s)] ", sizeparamname);
                methodstrs[m] += tmp;
            }


            sprintf(tmp, "%s %s", paramrpctype, mpi.Name());
            methodstrs[m] += tmp;

            sprintf(tmp, "%s %s", paramtype, mpi.Name());
            climethodstrs[m] += tmp;

            if (ishtype) sprintf(tmp, "PR_REMOTE(%s)", mpi.Name());
            else if (ispointer) sprintf(tmp, "(tCHAR*)%s", mpi.Name());
            else sprintf(tmp, "%s", mpi.Name());

            climethodbodystrs[m] += tmp;

            sprintf(tmp, "%s %s", paramrpctype, mpi.Name());
            srvmethodstrs[m] += tmp;

            if      (ishobject) sprintf(tmp, "PR_LOCAL(%s)", mpi.Name());
            else if (ishtype)   sprintf(tmp, "(%s)PR_LOCAL(%s)", paramtype, mpi.Name());
            else                sprintf(tmp, "%s", mpi.Name());

            srvmethodbodystrs[m] += tmp;
        }

        char comment[256];
        pmi.ShortComment(comment, sizeof(comment));

        sprintf(tmp, " );\t\t// -- %s;", comment);
        methodstrs[m] += tmp;

        sprintf(tmp, " ) {\t\t// -- %s;", comment);
        climethodstrs[m] += tmp;

        sprintf(tmp, " ) );\n}\n");
        climethodbodystrs[m] += tmp;

        sprintf(tmp, " ) {");
        //sprintf(tmp, " ) {\t\t// -- %s;", comment);
        srvmethodstrs[m] += tmp;

        sprintf(tmp, " );\n}\n");
        srvmethodbodystrs[m] += tmp;
    }


    fo = fopen(idlfullname,"w+t");

    time_t now = ::time( 0 );
    tm* tm = ::localtime( &now );
    char timestr[80];
    ::strftime( timestr, 80, "// -------- %A,  %d %B %Y,  %H:%M --------", tm );

    char ifaceguid[256];
    m_iface.Guid(ifaceguid,sizeof(ifaceguid));
    for(int i = strlen(ifaceguid) - 1;i >= 0; --i) if (ifaceguid[i] == '_') ifaceguid[i] = '-';

    fprintf(fo, 

"// --------------------------------------------------------------------------------\n"
"// AVP Prague stamp begin( Interface header,  )\n"



"%s\n"
"// File Name   -- %s\n"
"// Copyright (c) Kaspersky Labs. 1996-2003.\n"
"// -------------------------------------------\n"
"// AVP Prague stamp end\n\n\n"


"import \"prague.h\";\n"
"import \"%s.h\";\n\n"

    , timestr, idlname, name );


    for (int t = 0; t < rpctypes.size(); t++)
    {
        //if (rpctypes[t] == "rpcOBJECT") continue;
        fprintf(fo, "typedef rpcOBJECT\t%s;\n", rpctypes[t].c_str() );
    }

    char shortcomment[256];
    char extcomment[4096];

    m_iface.ShortComment(shortcomment, sizeof(shortcomment));
    m_iface.ExtComment(extcomment, sizeof(extcomment));

    fprintf(fo,

"// --------------------------------------------------------------------------------\n"
"// AVP Prague stamp begin( Interface comment,  )\n"
"// %s interface implementation\n"
"// Short comments -- %s\n"

    , m_iface.Name(), shortcomment );

    if (*extcomment)
    {
        char extcommline[256];
        char *extcommptr = extcomment;
        do
        {
            char *newextcommptr = strstr(extcommptr,"\r\n");
            if (newextcommptr)
            {
                int size = newextcommptr - extcommptr;
                strncpy(extcommline, extcommptr, size);
                extcommline[size] = '\0';
                newextcommptr++;
                newextcommptr++;
            }
            else
            {
                strcpy(extcommline, extcommptr);
            }

            fprintf(fo, "//    %s\n", extcommline);

            extcommptr = newextcommptr;
        }
        while (extcommptr);
    }

    fprintf(fo,

"// AVP Prague stamp end\n\n\n"


"// --------------------------------------------------------------------------------\n"
"// AVP Prague stamp begin( Interface function forward declarations,  )\n"
"[ uuid (%s),\n"
"  version(1.0),\n"
"  pointer_default(unique)\n"
"]\n"
"interface rpc_%s {\n"

    , ifaceguid, ifacelowcase );

    for (m = 0; m < methods; ++m)
    {
        fprintf(fo, "%s\n", methodstrs[m].c_str());
    }

/*
    tERROR Buffer_SeekRead  ( rpcBUFFER _this, [mout] tDWORD* result, tQWORD offset, [mout,size_is(size)] tCHAR* buffer, tDWORD size ); // -- reads content of the object to the buffer;
    tERROR Buffer_SeekWrite ( rpcBUFFER _this, [mout] tDWORD* result, tQWORD offset, [in,size_is(size)]  tCHAR* buffer, tDWORD size ); // -- writes buffer to the object;
    tERROR Buffer_GetSize   ( rpcBUFFER _this, [mout] tQWORD* result, IO_SIZE_TYPE type );                       // -- returns size of the requested type of the object;
    tERROR Buffer_SetSize   ( rpcBUFFER _this, tQWORD new_size );                                                // -- resizes the object;
    tERROR Buffer_Flush     ( rpcBUFFER _this );                                                                 // -- flush internal buffers;
*/

    fprintf(fo,

"}\n"
"// AVP Prague stamp end\n"

    );


    fclose(fo);

    // --------------------- CLI --------------------------

    fo = fopen(clifullname,"w+t");

    fprintf(fo,

"#include \"stdafx.h\"\n"
"#include \"%s.h\"\n"

    , rpcname );

    for (m = 0; m < methods; ++m)
    {
        fprintf(fo, "%s\n", climethodstrs[m].c_str());
        fprintf(fo, "%s", climethodbodystrs[m].c_str());
    }

    fprintf(fo, "i%sVtbl %s_iface = {\n", m_iface.Name(), ifacelowcase);

    for (m = 0; m < methods; ++m)
    {
        fprintf(fo, "%s", climethodvtblstrs[m].c_str());
    }
    fprintf(fo, "}; // \"%s\" external virtual table prototype\n\n", m_iface.Name());

    char id[256];
    m_iface.MnemonicID(id,sizeof(id));
    fprintf(fo, "PR_IMPLEMENT_PROXY(%s, %s_iface)\n", id, ifacelowcase);

    fclose(fo);

    // --------------------- SRV --------------------------

    fo = fopen(srvfullname,"w+t");

    fprintf(fo,

"#include <prague.h>\n"
"#include <rpc.h>\n"
"#include \"%s.h\"\n"
"#include \"../pr_manager.h\"\n"

"#define PR_%s_STUB\t\t((%s)PR_THIS_STUB)\n\n"

    , rpcname, ifaceuprcase, thistype  );

    for (m = 0; m < methods; ++m)
    {
        fprintf(fo, "%s\n", srvmethodstrs[m].c_str());
        fprintf(fo, "%s", srvmethodbodystrs[m].c_str());
    }

    fprintf(fo, "PR_IMPLEMENT_STUB(%s, rpc_%s_ServerIfHandle)\n", id, ifacelowcase);

    fclose(fo);
}
