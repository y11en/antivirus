/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	wat/authserver.h
 * \author	Mikhail Karmazine
 * \date	06.02.2003 11:41:59
 * \brief	Интерфейс KLWAT::AuthServer
 * 
 */

#ifndef __KLWAT_AUTHSERVER_H__
#define __KLWAT_AUTHSERVER_H__

#include <string>
#include <set>

#include "std/base/kldefs.h"
#include "std/par/params.h"
#include "./common.h"
#include "./osdep.h"

#define KLWAT_SEC_CONTEXT L"wat-security-context"

namespace KLWAT
{

	/*!
		\brief GroupVector - набор идентификаторов преодопределенных групп. Например, это может
            быть группа Everyone, или Domain Admins, или Nagent.
	*/
    typedef std::vector<int> GroupVector;

	/*!
		\brief CustomAccessToken - "самопальный" AccessToken. Содержит
            в себе список преодопределенных групп, которые он представляет.
	*/
    struct CustomAccessToken
    {
        GroupVector vectGroups;
		std::wstring wstrLogin;
    };


	/*!
		\brief AccessTokenHolder - содержит в себе и выдает наружу AccessToken.
            Объект, реализующий данный интерфейс, должен закрывать AccessToken
            при удалении.
	*/
    class KLSTD_NOVTABLE AccessTokenHolder : public KLSTD::KLBase
	{
    public:
	    /*!
		    \brief GetType - возвращает тип аутентификации, используемый при
                получении AccessToken'а.

                Если atOS, то класс содержит AccessToken, полученный от
                операционной системы, он возвращается методом GetOSAccessToken(). 

                Если atCustom, то класс содержит "самопальный" AccessToken,
                содержащий в себе список преодопределенных групп. Возвращается
                методом GetCustomGroupVector().
	    */      
        virtual AuthenticationType GetType() const = 0;


	    /*!
		    \brief GetOSAccessToken - возвращает AccessToken, полученный от
                операционной системы.
                Вызов валиден, только если GetType() == atOS. В противном
                случае сработает ASSERT.
	    */      
        virtual const OSAccessToken & GetOSAccessToken() const = 0;


	    /*!
		    \brief GetCustomAccessToken - возвращает самодельный AccessToken,
                содержащий в себе список преодопределенных групп. Группы могут быть
                такие, как Everyone, Nagent, или Domain Admins.
                Вызов валиден, только если GetType() == atCustom. В противном
                случае сработает ASSERT.
	    */      
        virtual const CustomAccessToken & GetCustomAccessToken() const = 0;
	};


	/*!
		\brief CustomCredentialsConverter - создает CustomAccessToken, 
		который содержит список предопределенный групп,      
		соответствующий переданным параметрам.


	*/      
    class KLSTD_NOVTABLE CustomCredentialsConverter : public KLSTD::KLBaseQI
	{
    public:
	    /*!
		    \brief ConvertCustomCredetials - принимает
            на вход некие абстракные credentials, переданные в Params, и
            выдает CustomAccessToken, который содержит список предопределенных групп,
            соответствующий этим credentials. Этот метод имеет право выбросить
            исключение..
        */
        virtual void ConvertCustomCredentials(
            const KLPAR::Params * pCustomCredetials,
            CustomAccessToken & customAccessToken,
			std::wstring &wstrUserName,
			std::wstring &wstrUserDomain,
			std::wstring &wstrUserPassword ) = 0;
	    /*!
		    \brief AdjustSSLCredetials - принимает
            на вход PublicKey клиента и переданные им Params, и
            выдает CustomAccessToken, который содержит список предопределенных групп,
            соответствующий этому клиенту. Этот метод имеет право выбросить
            исключение..
        */
        virtual void AdjustSSLCredentials(
			void* pPublicKey,
			size_t nPublicKeySize,
            CustomAccessToken & customAccessToken) = 0;
    };
    
    const wchar_t c_szwEventsOnlyContext[]= L"KLWAT_CTX_EVENTS_ONLY";
    const wchar_t c_szwAdmServerContext[]= L"KLWAT_CTX_ADMSERVER";
    const wchar_t c_szwFooProcessContext[]= L"KLWAT_CTX_PROCESS";
    const wchar_t c_szwSlaveAdmServerContext[]= L"KLWAT_CTX_SLAVE_ADMSERVER";
    const wchar_t c_szwMasterAdmServerContext[]= L"KLWAT_CTX_MASTER_ADMSERVER";

