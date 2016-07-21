#ifndef __CKAHSTM_H__
#define __CKAHSTM_H__

// Stealth Mode
namespace CKAHSTM
{
	typedef LPVOID HSTMNETLIST;

	struct StmNet {
        CKAHUM::IP Net;
		CKAHUM::IP Mask;
	};

	// запускает Stealth Mode
	CKAHUMMETHOD Start ();

	// останавливает Stealth Mode
	CKAHUMMETHOD Stop ();

	// приостанавливает Stealth Mode (при запуске плагин приостановлен)
	CKAHUMMETHOD Pause ();

	// возобновляет работу Stealth Mode (при запуске плагин приостановлен)
	CKAHUMMETHOD Resume ();

	// переводит интерфейс с указанным Mac-адресом в режим Stealth.
	// Mode : true - Stealthed, false - Unstealthed
	// По-умолчанию stealth-mode выставлен на все интерфейсы.
	CKAHUMMETHOD SetStealth(char MacAddr[6], bool Mode);

	// добавить unstealthed сеть
	CKAHUMMETHOD AddNet( StmNet* pNet );

	// удалить unstealthed сеть
	CKAHUMMETHOD RemoveNet( StmNet* pNet );

	// получить список unstealthed подсетей
	CKAHUMMETHOD GetNetList( HSTMNETLIST* phList );

	// получить размер списка
	CKAHUMMETHOD NetList_GetSize( HSTMNETLIST hList, int* psize);

	// получить элемент списка.
	CKAHUMMETHOD NetList_GetItem( HSTMNETLIST hList, int n, StmNet* pNet );

	// удалить список и освободить ресурсы
	CKAHUMMETHOD NetList_Delete(  HSTMNETLIST hList );
	
	// установить дефолтное поведение для сетей
	// Если true, то Stm будет применяться только в добавленных подсетях.
	// Если false, то Stm будет применяться везде, кроме добавленных сетей.
	// По умолчанию, т.е. если ничего не выставлять, то применено значение false.
	CKAHUMMETHOD SetNetDefaultReaction( bool NetUsesStm );
};


#endif