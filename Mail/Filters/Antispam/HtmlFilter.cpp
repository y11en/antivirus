// HtmlFilter.cpp: implementation of the CHtmlFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HtmlFilter.h"
#include "toupper.h"
#include "ExtractTokenFromURL.h"
#ifdef _USE_XYZ
	#include <math.h>
	#include "html_parser/colors.c"
#endif//_USE_XYZ

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmlFilter::CHtmlFilter():
	m_bBodyPresented(false)
{
	m_results.clear();
}

CHtmlFilter::~CHtmlFilter()
{

}

void CHtmlFilter::Process(hIO is, results_t& results)
{
	PR_TRACE(( is, prtNOTIFY, "CHtmlFilter::Process => Enter "));
	m_bBodyPresented = false;
	m_results.clear();
	html_parser hParser;
	html_elt_list hList;
	hParser.parse(is, hList);
	m_results.ulUnknownDefsCount = hParser.get_unknows_defs()->size();
	html_element* pElement = hList.head;
	state_t state;
	state.clear();

	do
	{
		print_element(pElement, state);
	}
	while ( pElement && ( pElement = pElement->next) );
	
	results = m_results;
	PR_TRACE(( is, prtNOTIFY, "CHtmlFilter::Process => Leave "));
}

const char* content_type_get_charset(const char* value)
{
	const char* charset = 0;
	char* value1 = _strupr(_strdup(value));
	char* pos = strstr(value1, "CHARSET=");
	if ( pos ) 
		charset = pos - value1 + value + strlen("CHARSET") + sizeof(char);
	free(value1);
	return charset;
}

