#ifndef _AL_H_6729021398213489
#define _AL_H_6729021398213489

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif

#define AL_CALLTYPE __cdecl
#define AL_NOVTABLE __declspec(novtable)
#define AL_PURE

#ifdef __cplusplus
#define AL_DECLARE_INTERFACE(iface_name) \
        struct iface_name; \
        struct AL_NOVTABLE iface_name
#define AL_INTERFACE_BASED(base_iface_name, member_name) : public base_iface_name
#define AL_INTERFACE_BASED_EX(base_iface_name, member_name) , base_iface_name
#define AL_DECLARE_INTERFACE_BEGIN {
#define AL_DECLARE_METHOD0(ret_type, method_name) virtual ret_type AL_CALLTYPE method_name() AL_PURE
#define AL_DECLARE_METHOD1(ret_type, method_name, a1) virtual ret_type AL_CALLTYPE method_name(a1) AL_PURE
#define AL_DECLARE_METHOD2(ret_type, method_name, a1,a2) virtual ret_type AL_CALLTYPE method_name(a1,a2) AL_PURE
#define AL_DECLARE_METHOD3(ret_type, method_name, a1,a2,a3) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3) AL_PURE
#define AL_DECLARE_METHOD4(ret_type, method_name, a1,a2,a3,a4) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3,a4) AL_PURE
#define AL_DECLARE_METHOD5(ret_type, method_name, a1,a2,a3,a4,a5) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3,a4,a5) AL_PURE
#define AL_DECLARE_METHOD6(ret_type, method_name, a1,a2,a3,a4,a5,a6) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3,a4,a5,a6) AL_PURE
#define AL_DECLARE_METHOD7(ret_type, method_name, a1,a2,a3,a4,a5,a6,a7) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3,a4,a5,a6,a7) AL_PURE
#define AL_DECLARE_METHOD8(ret_type, method_name, a1,a2,a3,a4,a5,a6,a7,a8) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3,a4,a5,a6,a7,a8) AL_PURE
#define AL_DECLARE_METHOD9(ret_type, method_name, a1,a2,a3,a4,a5,a6,a7,a8,a9) virtual ret_type AL_CALLTYPE method_name(a1,a2,a3,a4,a5,a6,a7,a8,a9) AL_PURE
#define AL_DECLARE_INTERFACE_END  };
#else
#define AL_DECLARE_INTERFACE(iface_name) \
        typedef struct tag_##iface_name iface_name; \
        typedef struct tag_##iface_name##_vtbl iface_name##_vtbl; \
        struct tag_##iface_name { const iface_name##_vtbl * vtbl; }; \
        struct tag_##iface_name##_vtbl {
#define AL_INTERFACE_BASED(base_iface_name, member_name) base_iface_name##_vtbl member_name;
#define AL_INTERFACE_BASED_EX AL_INTERFACE_BASED
#define AL_DECLARE_INTERFACE_BEGIN
#define AL_DECLARE_METHOD0(ret_type, method_name) ret_type (AL_CALLTYPE * method_name)(void* _this)
#define AL_DECLARE_METHOD1(ret_type, method_name, a1) ret_type (AL_CALLTYPE * method_name)(void* _this, a1)
#define AL_DECLARE_METHOD2(ret_type, method_name, a1,a2) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2)
#define AL_DECLARE_METHOD3(ret_type, method_name, a1,a2,a3) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3)
#define AL_DECLARE_METHOD4(ret_type, method_name, a1,a2,a3,a4) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3,a4)
#define AL_DECLARE_METHOD5(ret_type, method_name, a1,a2,a3,a4,a5) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3,a4,a5)
#define AL_DECLARE_METHOD6(ret_type, method_name, a1,a2,a3,a4,a5,a6) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3,a4,a5,a6)
#define AL_DECLARE_METHOD7(ret_type, method_name, a1,a2,a3,a4,a5,a6,a7) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3,a4,a5,a6,a7)
#define AL_DECLARE_METHOD8(ret_type, method_name, a1,a2,a3,a4,a5,a6,a7,a8) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3,a4,a5,a6,a7,a8)
#define AL_DECLARE_METHOD9(ret_type, method_name, a1,a2,a3,a4,a5,a6,a7,a8,a9) ret_type (AL_CALLTYPE * method_name)(void* _this, a1,a2,a3,a4,a5,a6,a7,a8,a9)
#define AL_DECLARE_INTERFACE_END  };
typedef int bool;
#define false 0
#define true 1
#endif

#ifdef __cplusplus

template<typename T> class alAutoRelease {
public:
        alAutoRelease(): m_obj(0) {};
        alAutoRelease(T obj): m_obj(obj) {};
        ~alAutoRelease()
        {
                if (m_obj)
                {
                        m_obj->Release();
                        m_obj = 0;
                }
        }
        alAutoRelease& operator = (T that)
        {
                if (m_obj)
                        m_obj->Release();
                m_obj = that;
                return *this;
        }
        T operator ->()
        {
                return m_obj;
        }
		operator T()
		{
			return m_obj;
		}
		T relinquish()
		{
			T ret_val = m_obj;
			m_obj = NULL;
			return ret_val;
		}
private:
        T m_obj;
};
#endif

#endif // _AL_H_6729021398213489
