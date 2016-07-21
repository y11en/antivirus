// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------

#include "avs.h"
#include "scan.h"

#define fCOMPARE_FLAGS  (fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE)

// -------------------------------------------

/* Предыдущий вариант
Маски без путей
1) *.exe - все файлы с маской *.exe
2) test  - все файлы с именем test

Маски с абсолютными путями
3) C:\dir\*.* - все файлы в директории C:\dir\
4) C:\dir\*.exe - все файлы с маской *.exe в директории C:\dir\
5) C:\dir\test - только файл C:\dir\test
6) C:\dir\ - все файлы в директории C:\dir\ и всех её поддиректориях

Маски с относительными путями
7) dir\*.* - все файлы во всех директориях dir\
8) dir\test - все файлы test во всех директориях dir\
9) dir\*.exe - все файлы с масками *.exe во всех директориях dir\
10) dir\ - все файлы во всех директориях dir\ и всех их поддиректориях
*/

/* Новый вариант
Маски без путей
1)  *.exe        - все файлы с маской *.exe
2)  test         - все файлы с именем test

Маски с абсолютными путями
3)  C:\dir\*.*   - все файлы в директории C:\dir\
4)  C:\dir\*     - все файлы в директории C:\dir\
5)  C:\dir\      - все файлы в директории C:\dir\
6)  C:\dir\*.exe - все файлы с маской *.exe в директории C:\dir\
7)  C:\dir\test  - файлы с именем test в директории C:\dir\

Маски с относительными путями
8)  dir\*.*      - все файлы в директории dir\
9)  dir\*        - все файлы в директории dir\
10) dir\         - все файлы в директории dir\
11) dir\*.exe    - все файлы с маской *.exe в директории dir\
12) dir\test     - файлы с именем test в директории dir\

Для случаев кроме 1 и 2 поиск НЕ ВЕДЕТСЯ В ПОДДИРЕКТОРИЯХ, если bRecurse = false
*/

// -------------------------------------------

//* s - сопоставляемая строка
//* p - шаблон. Символ \ отменяет спецзначение метасимвола.
bool CMaskChecker::MatchStrings(cStringPtr s, cStringPtr p)
{
#define  V(c)   (c.at(0,cCP_SYSTEM_DEFAULT, cSTRING_TO_UPPER))
#define  U(c,p) c=V(p), p++;

	tDWORD scc; // текущий символ строки                 
	tDWORD c, cc, lc;   // lc - предыдущий символ в [...] списке 
	tBOOL  ok, notflag;
	
	for(;;)
	{
        U(scc,s);          // очередной символ строки  
		U(c,p);
        switch (c)
		{ // очередной символ шаблона 
		case '\\':          // a*\*b 
		case '/':          // a*\*b 
            U(c,p);
            if( c == 0 ) return false; // ошибка: pattern\ 
            else goto def;
		case '[':   // любой символ из списка 
            ok = notflag = 0;
            lc = 077777;      // достаточно большое число 
            if(*p == '!')
			{ notflag=1; ++p; }
			
			U(cc,p);
            while( cc )
			{
				if(cc == ']')
				{   // конец перечисления 
					if (ok)
						break;      // сопоставилось      
					return false;     // не сопоставилось   
				}
				if(cc == '-')
				{   // интервал символов  
					U(c,p);
					--s;
					if( lc <= V(s) && V(s) >= c )
						ok++;
					else if( notflag )
						// из диапазона - неудача 
						return false;
					++s;
				}
				else
				{
					if (cc == '\\')
					{      // [\[\]] 
						U(cc,p);
						if(!cc) return false;// ошибка 
					}
					if (notflag)
					{
						if (scc && scc != (lc = cc))
							ok++;          // не входит в список 
						else return false;
					}
					else
					{
						if (scc == (lc = cc)) // входит в список 
							ok++;
					}
				}
				U(cc,p);
			}
            if (cc == 0)
			{    // конец строки 
				return false;        // ошибка 
			}
            continue;
		case '*':   // любое число любых символов 
			{
            if(!*p)  return true;
            for(--s; *s; s++)
				if(MatchStrings(s, p))
					return true;
			}
			return false;
		case 0:
            return (scc == 0);
		default: def:
            if(c != scc) return false;
            continue;
		case '?':   // один любой символ 
            if (scc == 0) return false;
            continue;
		}
	}
}

