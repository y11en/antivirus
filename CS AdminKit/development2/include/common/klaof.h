#ifndef __KLAOF_H__
#define __KLAOF_H__

#include <std/base/klstd.h>
#include <std/thr/locks.h>
#include <std/tmstg/timeoutstore.h>

#include <map>
#include <vector>


#define KLSTD_INTERAFCE_MAP_AOF()    \
        else if( AOF_QueryInterface(iid, ppObject) )  \
        {   \
            ;\
        }

#define KLAOF_EXTCALL()    \
    KLSTD::CAutoObjectLock srvacc(m_pAOF_Data->m_lckAOF_Lock);    \
    if(!srvacc) \
    {   \
        KLSTD_THROW_APP_PENDING();   \
    };


namespace KLAOF
{
    typedef std::pair<KLSTD::KLSTD_IID, KLSTD::KLBaseQI*> iid2ptr_t;

    template<class D>
        class KLSTD_NOVTABLE AOF_AttachedBase
            :   public KLSTD::KLBaseQI
    {
    public:
        virtual void AOF_Attach(KLSTD::KLBaseQI* p, D* pData) = 0;
        virtual void AOF_Create() = 0;
        virtual void AOF_OnPostInit() = 0;
        virtual KLSTD_NOTHROW void AOF_Destroy() throw() = 0;
        virtual KLSTD_NOTHROW void AOF_Delete() throw() = 0;
        virtual iid2ptr_t* AcquireIIds() throw() = 0;
    };

    #define AOF_DECLARE_MAP() \
        void AOF_FillAttached();

    #define AOF_MAP_BEGIN(_classname) \
        void _classname::AOF_FillAttached() \
        { \
            AOF_Factory_t data[] = \
            {\

    #define AOF_MAP_ENTRY(_factory)\
                &(_factory),


    #define AOF_MAP_END() \
                NULL \
            }; \
            AOF_FillAttachedI(data, KLSTD_COUNTOF(data));\
        };

