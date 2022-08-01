#include "DHConnection.h"

callbackfun handleMat;


void CALLBACK DisConnectFunc(LLONG lLoginID, char* pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
	std::cout << "�豸����" << std::endl;
	return;
}
//�豸�Զ������ص�����
void CALLBACK AutoReConnectFunc(LONG lLoginID, char* pchDVRIP, LONG nDVRPort, DWORD dwUser)
{
	std::cout<<"�Զ������ɹ�."<<std::endl;
	return;
}
//ʵʱ���ݻص��������ڴ˴���ԭʼ����

void CALLBACK RealDataCallBackEx(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, LONG lParam, DWORD dwUser)
{
	BOOL bInput = FALSE;
	if (0 != lRealHandle)
	{
		switch (dwDataType) {
		case 0:
			//ԭʼ����Ƶ�������
			//printf("receive real data, param: pBuffer[%p]\n", pBuffer);
			//cout << dwBufSize;
			bInput = PLAY_InputData(PLAYPORT, pBuffer, dwBufSize);
			if (!bInput)
			{
				printf("input data error: %d\n", PLAY_GetLastError(PLAYPORT));
			}
			break;
		case 1:
			//��׼��Ƶ����

			break;
		case 2:
			//yuv ����

			break;
		case 3:
			//pcm ��Ƶ����

			break;
		case 4:
			//ԭʼ��Ƶ����

			break;
		default:
			break;
		}
	}
}
//���뺯�� ��YUV420����ΪIplImage
IplImage* YUV420_To_IplImage_Opencv(char* pYUV420, int width, int height) {
	if (!pYUV420)
	{
		return NULL;
	}
	IplImage* yuvimage, * rgbimg, * yimg, * uimg, * vimg, * uuimg, * vvimg;
	int nWidth = width;
	int nHeight = height;
	rgbimg = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3);
	yuvimage = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3);
	yimg = cvCreateImageHeader(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 1);
	uimg = cvCreateImageHeader(cvSize(nWidth / 2, nHeight / 2), IPL_DEPTH_8U, 1);
	vimg = cvCreateImageHeader(cvSize(nWidth / 2, nHeight / 2), IPL_DEPTH_8U, 1);
	uuimg = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 1);
	vvimg = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 1);
	cvSetData(yimg, pYUV420, nWidth);
	cvSetData(uimg, pYUV420 + nWidth * nHeight, nWidth / 2);
	cvSetData(vimg, pYUV420 + long(nWidth * nHeight * 1.25), nWidth / 2);
	cvResize(uimg, uuimg, CV_INTER_LINEAR);
	cvResize(vimg, vvimg, CV_INTER_LINEAR);
	cvMerge(yimg, uuimg, vvimg, NULL, yuvimage);
	cvCvtColor(yuvimage, rgbimg, CV_YCrCb2RGB);
	cvReleaseImage(&uuimg);
	cvReleaseImage(&vvimg);
	cvReleaseImageHeader(&yimg);
	cvReleaseImageHeader(&uimg);
	cvReleaseImageHeader(&vimg);
	cvReleaseImage(&yuvimage);
	if (!rgbimg)
	{
		return NULL;
	}
	return rgbimg;
}
void CALLBACK DecCBFun(LONG nPort, char* pBuf, LONG nSize, FRAME_INFO* pFrameInfo, void* pUserData, LONG nReserved2)
{
	//�������ݺ󣬿��ٷ���,����ʼ����ص���һ֡����;��Ҫ�ڻص������г����񣬷�������������һ֡����
	// pbuf���������YUV I420��ʽ������ 
	if (pFrameInfo->nType == 3) //��Ƶ����
	{

		cv::Mat myuv(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);
		cv::Mat mgMat;
		//cvtColor(myuv, mgMat, CV_YUV2RGB_I420);
		IplImage* mgIplImage = YUV420_To_IplImage_Opencv(pBuf, pFrameInfo->nWidth, pFrameInfo->nHeight);
		mgMat = cv::cvarrToMat(mgIplImage);
		//std::cout << "( " << myuv.rows << "," << myuv.cols << "," << myuv.channels() << ")" << endl;
		//std::cout << "( " << mgMat.rows << "," << mgMat.cols << "," << mgMat.channels() << ")" << endl;
		//yolo_model.detect(mgMat);
		//imshow("demo", mgMat);
		handleMat(mgMat);
		cvReleaseImage(&mgIplImage);

		//printf("receive real data, param: pBuffer[%p]\n", pBuf);
	}
	else if (pFrameInfo->nType == T_AUDIO16)
	{
		//cout<<"Audio CallBack"<<endl;
	}
	else
	{
		printf("nType = %d\n", pFrameInfo->nType);
	}
	//cout << PLAY_GetSourceBufferRemain(PLAYPORT) << endl;
	return;
}



DHConnection::DHConnection(char*& ip, int port, char*& adminName, char*& password, callbackfun _handleMat, int nChannelID, int dwDataType)
{
	handleMat = _handleMat;
	//CLIENT_LogClose();
	NET_DEVICEINFO_Ex deviceInfo = { 0 };
	long long lLogin;
	CLIENT_Init((fDisConnect)(DisConnectFunc), 0);//��ʼ��sdk�����ö��߻ص�����
	CLIENT_SetAutoReconnect((fHaveReConnect)AutoReConnectFunc, 0);
	std::cout << " Login Device ..." << std::endl;
	int error = 0;
	lLogin = CLIENT_LoginEx2(ip, port, adminName, password, EM_LOGIN_SPEC_CAP_TCP, NULL, &deviceInfo, &error);//��½�豸���û�����ѡ�豸��
	LLONG lRealPlay;
	if (lLogin != 0)
	{
		printf("Login Success ,Start Real Play\n");
		//�򿪲���ͨ��
		PLAY_OpenStream(PLAYPORT, 0, 0, 900 * 1024);
		PLAY_SetDecCallBackEx(PLAYPORT, DecCBFun, NULL);
		BOOL bPlayRet = PLAY_Play(PLAYPORT, NULL);
		lRealPlay = CLIENT_RealPlayEx(lLogin, nChannelID, 0);
		if (lRealPlay != 0)
		{
			//���ھ������ֵ�������ֻ�ص�ԭʼ����
			CLIENT_SetRealDataCallBackEx2(lRealPlay, (fRealDataCallBackEx2)RealDataCallBackEx, (DWORD)dwDataType, 0x1f);
		}
		else {
			printf("CLIENT_RealPlay: failed! Error code: %x.\n", CLIENT_GetLastError());
		}
	}
	else
	{
		printf(" Login Fail,code:%d \n", error);
	}
	getchar();

	//�رղ���ͨ�����ͷ���Դ
	PLAY_Stop(PLAYPORT);
	PLAY_CloseStream(PLAYPORT);
	//�ͷ������
	CLIENT_StopRealPlay(lRealPlay);
	CLIENT_Logout(lLogin);
	CLIENT_Cleanup();
}

