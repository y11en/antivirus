#ifndef __KL_KLUPDLOC_H__922A1562_F0FE_4514_AFC7_F13BA6B31AC6
#define __KL_KLUPDLOC_H__922A1562_F0FE_4514_AFC7_F13BA6B31AC6

#include <string>

namespace KLUPD
{
	const int LOC_ID_START	= 0x10000;
	const int LOC_ID_END	= 0x10FFF;

    // task result is written in c_er_par2 parameter is indication of task result event
    static const std::wstring g_taskResult = L"task result";

    // load and free localization resource library
    bool loadLocalizationLibrary(const wchar_t *pathToLocalizationLibrary);
    void freeLocalizationLibrary();

    // Produces localized message from input params
    // input [in] - the input data to generate localized information
    // localizedDescription [out] - localized result string which describes event or error
    //  Note: the maximum length of result is restricted to 800 symbols,
    //    the rest may be cut by KCA infrastructure
    bool locilizeEvent(KLPAR::ParamsPtr input, std::wstring &localizedDescription);

    // Produces full information about event from input params, gives full information
    //   about error, details on problem and hints to solve problem.
    //  Note: the length of additional information is not restricted
    //  Note: the locilizeEventDetails() concatenates the result
    //    of locilizeEvent() with additional details
    bool locilizeEventDetails(KLPAR::ParamsPtr input, std::wstring &fullInformation);

    // Produces localized message for given result code
    // nId [in] - result code
    // input [in] - the input data to generate localized information
    // localizedDescription [out] - localized result string which describes event or error
    //  Note: the maximum length of result is restricted to 800 symbols,
    //    the rest may be cut by KCA infrastructure
	bool localizeResultCode(
		AVP_longlong llId, 
		const KLPAR::Params* input, 
		std::wstring& localizedDescription);
}   // namespace KLUPD

#endif  // __KL_KLUPDLOC_H__922A1562_F0FE_4514_AFC7_F13BA6B31AC6
