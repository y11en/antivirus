/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_text_analyzer.h
  Created: 2006/09/08
  Desc: class TextAnalyzer detect lines and words of symbols (see class Symbols,
        file osr_symbol.h)
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
#ifndef __osr_text_analyzer_h__
#define __osr_text_analyzer_h__
//--------------------------------------------------------------------------------------------------------------------//
#include "array.h"
#include "osr_common.h"
#include "osr_symbol.h"
//--------------------------------------------------------------------------------------------------------------------//
#define DEFAULT_MIN_SYMBOLS_IN_WORD     2
#define DEFAULT_MAX_SYMBOLS_IN_WORD     17
#define DEFAULT_MIN_WORDS_IN_LINE       3
#define DEFAULT_MIN_LINES_IN_TEXT       3
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
struct SymbolsLine
{
    Container <const Symbol*> symbols;
};
typedef Container <SymbolsLine> SymbolsLines;
//--------------------------------------------------------------------------------------------------------------------//
struct Word
{
    Container <const Symbol*> symbols;
};
//--------------------------------------------------------------------------------------------------------------------//
struct Line
{
    Container <Word> words;
};
typedef Container <Line> Lines;
//--------------------------------------------------------------------------------------------------------------------//
class TextAnalyzer
{
public:

    struct Filter
    {
        size_t
            min_symbols_in_word,
            max_symbols_in_word,
            min_words_in_line,
            min_lines_in_text;
                
        Filter()
        {
            min_symbols_in_word = DEFAULT_MIN_SYMBOLS_IN_WORD;
            max_symbols_in_word = DEFAULT_MAX_SYMBOLS_IN_WORD;
            min_words_in_line   = DEFAULT_MIN_WORDS_IN_LINE;
            min_lines_in_text   = DEFAULT_MIN_LINES_IN_TEXT;
        }
    };

    /* Initializing interface */

    TextAnalyzer ();
    virtual ~TextAnalyzer ();

    void SetFilter (const Filter& filter_arg) {filter = filter_arg;}

    /* Processing interface */

    ErrCode AnalyzeText(Symbols *symbols /*IN*/);

    /* Resulting interface */

    Lines* GetLines() {return &lines;}
    const Lines* GetLines() const {return &lines;}

private:

    TextAnalyzer(const TextAnalyzer&);
    TextAnalyzer& operator= (const TextAnalyzer&);

    ErrCode BuildIndexes ();
    ErrCode BuildLines (SymbolsLines& symbol_lines /*OUT*/);
    ErrCode BuildWords (const SymbolsLine& symbols_line /*IN*/, Line& line /*OUT*/);
    ErrCode ApplyFilter();

    enum Direction { dirForward, dirBackward};

    size_t FindYIndex(const short y, const Direction round) const;
    ErrCode ThreadSymbols (Symbol *symbol /*IN,OUT*/, MiniContainer <Symbol*>& array /*IN,OUT*/,
                           const Direction dir);
    short GetSpacing (const Symbol *symbol1 /*IN*/, const Symbol *symbol2 /*IN*/) const;
    ErrCode CalcWordsGap(const SymbolsLines& symbol_lines /*IN*/);
    
    Symbols *src_symbols; /*IN*/
    MiniContainer <Symbol*> y_index;
    Size words_gap;
    Lines lines;
    Filter filter;
};
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
#endif // __osr_text_analyzer_h__
//--------------------------------------------------------------------------------------------------------------------//
