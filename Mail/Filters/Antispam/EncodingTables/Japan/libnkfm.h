#ifndef _LIBNKFM_H_
#define _LIBNKFM_H_

/*
 *  libnkfm - Kanji conversion library using nkf
 *
 *  Copyright (C), KUBO Takehiro <kubo-t@cx.airnet.ne.jp>
 */

/* nkf_init - libnkfm の初期化
 *
 * 引数:
 *    option    : nkf のオプションと同じ
 * 戻値:
 *    なし
 * 備考:
 *    nkf_convert(), nkf_n_convert(), nkf_convert_new() を呼ぶ前に必ず
 *    呼ぶこと。
 */
void nkf_init(const char *option);

/* nkf_convert - 文字コードの変換(変換先バッファの長さ制限なし)
 *
 * 引数:
 *    outbuf    : 変換後の文字列。
 *    inbuf     : 変換元の文字列、NUL終端までが変換対象
 * 戻値:
 *    outbuf の先頭アドレス
 * 備考:
 *    outbuf のバッファあふれの検知はしないので、十分な大きさのバッファ
 *    を用意すること。
 */
char *nkf_convert(char *outbuf, const char *inbuf);

/* nkf_n_convert - 文字コードの変換(変換先バッファの長さ指定)
 *
 * 引数:
 *    outbuf    : 変換後の文字列。
 *    outbuflen : outbuf の長さ
 *    inbuf     : 変換元の文字列、NUL終端までが変換対象
 * 戻値:
 *    outbuf に全部入ったときは、outbuf の先頭アドレス
 *    outbuf に入りきらないときは、NULL
 */
char *nkf_n_convert(char *outbuf, size_t outbuflen, const char *inbuf);

/* nkf_convert_new - 文字コードの変換(変換先バッファの動的確保)
 *
 * 引数:
 *    inbuf     : 変換元の文字列、NUL終端までが変換対象
 * 戻値:
 *    変換後の文字列の先頭アドレス。
 *    メモリー確保に失敗したときは NULL。
 * 備考:
 *    戻値の文字列を再利用しないときは、メモリーリークをふせぐため
 *    必ず frre() すること。
 */
char *nkf_convert_new(const char *inbuf);

#endif//_LIBNKFM_H_