%option 8bit nodefault
%option c++
%option noyywrap
%option yyclass="idl::scaner::Scaner"

%{
//#define YY_BUF_SIZE       (64*1024)

#define YY_ALWAYS_INTERACTIVE   0
#define YY_FATAL_ERROR(msg)     idl::scaner::Scaner::fatalError(msg, lineno())
#define YY_USER_ACTION          updateContext(yytext, yyleng);

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <boost/lexical_cast.hpp>

#include "Scaner.hpp"
#include "Keywords.hpp"
#include "Exception.hpp"
#include "ErrorFactory.hpp"

#define KEYWORD(id)                                     \
  {                                                     \
    _lexeme.make(getCurrentContext(), Lexeme::Ids::id); \
    return _lexeme.getId();                             \
  }

    
static void excludeFileName(const char* yytext, std::string& file) {
  // purpose: exclude file name from strings like: #include "file.name" or #import "file.name"
  std::string text(yytext);
  
  assert(text.find('"') != std::string::npos);
  
  size_t start = text.find('"') + 1;
  size_t end = text.rfind('"');
  
  assert(start <= end);
  
  file = text.substr(start, end - start);
}
    
%}

%x COMMENT STRING

ws          [ \t\r\n]+
namestart   [A-Za-z_]
namechar    [A-Za-z_0-9]
name        {namestart}{namechar}*
digit       [0-9]
hexdigit    [0-9a-fA-F]
long_suffix  [lL]
dig_prefix  [+-]
exponent    [eE]{dig_prefix}?{digit}+
string      \"[^\"]*\"
%%

<INITIAL>"#include"[ \t]*{string}    {
                      std::string file;
                      excludeFileName(yytext, file);
                      
                      if (processInclude(file, false)) {
                        BEGIN(INITIAL);
                      }
                    }

<INITIAL>"#import"[ \t]*{string}    {
                      std::string file;
                      excludeFileName(yytext, file);
                      
                      bool inImport = isInImport();
                      
                      if (processInclude(file, true)) {
                        BEGIN(INITIAL);
                        ASSERT(isInImport());
                        if (!inImport) {
                          _lexeme.makeString(getCurrentContext(), Lexeme::Ids::ImportStart, file); 
                          return _lexeme.getId();
                        }
                      }
                    }

<INITIAL>{digit}+"."{digit}+{exponent}?  | 
<INITIAL>"."{digit}+{exponent}?     | 
<INITIAL>{digit}+"."{exponent}?     {
                      double value = boost::lexical_cast<double>(yytext);
                      _lexeme.makeFloat(getCurrentContext(), Lexeme::Ids::NumberFloat, value);
                      return _lexeme.getId();
                    }
                    
<INITIAL>{dig_prefix}?{digit}+           {
                      Int value = boost::lexical_cast<Int>(yytext);
                      _lexeme.makeInt(getCurrentContext(), Lexeme::Ids::NumberInt, value);
                      return _lexeme.getId();
                    }
<INITIAL>0x{hexdigit}+                   {
                      Int value = fromHexToInt(yytext + 2);
                      _lexeme.makeInt(getCurrentContext(), Lexeme::Ids::NumberInt, value);
                      return _lexeme.getId();
                    }
<INITIAL>0x{hexdigit}+{long_suffix}      {
                      Int value = fromHexToInt(yytext + 2);
                      _lexeme.makeInt(getCurrentContext(), Lexeme::Ids::NumberInt, value);
                      return _lexeme.getId();
                    }

<INITIAL>{name}      {
                       std::string identifier(yytext);
                       if (Keywords::exists(identifier)) {
                         _lexeme.make(getCurrentContext(), Keywords::getId(identifier));
                       } else {
                         /*if (identifier.find('_') != std::string::npos) {
                           ErrorFactory::instance().underscoresAreNotAllowed(getCurrentContext(), identifier);
                         }*/
                      
                         _lexeme.makeString(getCurrentContext(), Lexeme::Ids::Name, identifier);
                       }
                       return _lexeme.getId();
                     }

 /*<INITIAL>{string} {
                       std::string data(yytext);

                       ASSERT(data.size() >= 2);
                       ASSERT(data[0] == '"');
                       ASSERT(data[data.size() - 1] == '"');
                       data = data.substr(1, data.size() - 2);

                       _lexeme.makeString(getCurrentContext(), Lexeme::Ids::QuotedString, data);

                       return _lexeme.getId();
                     }*/

                                        
