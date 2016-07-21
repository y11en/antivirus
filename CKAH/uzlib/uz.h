#ifndef __UZ_H__
#define __UZ_H__

int  UnZip (const char *zipfile, const char*outdir);

int  UnZipToMemory(const char *zipfile, const char *file, const char *& ptr, int& size );
void UnZipFreeMemory(const char *ptr);

bool UnZipIsFile(const char *zipfile, const char *filename);

//int  UnZipGetFileList(const char *zipfile, const char **filenames, int files);
//void UnZipFreeFileList(const char **filenames, int files);

#define UZ_OK              0   /* no error */
#define UZ_COOL            0   /* no error */
#define UZ_WARN            1   /* warning error */
#define UZ_ERR             2   /* error in zipfile */
#define UZ_BADERR          3   /* severe error in zipfile */
#define UZ_MEM             4   /* insufficient memory (during initialization) */
#define UZ_MEM2            5   /* insufficient memory (password failure) */
#define UZ_MEM3            6   /* insufficient memory (file decompression) */
#define UZ_MEM4            7   /* insufficient memory (memory decompression) */
#define UZ_MEM5            8   /* insufficient memory (not yet used) */
#define UZ_NOZIP           9   /* zipfile not found */
#define UZ_PARAM          10   /* bad or illegal parameters specified */
#define UZ_FIND           11   /* no files found */
#define UZ_DISK           50   /* disk full */
#define UZ_EOF            51   /* unexpected EOF */

#endif //__UZ_H__