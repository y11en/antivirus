/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	KLChartPicGen.h
 * \author	Andrew Lashenkov
 * \date	29.01.2004 18:09:00
 * \brief	
 * 
 */

#ifndef __KL_CHART_PIC_GEN_H_
#define __KL_CHART_PIC_GEN_H_

#include <string>

#ifdef KLCHARTPICGEN_EXPORTS
	#define KLCHARTPICGEN_DECL __declspec(dllexport)
#else
	#define KLCHARTPICGEN_DECL __declspec(dllimport)
#endif

#define KLCHARTPICGEN_CALL extern "C" KLCHARTPICGEN_DECL HRESULT __stdcall

namespace KLCHARTPICGEN {
	
const COLORREF c_clrRed = RGB(192, 8, 8);
const COLORREF c_clrDarkRed = RGB(128, 8, 8);
const COLORREF c_clrGreen = RGB(16, 128, 64);
const COLORREF c_clrDarkGreen = RGB(8, 64, 8);
const COLORREF c_clrLightGreen = RGB(8, 224, 8);
const COLORREF c_clrBlue = RGB(32, 32, 128);
const COLORREF c_clrYellow = RGB(224, 224, 64);
const COLORREF c_clrGray = RGB(128, 128, 128);
const COLORREF c_clrLightGray = RGB(224, 224, 224);
const COLORREF c_clrDarkGray = RGB(64, 64, 64);
const COLORREF c_clrViolet = RGB(192, 8, 192);


typedef std::pair<std::wstring, long> TSimpleDiagramEntry;

struct TDiagramEntry{	
	std::wstring m_strRawName;
	std::vector<int> m_VectData;
};

enum EDiagramTypeFlags{
	DTF_STACK_SERIES				= 0x01,
	DTF_PIE							= 0x02,
	DTF_USE_SERIES_COLORS_FOR_DATA	= 0x04,
	DTF_TRANSPARENT_BG				= 0x08
};

typedef std::pair<time_t, time_t> TTimeInterval;
/*!
  \brief	Создает GIF-файл, содержащий диаграмму.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateSimpleDiagram(
	const std::vector<TSimpleDiagramEntry>& data, 
	std::wstring& wstrFileName);

/*!
  \brief	Создает GIF-файл, содержащий диаграмму.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateDiagram(const std::vector<TDiagramEntry>& data, 
	const std::vector<std::wstring>& Series, 
	BYTE btType, // combination of EDiagramTypeFlags
	std::wstring& wstrFileName);

/*!
  \brief	Создает GIF-файл, содержащий диаграмму.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateDiagramEx(const std::vector<TDiagramEntry>& data, 
	const std::vector<std::wstring>& Series, 
	BYTE btType, // combination of EDiagramTypeFlags
	std::wstring& wstrFileName,
	int nWidth,
	int nHeight);

/*!
  \brief	Создает графический файл, содержащий диаграмму.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateDiagramEx2(const std::vector<TDiagramEntry>& data, 
	const std::vector<std::wstring>& Series, 
	BYTE btType, // combination of EDiagramTypeFlags
	std::wstring& wstrFileName,
	int nWidth,
	int nHeight,
	const std::vector<COLORREF>* pSeriesColors);


/*!
  \brief	Рисует диаграмму в заданном HDC.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateDiagramInDC(
	const std::vector<TDiagramEntry>& data, 
	const std::vector<std::wstring>& Series, 
	BYTE btType, // combination of EDiagramTypeFlags
	HDC hDC,
	int nWidth,
	int nHeight,
	const std::vector<COLORREF>* pSeriesColors);

/*!
  \brief	Создает графический файл, содержащий гистограмму.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateHistogram(
	const std::vector<TDiagramEntry>& data, 
	const std::vector<TTimeInterval>& times,
	const std::vector<std::wstring>& Series, 
	const std::wstring& wstrTimeFormat,
	BYTE btType, // combination of EDiagramTypeFlags
	std::wstring& wstrFileName,
	int nWidth,
	int nHeight,
	const std::vector<COLORREF>* pSeriesColors);


/*!
  \brief	Рисует гистограмму в заданном HDC.

  \param	
  \return	S_OK - 
            S_FALSE - 
*/
KLCHARTPICGEN_CALL CreateHistogramInDC(
	const std::vector<TDiagramEntry>& data, 
	const std::vector<TTimeInterval>& times,
	const std::vector<std::wstring>& Series, 
	const std::wstring& wstrTimeFormat,
	BYTE btType, // combination of EDiagramTypeFlags
	HDC hDC,
	int nWidth,
	int nHeight,
	const std::vector<COLORREF>* pSeriesColors);
};

#endif //__KL_CHART_PIC_GEN_H_