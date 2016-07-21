// HtmlFilter.h: interface for the CHtmlFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLFILTER_H__1FE509B1_01DA_4189_BA19_4648A53AEF93__INCLUDED_)
#define AFX_HTMLFILTER_H__1FE509B1_01DA_4189_BA19_4648A53AEF93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <sstream>
#include "html_parser/htmparse.h"

#define HTML_COLOR_SEEM_LENGHT	5	// Максимальное отличие двух цветов, незаметное человеческому глазу

class CHtmlFilter  
{
public:
	CHtmlFilter();
	virtual ~CHtmlFilter();

	typedef vector<string> tokens_t;
	struct results_t 
	{
		unsigned long ulKnownDefsCount;			// Количество тегов в коде HTML
		unsigned long ulUnknownDefsCount;		// Количество несуществующих тегов
		unsigned long ulSeemsColors;			// Количество блоков текста с цветом фона
		unsigned long ulSmallText;				// Количество блоков с очень маленьким текстом
		unsigned long ulInvisibleCharCount;		// Количество невидимых символов
		unsigned long ulVisibleCharCount;		// Количество видимых символов
		unsigned long ulObjects;				// Количество встроенных объектов (в том числе внешних)
		unsigned long ulExternalObjects;		// Количество встроенных внешних объектов
		unsigned long ulScripts;				// Количество скриптов
		unsigned long ulHiddenElements;			// Количество скрытых элементов (style="visibility:hidden")
		std::string   szVisibleText;			// Видимый текст
		std::string   szCharset;				// Кодировка документа
		tokens_t tokens;						// Вектор токенов

		void clear()
		{
			ulKnownDefsCount = 0;
			ulUnknownDefsCount = 0;
			ulSeemsColors = 0;
			ulSmallText = 0;
			ulInvisibleCharCount = 0;
			ulVisibleCharCount = 0;
			ulObjects = 0;
			ulExternalObjects = 0;
			ulScripts = 0;
			ulHiddenElements = 0;
			szVisibleText.erase();
			szCharset.erase();
			tokens.clear();
		};
	};

	struct state_t								// Состояние докумета в данной точке парсинга
	{
		bool bNeedDelimiter;					// При выводе текста необходимо отделить его от предыдущего блока
		bool bCheckOnly;						// Текущий элемент требуется только оценить, но не выводить на печать
		unsigned long rgb_color;				// Текущий цвет текста
		unsigned long rgb_bgcolor;				// Текущий цвет фона
		double font_size;						// Текущий размер символов
		
		void clear()
		{
			bNeedDelimiter = false;
			bCheckOnly = false;
			rgb_color = 0;
			rgb_bgcolor = 0xffffff;
			font_size = 7;
		}
	};

	void Process(hIO is, results_t& results);

private:
	results_t m_results;
	bool m_bBodyPresented;

	void print_element(
						html_element* pElement, 
						const state_t &state
						);

	bool CheckColors(
					 html_elt_tag* pNextTag,
					 unsigned long &rgb_color,
					 unsigned long &rgb_bgcolor
					 );

	bool CheckFontSize(
					 html_elt_tag* pNextTag,
					 double& font_size
					 );
	bool CheckHiddenText(html_elt_tag* pNextTag);
//	void ExtractTokenFromURL(const char* url);

};

#endif // !defined(AFX_HTMLFILTER_H__1FE509B1_01DA_4189_BA19_4648A53AEF93__INCLUDED_)
