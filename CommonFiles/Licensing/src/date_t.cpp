#include "../include/date_t.h"

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#pragma warning (disable : 4514)
#endif

namespace LicensingPolicy
{

//-----------------------------------------------------------------------------

int date_t::operator == (const date_t& d) const
{
  return (d.m_day == m_day && d.m_month == m_month && d.m_year == m_year);
}

//-----------------------------------------------------------------------------

int date_t::operator < (const date_t& d) const
{
	return days() < d.days();
}

//-----------------------------------------------------------------------------

int date_t::operator > (const date_t& d) const
{
	return (! ( (*this < d) || (*this == d)) );
}

//-----------------------------------------------------------------------------

unsigned int date_t::diff (const date_t& d) const
{
	return days() - d.days();
}

//-----------------------------------------------------------------------------

date_t operator+ (const date_t& date, int dx)
{
	date_t tmp(date);
	tmp.m_day += dx;
	tmp.normailize();
	return tmp;
}

//-----------------------------------------------------------------------------

unsigned int date_t::days() const
{
	if (empty()) 
		return 0;

	// rata die algorithm
	// see http://vsg.cape.com/~pbaum/date/rata.htm
	unsigned int Z = m_month < 3 ? m_year - 1 : m_year;
	unsigned int M = m_month < 3 ? m_month + 12 : m_month;
	unsigned int F = (979 * M - 2918) >> 5;

	return m_day + F + 365 * Z + Z / 4 - Z / 100 + Z / 400 - 306;
}

//-----------------------------------------------------------------------------

void date_t::normailize()
{
	if (empty())
		return;

	unsigned int RD = days();

	// inverse rata die algorithm
	// see http://vsg.cape.com/~pbaum/date/rata.htm
	unsigned int Z = RD + 306;
	unsigned int H = 100 * Z - 25;
	unsigned int A = H / 3652425;
	unsigned int B = A - A / 4;
	unsigned int year = (100 * B + H) / 36525;
	unsigned int C = B + Z - 365 * year - year / 4;
	unsigned int month = (5 * C + 456) / 153;
	unsigned int day = C - (153 * month - 457) / 5;

	if (month > 12)
	{
		year++; month -= 12;
	}
	m_year = year;
	m_month = month;
	m_day = day;
}

//-----------------------------------------------------------------------------

} // namespace LicensingPolicy
