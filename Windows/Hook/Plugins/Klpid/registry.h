NTSTATUS	MyQueryRegistryValue(PUNICODE_STRING	puRegString, 
								 PUNICODE_STRING	puRegValue,
								 PULONG				Data,
								 ULONG				DataSize);

NTSTATUS	MyWriteRegistryValue(PUNICODE_STRING	puRegString, 
								 PUNICODE_STRING	puRegValue,
								 PVOID				Data,
								 ULONG				DataSize);

NTSTATUS	QueryMyParam(PVOID	Value, ULONG	ValueSize, PCWSTR	Key);

NTSTATUS	SetMyParam(PVOID	Value, ULONG	ValueSize, PCWSTR	Key);

NTSTATUS	KlQueryParam(PVOID Param, PULONG ParamSize, ULONG ParamType, PCHAR ParamName);

NTSTATUS	KlWriteParam(PVOID Param, PULONG ParamSize, ULONG ParamType, PCHAR ParamName);