void CHtmlFilter::print_element(
				   html_element* pElement, 
				   const state_t &state
)
{
	if ( !pElement ) 
		return;

	html_elt_tag* pEltTag =	dynamic_cast<html_elt_tag*>(pElement);
	html_elt_sgml* pEltSgml	= dynamic_cast<html_elt_sgml*>(pElement);
	html_elt_text* pEltText = dynamic_cast<html_elt_text*>(pElement);
	html_elt_comment* pEltComment = dynamic_cast<html_elt_comment*>(pElement);

	if ( pEltTag )
	{
		if ( !(pEltTag->def->cflags & HTML_EF_DYNAMIC) ) 
			m_results.ulKnownDefsCount++;
		
		bool bNeedDelimiter = 
				0 != strcmp(pEltTag->def->tag, "FONT") &&
				0 != strcmp(pEltTag->def->tag, "SPAN");

		//////////////////////////////////////////////////////////////////////////
		//
		// Подсчитываем объекты в этом теге
		//
		if ( 
			0 == strcmp(pEltTag->def->tag, "IMG") ||
			0 == strcmp(pEltTag->def->tag, "INPUT") ||
			0 == strcmp(pEltTag->def->tag, "OBJECT")
			) 
		{
			const char* value = 0;
			if (0 == strcmp(pEltTag->def->tag, "OBJECT")) 
				value = html_get_attr_s(pEltTag->attrs, "DATA");
			else
				value = html_get_attr_s(pEltTag->attrs, "SRC");
			if ( value )
				m_results.ulObjects++;
			if ( value && strstr(value, "http://") )
			{
				m_results.ulExternalObjects++;
				ExtractTokenFromURL(value,m_results.tokens);
			}
		}
		//
		// Подсчитываем объекты в этом теге
		//
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//
		// Токенизируем ссылки на внешние объекты
		//
		if ( 
			0 == strcmp(pEltTag->def->tag, "A")
			) 
		{
			const char* value = 0;
			value = html_get_attr_s(pEltTag->attrs, "HREF");
			ExtractTokenFromURL(value,m_results.tokens);
		}
		//
		// Токенизируем ссылки на внешние объекты
		//
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		//
		// Узнаем кодировку документа
		//
		if ( 
			0 == strcmp(pEltTag->def->tag, "META") 
			)
		{
			const char* value = 0;
			if ( value = html_get_attr_s(pEltTag->attrs, "HTTP-EQUIV") )
			{
				char* value1 = 0;
				value1 = _strupr(_strdup(value));
				if ( 0 == strcmp(value1, "CONTENT-TYPE") )
				{
					if ( value = html_get_attr_s(pEltTag->attrs, "CONTENT") )
					{
						free(value1);
						value1 = 0;
						const char* charset = content_type_get_charset(value);
						if ( charset )
						{
							value1 = _strupr(_strdup(charset));
							m_results.szCharset = value1;
						}
					}
				}
				if ( value1 )
					free(value1);
			}
		}
		//
		// Узнаем кодировку документа
		//
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		//
		// "Проваливаемся" внутрь элемента (исключая скрипты. Скрипты в письме - это плохо)
		//
		if ( 0 == strcmp(pEltTag->def->tag, "SCRIPT") ) 
		{
			m_results.ulScripts++;
		}
		// Содержимое заголовка, выпадающих списков и тегов STYLE нас не интересуют
		else if ( 0 != strcmp(pEltTag->def->tag, "TITLE") &&
					0 != strcmp(pEltTag->def->tag, "OPTION") &&
					0 != strcmp(pEltTag->def->tag, "STYLE"))
		{
			html_element* pNextElement = pEltTag->childs.head;
			bool bPrevTextWasInvisible = false;
			do
			{
				html_elt_tag* pNextTag = dynamic_cast<html_elt_tag*>(pNextElement);
				state_t _state = state;
				{
					bool bInvisibleText = false;
					//_state.bNeedDelimiter = true; // Нужда в разделителе не является передаваемым ниже свойством состояния
					if ( pNextTag )
					{
						if ( CheckColors(pNextTag, _state.rgb_color, _state.rgb_bgcolor) )
						{
#if defined(_USE_RGB)
							// Сравниваем текущий цвет текста и фона 
							// (как длину отрезка меж двух точек в трехмерном пространстве RGB)
							bInvisibleText = sqrt(
								pow( (GetRValue(_state.rgb_color)-GetRValue(_state.rgb_bgcolor)), 2 ) +
								pow( (GetGValue(_state.rgb_color)-GetGValue(_state.rgb_bgcolor)), 2 ) +
								pow( (GetBValue(_state.rgb_color)-GetBValue(_state.rgb_bgcolor)), 2 )
								) <= HTML_COLOR_SEEM_LENGHT
								;
#else if defined(_USE_XYZ)
							bInvisibleText = 
								1 == rgb_colors_similiarity(_state.rgb_color, _state.rgb_bgcolor);
#endif//_USE_RGB
							if ( bInvisibleText ) 
								m_results.ulSeemsColors++;
						}
						if ( CheckFontSize(pNextTag, _state.font_size) )
						{
	//						bInvisibleText = (_state.font_size < 4);
							PR_TRACE(( 0, prtNOTIFY, "CHtmlFilter::print_element => _state.font_size : %d ", (int)(_state.font_size + .5) ));
							if ( _state.font_size < 4.0 ) 
							{
								PR_TRACE(( 0, prtNOTIFY, "CHtmlFilter::print_element => m_results.ulSmallText++ " ));
								m_results.ulSmallText++;
							}
							else if ( _state.font_size != state.font_size )
								_state.bNeedDelimiter = true;
						}
						if ( CheckHiddenText(pNextTag) )
						{
							bInvisibleText = true;
							m_results.ulHiddenElements++;
						}
					}
					if ( bInvisibleText )
						_state.bCheckOnly = bPrevTextWasInvisible = true;
					else
					{
						bPrevTextWasInvisible = false;
						_state.bCheckOnly = state.bCheckOnly;
					}
				}
				print_element(pNextElement, _state);
			}
			while ( pNextElement && (pNextElement = pNextElement->next) );
		}
		//
		// "Проваливаемся" внутрь элемента
		//
		//////////////////////////////////////////////////////////////////////////
		
		if ( 
			(bNeedDelimiter || state.bNeedDelimiter) && 
			!m_results.szVisibleText.empty() &&
			( m_results.szVisibleText.c_str()[m_results.szVisibleText.length()-sizeof(char)] != ' '  )
			) 
		{
			m_results.szVisibleText.append( " " ); // Дописываем разделитель в конец строки, если он необходим
//			state.bNeedDelimiter = false;
		}
	}

	if ( pEltText && !pEltSgml && !pEltComment )
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// Выводим читаемый текст
		//
		ostringstream szElementBody; // Из ostringstream можно получить std::string, поэтому используем этот класс
		html_obj_printer hPrinter(szElementBody);
		pElement->print(&hPrinter);
		if ( !state.bCheckOnly && state.font_size >= 4 ) 
		{
			m_results.szVisibleText.append( szElementBody.str() );
			m_results.ulVisibleCharCount += szElementBody.str().length();
			PR_TRACE(( 0, prtNOTIFY, "CHtmlFilter::print_element => visible text is: %s ", szElementBody.str().c_str() ));
		}
		else
		{
			// Посчитываем количество скрытых символов
			m_results.ulInvisibleCharCount += szElementBody.str().length();
			string sz = szElementBody.str();
			if ( pEltText->flags & (HTML_F_LF|HTML_F_LFE) ) 
				m_results.szVisibleText.append( "/r/n" );
			PR_TRACE(( 0, prtNOTIFY, "CHtmlFilter::print_element => invisible text is: %s ", szElementBody.str().c_str() ));
		}
		//
		// Выводим читаемый текст
		//
		//////////////////////////////////////////////////////////////////////////
	}
}

