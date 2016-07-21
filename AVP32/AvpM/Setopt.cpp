

/*
    SetDvpOption()

    Sets an option for DVP.VXD.  <optionid_> must be one of the DVPOPTION_
    values from DVPAPI.H.

	Returns TRUE on success, FALSE on error.  Use GetLastError() for error code.
*/

BOOL SetDvpOption(HANDLE handle_, WORD optionid_, WORD value_)
	{
	DWORD count;
	WORD parameters [4];
	parameters[0] = optionid_;
	parameters[1] = value_;
	SetLastError(0);
	BOOL ret=DeviceIoControl(handle_, DVPIOC_SETOPTION, parameters, 4, 0, 0, &count, 0);
	DWORD err= GetLastError();
	return  (err == 0) && ret;
	}

BOOL SetDvpOption2(HANDLE handle_, WORD optionid_, void * value_, WORD valuelen_)
    {
    DWORD bytesreturned = 0L;
    char * buf = (char *)malloc(valuelen_ + sizeof(WORD));
    if (!buf)
        return FALSE;                               // out of memory

    *(WORD *)buf = optionid_;
    memcpy(buf + sizeof(WORD), value_, valuelen_);

    SetLastError(0);
    BOOL ret=DeviceIoControl(handle_, DVPIOC_SETOPTION2, buf, valuelen_ + sizeof(WORD), 0, 0, &bytesreturned, 0);
	DWORD err= GetLastError();
	free(buf);
	return  (err == 0) && ret;
    }

BOOL DvpSubDriverCtrl(HANDLE handle_, DWORD control_, char* options)
	{
	DWORD count;
	DWORD parameters [0x80];
	parameters[0] = DVPSUBDRV_AVP95;
	parameters[1] = control_;
	DWORD size=8;

	if(options){
		strcpy((char*)&(parameters[2]),options);
		size+=strlen(options)+1;
	}

	SetLastError(0);
                           //0x0800,    // DVPAPI_SUBDRIVERCTRL
	BOOL ret=DeviceIoControl(handle_, 0x0800, parameters, size, 0, 0, &count, 0);
	DWORD err= GetLastError();
	return  (err == 0) && ret;
	}


/*

    Get GK95.VXD handle by:

    HANDLE handle = CreateFile("\\\\.\\GK95", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL);


    Close it by:

    CloseHandle(handle);

*/

