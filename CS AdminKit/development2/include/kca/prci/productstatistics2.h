/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ProductStatistics2.h
 * \author	Andrew Kazachkov
 * \date	10.02.2004 10:07:13
 * \brief	
 * 
 */

#ifndef __KL_PRODUCTSTATISTICS2_H__
#define __KL_PRODUCTSTATISTICS2_H__

#include "./productstatistics.h"

namespace KLPRCI
{

    //Don't modify pFilter !!!
    typedef void (*UpdateStatisticsCallback)(void* pContext, KLPAR::Params* pFilter);

	class KLSTD_NOVTABLE ProductStatistics2 : public ProductStatistics
    {
    public:
       virtual void	SetUpdateStatisticsCallback(
						void*					    context,
                        UpdateStatisticsCallback    callback)  = 0;
    };

};

#endif //__KL_PRODUCTSTATISTICS2_H__
