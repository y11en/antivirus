#ifndef __BT_DESER_H
#define __BT_DESER_H

bool _bt_deserialize(char* format, cCachedRead& data, void* args[], bool allocated[], size_t* pargc);

bool _bt_deserialize_int64(__int64* pvalue, cCachedRead& data );
bool _bt_deserialize_int(int* pvalue, cCachedRead& data );
bool _bt_deserialize_byte(unsigned char* pvalue, cCachedRead& data );
bool _bt_deserialize_str_ansi(char** pstr, cCachedRead& data );
bool _bt_deserialize_str_uni(wchar_t** pwstr, cCachedRead& data );

#endif // __BT_DESER_H