bool CHtmlFilter::CheckFontSize(
				 html_elt_tag* pNextTag,
				 double& font_size
				 )
{
	PR_TRACE(( 0, prtNOTIFY, "CHtmlFilter::CheckFontSize => tag: %s ", pNextTag->def->tag));
	bool bResult = false;
	int _font_size = (int)font_size;
	if	(
			(0 == strcmp(pNextTag->def->tag, "FONT")) ||
			(0 == strcmp(pNextTag->def->tag, "BASEFONT")) ||
			(0 == strcmp(pNextTag->def->tag, "SPAN")) 
		) 
	{
		bResult = html_get_attr_i(pNextTag->attrs, "SIZE", _font_size);
		if ( bResult ) 
		{
			const char* szSize = html_get_attr_s(pNextTag->attrs, "SIZE");
			if ( szSize ) 
			{
				if ( 
					(szSize[0] == '+') ||
					(szSize[0] == '-')
					) 
					font_size += _font_size; // Относительный размер
				else
					font_size = _font_size;
				font_size *= 7; //Считаем, что один кегль = 7 пикселям
			}
		}
	}

	{// атрибут STYLE может быть у любого тега.
		const char* _style = html_get_attr_s(pNextTag->attrs, "STYLE");
		if ( _style ) 
		{
			char* style = _strupr(_strdup(_style));
			PR_TRACE(( 0, prtNOTIFY, "CHtmlFilter::CheckFontSize => style: %s ", style));
			if(strstr(style, "FONT-SIZE"))
			{
				//////////////////////////////////////////////////////////////////////////
				//
				//  Взято отсюда: http://www.reeddesign.co.uk/test/points-pixels.html
				//
				//	Points  Pixels  Ems  	Percent
				//	7.5pt  	10px  	0.625em 62.5%
				//	12pt  	16px  	1em  	100%
				//
				//	Я буду приводить к пикселям
				//
				bResult = true;
				enum size_type_t 
				{
					_undefined = -1,
					_em_ = 0, // The height of the element's font.
					_ex_ ,    // The height of the letter "x".
					_prc_,    // Percentage.
					_px_ ,    // Pixels.
					_in_ ,    // Inches (1 inch = 2.54 centimeters).
					_cm_ ,    // Centimeters.
					_mm_ ,    // Millimeters.
					_pt_ ,    // Points (1 point = 1/72 inches).
					_pc_ ,    // Picas (1 pica = 12 points).
				} sizeType = _undefined;
				if(strstr(style, "EM"))
					sizeType = _em_;
				else if(strstr(style, "EX"))
					sizeType = _ex_;
				else if(strstr(style, "%"))
					sizeType = _prc_;
				else if(strstr(style, "IN"))
					sizeType = _in_;
				else if(strstr(style, "CM"))
					sizeType = _cm_;
				else if(strstr(style, "MM"))
					sizeType = _mm_;
				else if(strstr(style, "PT"))
					sizeType = _pt_;
				else if(strstr(style, "PC"))
					sizeType = _pc_;
				else if(strstr(style, "PX"))
					sizeType = _px_;
				if(sizeType != _undefined)
				{
					char *size = strpbrk(style, "0123456789");
					if(size)
					{
						double nStyle = atof(size);
						switch ( sizeType ) 
						{
						case _em_:  font_size  = nStyle               *16.0; break;
						case _ex_:  font_size  = (nStyle/2.0)         *16.0; break;
						case _prc_: font_size *= (nStyle/100.0)            ; break;
						case _in_:  font_size  = (nStyle*254.0)        /2.5; break;
						case _cm_:  font_size  = (nStyle*10.0)         *2.5; break;
						case _mm_:  font_size  = nStyle                *2.5; break;
						case _pt_:  font_size  = (nStyle*16.0/12.0)        ; break;
						case _pc_:  font_size  = (nStyle/12.0)             ; break;
						case _px_:  font_size  = nStyle                    ; break;
						default:
							bResult = false;
						}
					}
				}
				else
				{
					bResult = false;
				}
			}
			free(style);
		}
	}

	return bResult;
}