<INITIAL>"&&"        KEYWORD(And)
<INITIAL>"&"         KEYWORD(BitAnd)
<INITIAL>"~"         KEYWORD(BitInv)
<INITIAL>"|"         KEYWORD(BitOr)
<INITIAL>"^"         KEYWORD(BitXor)
<INITIAL>","         KEYWORD(Comma)
<INITIAL>"/"         KEYWORD(Div)
<INITIAL>"."         KEYWORD(Dot)
<INITIAL>"=="        KEYWORD(Eq)
<INITIAL>">="        KEYWORD(Ge)
<INITIAL>">"         KEYWORD(Gt)
<INITIAL>"<="        KEYWORD(Le)
<INITIAL>"{"         KEYWORD(LeftBrace)
<INITIAL>"("         KEYWORD(LeftParenthes)
<INITIAL>"="         KEYWORD(Let)
<INITIAL>"<"         KEYWORD(Lt)
<INITIAL>"-"         KEYWORD(Minus)
<INITIAL>"*"         KEYWORD(Mul)
<INITIAL>"!="        KEYWORD(Ne)
<INITIAL>"!"         KEYWORD(Not)
<INITIAL>"||"        KEYWORD(Or)
<INITIAL>"+"         KEYWORD(Plus)
<INITIAL>"}"         KEYWORD(RightBrace)
<INITIAL>")"         KEYWORD(RightParenthes)
<INITIAL>";"         KEYWORD(Semicolon)
<INITIAL>"..."       KEYWORD(TripleDot)
<INITIAL>":"         KEYWORD(Colon)
<INITIAL>"<<"        KEYWORD(ShiftLeft)
<INITIAL>">>"        KEYWORD(ShiftRight)

<INITIAL>"["         KEYWORD(LeftSquareBracket)
<INITIAL>"]"         KEYWORD(RightSquareBracket)

<INITIAL>"\\"        KEYWORD(BackSlash)

<INITIAL>{ws}        {}

"\""                 {
                       BEGIN(STRING);
                       _stringBuffer = "";
                     }

<STRING>"\""         {
                       BEGIN(INITIAL);
                       _lexeme.makeString(getCurrentContext(), Lexeme::Ids::QuotedString, _stringBuffer);
                       return _lexeme.getId();
                     }

<STRING>"\\\""       { _stringBuffer += '\"'; }
<STRING>"\\t"        { _stringBuffer += '\t'; }
<STRING>"\\n"        { _stringBuffer += '\n'; }

<STRING>\r           { _stringBuffer += *yytext;}
<STRING>\n           { _stringBuffer += *yytext;}
<STRING>.            { _stringBuffer += *yytext;}

<INITIAL>"//"[^\n]*  {}
<INITIAL>"/*"        BEGIN(COMMENT);

<COMMENT>[^*]*       /* eat anything that's not a '*' */
<COMMENT>"*"+[^*/]*  /* eat up '*'s not followed by '/'s */
<COMMENT>"*"+"/"     BEGIN(INITIAL);

<<EOF>>              {
                       if (_contextStack.empty()) {
                         _lexeme.make(getCurrentContext(), Lexeme::Ids::Eof);
                         assert(_lexeme.getId() == 0);
                         return _lexeme.getId(); 
                       }
                       bool inImport = isInImport();
                       popContext();
                       if (inImport && !isInImport()) {
                         _lexeme.makeString(getCurrentContext(), Lexeme::Ids::ImportEnd, ""); 
                         return _lexeme.getId();
                       }
                     }

.                    { 
                       std::string message = "unexpected symbol `";
                       message += *yytext;
                       message += "'";
                       _lexeme.makeString(getCurrentContext(), Lexeme::Ids::ScanerError, message); 
                       return _lexeme.getId(); 
                     }
%%
