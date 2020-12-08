#include <stdbool.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "aflog.h"
#include "affont.h"

//#define SUPPORT_ARBIC
// 注：ToArabic部分代码已注掉，这里不提供源码。

#define FONT_PATH "/config/arial.ttf"

static FT_Library g_lib = NULL;
static FT_Face g_face = NULL;
static unsigned int g_size = 0;// 16 22 28

static bool g_Init = false;
static pthread_mutex_t g_FontMutex;

int AF_FT_Init()
{
	if (g_Init)
		return 0;
	
	if (pthread_mutex_init(&g_FontMutex, NULL) != 0)
		return -1;
		
	pthread_mutex_lock(&g_FontMutex);
	g_Init = true;	
		
	if (g_lib != NULL)
	{
		FT_Done_FreeType(g_lib);
		g_lib = NULL;
		g_face = NULL;
	}

	int nRet = FT_Init_FreeType(&g_lib);
	if (nRet != 0)
	{
		AFERR("FT_Init_FreeType failed nRet = %d\n", nRet);
		pthread_mutex_unlock(&g_FontMutex);
		return -1;
	}

	nRet = FT_New_Face(g_lib, FONT_PATH, 0, &g_face);
	if (nRet != 0)
	{
		AFERR("FT_New_Face failed nRet = %d\n", nRet);
		FT_Done_FreeType(g_lib);
		g_lib = NULL;
		pthread_mutex_unlock(&g_FontMutex);
		return -1;
	}

	AF_FT_SetFontSize(22, NULL, NULL, NULL);// 16 22 28
	pthread_mutex_unlock(&g_FontMutex);

	return 0;
}

int AF_FT_Exit()
{
	pthread_mutex_lock(&g_FontMutex);
   	if (g_lib != NULL)
	{
		FT_Done_FreeType(g_lib);
		g_lib = NULL;
		g_face = NULL;
	}
	g_Init = false;
	pthread_mutex_unlock(&g_FontMutex);

	pthread_mutex_destroy(&g_FontMutex);
    return 0;
}

static int AF_FT_SetFontSize(unsigned int nSize, unsigned int *nLineNum, unsigned int *nLineBase, unsigned int *nPixelMax)
{
	if (g_face == NULL)
		return -1;
		
	if (nSize < 16)
		nSize = 16;
	if (nSize > 28)
		nSize = 28;

	int nRet = 0;
	if (nSize != g_size)
	{
		nRet = FT_Set_Pixel_Sizes(g_face, 0, nSize);// 设置成nSize*nSize像素
		if (nRet != 0)
		{
			AFERR("FT_Set_Pixel_Sizes nSize(%d) failed nRet = %d\n", nSize, nRet);
		}
		g_size = nSize;
	}
	FT_Size size = g_face->size;
	if (nLineNum != NULL)
		*nLineNum = (unsigned int)(size->metrics.height >> 6);
	if (nLineBase != NULL)
		*nLineBase = (unsigned int)((size->metrics.height >> 6) + (size->metrics.descender >> 6) - 1);
   	if (nPixelMax != NULL)
        *nPixelMax = (unsigned int)(size->metrics.max_advance >> 6);

	return 0;
}

int AF_FT_SetSize(unsigned int nSize)
{
	pthread_mutex_lock(&g_FontMutex);
	int nRet = AF_FT_SetFontSize(nSize, NULL, NULL, NULL);
	pthread_mutex_unlock(&g_FontMutex);
	return nRet;
}

int AF_FT_GetSize()
{
	pthread_mutex_lock(&g_FontMutex);
	int nSize = g_size;
	pthread_mutex_unlock(&g_FontMutex);
	return nSize;
}

static unsigned short GetCharCode(const char* pch, unsigned int *pn)
{
	unsigned char ch = 0;//single char
	unsigned short code = 0;//unicode
	int flag = 0; //0 - empty, 1 - 1 char to finish unicode, 2 - 2 chars to finish unicode, -1 - error

	*pn = 0;
	while ((ch = (unsigned char)*pch))
	{
		pch++;
		if(ch & 0x80)
		{
			if((ch & 0xc0) == 0xc0)
			{
				if((ch & 0xe0) == 0xe0)
				{
					if((ch & 0xf0) == 0xf0) //ucs-4?
						break;
					if(flag)
						break;
					*pn = 3;
					flag = 2;
					code |= ((ch & 0x0f) << 12);
				}
				else
				{
					if(flag)
						break;
					*pn = 2;
					flag = 1;
					code |= ((ch & 0x1f) << 6);
				}
			}
			else
			{
				if(flag == 0)
				{
					break;
				}
				else if(flag == 1) //unicode finished
				{
					code |= (ch & 0x3f);
					break;
				}
				else
				{
					code |= ((ch & 0x3f) << 6);
				}
				flag--;
			}
		}
		else //ASCII
		{
			if(flag)
				break;
			*pn = 1;
			code = ch;
			break;
		}
	}

	if(ch == 0)
		code = 0;
	return code;
}

