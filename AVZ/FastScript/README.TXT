FastScript 1.9
for Delphi 4-7/2005/2006 & C++Builder 4-6 & Kylix 1-3


READ THIS FILE ACCURATELY BEFORE INSTALLING FastScript!

----------------------------------------------------------------------------
TABLE OF CONTENTS

1. Introduction
2. Features
3. Missing features
4. Installing
5. Registering


----------------------------------------------------------------------------
1. INTRODUCTION
   FastScript is scripting library. It is intended for programmers who want
to add scripting ability to their projects. FastScript is written on 100%
Object Pascal and can be installed in Delphi 4-7/2005/2006, C++Builder 4-6 
and Kylix 1-3.
   Unique feature of FastScript is ability to use several languages
(PascalScript, C++Script, JScript, BasicScript), so you can write scripts 
using your favourite language.

   FastScript is shareware. See "Registering" topic for details.


----------------------------------------------------------------------------
2. FEATURES

- Classes and functions tree-view
- Syntax highlithing memo with bookmarks
- Multi-language architecture allows you to use many languages (at this
  moment - PascalScript, C++Script, JScript, BasicScript).
- Standard language features: variables, constants, procedures, functions
  (nested allowed) with var/const/default parameters, all standard operators
  and statements (includind case, try/finally/except, with), types (int, float,
  bool, char, string, multi-dimensional array, enum, variant), classes (with 
  methods, events, properties, index and default properties).
- Strict type checking.
- Access to any object inside your application. Standard libraries to access
  to base classes, controls, forms and DB. Easily expandable library
  architecture.
- Small footprint - 90-160Kb depending on modules used.
- Can be used in multithread environment.


----------------------------------------------------------------------------
3. MISSING FEATURES
- No type definition (records, classes) inside script; no records, no pointers,
  no sets (but you can use 'IN' operator - "a in ['a'..'c','d']"), no
  shortstrings, no GOTO statement, no external dll calls.
- C++Script: no octal constants; no 'break' in the SWITCH operator (SWITCH
  works like Pascal CASE); '++' and '--' operators allowed only after the
  variable, i.e. '++i' is not allowed; '--', '++' and '=' operators doesn't
  return a value, i.e. 'if(i++)' is not allowed; identifiers are 
  case-insensitive; NULL constant is Pascal Null - use nil instead of NULL.
- JScript and BasicScript - see syntax diagram.


----------------------------------------------------------------------------
4. INSTALLING

  Follow to instructions of installation program.

----------------------------------------------------------------------------
5. REGISTERING
   FastScript is shareware. You can try trial version of FastScript, which
works only under Delphi IDE and shows nag message at aplication start.
   Single copy of FastScript costs $79. You can register it with RegisterNow!
or ShareIt online services which accepts all kinds of payment. 
   To register, go web page http://www.fast-report.com, "Purchase"
Link. Registered users gets technical support and can upgrade to the next
version of FastScript 1.x with no additional payment.

----------------------------------------------------------------------------
Author:    Alexander Tzyganenko
e-mail:    tz@fast-report.com
home page: http://www.fast-report.com
           (c) 2003-2005 by Alexander Tzyganenko, Fast Reports, Inc.
