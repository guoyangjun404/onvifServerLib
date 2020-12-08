#ifndef __OSD_H__
#define __OSD_H__

#define OSD_UDEF_STRING_LEN 64
#define OSD_STRING_MAX 4

typedef enum tagOSD_TYPE
{
	OSD_TYPE_TIME = 0,
	OSD_TYPE_GPS,
	OSD_TYPE_TEMP,
	OSD_TYPE_ATTI,

	OSD_TYPE_UDEF0,
	OSD_TYPE_UDEF1,
	OSD_TYPE_UDEF2,

	OSD_TYPE_MAX
}OSD_TYPE;

typedef struct tag_AF_OSDAttr
{
	int nPosX;
	int nPosY;
	int bShow;

	int nStringNum;// <=OSD_STRING_MAX
	char ppString[OSD_STRING_MAX][OSD_UDEF_STRING_LEN];
}AF_OSDAttr;

int AF_OSD_START(int nVenc, OSD_TYPE nOSDType, AF_OSDAttr *pstOSDAttr);
int AF_OSD_STOP(int nVenc, OSD_TYPE nOSDType);
int AF_OSD_REFRESH(int nVenc, OSD_TYPE nOSDType, AF_OSDAttr *pstOSDAttr);

#endif

