/*******************************************************************************
 * Project: Content Filtration Library                                         *
 * (C) 2001-2003, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: categories.h                                                          *
 * Created: Fri Nov 09 14:02:05 2001                                           *
 * Desc: Rubricator interface                                                  *
 *******************************************************************************/

/**
 * \file  categories.h
 * \brief Rubricator interface.
 */

#ifndef __categories_h__
#define __categories_h__

#include <string.h>

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _category category;

    /**
     * \brief Category description.
     */
    struct _category
    {
	const char*        textID;        /*!< Text id of category (ID attribute in XML) */
	unsigned int       CID;           /*!< Internal unique category number */
	const char*        title;         /*!< Title of category (title atribute in XML) */

	short              hidden;        /*!< ==1 if category is hidden */

	int rel_limit;                    /*!< rellimit in XML, if < 0 then using defaults */
	int diff_limit;                   /*!< difflimit in XML, if < 0 then using defaults */

	const category*    parent;        /*!< Pointer to parent category, may be NULL */

	/**
	 * \brief Pointers to subcategories.
	 */
	struct 
	{
	    size_t     count;        
	    const category ** categories;
	} sub;
    };

  
    typedef struct _rubricator rubricator;

    struct _reverse_index_entry
    {
	unsigned int    CID;
	const category* cat;
    };

    typedef struct _reverse_index_entry reverse_index_entry;

    /**
     * \brief Rubricator description.
     */
    struct _rubricator
    {
	const char* title;
	unsigned int version[4];      /*!< Version attribute of CFDoc */
	unsigned int used_version[2]; /*!< Version of library, used for compile rubricator */

	/**
	 * \brief Top categories (all have parent field == NULL).
	 */
	struct
	{
	    size_t     count;
	    const category** categories;
	} top;

	struct
	{
	    size_t         count;
	    const reverse_index_entry *array;
	} reverse_index;
    };

    /**
     * \brief Search category with specified CID: NULL if such
     *        category not exist.
     */
    const category*   rub_search(const rubricator* r, unsigned int CID);

#ifdef __cplusplus
}
#endif

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif // __categories_h__

/*
 * <eof categories.h>
 */