	/*!
		\brief AuthServer - серверная часть аутентификации. См. AVT_TEST для
        примера использования.
	*/
    class KLSTD_NOVTABLE AuthServer : public KLSTD::KLBase
    {
    public:
        virtual ~AuthServer () {};

        /*!
          \brief	HandshakeWithDataFromClient
             Производит обработку массива данных, полученного от клиента, желающего
                аутентифицироваться.

          \param	std::wstring & wstrClientIdentity [in/out] - на входе содержит
                        либо пустую строку - тогда считается, что это первый вызов от данного клиента,
                        и генерируется новый ID, который и возвращается в этом параметере. Либо
                        параметр содержит тот самый ID, который сгенерировался при предыдущем вызове
                        от данного клиента.
          \param	const std::string & strInData [in] - данные от клиента. Это бинарные 
                        данные (содержащие в т.ч. non-printable characters), и строка не 
                        должна рассматриваться как NULL-terminated.
          \param	std::string & strOutData [out] - данные, которые надо передать клиенту.
                        Это бинарные данные (содержащие в т.ч. non-printable characters), и
                        строка не должна рассматриваться как NULL-terminated.
          \return	bool - возвращает true, когда сервер закончил аутентифицировать клиента.
        */
        virtual bool HandshakeWithDataFromClient( std::wstring & wstrClientIdentity,
                                                  const std::string & strInData,
                                                        std::string & strOutData  ) = 0;


        /*!
          \brief	GenClientContext - генерирует объект, котролирующий время жизни
            клиентского контекста, полученного с помощью данного объекта.
            Когда такой объект удаляется, то у данного объекта вызывается
            метод ForgetClient.

            Этот метод, вызванный несколько раз с одним и тем же ID клиента, возвращает
            указатель на один и тот же объект.

          \param	wstrClientIdentity - ID клиента, полученный в процессе handshake'а.
          \return	KLSTD::CAutoPtr<ClientContext> - объект, котролирующий время жизни
            клиентского контекста.
        */
        virtual KLSTD::CAutoPtr<ClientContext>
            GenClientContext( const std::wstring & wstrClientIdentity ) = 0;		


        /*!
          \brief	ImpersonateThread Impersonate thread by client access token 
                received by handshake process.
                Вызов метода возможен только для клиентов, использующих windows-аутентификацию

          \param	const std::wstring & wstrClientIdentity - ID клиента, полученный
                в процессе handshake'а
        */
        virtual void ImpersonateThread( const std::wstring & wstrClientIdentity ) = 0;

        /*!
          \brief	IsThreadImpersonated - проверяет, не имперсонирован ли поток
                клиентом, использующим windows-аутентификацию.
          \return	bool -  true  если поток имперсонирован, false otherwise
        */
        virtual bool IsThreadImpersonated() = 0;

        /*!
          \brief	GetImpersonationToken() Возвращает поток в состояние, которое было 
            на момент вызова ImpersonateThread()
                Вызов метода возможен только для клиентов, использующих windows-аутентификацию
          \param	const std::wstring & wstrClientIdentity - ID клиента, полученный
                в процессе handshake'а
        */
        virtual void RevertThread( const std::wstring & wstrClientIdentity ) = 0;

        /*!
          \brief	GetImpersonationToken() Возвращает impersonation access token,
            полученный в процессе handshake'а. При этом поток, из которого
            вызывается эта функция, НЕ ДОЛЖЕН БЫТЬ ИМПЕРСОНИРОВАН.
                Вызов метода возможен для клиентов, использующих оба вида аутентификации

          \param	const std::wstring & wstrClientIdentity - ID клиента, полученный
                в процессе handshake'а
        */
        virtual KLSTD::CAutoPtr<AccessTokenHolder> GetImpersonationToken( const std::wstring & wstrClientIdentity ) = 0;

