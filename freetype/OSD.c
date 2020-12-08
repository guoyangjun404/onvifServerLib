int AF_OSD_START(int nVenc, OSD_TYPE nOSDType, AF_OSDAttr *pstOSDAttr)
{
	HI_S32 s32Ret;
	RGN_HANDLE Handle = nVenc * (int)OSD_TYPE_MAX + (int)nOSDType;
	RGN_ATTR_S stRgnAttr;
	stRgnAttr.enType = OVERLAY_RGN;
	stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;
	stRgnAttr.unAttr.stOverlay.stSize.u32Width = 512;// >= 2 * FontSize * FontNum
	stRgnAttr.unAttr.stOverlay.stSize.u32Height = 128;// >= 2 * FontSize
	stRgnAttr.unAttr.stOverlay.u32BgColor = 0x0000ffff;
	stRgnAttr.unAttr.stOverlay.u32CanvasNum = 2;

	s32Ret = HI_MPI_RGN_Create(Handle, &stRgnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_Create failed %#x\n", s32Ret);
		return -1;
	}

    MPP_CHN_S stChn;
    RGN_CHN_ATTR_S stChnAttr;
	stChn.enModId = HI_ID_VENC;
	stChn.s32DevId = 0;
	stChn.s32ChnId = nVenc;

	stChnAttr.bShow = pstOSDAttr->bShow;
	stChnAttr.enType = OVERLAY_RGN;
	stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = pstOSDAttr->nPosX & 0x1ffe;
	stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = pstOSDAttr->nPosY & 0x1ffe;
	stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
	stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
	stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;

	stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
	stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;
	stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = HI_FALSE;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = HI_FALSE;

	s32Ret = HI_MPI_RGN_AttachToChn(Handle, &stChn, &stChnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_AttachToChn failed %#x\n", s32Ret);
		return -1;
	}

	return 0;
}

int AF_OSD_STOP(int nVenc, OSD_TYPE nOSDType)
{
	HI_S32 s32Ret;
	MPP_CHN_S stChn;
	stChn.enModId = HI_ID_VENC;
	stChn.s32DevId = 0;
	stChn.s32ChnId = nVenc;
	RGN_HANDLE Handle = nVenc * (int)OSD_TYPE_MAX + (int)nOSDType;

	s32Ret = HI_MPI_RGN_DetachFromChn(Handle, &stChn);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_DetachFromChn failed %#x\n", s32Ret);
		return -1;
	}

	s32Ret = HI_MPI_RGN_Destroy(Handle);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_Destroy failed %#x\n", s32Ret);
		return -1;
	}

	return 0;
}

int AF_OSD_REFRESH(int nVenc, OSD_TYPE nOSDType, AF_OSDAttr *pstOSDAttr)
{
	HI_S32 s32Ret;
	RGN_HANDLE Handle = nVenc * (int)OSD_TYPE_MAX + (int)nOSDType;
	RGN_ATTR_S stRgnAttr;
	s32Ret = HI_MPI_RGN_GetAttr(Handle, &stRgnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_GetAttr failed %#x\n", s32Ret);
		return -1;
	}

	MPP_CHN_S stChn;
	stChn.enModId = HI_ID_VENC;
	stChn.s32DevId = 0;
	stChn.s32ChnId = nVenc;
	RGN_CHN_ATTR_S stChnAttr;
	s32Ret = HI_MPI_RGN_GetDisplayAttr(Handle, &stChn, &stChnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_GetDisplayAttr failed %#x\n", s32Ret);
		return -1;
	}

	stChnAttr.bShow = pstOSDAttr->bShow;
	stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = pstOSDAttr->nPosX & 0x1ffe;
	stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = pstOSDAttr->nPosY & 0x1ffe;
	s32Ret = HI_MPI_RGN_SetDisplayAttr(Handle, &stChn, &stChnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_GetDisplayAttr failed %#x\n", s32Ret);
		return -1;
	}

	unsigned int nWidth = stRgnAttr.unAttr.stOverlay.stSize.u32Width;
	unsigned int nHeight = stRgnAttr.unAttr.stOverlay.stSize.u32Height;
	unsigned char *pBuffer = (unsigned char *)malloc(nWidth * nHeight * 2);
	memset(pBuffer, 0, nWidth * nHeight * 2);
	int i = 0;
	int nFontSize = AF_FT_GetSize() + 4;
	unsigned short usColor = (1 << 15) /*| (0x1f << 10)*/ | (0x1f << 5)/* | 0x1f*/; // 绿色
	for (i = 0; i < pstOSDAttr->nStringNum; i++)
	{
		char *pstring = pstOSDAttr->ppString[i];
		int nHeightPos = nFontSize * i;
		if (nHeightPos >= nHeight)
			break;
		unsigned char *pBuf = pBuffer + nHeightPos * nWidth * 2;
		if (AF_FT_StringConvert(pstring, nWidth, nFontSize, usColor, pBuf) != 0)
		{
			continue;
		}
	}

	BITMAP_S stBitmap;
	stBitmap.u32Width		= stRgnAttr.unAttr.stOverlay.stSize.u32Width;
	stBitmap.u32Height		= stRgnAttr.unAttr.stOverlay.stSize.u32Height;
	stBitmap.enPixelFormat	= PIXEL_FORMAT_RGB_1555;
	stBitmap.pData 			= (HI_VOID *)pBuffer;
	s32Ret = HI_MPI_RGN_SetBitMap(Handle, &stBitmap);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_RGN_SetBitMap failed %#x\n", s32Ret);
		free(pBuffer);
		return -1;
	}

	free(pBuffer);
	return 0;
}

