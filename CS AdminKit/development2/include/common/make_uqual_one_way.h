/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	make_uqual_one_way.h
 * \author	Andrew Kazachkov
 * \date	28.07.2003 11:27:52
 * \brief	
 * 
 */

#ifndef __KL_MAKE_UQUAL_ONE_WAY_H__
#define __KL_MAKE_UQUAL_ONE_WAY_H__

namespace KLSTD
{
    /*!
        NewInSrc.NewInSrc(Container::iterator itSrc)
        NewInDst.NewInDst(Container::iterator itDst)
        Diff.Diff(Container::iterator itSrc, Container::iterator itDst);
    */
    template<class Container, class NewInSrc, class NewInDst, class Diff>
    void MakeEqualOneWay(
                        Container&  src,
                        Container&  dst,
                        NewInSrc&   newInSrc,
                        NewInDst&   newInDst,
                        Diff&       diff)
    {
        typedef typename Container::iterator iterator;

        for( iterator itSrc=src.begin(); itSrc != src.end();)
        {
            iterator itSrcOld=itSrc++;
            iterator itDst=dst.find(itSrcOld->first);
            if(itDst == dst.end())
                newInSrc.NewInSrc(itSrcOld);
            else
            if(!(itDst->second == itSrcOld->second))
                diff.Diff(itSrcOld, itDst);
        };

        for( iterator itDst=dst.begin(); itDst != dst.end();)
        {
            iterator itDstOld=itDst++;
            iterator itSrc=src.find(itDstOld->first);
            if(itSrc == src.end())
                newInDst.NewInDst(itDstOld);
        };
    }
}

#endif //__KL_MAKE_UQUAL_ONE_WAY_H__
