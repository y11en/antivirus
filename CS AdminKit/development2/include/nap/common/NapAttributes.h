/*!
 * (C) 2007 Kaspersky Lab
 *
 * \file	NapAttributes.h
 * \author	Eugene Rogozhnikov
 * \date	06.11.2007 17:13:27
 * \brief	Наши атрибуты для NAP
 *
 */

#ifndef __NAPATTRIBUTES_H__
#define __NAPATTRIBUTES_H__

namespace KLNAP
{

	enum AttrType {
		AT_BOOL = 0,
		AT_INT,
		AT_STRING,
		AT_DATE
	};

	struct CNAPAttrDefinition {
		unsigned short uCode; // Код аттрибута.
		AttrType lType; // Тип аттрибута.
	};

	bool IsAttributeExist( unsigned short uCode );
	AttrType GetAttributeType( unsigned short uCode );


}; // namespace KLNAP


#endif //__NAPATTRIBUTES_H__