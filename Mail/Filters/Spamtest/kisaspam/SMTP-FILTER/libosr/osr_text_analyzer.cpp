/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_text_analyzer.cpp
  Created: 2006/09/08
  Desc: see osr_text_analyzer.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#include "osr_text_analyzer.h"
//--------------------------------------------------------------------------------------------------------------------//
// SYMBOL_FLAGS
#define     SIZE_T_LIMIT            ((size_t)-1)
#define     SIZE_LIMIT              ((Size)-1)
//--------------------------------------------------------------------------------------------------------------------//
namespace OSR
{
//--------------------------------------------------------------------------------------------------------------------//
TextAnalyzer::TextAnalyzer ()
{
    src_symbols = NULL;
    words_gap = 0;
}
//--------------------------------------------------------------------------------------------------------------------//
TextAnalyzer::~TextAnalyzer ()
{
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::AnalyzeText(Symbols *symbols /*IN*/)
{
    if (NULL == symbols)
        return OSR_INVALID_ARGUMENT;

    src_symbols = symbols;

    ErrCode err = OSR_OK;
    
    err = BuildIndexes();
    if (OSR_OK != err)
        return err;

    SymbolsLines symbols_lines; // temporary array
                                // result array is this->lines
    err = BuildLines(symbols_lines);
    if (OSR_OK != err)
        return err;

    err = CalcWordsGap(symbols_lines);
    if (OSR_OK != err)
        return err;

    for ( size_t l = 0; l < symbols_lines.Size(); l ++ )
    {
        Line* line = lines.Add();
        if ( NULL == line )
            return OSR_MEMORY_FULL;
        err = BuildWords(symbols_lines[l], *line);
        if (OSR_OK != err)
            return err;
    }

    err = ApplyFilter();
    if (OSR_OK != err)
        return err;

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
static int osr_compare_symbols_by_center_y(const void *val1, const void *val2)
{
    Symbol *symbol1 = *(Symbol**)val1, *symbol2 = *(Symbol**)val2;
    const int y1 = symbol1->GetCenterY(), y2 = symbol2->GetCenterY();

    if ( y1 == y2 )
        return symbol1->GetCenterX() - symbol2->GetCenterX();
    return y1 - y2;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::BuildIndexes()
{
    if (NULL == src_symbols)
        return OSR_INVALID_DATA;
    if ( !y_index.AllocMore (src_symbols->Size()) )
        return OSR_MEMORY_FULL;
    for (size_t i = 0; i < src_symbols->Size(); i ++)
    {
        if ( NULL == y_index.Add() )
            return OSR_MEMORY_FULL;
        y_index[i] = src_symbols->Get(i);
    }

    y_index.Qsort(&osr_compare_symbols_by_center_y);

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::BuildLines(SymbolsLines& symbols_lines /*OUT*/)
{
    if (NULL == src_symbols)
        return OSR_INVALID_DATA;
    ErrCode err = OSR_OK;

    for ( size_t i = 0; i < src_symbols->Size(); i ++ )
    {
        Symbol *symbol = src_symbols->Get(i);
        if (NULL == symbol)
            return OSR_INTERNAL_ERROR;

        if ( SYMBOL_USED_IN_LINE & symbol->Flags() )
            continue;
        
        symbol->Flags() |= SYMBOL_USED_IN_LINE;

        MiniContainer <Symbol*> left, right;
        err = ThreadSymbols (symbol, left, dirBackward);
        if (OSR_OK != err)
            return err;
        err = ThreadSymbols (symbol, right, dirForward);
        if (OSR_OK != err)
            return err;

        SymbolsLine* symbols_line = symbols_lines.Add();
        if ( NULL ==  symbols_line)
            return OSR_MEMORY_FULL;

        size_t j = 0;
        for (j = left.Size(); j > 0; j -- )
        {
            if ( NULL == symbols_line->symbols.Add() )
                return OSR_MEMORY_FULL;
            *symbols_line->symbols.GetLast() = left[j-1];
        }

        if ( NULL == symbols_line->symbols.Add() )
            return OSR_MEMORY_FULL;
        *symbols_line->symbols.GetLast() = symbol;
        
        for (j = 0; j < right.Size(); j ++ )
        {
            if ( NULL == symbols_line->symbols.Add() )
                return OSR_MEMORY_FULL;
            *symbols_line->symbols.GetLast() = right[j];
        }
    }
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
Size TextAnalyzer::GetSpacing (const Symbol *symbol1, const Symbol *symbol2) const
{
    Size spacing = (Size)(symbol2->Left() - symbol1->Right());
    if (spacing < 0)
        return 0;
    return spacing;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::CalcWordsGap(const SymbolsLines& symbols_lines /*IN*/)
{
    size_t histogram_size = 0, i = 0;
    
    Size *spacing_histogram = NULL;

    for (size_t l = 0; l < symbols_lines.Size(); l ++)
    {
        const SymbolsLine& symbols_line = symbols_lines[l];
        for (i = 1; i < symbols_line.symbols.Size(); i ++)
        {
            Size spacing = GetSpacing(symbols_line.symbols[i-1], symbols_line.symbols[i]);
            if ( (size_t) spacing >= histogram_size)
            {
                size_t new_size = spacing + 10;
                spacing_histogram = (Size*)realloc (spacing_histogram, sizeof(Size) * new_size);
                if (NULL == spacing_histogram)
                    return OSR_MEMORY_FULL;
                memset (spacing_histogram + histogram_size, 0, (new_size - histogram_size) * sizeof(Size) );
                histogram_size = new_size;
            }
            spacing_histogram [spacing] ++;
        }
    }
    size_t max = 0;
    for (i = 1; i < histogram_size; i ++)
        if (spacing_histogram [i] > spacing_histogram [max])
            max = i;
    words_gap = (Size) (max * 2);
    if (0 == words_gap)
        words_gap = 2;

    if (spacing_histogram)
        free (spacing_histogram);
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::BuildWords (const SymbolsLine& symbols_line /*IN*/, Line& line /*OUT*/)
{
    if ( 0 == symbols_line.symbols.Size() )
        return OSR_OK;

    for ( size_t i = 0; i < symbols_line.symbols.Size(); i ++)
    {
        if ( 0 == i || GetSpacing(symbols_line.symbols[i-1], symbols_line.symbols[i]) >= words_gap ||
                       symbols_line.symbols[i-1]->GetHeight() > 3*symbols_line.symbols[i]->GetHeight() )
            if ( NULL == line.words.Add() ) // add new word to the line
                return OSR_MEMORY_FULL;

        if ( !line.words.GetLast()->symbols.Add() ) // add new symbol to the word
            return OSR_MEMORY_FULL;
        *line.words.GetLast()->symbols.GetLast() = symbols_line.symbols[i];
    }
    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
size_t TextAnalyzer::FindYIndex(const Size y, const Direction round) const
{
    size_t l = 0, r = y_index.Size()?y_index.Size()-1:0, mid = 0;
    while ( l < r - 1)
    {
        mid = (l+r)/2;
        if ( y_index[mid]->GetCenterY() > y)
            r = mid;
        else
            l = mid;
    }
    if ( y_index[l]->GetCenterY() <= y && y <= y_index[r]->GetCenterY()) // y has been found
        return ( dirBackward == round ) ? l : r;
    return SIZE_T_LIMIT;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::ThreadSymbols (Symbol *symbol, MiniContainer <Symbol*>& array, const Direction dir)
{
    /* Find next symbol, it must be nearest to the current symbol,
       and its center must be between top and bottom of current symbol.
       Save found next symbol into array.
       Process next symbol recursively */
    if ( dirForward != dir && dirBackward != dir )
        return OSR_INTERNAL_ERROR;

    symbol->Flags() |= SYMBOL_USED_IN_LINE;

    ErrCode err = OSR_OK;
    size_t next = SIZE_T_LIMIT;

    size_t t = 0, b = 0;
    Size dh = (Size)(symbol->GetHeight()/3);
    Size top = (Size)(symbol->Top() - dh);
    Size bottom = (Size)(symbol->Bottom() + dh);

    if (y_index.Size() > 1)
    {
        if ( top <= (*y_index.GetFirst())->GetCenterY() )
            t = 0;
        else
            t = FindYIndex ( top, dirForward);
        if ( bottom >= (*y_index.GetLast())->GetCenterY() )
            b = y_index.Size()-1;
        else
            b = FindYIndex ( bottom, dirBackward);
        if (SIZE_T_LIMIT == t || SIZE_T_LIMIT == b)
            return OSR_INTERNAL_ERROR;
    
        for ( ; t <= b; t ++)
            if ( 0 == (SYMBOL_USED_IN_LINE & y_index[t]->Flags()) )
            {
                if ( dirForward == dir && y_index[t]->GetCenterX() > symbol->GetCenterX() )
                {
                    if ( SIZE_T_LIMIT == next || y_index[t]->GetCenterX() < y_index[next]->GetCenterX() )
                         next = t;
                }
                else if ( dirBackward == dir && y_index[t]->GetCenterX() < symbol->GetCenterX() )
                {
                    if ( SIZE_T_LIMIT == next || y_index[t]->GetCenterX() > y_index[next]->GetCenterX() )
                         next = t;
                }
            }
    }

    if ( SIZE_T_LIMIT != next )
    {
        if (!array.Add())
            return OSR_MEMORY_FULL;
        *(array.GetLast()) = y_index[next];
        err = ThreadSymbols(y_index[next], array, dir);
    }
    return err;
}
//--------------------------------------------------------------------------------------------------------------------//
ErrCode TextAnalyzer::ApplyFilter()
{
    size_t l = 0;
    for ( l = lines.Size(); l > 0; ) // lines
    {
        l --;
        for (size_t w = lines[l].words.Size(); w > 0; ) // words
        {
            w --;
            if (    lines[l].words[w].symbols.Size() < filter.min_symbols_in_word ||
                    lines[l].words[w].symbols.Size() > filter.max_symbols_in_word )
                 lines[l].words.Remove(w); // not "good" word
        }
        if ( lines[l].words.Size() < filter.min_words_in_line )
            lines.Remove(l); // not "good" line
    }
    if ( lines.Size() < filter.min_lines_in_text ) // no "good" text
            lines.Empty();

    return OSR_OK;
}
//--------------------------------------------------------------------------------------------------------------------//
} // namespace OSR
//--------------------------------------------------------------------------------------------------------------------//
