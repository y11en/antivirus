; // This is the header section. 

MessageIdTypedef=DWORD

SeverityNames=(Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(Runtime=0x2:FACILITY_RUNTIME
              )

LanguageNames=(English=0x409:MSG00409)

; // The following are message definitions.

MessageId=0x1234
Severity=Error
Facility=Runtime
SymbolicName=MSG_ERROR
Language=English
%1
.
MessageId=0x1236
Severity=Warning
Facility=Runtime
SymbolicName=MSG_WARNING
Language=English
%1
.
MessageId=0x1237
Severity=Informational
Facility=Runtime
SymbolicName=MSG_INFORMATIONAL
Language=English
%1
.