        /*!
          \brief	ForgetClient - удаляет клиентский контекст. Если указанного клиента нет,
            то исключение НЕ выбрасывается.

          \param	const std::wstring & wstrClientIdentity
        */
        virtual void ForgetClient( const std::wstring & wstrClientIdentity ) = 0;
        
        /*!
          \brief	GetImpersonationToken() Возвращает access token, 
                    предназначенный для проверки прав доступа. При этом 
                    поток, из которого вызывается эта функция, НЕ ДОЛЖЕН
                    БЫТЬ ИМПЕРСОНИРОВАН. Вызов метода возможен для 
                    клиентов, использующих оба вида аутентификации

          \param	const std::wstring & wstrClientIdentity - ID клиента, полученный
                в процессе handshake'а
        */
        virtual KLSTD::CAutoPtr<AccessTokenHolder> GetAccessCheckToken( const std::wstring & wstrClientIdentity ) = 0;

		/*!
          \brief	GenClientContext - генерирует объект, котролирующий время жизни
            клиентского контекста, полученного с помощью данного объекта.
            Когда такой объект удаляется, то у данного объекта вызывается
            метод ForgetClient.

            Этот метод, вызванный несколько раз с одним и тем же ID клиента, возвращает
            указатель на один и тот же объект.

          \param	wstrClientIdentity - ID клиента, полученный в процессе handshake'а.
		  \param	customAccessToken - пользовательский контекст
          \return	KLSTD::CAutoPtr<ClientContext> - объект, котролирующий время жизни
            клиентского контекста.
        */
        virtual KLSTD::CAutoPtr<ClientContext> GenClientContext( const std::wstring & wstrClientIdentity, 
			const KLWAT::CustomAccessToken& customAccessToken ) = 0;		

    };
}

/*!
  \brief	KLWAT_CreateAuthServer - создает объект типа KLWAT::AuthServer

  \param	KLWAT::AuthServer **ppAuthServer [out] - результат
  \param	bool bMultithreaded = true [in] - obvious
  \return	void 
*/
KLCSTR_DECL void KLWAT_CreateAuthServer( KLWAT::AuthServer **ppAuthServer,
                                        bool bMultithreaded = true );

KLCSTR_DECL void KLWAT_GetGlobalAuthServer( KLWAT::AuthServer **ppAuthServer );

/*!
  \brief	KLWAT_SetCustomCredentialsConverter - устанавливает глобальный 
        конвертер для custom credentials
*/
KLCSTR_DECL void KLWAT_SetCustomCredentialsConverter( KLWAT::CustomCredentialsConverter * pConverter );

/*!
  \brief	KLWAT_GetCurrentToken() Возвращает access token,
    с которым в данный момент работает поток. Если поток имперсонирован,
    то это access token, которым он имперсонирован. Если поток не имперсонирован,
    то возвращается access token процесса.
*/
KLCSTR_DECL KLSTD::CAutoPtr<KLWAT::AccessTokenHolder> KLWAT_GetCurrentToken();


/*!
  \brief	KLWAT_StorePrivateString - "безопасно" сохраняет строку.
    Для WINDOWS UNICODE - реализации строка сохраняется где-то в реестре,
    как-то при этом шифруясь. Записать и прочитать строку имеют право члены
    локальной группы BUILTIN/Administrators. Подробности см. в MSDN, в описании
    функций LsaStorePrivateData() и LsaRetrievePrivateData().

  \param	onst std::wstring& wstrKey [in] - ключ, по которому в дальнейшем будет 
        вытягиваться строка.
  \param	const std::wstring& wstrPrivateString [in] - сохраняемая строка.
*/
KLCSTR_DECL void KLWAT_StorePrivateString(
    const std::wstring& wstrKey,
    const std::wstring& wstrPrivateString );


/*!
  \brief	KLWAT_RetrievePrivateString - читает строку, сохраненную предварительно
    функцией KLWAT_StorePrivateString.

  \param	onst std::wstring& wstrKey [in] - ключ, по надо прочитать искать строку.
  \param	const std::wstring& wstrPrivateString [out] - результат.
*/
KLCSTR_DECL void KLWAT_RetrievePrivateString(
    const std::wstring& wstrKey,
          std::wstring& wstrPrivateString );

#endif //__KLWAT_AUTHSERVER_H__

// Local Variables:
// mode: C++
// End:
