BOOLEAN	CheckTcpScan(PSTAT_ELEMENT	Element)
{
	ULONG	ListSize;
	ListSize = GetListCount(&Element->TcpPortList);
	// Если портов в листе больше, чем заявлено в параметрах - это сканирование !
	if (ListSize >= AttackOptions.MaxOpenedTcpPorts)
		return TRUE;
	return FALSE;
}

BOOLEAN	CheckUdpScan(PSTAT_ELEMENT	Element)
{	
	ULONG	ListSize;
	ListSize = GetListCount(&Element->UdpPortList);
	// Если портов в листе больше, чем заявлено в параметрах - это сканирование !
	if (ListSize >= AttackOptions.MaxOpenedUdpPorts)
		return TRUE;
	return FALSE;	
}
