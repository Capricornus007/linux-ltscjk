#include <linux/font.h>
#include <linux/module.h>

/* 定義一個足夠大的空數據塊，防止連結器報錯，16x16 字體大約需要每字元 32 字節 */
/* CJKTTY 通常有 65536 個字元 */
const unsigned char fontdata_cjk_16x16[2097152] = { 0 };
EXPORT_SYMBOL(fontdata_cjk_16x16);

const struct font_desc font_cjk_16x16 = {
	.idx	= -1,
	.name	= "CJK-16x16",
	.width	= 16,
	.height	= 16,
	.data	= fontdata_cjk_16x16,
	.pref	= 0,
};
EXPORT_SYMBOL(font_cjk_16x16);

/* 這是 fbcon 核心代碼在找的關鍵符號 */
const void *font_bits = fontdata_cjk_16x16;
EXPORT_SYMBOL(font_bits);
