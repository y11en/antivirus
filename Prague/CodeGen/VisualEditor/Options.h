#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

struct SOptions
{
	BOOL m_bUseRegistryNotSql;

	SOptions();

	BOOL Load();
	BOOL Save();
};

#endif