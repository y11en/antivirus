#if !defined(_KAVSHELL_CMD)
#error "_KAVSHELL_CMD must be defined before including this file"
#endif

// Команды KAV Shell

// Основные команды управления задачами

_KAVSHELL_CMD( START )
_KAVSHELL_CMD( PAUSE )
_KAVSHELL_CMD( RESUME )
_KAVSHELL_CMD( STOP )
_KAVSHELL_CMD( STATUS )
_KAVSHELL_CMD( STATISTICS )
_KAVSHELL_CMD( MODE )
_KAVSHELL_CMD( HELP )
_KAVSHELL_CMD( EXPORT )
_KAVSHELL_CMD( IMPORT )
_KAVSHELL_CMD( EXIT )
_KAVSHELL_CMD( ADDKEY )
_KAVSHELL_CMD( ACTIVATE )
_KAVSHELL_CMD( INETSWIFT )


// Дополнительные команды

_KAVSHELL_CMD( SCAN )
_KAVSHELL_CMD( UPDATE )
_KAVSHELL_CMD( ROLLBACK )

// Внутренние команды

_KAVSHELL_CMD( TRACE )
_KAVSHELL_CMD( CLS )
_KAVSHELL_CMD( SPYWARE )
