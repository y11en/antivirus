/*
*/

#if !defined(__KL_TRAFFICPROTOCOLLERTYPES_H)
#define __KL_TRAFFICPROTOCOLLERTYPES_H

namespace TrafficProtocoller
{
	enum data_source_t
	{
		dsUnknown = 0,
		dsClient,					//!< Источник данных - клиент
		dsServer					//!< Источник данных - сервер
	};

	enum detect_code_t
	{
		dcProtocolUnknown = 0,			//!< Протокол не опознан
		dcProtocolDetected,				//!< Протокол опознан
		dcNeedMoreInfo,					//!< Не достаточно данных для опознания протокола
		dcDataDetected,					//!< Распознана команда передачи данных
		dc_last,
	};
}

#endif  // !defined(__KL_TRAFFICPROTOCOLLERTYPES_H)
