#ifndef _CRC32_H_AE11EB8A_0293_4f5a_B3FA_535FAE45E318
#define _CRC32_H_AE11EB8A_0293_4f5a_B3FA_535FAE45E318


#include "../../helper/local_file.h"

unsigned long CRC32(const unsigned char *data, unsigned int count, unsigned long seed);
// the calculateFileCRC() function is used outside Updater in File CRC32 calculator tool
bool calculateFileCRC(const KLUPD::Path &sourceFileName, unsigned int &crc);


#endif // _CRC32_H_AE11EB8A_0293_4f5a_B3FA_535FAE45E318