// -------------------------------------------

CMaskChecker::CMaskChecker(cStringObj& strPath, bool bShort) :
	m_strPath(strPath),
	m_bShort(bShort),
	m_nNamePos(m_strPath, m_strPath.find_last_of(L"\\/"))
{
	if( !*m_nNamePos )
		m_nNamePos = m_strPath;
	else
		m_nNamePos++;
}

bool CMaskChecker::MatchPath(cStringObj& strMask, bool bRecurse, bool bCheckAbsolute)
{
	if( !m_nNamePos && !m_nNamePos.size() )
		return false;

	bool bAbsolute = IsAbsolute(strMask);
	if( bAbsolute && !bCheckAbsolute )
		return false;

	if( m_bShort && strMask.find_first_of(L"*") != cStrObj::npos )
		return false;

	cStringPtr nMaskNamePos(strMask, strMask.find_last_of("\\/"));
	if( !*nMaskNamePos )
		nMaskNamePos = strMask;
	else
		nMaskNamePos++;

	if( m_nNamePos < nMaskNamePos )
		return false;

	if( bAbsolute ) // Маски с абсолютными путями
	{
		// Если не совпадает путь маски и объекта, то несовпадение
		if( m_strPath.compare(0, nMaskNamePos, strMask, 0, nMaskNamePos, fCOMPARE_FLAGS) )
			return false;

		// Если исключены поддиректории и есть дополнительный путь, то несовпадение
		if( !bRecurse && (tUINT)nMaskNamePos != (tUINT)m_nNamePos )
			return false;
	}
	else if( (tUINT)nMaskNamePos ) // Маски с относительными путями
	{
		// Если не найден путь маски в пути объекта, то несовпадение
		cStringPtr nMaskDirPos(m_strPath, m_strPath.find(cStringObj(strMask, 0, nMaskNamePos),
			0, cStringObj::whole, fCOMPARE_FLAGS|fSTRING_FIND_BACKWARD));

		if( !*nMaskDirPos )
			return false;

        // check subpath found to start exactly at folder begining
        if( (tUINT)nMaskDirPos != 0 && *(nMaskDirPos-1) != '\\' && *(nMaskDirPos-1) != '/' )
            return false;

		// Если исключены поддиректории и есть дополнительный путь после найденного пути маски, то несовпадение
		if( !bRecurse && (((tUINT)nMaskDirPos + (tUINT)nMaskNamePos) != (tUINT)m_nNamePos) )
			return false;
	}
	
	// Если маска "все"
	if( !*nMaskNamePos || !nMaskNamePos.compare("*") || !nMaskNamePos.compare("*.*") )
	{
		// Если имя не пустое, то совпадение
		if( !!*m_nNamePos )
			return true;
		
		// Если директория и рекурсивно, то совпадение
		if( bRecurse && (tUINT)nMaskNamePos )
			return true;
		
		// Иначе несовпадение
		return false;
	}

	// Если маска заканчивается на ".*" или на ".", и имя файла без расширения, то обрезаем маску по точку
	tDWORD nMaskSize = nMaskNamePos.size();
	if (*m_nNamePos && nMaskSize >= 2)
	{
		tDWORD nPointEndOffset = 0;
		if (nMaskNamePos[nMaskSize - 2] == L'.' && nMaskNamePos[nMaskSize - 1] == L'*')
			nPointEndOffset = 2;
		else if (nMaskNamePos[nMaskSize - 1] == L'.')
			nPointEndOffset = 1;
		if (nPointEndOffset)
		{
			tDWORD nExtPos = m_strPath.find(L".", (tUINT)m_nNamePos, cStrObj::whole, fSTRING_FIND_BACKWARD);
			if( nExtPos == cStrObj::npos )
			{
				cStrObj strNameOnlyMask;
				strNameOnlyMask.assign(strMask, (tUINT)nMaskNamePos, nMaskSize - nPointEndOffset);
				return MatchStrings(m_nNamePos, cStringPtr(strNameOnlyMask));
			}
		}
	}
	
	return MatchStrings(m_nNamePos, nMaskNamePos);
}

// -------------------------------------------