static int IsArbic(const unsigned short word)
{
	if ((word >= 0x0600 && word <= 0x06FF)
		//|| (word >= 0x0750 && word <= 0x077F)
		|| (word >= 0xFB50 && word <= 0xFDFF)
		|| (word >= 0xFE70 && word <= 0xFEFF))
		return 1;
	return 0;
}

static unsigned int ToUCS2(const char* str, unsigned int nSize, unsigned short *pCode, bool *pbArabic)
{
	*pbArabic = false;
	if (str == NULL || nSize == 0 || pCode == NULL)
		return 0;
	unsigned int i = 0, l = 0;
	unsigned int nNum = 0;
	for (i = 0; i < nSize; i += l)
	{
		pCode[nNum] = GetCharCode(&str[i], &l);
		if (l == 0 || pCode[nNum] == 0)
			break;
		if ((*pbArabic == false) && IsArbic(pCode[nNum]))
		{
			*pbArabic = true;
		}
		else if (('n' == pCode[nNum]) && (nNum > 0) && ('\\' == pCode[nNum - 1]))
		{
			pCode[nNum - 1] = '\n';
			continue;
		}
        nNum++;
        if (nNum >= nSize)
            break;
	}

	return nNum;
}

int AF_FT_StringConvert(const char *str, unsigned int nWidth, unsigned int nHeight, unsigned short usColor, unsigned char *pBuf)
{
	pthread_mutex_lock(&g_FontMutex);
	
	if ((g_face == NULL) || (str == NULL) || (nWidth == 0) || (nHeight == 0) || (pBuf == NULL))
	{
		pthread_mutex_unlock(&g_FontMutex);
		return -1;
	}

	unsigned int nLen = strlen(str);
	if (nLen == 0)
	{
		pthread_mutex_unlock(&g_FontMutex);
		return -1;
	}

	FT_Size	size = g_face->size;
	unsigned int nLineNum = (unsigned int)(size->metrics.height >> 6);// 最大线数
	unsigned int nLineBase = (unsigned int)((size->metrics.height >> 6) + (size->metrics.descender >> 6) - 1);// 基线
	unsigned int nPixelnum = (unsigned int)(size->metrics.max_advance >> 6);//最大像素数
	if (nHeight * 2 < nPixelnum)// 因为字体设置成宽高像素相等
	{
		pthread_mutex_unlock(&g_FontMutex);
		return -1;
	}
	
	bool bArabic = false;
	unsigned short *pCode = (unsigned short *)malloc(nLen * sizeof(unsigned short));
	unsigned int nNum = ToUCS2(str, nLen, pCode, &bArabic);
	if (bArabic)
	{
#ifdef SUPPORT_ARBIC	// 注：ToArabic部分代码已注掉，这里不提供源码。
		nNum = ToArabic(pCode, nNum);
#else
		nNum = 0;
#endif
	}
	if (nNum == 0)
	{
		AFERR("ToUCS2 error\n");
		free(pCode);
		pthread_mutex_unlock(&g_FontMutex);
		return -1;
	}
	
	nPixelnum = (nPixelnum * nNum < nWidth) ? nPixelnum * nNum : nWidth;

	unsigned int idx = 0;
	unsigned int nPixel = 0;
	for (idx = 0; idx < nNum; idx++)
	{
		if (nPixel >= nPixelnum)
			break;
		FT_UInt nIdx = FT_Get_Char_Index(g_face, pCode[idx]);
		int nRet = FT_Load_Glyph(g_face, nIdx, FT_LOAD_MONOCHROME | FT_LOAD_RENDER);
//		int nRet = FT_Load_Char(g_face, pCode[idx], FT_LOAD_MONOCHROME | FT_LOAD_RENDER);
		if (nRet != 0)
			continue;
		FT_GlyphSlot slot = g_face->glyph;
		if (slot == NULL)
			continue;
		int startx = nPixel + slot->bitmap_left;
		nPixel += (slot->advance.x >> 6);
    	if (nPixel >= nPixelnum)
        	nPixel = nPixelnum;		
		if (startx >= nPixelnum)
			break;
		int starty = nLineBase - slot->bitmap_top;
		int row, col, x;
		for (row = starty; row < (int)(starty + slot->bitmap.rows); row++)
		{
			if (row < 0)
				continue;
			if (row >= nLineNum)
				break;
			unsigned char *dst = pBuf + row * (nWidth * 2);
			unsigned char *src = slot->bitmap.buffer + (row - starty) * slot->bitmap.pitch;
			for (col = 0; col < (int)slot->bitmap.width; col++)
			{
				x = startx + col;
				if (x < 0)
					continue;
				if (x >= nPixel)
					break;
				if (src[col >> 3] & (0x80 >> (col & 0x07)))
					*(((unsigned short *)dst) + x) |= usColor;
			}
		}
	}

	free(pCode);
	pthread_mutex_unlock(&g_FontMutex);
	return 0;
}



