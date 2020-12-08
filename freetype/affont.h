#ifndef __AF_FONT_H__
#define __AF_FONT_H__

#ifdef __cplusplus 
extern "C" { 
#endif

int AF_FT_Init();
int AF_FT_Exit();

int AF_FT_SetSize(unsigned int nSize);
int AF_FT_GetSize();

int AF_FT_StringConvert(const char *str, unsigned int nWidth, unsigned int nHeight, unsigned short usColor, unsigned char *pBuf);

#ifdef __cplusplus 
} 
#endif 

#endif