    template<class D>
        class AOF_MainBase
            :   public KLSTD::KLBaseQI
        {
        public:
            typedef AOF_AttachedBase<D> AOF_AttachedBase_t;

            typedef AOF_AttachedBase_t* (*AOF_Factory_t)();

            AOF_MainBase()
            {;};

            virtual ~AOF_MainBase()
            {
                AOF_CallDetach();
            };

            //sample method
            void AOF_CreateMain()
            {
                AOF_FillAttached();
                AOF_CallAttach();
                AOF_CallCreate();
                m_lckAOF_Lock.Allow();
            };

            //sample method
            void AOF_DestroyMain()
            {
                //disallow external calls (but some calls may be in-server yet)
                m_lckAOF_Lock.Disallow();

                //add wait for all external calls to be completed
                m_lckAOF_Lock.Wait();

                //Remove all references to server object which prevent destructor from being called
                AOF_CallDestroy();
            };

        protected:
            typedef std::vector<AOF_AttachedBase_t*> vec_AOF_AttachedBase_t;

            typedef std::map<
                            KLSTD::KLSTD_IID,
                            KLSTD::KLBaseQI* >  map_iid2obj_t;
            typedef map_iid2obj_t::iterator     it_map_iid2obj_t;
            typedef map_iid2obj_t::value_type   val_map_iid2obj_t;

        protected:
            //! implemented in AOF_MAP_BEGIN/AOF_MAP_END
            void AOF_FillAttached();

            void AOF_FillAttachedI(AOF_Factory_t* data, size_t nSize)
            {
                KLSTD_ASSERT_THROW(m_vecAOF_Objects.empty());
                KLSTD_ASSERT_THROW(m_mapAOF_Objects.empty());
                ;
                KLSTD::AutoCriticalSection acs(m_pAOF_CS);
                m_vecAOF_Objects.reserve(nSize);
                for(size_t ii = 0; ii < nSize; ++ii)
                {
                    if(!data[ii])
                        continue;

                    KLSTD::CPointer<AOF_AttachedBase_t> p = 
                                                (*data[ii])();
                    KLSTD_CHKMEM(p);

                    iid2ptr_t* pIIds = p->AcquireIIds();

                    m_vecAOF_Objects.push_back(p.Relinquish());

                    for(size_t j =0; pIIds[j].first; ++j)
                    {                        
                        m_mapAOF_Objects.insert(
                                    val_map_iid2obj_t(
                                        pIIds[j].first, 
                                        pIIds[j].second));
                    };
                };
            };

            void AOF_CallAttach()
            {
                for(size_t i = 0; i < m_vecAOF_Objects.size(); ++i )
                {
                    if(m_vecAOF_Objects[i])
                        m_vecAOF_Objects[i]->AOF_Attach(this, m_pAOF_Data);
                };
            };

            void AOF_CallCreate()
            {
                for(size_t i = 0; i < m_vecAOF_Objects.size(); ++i )
                {
                    if(m_vecAOF_Objects[i])
                        m_vecAOF_Objects[i]->AOF_Create();
                };
            };

            void AOF_CallPostInit()
            {
                for(size_t i = 0; i < m_vecAOF_Objects.size(); ++i )
                {
                    if(m_vecAOF_Objects[i])
                        m_vecAOF_Objects[i]->AOF_OnPostInit();
                };
            };

            void AOF_CallDestroy()
            {
                for(size_t i = 0; i < m_vecAOF_Objects.size(); ++i )
                {
                    const size_t j = m_vecAOF_Objects.size() - i - 1;
                    
                    if(m_vecAOF_Objects[j])
                        m_vecAOF_Objects[j]->AOF_Destroy();
                };
            };

            void AOF_CallDetach()
            {
                for(size_t i = 0; i < m_vecAOF_Objects.size(); ++i )
                {
                    const size_t j = m_vecAOF_Objects.size() - i - 1;
                    if(m_vecAOF_Objects[j])
                    {
                        m_vecAOF_Objects[j]->AOF_Delete();
                        m_vecAOF_Objects[j] = NULL;
                    };
                };
            };

            bool AOF_QueryInterface(KLSTD::KLSTD_IID iid, void** ppObject)
            {
                KLSTD::AutoCriticalSection acs(m_pAOF_CS);
                it_map_iid2obj_t it = m_mapAOF_Objects.find(iid);
                if( m_mapAOF_Objects.end() != it )
                {
                    *ppObject = it->second;
                    (it->second)->AddRef();
                };
                return !!(*ppObject);
            };
        protected:
            vec_AOF_AttachedBase_t              m_vecAOF_Objects;
            map_iid2obj_t                       m_mapAOF_Objects;
            KLSTD::ObjectLock                   m_lckAOF_Lock;
            KLSTD::CPointer<D>                  m_pAOF_Data;
            KLSTD::CAutoPtr<KLSTD::CriticalSection>
                                                m_pAOF_CS;
        };

    #define AOF_DECLARE_FACTORY(_name, _res)  \
        _res* _name();

    #define AOF_IMPLEMENT_FACTORY(_name, _res, _class)  \
        _res* _name() \
        { \
            _res* pRes = new _class; \
            KLSTD_CHKMEM(pRes); \
            return pRes; \
        };

    #define AOF_MODULE_MAP_BEGIN() \
        KLAOF::iid2ptr_t* AcquireIIds() throw() \
        { \
            static KLAOF::iid2ptr_t data[] = \
            {

    #define AOF_MODULE_MAP_ENTRY(_iface) \
                KLAOF::iid2ptr_t(KLSTD_IIDOF(_iface), static_cast<_iface*>(this)),

    #define AOF_MODULE_MAP_END() \
                KLAOF::iid2ptr_t(NULL, NULL) \
            }; \
            return data; \
        };

    template<class D>
        class AOF_AttachedBaseImpl
                :   public AOF_AttachedBase<D>
        {
        public:
            AOF_AttachedBaseImpl()
                :   m_pAOF_Base(NULL)
            {
                ;
            };

            virtual ~AOF_AttachedBaseImpl()
            {
                ;
            };

            KLSTD_INTERAFCE_MAP_REDIRECT(m_pAOF_Base);

            void AOF_Attach(KLSTD::KLBaseQI* p, D* pData)
            {
                m_pAOF_Base = p;
                m_pAOF_Data = pData;
            };

            void AOF_Create()
            {
                ;
            };

            KLSTD_NOTHROW void AOF_Destroy() throw()
            {
                ;
            };

            void AOF_OnPostInit()
            {
                ;
            };

            KLSTD_NOTHROW void AOF_Delete() throw()
            {
                delete this;
            };
        protected:
            KLSTD::KLBaseQI* m_pAOF_Base;
            D*               m_pAOF_Data;
        };
};

#endif //__KLAOF_H__
