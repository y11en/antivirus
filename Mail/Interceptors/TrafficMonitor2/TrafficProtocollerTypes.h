/*
*/

#if !defined(__KL_TRAFFICPROTOCOLLERTYPES_H)
#define __KL_TRAFFICPROTOCOLLERTYPES_H

namespace TrafficProtocoller
{
	enum data_source_t
	{
		dsUnknown = 0,
		dsClient,					//!< »сточник данных - клиент
		dsServer					//!< »сточник данных - сервер
	};

	enum detect_code_t
	{
		dcProtocolUnknown = 0,			//!< ѕротокол не опознан
		dcProtocolDetected,				//!< ѕротокол опознан
		dcNeedMoreInfo,					//!< Ќе достаточно данных дл€ опознани€ протокола
		dcDataDetected,					//!< –аспознана команда передачи данных
		dcProtocolLikeDetected,			//!< ≈сть подозрение на соответствие протоколу
		dc_last,
	};

	inline const char *GetDataSourceStr(data_source_t ds)
	{
		switch(ds)
		{
		case dsClient:
			return "dsClient";
		case dsServer:
			return "dsServer";
		}
		return "dsUnknown";
	}
}

#define npPROTOCOL_TYPE ((tSTRING)("PROTOCOL_TYPE")) // тип протокола (нужен только дл€ внешних задач)


#endif  // !defined(__KL_TRAFFICPROTOCOLLERTYPES_H)
