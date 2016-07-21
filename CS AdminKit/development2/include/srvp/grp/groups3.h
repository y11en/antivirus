/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	groups3.h
 * \author	Andrew Kazachkov
 * \date	10.02.2005 12:04:39
 * \brief	
 * 
 */

#ifndef KLGRP_GROUPS3_H
#define KLGRP_GROUPS3_H

#include "./groups2.h"

namespace KLGRP
{

	/*!
	* \brief See Groups.
	*/

    class Groups3 : public Groups2
	{
    public:
        /*!
          \brief	Returns id of the Super group. The group Super is parent
                    of the group Groups and is intended for assignment of
                    group tasks and policies received from the master server.

          \return	long id of the group Super
        */
        virtual long GetSuperGroup() = 0;

    };

};

#endif // KLGRP_GROUPS3_H