bool CHtmlFilter::CheckColors(
				 html_elt_tag* pNextTag,
				 unsigned long &rgb_color,
				 unsigned long &rgb_bgcolor
				 )
{
	bool bCheckColors = false;
	//////////////////////////////////////////////////////////////////////////
	//
	// Смотрим на переопределенные цвета в данном элементе
	//
	if	(
			(0 == strcmp(pNextTag->def->tag, "FONT")) ||
			(0 == strcmp(pNextTag->def->tag, "BASEFONT")) 
		) 
		bCheckColors |= 
		html_color_parse( html_get_attr_s(pNextTag->attrs, "COLOR"), &rgb_color );
	if	(
			( !m_bBodyPresented ) &&
			(0 == strcmp(pNextTag->def->tag, "BODY"))
		)
	{
		bCheckColors |= 
		html_color_parse( html_get_attr_s(pNextTag->attrs, "TEXT"), &rgb_color );
		html_color_parse( html_get_attr_s(pNextTag->attrs, "BGCOLOR"), &rgb_bgcolor );
		m_bBodyPresented = true;
	}
	if	(
			(
			(0 == strcmp(pNextTag->def->tag, "TABLE")) ||
			(0 == strcmp(pNextTag->def->tag, "TR")) ||
			(0 == strcmp(pNextTag->def->tag, "TD")) ||
			(0 == strcmp(pNextTag->def->tag, "TH")) 
			) 
		)
		html_color_parse( html_get_attr_s(pNextTag->attrs, "BGCOLOR"), &rgb_bgcolor );
	//
	// Смотрим на переопределенные цвета в данном элементе
	//
	//////////////////////////////////////////////////////////////////////////
	return bCheckColors;
}

bool CHtmlFilter::CheckHiddenText(html_elt_tag* pNextTag)
{
	// Скрытым считаем элемент, для которого определен атрибут STYLE со значением "visibility:hidden|collapse"
	bool bResult = false;
	const char* _style = html_get_attr_s(pNextTag->attrs, "STYLE");
	if ( _style ) 
	{
		char* style = _strupr(_strdup(_style));
		bResult =  strstr(style, "VISIBILITY") && ( strstr(style, "HIDDEN")/* || strstr(style, "COLLAPSE")*/ ); // DVi: COLLAPSE игнорируется парсером от IE!
		bResult |= strstr(style, "DISPLAY")    && ( strstr(style, "NONE") );
		free(style);
	}
	return bResult;
}

//void CHtmlFilter::ExtractTokenFromURL(const char* src)
//{
//	return ::E
//	if ( src ) 
//	{
//		char* pos = strstr(src, "http://");
//		string domain = "";
//		bool bNeedToFree = false;
//		if ( pos ) 
//		{
//			char* url = pos + strlen("http://");
//			char* pos_path = strstr(url, "/");
//			domain = pos;
//			// Засовываем целиком URL
//			if ( !domain.empty() ) 
//			{
////				ToUpper(domain);
//				m_results.tokens.push_back(domain);
//			}
//			domain = url;
//			// Ищем домен
//			if ( pos_path )
//				domain.erase(pos_path - url, domain.length()-(pos_path - url));
//			char* pos_dog = strstr(domain.c_str(), "@");
//			if ( pos_dog ) 
//				domain = pos_dog + sizeof(char);
//			// Засовываем домен
//			if ( !domain.empty() ) 
//			{
//				ToUpper(domain);
//				m_results.tokens.push_back(domain);
//			}
//		}
//	}
//}