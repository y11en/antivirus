#ifndef __DATE_T_H__
#define __DATE_T_H__

#if defined (_MSC_VER)
  #pragma once
#endif

//-----------------------------------------------------------------------------

#include <time.h>

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

namespace LicensingPolicy {

//-----------------------------------------------------------------------------

struct date_t
{

	date_t ();

	int           operator <  (const date_t& d) const;
	int			  operator >  (const date_t& d) const;
	int           operator == (const date_t& d) const;
	date_t&       operator =  (const date_t& d);
	int           empty       () const;
	void          clear       ();
	unsigned int  diff        (const date_t& d) const;
	unsigned int  days        () const;
	void          normailize  ();

	unsigned int m_day;
	unsigned int m_month;
	unsigned int m_year;
};

date_t operator+ (const date_t& date, int dx);

//-----------------------------------------------------------------------------

inline date_t::date_t () : m_day (0), m_month (0), m_year (0)
{
}

//-----------------------------------------------------------------------------

inline int date_t::empty () const
{
	return (!m_day && !m_month && !m_year);
}

//-----------------------------------------------------------------------------

inline date_t& date_t::operator = (const date_t& d)
{
  m_day = d.m_day;
  m_month = d.m_month;
  m_year = d.m_year;

  return (*this);
}

//-----------------------------------------------------------------------------

inline void date_t::clear ()
{
	m_day = 0;
	m_month = 0;
	m_year = 0;
}

}; // namespace LicensingPolicy

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
