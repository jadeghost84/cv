#pragma once
#ifndef DH_CONNECTION
#define DH_CONNECTION

#include "dhnetsdk.h"
#include "dhplay.h"
#include <iostream>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/highgui.hpp>
#define PLAYPORT 1
typedef void(*callbackfun)(cv::Mat);

class DHConnection
{
public:
	DHConnection(char*&, int, char*&, char*&, callbackfun, int nChannelID = 0, int dwDataType = 0);
	//void CALLBACK DisConnectFunc(LLONG lLoginID, char* pchDVRIP, LONG nDVRPort, LDWORD dwUser);
	//void CALLBACK AutoReConnectFunc(LONG lLoginID, char* pchDVRIP, LONG nDVRPort, DWORD dwUser);
	//void CALLBACK RealDataCallBackEx(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, LONG lParam, DWORD dwUser);
	//void CALLBACK DecCBFun(LONG nPort, char* pBuf, LONG nSize, FRAME_INFO* pFrameInfo, void* pUserData, LONG nReserved2);
	
};

#endif // !DH_CONNECTION