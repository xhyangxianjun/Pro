/***************************************************************
Copyright (C), 2019-2100, CFHS Tech. Co., Ltd.
��  ������Server.cpp
��    ��������socket���������ݿ⣬�߼�ҵ����
�����б�: 1.Start()					�������г���
		  2.CloseMySocket			�ر�����
		  3.CreateSocket			����socket
		  4.BandSocket				�󶨱��ص�ַ�Ͷ˿ں�
		  5.ListenSocket			��ʼ������������������ӵȴ�����
		  6.AcceptSocket			�����������ӵ����󣬲����ؾ��
		  7.RecvAndSendSocket		���ܺͷ������ݸ��ͷ���
		  8.DefectDetect			ȱ�ݼ��
		  9.ResultProcess			����ж�
		  10.initInfo				��ʼ����Ϣ
		  11.CreateFolder			�����ļ���·��
		  12.FindFreeSpace			��ѯ����ʣ��ռ�
		  13.GetLocalIpAddress		��ȡ����IP��ַ
��    �ߣ�ZHZ
��    �ڣ�2019/9/23
��    ����1.0
��ʷ�汾��<�޸���>  <��  ��>  <��  ��> <��  ��>
		    ZHZ	    2019/9/23   1.0     �����ļ�
***************************************************************/
#pragma once
#include "stdafx.h"
#include "Server.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h> 
#include <thread>
#include <map>
#include <queue>
#include <iostream>

#include <vector>
#include <experimental/filesystem> 
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>  //highgui ģ��ͷ�ļ�
#include <opencv2/imgproc.hpp>  //ͼ����ͷ�ļ�
#include "Algorithm.h"


#include <Iphlpapi.h>       //��ȡIp��ַ��������ͷ�ļ�
#include <iostream>
#include <fstream>
#include <sstream>        //istringstream ����������ͷ�ļ�
#include <mutex>

#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��

#include "json.h"

typedef Json::Writer JsonWriter;
typedef Json::Reader JsonReader;
typedef Json::Value  JsonValue;

using namespace cv;
using namespace std;
// �ͷ�ָ���
#define RELEASE(x)                      {if(NULL != x){delete x; x = NULL;}}
// �ͷ�ָ�������
#define RELEASE_ARRAY(x)                {if(NULL != x){delete[] x; x = NULL;}}

#define MYPROT	6000

//SOCKET sClient;
//��־�ļ�
FILE	*pFile;	
string  CompressOKFolderPath;    //ѹ��OKͼ�ļ���·��
string  CompressNGFolderPath;    //ѹ��NGͼ�ļ���·��
string  DefectFolderPath;		//ȱ��Сͼ�ļ���·��
string  OriginalOKFolderPath;    //ԭOKͼ�ļ���·��
string  OriginalNGFolderPath;    //ԭNGͼ�ļ���·��
string	OriginalFolderPath;		 //ԭͼ�ļ���·��
string  ProcedureFolderPath;    //����ͼ�ļ���·��
string  CcfFilePath;				//ccf�ļ�·��

vector<string> AlgorithmData;  //�㷨����
vector<string> ParameterData;  //��������
vector<string> CameraInfoData; //�����Ϣ����
vector<string> RoiInfoData;	   //ROi��Ϣ����
vector<string> Offlinefiles;	//����ģʽ�±���ͼƬ�ļ�
vector<string> OfflinefilesId;  //����ģʽ�±���ͼƬ��ID

vector<string> featureInfo;					//����������Ϣ
multimap<string, vector<string>> Defectdefinecage; //ȱ�ݶ������

queue<string> RecvData;  //socket���յ�����
queue<string> ReRecvData; ////socket���յ�����
queue<string> RecvImg;
queue<string> RecvID;
vector<string> Socketdata;     //������λ������·������ݷָ�����
queue<Mat> ImgMat;


int	nResultMode;
bool ControlF;
Server m_server;
int currentImg;

void DefectDetect(SOCKET recvsocket); //ȱ�ݼ��
int FindFreeSpace();				//������ʣ��ռ�
void FindCameraImg(SOCKET recvsocket); //����ͼƬ���з��ദ��
int ServerThread(SOCKET *ClientSocket); //��������socket����
int SendDetect(SOCKET recvsocket, vector<string> &RecvInfo, int iflag);  //������Ϣ����λ��
int Algorithm_Type();
void InitAlgrithmPara_hjh(int i);
void InitAlgrithmPara(int i);
void InitAlgrithmPara_ITO(int i);

//�������
/*********************************************************************/

std::mutex my_mutex_1;

#include <time.h> 
#include <stdio.h>
#include <stdarg.h>
#include "conio.h"
#include <cstdio>
#include "math.h"
#include "sapclassbasic.h"
#include "ExampleUtils.h"
#include <experimental/filesystem> 

//�����ռ�
#pragma warning(disable: 4996)//����itoa���﷨����

//�ļ�����·��

char *fileBmpFormat = "-format bmp";
char *fileJpgFormat = "-format jpg";
string strOrigBmpName;
string strOrigJpgName;
//static int icount = 0;
//int bufType = -1;

SapAcquisition	*Acq = NULL;
SapBuffer      *Buffers = NULL;
SapTransfer    *Xfer = NULL;
SapView        *View = NULL;

//SapBuffer outBuf, compBuf;

////Static Functions

string strCamType;

int nSaveImg;
int nReady;
bool bflag;
bool bCamera;


static void XferCallback(SapXferCallbackInfo *pInfo);
static BOOL GetOptions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);
void CameraInterface(string CameraType, int SaveImg);
void CameraGrab();
void saveOriginalBmpImage();
void saveOriginalJpgImage();
int write_log(FILE* pFile, const char *format, ...);


void CameraInterface(string CameraType, int SaveImg)
{
	//strCamType = CameraType;

	nSaveImg = SaveImg;
	nReady = 0;
	std::thread t(CameraGrab);
	t.detach();
}


void CameraGrab()
{
	UINT32   acqDeviceNumber;
	char*    acqServerName = new char[CORSERVER_MAX_STRLEN];
	char*    configFilename = new char[MAX_PATH];

	bCamera = false;
	printf("Sapera Console CyGrabber Program (C++ version)\n");

	if (!GetOptions(acqServerName, &acqDeviceNumber, configFilename))
	{
		printf("\nPress any key to terminate\n");
		CorGetch();
		return;
	}

	SapLocation loc(acqServerName, acqDeviceNumber);

	if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq) > 0)
	{
		Acq = new SapAcquisition(loc, configFilename);
		Buffers = new SapBuffer(1, Acq);
		View = new SapView(Buffers, SapHwndAutomatic);
		Xfer = new SapAcqToBuf(Acq, Buffers, XferCallback, View);


		// Create acquisition object
		if (Acq && !*Acq && !Acq->Create())
			goto FreeHandles;
	}

	// Create buffer object
	if (Buffers && !*Buffers && !Buffers->Create())
		goto FreeHandles;

	// Create transfer object
	if (Xfer && !*Xfer && !Xfer->Create())
		goto FreeHandles;

	nReady = 1;
	// Start continous grab
	Xfer->Grab();

	printf("Press any key to stop grab\n");
	//CorGetch();
	while (!ControlF)
	{
		Sleep(1000);
	}

	Xfer->Freeze();
	if (!Xfer->Wait(5000))
		printf("Grab could not stop properly.\n");


FreeHandles:

	// Destroy transfer object
	if (Xfer && *Xfer && !Xfer->Destroy()) return;

	// Destroy buffer object
	if (Buffers && *Buffers && !Buffers->Destroy()) return;

	// Destroy acquisition object
	if (Acq && *Acq && !Acq->Destroy()) return;

	// Delete all objects
	if (View)     delete View;
	if (Xfer)     delete Xfer;
	if (Buffers)  delete Buffers;
	if (Acq)      delete Acq;
	cout << "View & Xfer & Buffers were delete!" << endl;
	cout << "Camera  connect close!" << endl;
	cout << "--------------------------------------------------------------------" << endl;
	write_log(pFile, "%s \n", "Camera  connect close!");
	write_log(pFile, "%s \n", "--------------------------------------------------------------------!");
	return;
}

//����ԭͼ
void saveOriginalBmpImage()
{
	strOrigBmpName	= OriginalFolderPath + "\\Org.bmp";
	cout << strOrigBmpName.c_str() << endl;
}

void saveOriginalJpgImage()
{
	strOrigJpgName = OriginalFolderPath + "\\Org.jpg";
	cout << strOrigJpgName.c_str() << endl;
}

int iiii = 0;
static void XferCallback(SapXferCallbackInfo *pInfo)
{
	//printf("into xfercallback \n");

	SapView *pView = (SapView *)pInfo->GetContext();

	if (Buffers > 0 && Xfer > 0)
	{

		//double start, end, cost;
		//saveOriginalBmpImage();
		//start = clock();		
		//Buffers->Save(strOrigBmpName.c_str(), fileBmpFormat);
		//end = clock();		cost = end - start;
		//cout << "save bmp time: =" << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << endl;

		/*********�������ȡͼ��ȡ����**********/
		PUINT8 pData;
		Buffers->GetAddress((void**)&pData);
		Mat img = Mat::zeros(Buffers->GetHeight(), Buffers->GetWidth(), CV_8UC1);
		memcpy(img.data, pData, Buffers->GetHeight() * Buffers->GetWidth() * sizeof(char));
		if (img.empty())
		{
			write_log(pFile, "%s \n", "�����ͼΪ�ջ����쳣!");
			imwrite("D:\\error.bmp", img);		
		}

		ImgMat.push(img);
		bCamera = true;

		/**********************����ʹ�ô���******************************/
		//Mat image;
		//string strimg;
		//strimg = "D:\\20191220\\111\\" + to_string(iiii) + ".bmp";
		//image = imread(strimg);
		//cout << "strimg = " << strimg << endl;
		//iiii++;
		//Mat imgGray;
		////�ҶȻ�
		//cvtColor(image, imgGray, CV_BGR2GRAY);
		//ImgMat.push(imgGray);
		//bCamera = true;
		//cout << "bCamera = " << bCamera << endl;
		/**********************************************************************/

		//char* pImageData = (char*)malloc(width*height * sizeof(char));
		//memcpy(pImageData, pData, height *width * sizeof(char));
		//memcpy(img1.data, pImageData, height *width * sizeof(char));
		//namedWindow("img", 0); imshow("img", img); waitKey(1);
		//imwrite("D:\\���\\4444.bmp", img);
		/**************************/
		printf("Save ImgMat sucess \n");
	}
	//printf("leave xfercallback \n");
}


static BOOL GetOptions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
	return GetOptionsFromQuestions(acqServerName, pAcqDeviceIndex, configFileName);
}


/*********************************************************************/

//�����ļ����������ļ���
void getFiles(const std::string & path, std::vector<std::string> & files, std::vector<std::string> & filesId, string format);

map<string, string> mDefine_json;  //ȱ�ݶ�����Ϣ
map<string, string> mScreening_json;  //ȱ��ɸѡ��Ϣ

map<string, string> m_json; //�м����
vector<map<string, string>> mToolPara_json; //���湤�߲���
vector<JsonValue> m_strJson; //��������Ĳ�������

map<string, string> mLanguage_json;	 //�������Խ�������

void printDefine(JsonValue v, map<string, string> &mm_json)
{
	JsonValue::Members mem = v.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		if (v[*iter].type() == Json::objectValue)
		{
			printDefine(v[*iter], mm_json);
		}
		else if (v[*iter].type() == Json::arrayValue)
		{
			auto cnt = v[*iter].size();
			for (auto i = 0; i < cnt; i++)
			{
				printDefine(v[*iter][i], mm_json);
			}
		}
		else if (v[*iter].type() == Json::stringValue)
		{
			//cout << v[*iter].asString() << endl;
		}
		mm_json.insert(pair<string, string>(*iter, v[*iter].asString()));
	}
	return;
}

void printDefine1(JsonValue v)
{
	JsonValue::Members mem = v.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		if (v[*iter].type() == Json::objectValue)
		{
			printDefine1(v[*iter]);
		}
		else if (v[*iter].type() == Json::arrayValue)
		{
			auto cnt = v[*iter].size();
			for (auto i = 0; i < cnt; i++)
			{
				printDefine1(v[*iter][i]);
			}
		}
		else if (v[*iter].type() == Json::stringValue)
		{
			m_json.insert(pair<string, string>(*iter, v[*iter].asString()));
		}		
	}
	return;
}

void printJson(JsonValue v)
{
	JsonValue::Members mem = v.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		if (v[*iter].type() == Json::objectValue)
		{
			printJson(v[*iter]);
		}
		else if (v[*iter].type() == Json::arrayValue)
		{
			auto cnt = v[*iter].size();
			for (auto i = 0; i < cnt; i++)
			{
				//cout << "array :" << v[*iter][i];
				m_strJson.push_back(v[*iter][i]);
				printJson(v[*iter][i]);
			}
		}
	}
	return;
}

void CreateJson( )
{
	Json::Value root;
	root["id"] = 1244235;
	root["describe"] = "how SB you are";
	Json::StyledWriter sw;
	std::string res_str = sw.write(root);
	std::cout << res_str << std::endl;
}

//д�ļ���־
int write_log(FILE* pFile, const char *format, ...)
{
	va_list arg;
	int done;
	va_start(arg, format);
	SYSTEMTIME st = { 0 };  	
	GetLocalTime(&st);  
	//��ȡ��ǰʱ�� �ɾ�ȷ��ms	
	fprintf(pFile, "[%04d-%02d-%02d %02d:%02d:%02d:%02d]   ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	done = vfprintf(pFile, format, arg);
	va_end(arg);
	fflush(pFile);
	return done;
}

void CurrentTime()
{
	SYSTEMTIME sys = { 0 };
	GetLocalTime(&sys);
	//��ȡ��ǰʱ�� �ɾ�ȷ��ms	

	printf("��ǰϵͳʱ��Ϊ��  %4d/%02d/%02d %02d:%02d:%02d.%03d \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	return;
}
//�ж��ļ����Ǵ���
bool file_exists(string path)
{
	return std::experimental::filesystem::exists(path);
}

Server::Server() :m_nServerSocket(-1)
{
}

Server::~Server()
{
	
}
/*************************************************************************
������: Start
��  ��: �������г���
�ļ���: Server.cpp
��  ��: ��
����ֵ: void:��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
void Server::Start()
{	
	//��־�ļ�
	string CameraType;
	string CcfFilePath;
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);

	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�

	char strLogName[MAX_PATH];
	sprintf(strLogName, "%s\\%04d-%02d-%02d.log", szFilePath, tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday);

	pFile = fopen(strLogName, "a");
	if (!CreateSocket())   //socket�����ļ�
	{
		system("pause");
		return;
	}
	AcceptSocket();
}

/*************************************************************************
������:CreateSocket			
��  ��: ����socket
�ļ���: Server.cpp
��  ��: ��
����ֵ: void:��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
bool Server::CreateSocket()
{	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		cout << "Socket error" << endl;
	m_nServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//�趨TCPЭ��ӿ�;ʧ�ܷ���INVALID_SOCKET
	if (m_nServerSocket == INVALID_SOCKET)
	{	
		cout << "Start Server Failed" << endl;
		write_log(pFile, "%s \n", "Start Server Failed");
		return false;
	}
	
	cout << "Start Server Succeed" << endl;	
	write_log(pFile, "%s \n", "Start Server Succeed");	
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(MYPROT); //1024���ϵĶ˿ں�  
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	int retVal  = ::bind(m_nServerSocket, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {   //�󶨷�������ַ�Ͷ˿ں�;�ɹ�������0������ΪSOCKET_ERROR
		printf("Failed bind:%d\n", WSAGetLastError());
		return false;
	}

	cout << "Bind Succeed !" << endl;
			
	if (listen(m_nServerSocket, 10) == SOCKET_ERROR)
	{
	
		printf("Listen failed:%d", WSAGetLastError());
		write_log(pFile, "%s \n", "Listen failed");
		return false;
	}

	cout << "Listen Succeed" << endl;
	write_log(pFile, "%s \n", "Listen Succeed");
	return true;
}

int ServerThread(SOCKET *ClientSocket) 
{
	if (!m_server.m_CDatabase.ConnectDatabase())  //���ݿ�����
	{
		write_log(pFile, "%s \n", "Database connection fail !");
	}
	
	cout << "Database connection success !" << endl;
	write_log(pFile, "%s \n", "Database connection success !");

	m_server.initInfo();
	m_server.CreateFolder();
	if (m_server.m_CDatabase.m_StationNoInfo.SysOnLine == 0)//0Ϊ����
		m_server.ConnectCamera();

	m_server.RecvAndSendSocket(*ClientSocket);//���ܿͻ��˵���Ϣ		
	closesocket(*ClientSocket);
	free(ClientSocket);
	m_server.m_CDatabase.CloseDatabase();
	return 0;
}

/*************************************************************************
������:.AcceptSocket			
��  ��: �����������ӵ����󣬲����ؾ��
�ļ���: Server.cpp
��  ��: ��
����ֵ: SOCKET:���
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
SOCKET Server::AcceptSocket()
{	
	//����SocketΪ������ģʽ
	int iMode = 1;
	int retVal;//����Socket�����ķ���ֵ
	retVal = ioctlsocket(m_nServerSocket, FIONBIO, (u_long FAR*) &iMode);
	if (retVal == SOCKET_ERROR)
	{
		printf("����SocketΪ������ģʽʧ��!\n");
		WSACleanup();
		return -1;
	}

	while (1)
	{		
		//ѭ�����տͻ����������󲢴��������߳�
		SOCKET *ClientSocket = new SOCKET;
		ClientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		sockaddr_in nClientSocket;
		//���տͻ�����������
		int SockAddrlen = sizeof(sockaddr);		
		*ClientSocket = ::accept(m_nServerSocket, (SOCKADDR*)&nClientSocket, &SockAddrlen);
		if (INVALID_SOCKET == *ClientSocket)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(50);
				continue;
			}
			else
			{
				printf("accept failed!\n");
				closesocket(m_nServerSocket);
				WSACleanup();
				return -1;
			}
		}
		ControlF = false;
		CurrentTime();
		cout << "һ���ͻ��������ӵ���������socket�ǣ�" << *ClientSocket << endl;
		thread ServerThd(ServerThread, ClientSocket);//CreateThread(NULL, 0, &ServerThread, ClientSocket, 0, NULL);
		ServerThd.detach();
	}
	fclose(pFile);
	if (m_nServerSocket != -1)
		closesocket(m_nServerSocket);	//�ر�socket����
	m_nServerSocket = -1;
	WSACleanup();	//��ֹws2_32.lib��ʹ��
	return 0;
}

/*************************************************************************
������:RecvAndSendSocket		
��  ��: ���ܺͷ������ݸ��ͷ���
�ļ���: Server.cpp
��  ��: recvsocket���ͻ��˾��
����ֵ: �ɹ���ʧ��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
void Server::RecvAndSendSocket(SOCKET recvsocket)
{
	while (!ImgMat.empty()) ImgMat.pop();
	while (!RecvImg.empty()) RecvImg.pop();
	while (!RecvID.empty()) RecvID.pop();
	if (!Socketdata.empty()) Socketdata.clear();
	while (true)
	{
		  //1Ϊ������ӳɹ�			0Ϊ����ģʽ									1Ϊ����ģʽ											0Ϊ�㷨�رգ�1Ϊ�㷨��
		if( ((nReady == 1) && (0 == m_CDatabase.m_StationNoInfo.SysOnLine))||(1 == m_CDatabase.m_StationNoInfo.SysOnLine))
		{			
			char mess[100] = "#ReadyOK@"; // �ظ�ֵ������������ɹ�		
			int isend;
		
			isend = send(recvsocket, mess, (int)strlen(mess), 0);
			if (SOCKET_ERROR == isend)
			{
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
				{
					Sleep(5);
					continue;
				}
				else
				{
					write_log(pFile, "%s \n", " send Camera connect info failed");
					cout << "send Camera connect info failed" << endl;
					return;
				}
			}
			if (isend == 0)
			{
				write_log(pFile, "%s \n", "Sending message failed, disconnect");
				cout << "Sending message failed, disconnect" << endl;
				return;
			}
			else
			{
				if (nReady == 1)
				{
					write_log(pFile, "Send Data %s \n", "#ReadyOK@");
					write_log(pFile, "%s \n", "Camera connect sucess");
					cout << "Camera connect sucess" << endl;
					break;
				}
				else
				{
					write_log(pFile, "%s \n", "Socket connect sucess");
					cout << "Socket connect sucess" << endl;
					break;
				}
			}	
		}
		Sleep(100);
	}
	while (!RecvData.empty()) RecvData.pop();
	while (!ReRecvData.empty()) ReRecvData.pop();
	
	bool ControlCame = true;		
	while (true)
	{
		// �ӿͻ��˽�������
		char buff[256];	
		try {
			memset(buff, 0, sizeof(buff));
			std::lock_guard<std::mutex> my_lock_guard(my_mutex_1);
			int nRecv = recv(recvsocket, buff, 256, 0);//�ӿͻ��˽�����Ϣ  #���͹�λ��#������#����#��ƷID@
			if (SOCKET_ERROR == nRecv)
			{
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
				{
					Sleep(1);
					continue;
				}
				else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
				{
					write_log(pFile, "%s \n", "File accept Failed, disconnect");
					cout << "File accept Failed, disconnect" << endl;
					ControlF = true;
					return;
				}
			}
			if (nRecv == 0)
			{
				write_log(pFile, "%s \n", "File accept Failed, disconnect");
				cout << "File accept Failed, disconnect" << endl;
				ControlF = true;
				return;
			}
		}
		catch (const std::exception& e) { // caught by reference to base
			std::cout << " socket error :"	<< e.what() << "'\n";
		}



		write_log(pFile, " Recv data : %s \n", buff);
		cout << "Recv data:" << buff << endl;
		string strImgName = OriginalFolderPath + "\\Org.bmp";
		if (file_exists(strImgName))
		{
			remove(strImgName.c_str());
		}
		string strbuf = buff;
		if (strbuf.compare("#Close@") == 0)//�ػ�
		{
			char cmd[20] = "shutdown -s -t";
			system(cmd);
			break;
		}
		else if (strbuf.compare("#Reset@") == 0)//�ػ�����
		{
			char cmd1[20] = "shutdown -r";
			system(cmd1);
			break;
		}
		RecvData.push(strbuf);
		ReRecvData.push(strbuf);
		cout << "socket RecvData size0  = " << RecvData.size() <<  endl;
		if (ControlCame)
		{
			thread findImg(FindCameraImg, recvsocket); //���������Լ������ͼ
			findImg.detach();
			ControlCame = false;
		}
	}
}

void FindCameraImg(SOCKET recvsocket)
{
	bool Control = true;			//�㷨��������
	
	const char *t = "#@";
	char data[256];
	string OrgPathName;
	string CompPathName;
	string strProductID;

	bool ControlDefect = true;
	
	while (!RecvData.empty())
	{		
		cout << "socket RecvData size1 = " << RecvData.size() << endl;
		if (!Socketdata.empty())
		{
			Socketdata.clear();
		}
		
		string strInfo = RecvData.front();
		strcpy(data, strInfo.c_str());
		//#���͹�λ��#������#����#��ƷID#���Է�ʽ@      ���յ�����Ϣ��ʽ
		char *p = strtok(data, t);
		while (p)
		{
			string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
			Socketdata.push_back(s);
			p = strtok(NULL, t);
		}

		//������������0
		//����ͼƬ��outimg
		//����ִ�У�retest
		//��һ�ţ�last
		//��һ�ţ�next
		//ѭ��ģʽ��loop
		//���Ҳ�ƷIDͼ��ʵ��ID
		string str = Socketdata[4]; //���Է�ʽ         #���͹�λ��#������#����#��ƷID#���Է�ʽ@
		strProductID = Socketdata[3]; //��ƷID         #���͹�λ��#������#����#��ƷID#���Է�ʽ@
		int flag = 1;
		string new_BmpName;

		if (m_server.m_CDatabase.m_StationNoInfo.SysOnLine == 1) //ģʽ  ������ߣ�1-���ߣ�0-���ߣ�
		{
			if (("OutImg" == str))  //����ģʽ�µ�ͼƬ���� ��һλ���ݻظ�1
			{
				string strQuery = "SELECT DownFilePath FROM table_config  WHERE NowProName  = '" + Socketdata[1] + "'; ";
				m_server.m_CDatabase.ReadInAndOutImgPath(strQuery.c_str(), flag);   //flag = 1 Ϊ����

				if (!file_exists(m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath))
				{
					string command;
					cout << "strOutputImgPath= : " << m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath << endl;
					size_t pos;
					pos = m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath.find("/");
					while (pos != string::npos)
					{
						m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath.replace(pos, 1, "\\");
						pos = m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath.find("/");
					}
					command = "mkdir " + m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath;
				    cout << "command " << command << endl;
					system(command.c_str());

				}
				int pos = 0;
				int leng = 0;
				string strFile;
				string strNewFile;
				if (Offlinefiles.empty())
					goto FreeHandles;
				
				pos = Offlinefiles[0].find_last_of("/");
				for (int i = 0; i < Offlinefiles.size(); i++)
				{
					leng = Offlinefiles[i].length();
					strFile = Offlinefiles[i].substr(pos + 1, leng - pos);
					strNewFile = m_server.m_CDatabase.m_StationNoInfo.strOutputImgPath + "/" + strFile;
					bool flagfile = CopyFile(Offlinefiles[i].c_str(), strNewFile.c_str(), false);
					cout << "outImg flagfile :" << flagfile << endl;
					cout << "Oldfilepath :" << Offlinefiles[i].c_str() << endl;
					cout << "Newfilepath :" << strNewFile.c_str() << endl;
					Sleep(300);
					if (ControlF)
					{
						break;
					}
				}
				FreeHandles:
				int isend = SendDetect(recvsocket, OfflinefilesId, flag);
				if (isend == 0 || isend == SOCKET_ERROR)
				{
					write_log(pFile, "%s \n", "SendDetect message failed");
					cout << "SendDetect message failed" << endl;
					break;
				}
				RecvData.pop();
				ReRecvData.pop();
			}
			else if ("InImg" == str) //ͼ����    ÿ���յ���Ϣ���¶�ȡ���ݿ�,��ȡ·��,��ȡ��ͼƬ  ��һλ���ݻظ�1
			{
				flag = 0;
				currentImg = 0;
				string strQuery = "SELECT InImage   FROM table_config  WHERE NowProName  = '" + Socketdata[1] + "'; ";
				m_server.m_CDatabase.ReadInAndOutImgPath(strQuery.c_str(), flag);
				if (!OfflinefilesId.empty())
					OfflinefilesId.clear();
				if (file_exists(m_server.m_CDatabase.m_StationNoInfo.strInputImgPath))
				{
					getFiles(m_server.m_CDatabase.m_StationNoInfo.strInputImgPath, Offlinefiles, OfflinefilesId, ".bmp");
				}
				else
				{
					write_log(pFile, "%s \n", "The path to import the image does not exist");
					cout << "The path to import the image does not exist" << endl;
				}
				flag = 1;
				int isend = SendDetect(recvsocket, OfflinefilesId, flag);
				if (isend == 0 || isend == SOCKET_ERROR)
				{
					write_log(pFile, "%s \n", "SendDetect message failed, disconnect");
					cout << "SendDetect message failed, disconnect" << endl;
					break;
				}
				write_log(pFile, " file name =    %s \n", to_string(Offlinefiles.size()).c_str());
				for (int i = 0; i < Offlinefiles.size(); i++)
				{
					write_log(pFile, " file name =    %s \n", Offlinefiles[i].c_str());
				}

				RecvData.pop();
				ReRecvData.pop();
			}
			else if ("Retest" == str) //����ִ��   ����ͼ��ʱ��  ִ���㷨�����ظ���Ϣ, �ļ��е�ʱ��,ִ�е�һ��ͼƬ,ִ���㷨,�ظ���Ϣ   ��һλ���ݻظ�1
			{
				new_BmpName = Offlinefiles[currentImg];
				cout << "new_BmpName  Retest == str" << new_BmpName << "::::" << currentImg << endl;
				RecvImg.push(new_BmpName);
				RecvID.push(OfflinefilesId[currentImg]);
				RecvData.pop();
			}
			else if ("Last" == str) //��һ��  ��ǰͼƬ����һ��ͼ,ִ���㷨   ��һλ���ݻظ�1
			{
				currentImg--;
				if (0 >= currentImg)
				{
					currentImg = 0;
				}
				new_BmpName = Offlinefiles[currentImg];
				cout << "new_BmpName  Last == str" << new_BmpName << "::::" << currentImg << endl;
				RecvImg.push(new_BmpName);
				RecvID.push(OfflinefilesId[currentImg]);
				RecvData.pop();

			}
			else if ("Next" == str) //��һ�� ��ǰͼƬ����һ��ͼ,ִ���㷨  ��һλ���ݻظ�1
			{
				currentImg++;
				if (Offlinefiles.size() <= currentImg)
				{
					currentImg = Offlinefiles.size() - 1;
				}
				new_BmpName = Offlinefiles[currentImg];
				RecvImg.push(new_BmpName);
				RecvID.push(OfflinefilesId[currentImg]);
				RecvData.pop();
			}
			else if ("Loop" == str) //ѭ��ģʽ  ���һλ���ݻظ�1,ûִ����ظ�0
			{
				currentImg = 0;
				for (int i = 0; i < Offlinefiles.size(); i++)
				{
					new_BmpName = Offlinefiles[currentImg];
					RecvImg.push(new_BmpName);
					RecvID.push(OfflinefilesId[currentImg]);
					currentImg++;
					if (i < Offlinefiles.size() - 1)
					{
						ReRecvData.push(strInfo);
					}
				}
				if (Offlinefiles.size() == 0)
				{
					flag = 1;
					int isend = SendDetect(recvsocket, OfflinefilesId, flag);
					if (isend == 0 || isend == SOCKET_ERROR)
					{
						write_log(pFile, "%s \n", "SendDetect message failed, disconnect");
						cout << "SendDetect message failed, disconnect" << endl;
					}
				}
				RecvData.pop();
			}
			else   //��ѯ��ƷIDͼ   ��һλ���ݻظ�1
			{
				for (int i = 0; i < Offlinefiles.size(); i++)
				{
					string file = Offlinefiles[i];
					int pos1 = file.find_last_of("/");
					int pos2 = file.find_last_of("_");
					string strid = file.substr(pos1, pos2);
					if (str == strid)
					{
						flag = 1;
						break;
					}
					else
						flag = 0;
				}
				int isend = SendDetect(recvsocket, Socketdata, flag);
				if (isend == 0 || isend == SOCKET_ERROR)
				{
					write_log(pFile, "%s \n", "SendDetect message failed, disconnect");
					cout << "SendDetect message failed, disconnect" << endl;
					break;
				}
			}
		}
		else if (("0" == str) && (m_server.m_CDatabase.m_StationNoInfo.SysOnLine != 1)) // ������������0  �����������£�ģʽ  ������ߣ�1-���ߣ�0-���ߣ�  
		{
			write_log(pFile, "%s %s\n", "The current mode is 0 and SysOnLine is ", to_string(m_server.m_CDatabase.m_StationNoInfo.SysOnLine).c_str());
			//OrgPathName = OriginalFolderPath + "\\Org.bmp";
			//while (true)
			//{
			//	if (file_exists(OrgPathName))
			//	{
			//		write_log(pFile, " Find if the OrgPathName file exists:: : %s \n", OrgPathName.c_str());
			//		cout << " Find if the OrgPathName file exists:::" << OrgPathName << endl;
			//		break;
			//	}
			//	Sleep(1000);
			//}

			while (!bCamera)
			{
				Sleep(500);
			}
			bCamera = false;
			RecvID.push(strProductID);

			cout << "RecvImg and RecvID push :" << strProductID << " RecvData is pop :" << RecvData.front() << endl;
			RecvData.pop();
		}
		else
		{
			cout << "���ܵ������ݸ�ʽ���ԣ�" << endl;
			write_log(pFile, "%s %s\n", "���ܵ������ݸ�ʽ���ԣ�: ", strInfo.c_str());
			return;
		}
		
		//�������ִ���	
		if (ControlDefect == true) 
		{
			thread mydataTest(DefectDetect, recvsocket); //�����㷨
			mydataTest.detach();
			ControlDefect = false;
		}
		int ii = 0;
		while (RecvData.empty())
		{
			if (ControlF)
			{
				cout << "End of FindCameraImg" << endl;
				return;
			}

			ii++;
			Sleep(500);
			if ((ii % 6) == 0)
			{
				cout << "socket RecvData is empty, �ȴ���λ���·����� ... " << endl;
			}

			
		}
		Socketdata.clear();
	}	
	return;
}

int SendDetect(SOCKET recvsocket, vector<string> &RecvInfo, int iflag)
{
	string strResult = "#";
	if (RecvInfo.empty())
	{
		strResult += "0";   // ��Id
	}
	else
	{
		strResult += RecvInfo[0];   // ��Id
	}
	
	strResult += "#0#";			//û��ȱ��	
	strResult += "0#";			//���̿ռ��������ֵ��ʱ���ϱ�0,��������ԭ�����ϵ�ͼ,���Բ��ô��̼��						
	strResult += to_string(iflag);		
	strResult += "@";			//��ѯID�ɹ��ظ�1,ʧ��0
	char mess[100];				// "#��ƷID#���#���̽��@";   OK--0;  NG---ʵ������
	strcpy(mess, strResult.c_str());

	int isend;
	while (true)
	{
		std::lock_guard<std::mutex> my_lock_guard(my_mutex_1);
		isend = send(recvsocket, mess, (int)strlen(mess), 0);
		if (SOCKET_ERROR == isend)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(5);
				continue;
			}
			else
			{
				write_log(pFile, "%s \n", " SendDetect message failed");
				cout << "SendDetect message failed" << endl;
				return isend;
			}
		}
		if (isend == 0)
		{
			write_log(pFile, "%s \n", "Sending message failed, disconnect");
			cout << "Sending message failed, disconnect" << endl;
		}
		else
		{
			break;
		}
	}
	cout << "SendDetect data :" << mess << endl;
	write_log(pFile, "SendDetect data : %s \n", mess);
	return isend;
}
/*************************************************************************
������:DefectDetect			
��  ��: ȱ�ݼ��
�ļ���: Server.cpp
��  ��: strDefectDectResult��
����ֵ: ��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
void DefectDetect(SOCKET recvsocket)
{	
	
	Mat src;
	string Pid;
	map<int, vector<string>> DefectInfoData;  //��ȡ�㷨ȱ����Ϣ��map
	while (true)
	{
		write_log(pFile, "RecvID size =   %s \n",to_string( RecvID.size()).c_str());		
		if (!RecvID.empty())
		{		
			string strResult = "#";

			if (m_server.m_CDatabase.m_StationNoInfo.SysOnLine == 1) //�x�Q
			{
				write_log(pFile, "RecvImg size =   %s \n", to_string(RecvImg.size()).c_str());
				string strImg;
				strImg = RecvImg.front();
				RecvImg.pop();

				cout << "�㷨����ͼƬ����=" << strImg << endl;
				write_log(pFile, "%s  %s \n", "�㷨����ͼƬ����=", strImg.c_str());
				src = imread(strImg);
				if (src.empty())
				{
					write_log(pFile, "%s  %s \n", "���뵽�㷨��ͼ��Ϊ��,����ԭͼ�Ƿ���ڻ��߲ɼ�������");
					return;
				}
				grayScale(src, src);
			}
			else
			{
				write_log(pFile, "ImgMat size =   %s \n", to_string(ImgMat.size()).c_str());
				src = ImgMat.front();
				ImgMat.pop();		
				if (src.empty())
				{
					write_log(pFile, "%s  %s \n", "���뵽�㷨��ͼ��Ϊ��,����ԭͼ�Ƿ���ڻ��߲ɼ�������");
					return;
				}
			}			
					
			strResult += RecvID.front(); //��ƷID
			Pid = RecvID.front();
			RecvID.pop();
			string new_BmpName;
			string new_JpgName;
			int result = 0;
			int AlgoriFlag = 0;
			Mat img;
			int iSaveBmpStatus = 0;
			CurrentTime();
			if (0 == m_server.m_CDatabase.m_StationNoInfo.AlgorithmOnOrOff) //0Ϊ�㷨�ر�  1Ϊ�㷨��
			{
				img = src.clone();
			}
			if ((1 == nResultMode) && (1 == m_server.m_CDatabase.m_StationNoInfo.AlgorithmOnOrOff))  //1-�������̣�2-ǿ��OK��3-ǿ��NG    0Ϊ�㷨�ر�  1Ϊ�㷨��
			{
				try
				{
					if (!DefectInfoData.empty())
					{
						DefectInfoData.clear();
					}
					int i = 2; //������vector���������
					if (1 == Algorithm_Type())    //AA������������1--�¹���2--�ƹ�
					{						
						InitAlgrithmPara(i );
						AlgoriFlag = AlgorithmInterface(src, ProcedureFolderPath, DefectInfoData, Pid,
							m_server.m_Algorithm_Para.aa_reg_min,
							m_server.m_Algorithm_Para.aa_reg_max,
							m_server.m_Algorithm_Para.grey_dif_pos,
							m_server.m_Algorithm_Para.grey_dif_neg,
							m_server.m_Algorithm_Para.excep_dilate_width,
							m_server.m_Algorithm_Para.excep_dilate_height,
							m_server.m_Algorithm_Para.loop_stride_to_decide,
							m_server.m_Algorithm_Para.defect_area,
							m_server.m_Algorithm_Para.defect_num,
							m_server.m_Algorithm_Para.defect_accep_min_area ,img);

						cout << "m_server.m_Algorithm_Para.aa_reg_min = " << m_server.m_Algorithm_Para.aa_reg_min << endl;
						cout << "m_server.m_Algorithm_Para.aa_reg_max = " << m_server.m_Algorithm_Para.aa_reg_max << endl;
						cout << "m_server.m_Algorithm_Para.grey_dif_pos = " << m_server.m_Algorithm_Para.grey_dif_pos << endl;
						cout << "m_server.m_Algorithm_Para.grey_dif_neg = " << m_server.m_Algorithm_Para.grey_dif_neg << endl;
						cout << "m_server.m_Algorithm_Para.excep_dilate_width = " << m_server.m_Algorithm_Para.excep_dilate_width << endl;
						cout << "m_server.m_Algorithm_Para.excep_dilate_height = " << m_server.m_Algorithm_Para.excep_dilate_height << endl;
						cout << "m_server.m_Algorithm_Para.loop_stride_to_decide = " << m_server.m_Algorithm_Para.loop_stride_to_decide << endl;
						cout << "m_server.m_Algorithm_Para.defect_area = " << m_server.m_Algorithm_Para.defect_area << endl;
						cout << "m_server.m_Algorithm_Para.defect_num = " << m_server.m_Algorithm_Para.defect_num << endl;
						cout << "m_server.m_Algorithm_Para.defect_accep_min_area = " << m_server.m_Algorithm_Para.defect_accep_min_area << endl;
						
					}
					else if(2 == Algorithm_Type())
					{
						InitAlgrithmPara_hjh(i);
						AlgoriFlag = AlgorithmInterface_hjh(src, ProcedureFolderPath, DefectInfoData, Pid,
							m_server.m_Algorithm_Para_hjh.new_model,
							m_server.m_Algorithm_Para_hjh.aa_region_positive,
							m_server.m_Algorithm_Para_hjh.aa_region_negative,
							m_server.m_Algorithm_Para_hjh.grey_difference_positive,
							m_server.m_Algorithm_Para_hjh.grey_difference_negative,
							m_server.m_Algorithm_Para_hjh.max_area,
							m_server.m_Algorithm_Para_hjh.min_area,
							m_server.m_Algorithm_Para_hjh.num_for_stop_inspection,						
							img);

					    cout << "m_server.m_Algorithm_Para_hjh.new_model_hjh = " << m_server.m_Algorithm_Para_hjh.new_model << endl;
					    cout << "m_server.m_Algorithm_Para_hjh.aa_region_positive_hjh = " << m_server.m_Algorithm_Para_hjh.aa_region_positive << endl;
					    cout << "m_server.m_Algorithm_Para_hjh.aa_region_negative_hjh = " << m_server.m_Algorithm_Para_hjh.aa_region_negative << endl;
					    cout << "m_server.m_Algorithm_Para_hjh.grey_difference_positive_hjh = " << m_server.m_Algorithm_Para_hjh.grey_difference_positive << endl;
					    cout << "mm_server.m_Algorithm_Para_hjh.grey_difference_negative_hjh = " << m_server.m_Algorithm_Para_hjh.grey_difference_negative << endl;
					    cout << "m_server.m_Algorithm_Para_hjh.max_area_hjh = " << m_server.m_Algorithm_Para_hjh.max_area << endl;
					    cout << "m_server.m_Algorithm_Para_hjh.min_area_hjh = " << m_server.m_Algorithm_Para_hjh.min_area << endl;
					    cout << "m_server.m_Algorithm_Para_hjh.num_for_stop_inspection_hjh = " << m_server.m_Algorithm_Para_hjh.num_for_stop_inspection<< endl;
					}
					else if (3 == Algorithm_Type())
					{
						InitAlgrithmPara_ITO(i);
						AlgoriFlag = AlgorithmInterface_ITO(src, ProcedureFolderPath, DefectInfoData,  
							m_server.m_Algorithm_Para_ITO.detector_length ,
							m_server.m_Algorithm_Para_ITO.detector_thresh, 						
							m_server.m_Algorithm_Para_ITO.detector_top,
							m_server.m_Algorithm_Para_ITO.detector_left,
							m_server.m_Algorithm_Para_ITO.detector_right,
							m_server.m_Algorithm_Para_ITO.detector_bottom,
							m_server.m_Algorithm_Para_ITO.detector_limit,
							m_server.m_Algorithm_Para_ITO.detector_iter,
							m_server.m_CDatabase.m_StationNoInfo.DefectNum,
							img);

						cout << "m_server.m_Algorithm_Para_ITO.detector_length = " << m_server.m_Algorithm_Para_ITO.detector_length << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_thresh = " << m_server.m_Algorithm_Para_ITO.detector_thresh << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_top = " << m_server.m_Algorithm_Para_ITO.detector_top << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_left = " << m_server.m_Algorithm_Para_ITO.detector_left << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_right = " << m_server.m_Algorithm_Para_ITO.detector_right << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_bottom = " << m_server.m_Algorithm_Para_ITO.detector_bottom << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_limit = " << m_server.m_Algorithm_Para_ITO.detector_limit << endl;
						cout << "m_server.m_Algorithm_Para_ITO.detector_iter = " << m_server.m_Algorithm_Para_ITO.detector_iter << endl;
					}
					else
					{
						cout << "�������ô�����˶�" << endl;
						write_log(pFile, "%s  \n", "�������ô�����˶�");
					}
				}
				catch (const cv::Exception& err)
				{
					AlgoriFlag = -1;
					imwrite("�㷨ʧ��.bmp", src);
					cout << "�㷨ʧ��:" << err.what() << endl;
				}
				CurrentTime();
				result = m_server.ResultProcess(DefectInfoData, AlgoriFlag, Pid); //����NG������
				write_log(pFile, "%s \n", "������ɸѡ����");

				if (0 != result) {
					iSaveBmpStatus = 1;
					new_BmpName = OriginalNGFolderPath + "\\" + Pid + "_NG.bmp";
					new_JpgName = CompressNGFolderPath + "\\" + Pid + "_NG.jpg"; //"_NG.jpg";
					strResult += "#" + to_string(result) + "#"; //��ȱ��	
					cout << "ȱ�ݼ����ΪNG" << endl;	
				}
				else if ((0 == result) && (0 == AlgoriFlag))
				{		
					iSaveBmpStatus = 2;
					new_BmpName = OriginalOKFolderPath + "\\" + Pid + "_OK.bmp";
					new_JpgName = CompressOKFolderPath + "\\" + Pid + "_OK.jpg"; //"_OK.jpg";					
					strResult += "#0#"; //û��ȱ��
					cout << "ȱ�ݼ����ΪOK" << endl;
				}
				else {
					iSaveBmpStatus = 1;
					new_BmpName = OriginalNGFolderPath + "\\" + Pid + "_NG.bmp";
					new_JpgName = CompressNGFolderPath + "\\" + Pid + "_NG.jpg"; //"_NG.jpg";
					strResult += "#1#"; //��ȱ��	
					cout << "ȱ�ݼ����ΪNG" << endl;
				}
			}
		
			
			if ((1 == nResultMode)&&(0 == m_server.m_CDatabase.m_StationNoInfo.AlgorithmOnOrOff)) //0Ϊ�㷨�ر�  1Ϊ�㷨��
			{
				iSaveBmpStatus = 3;
				new_BmpName = OriginalFolderPath + "\\" + Pid + ".bmp"; //�ر��㷨ֻΪ��ͼ
				new_JpgName = OriginalFolderPath + "\\" + Pid + ".jpg"; //".jpg";
				strResult += "#0#"; //û��ȱ�� 
			}
			else if (2 == nResultMode)  //ǿ��OK
			{
				iSaveBmpStatus = 2;
				new_BmpName = OriginalOKFolderPath + "\\" + Pid + "_OK.bmp";
				new_JpgName = CompressOKFolderPath + "\\" + Pid + "_OK.jpg"; //"_OK.jpg";
				strResult += "#0#"; //û��ȱ��   ����NG����OK���ظ�0
			}
			else if (3 == nResultMode)  //ǿ��NG
			{
				iSaveBmpStatus = 1;
				new_BmpName = OriginalNGFolderPath + "\\" + Pid + "_NG.bmp";
				new_JpgName = CompressNGFolderPath + "\\" + Pid + "_NG.jpg"; //"_NG.jpg";
				strResult += "#0#"; //û��ȱ��   ����NG����OK���ظ�0
			}			
		
			if (!img.empty())
			{				
				cout << "bmp_new_name = " << new_BmpName << endl;
				write_log(pFile, "%s %s\n", " bmp_new_name = ", new_BmpName.c_str());
				cout << "SaveCompressedOKImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveCompressedOKImg << "   SaveCompressedNGImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveCompressedNGImg << endl;
				cout << "SaveOKImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveOKImg << "   SaveNGImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveNGImg << endl;

				if (3 == iSaveBmpStatus) // �㷨�ر�ֻΪ��ͼ
				{
				    cout << "��ʼ����bmpԭͼ" << endl;	write_log(pFile, "%s \n", "��ʼ����bmpԭͼ");
					imwrite(new_BmpName, src);
					write_log(pFile, "%s \n", "����bmpԭͼ����");	 cout << "����bmpԭͼ����" << endl;
				}

				if ( (1 == m_server.m_CDatabase.m_StationNoInfo.SaveNGImg) && (0 == m_server.m_CDatabase.m_StationNoInfo.SysOnLine)&& (1 == iSaveBmpStatus))  //��������� 0--����  1--����
				{
					cout << "��ʼ����NG-bmpԭͼ" << endl;	write_log(pFile, "%s \n", "��ʼ����NG-bmpԭͼ");
					imwrite(new_BmpName, src);		
					write_log(pFile, "%s \n", "����NG-bmpԭͼ����");	 cout << "����NG-bmpԭͼ����" << endl;
				}

				if ((1 == m_server.m_CDatabase.m_StationNoInfo.SaveOKImg) && (0 == m_server.m_CDatabase.m_StationNoInfo.SysOnLine) && (2 == iSaveBmpStatus))   //��������� 0--����  1--����
				{
					cout << "��ʼ����OK-bmpԭͼ" << endl;	write_log(pFile, "%s \n", "��ʼ����OK-bmpԭͼ");
					imwrite(new_BmpName, src);
					write_log(pFile, "%s \n", "����OK-bmpԭͼ����");	cout << "����OK-bmpԭͼ����" << endl;
				}
				if (1 == m_server.m_CDatabase.m_StationNoInfo.SamplingCompressedImg) //1 - ���棬0 - ������
				{
					pyrDown(img, img);
				}
				if (1 == m_server.m_CDatabase.m_StationNoInfo.StationDynamic) //��λͼ��Դ��1-��̬��0-��̬
				{
					cout << "��ʼ����jpgѹ��ͼ" << endl;		write_log(pFile, "%s \n", "��ʼ����jpgѹ��ͼ");
					imwrite(new_JpgName, img); //��̬�����һ������
					write_log(pFile, "%s \n", "��������jpgѹ��ͼ");		 cout << "��������jpgѹ��ͼ" << endl;
				}
				else
				{
					if ((1 == m_server.m_CDatabase.m_StationNoInfo.SaveCompressedOKImg) && (2 == iSaveBmpStatus)) // 1-NG; 2- OK
					{
						cout << "��ʼ����OK-jpgѹ��ͼ" << endl;	write_log(pFile, "%s \n", "��ʼ����OK-jpgѹ��ͼ");
						imwrite(new_JpgName, img);
						write_log(pFile, "%s \n", "��������OK-jpgѹ��ͼ");	cout << "��������OK-jpgѹ��ͼ" << endl;
					}
					if ((1 == m_server.m_CDatabase.m_StationNoInfo.SaveCompressedNGImg) && (1 == iSaveBmpStatus)) // 1-NG; 2- OK
					{
						cout << "��ʼ����NG-jpgѹ��ͼ" << endl;	write_log(pFile, "%s \n", "��ʼ����NG-jpgѹ��ͼ");
						imwrite(new_JpgName, img);
						write_log(pFile, "%s \n", "��������NG-jpgѹ��ͼ");	cout << "��������NG-jpgѹ��ͼ" << endl;
					}
				}
	
			}			

			if (m_server.m_CDatabase.m_StationNoInfo.DiskSpaceAlarm > FindFreeSpace())
			{
				strResult += "1#";//���̿ռ�С������ֵ��ʱ���ϱ�1,Ȼ����λ���������
			}
			else
				strResult += "0#";//���̿ռ��������ֵ��ʱ���ϱ�0
								  //�������ݸ��ҿͻ���   //���ݿ����
			if (RecvImg.size() == 0)
				strResult += "1@";	 //  ִ����ظ�1
			else
				strResult += "0@";	 //  ִ����ظ�1

			char mess[100];		  // "#��ƷID#���#���̽��@";   OK--0;  NG---ʵ������
			strcpy(mess, strResult.c_str());
			int isend;
			while (true)
			{
				std::lock_guard<std::mutex> my_lock_guard(my_mutex_1);
				isend = send(recvsocket, mess, (int)strlen(mess), 0);
				if (SOCKET_ERROR == isend)
				{
					int err = WSAGetLastError();
					if (err == WSAEWOULDBLOCK)
					{
						Sleep(1);
						continue;
					}
					else
					{
						write_log(pFile, "%s \n", " SendDetect message failed");
						cout << "SendDetect message failed" << endl;					
					}
				}
				if (isend == 0 )
				{
					write_log(pFile, "%s \n", "Sending message failed, disconnect");
					cout << "Sending message failed, disconnect" << endl;					
				}
				else
				{
					cout << "Sending message data ��" << mess << endl;
					write_log(pFile, "Sending message data �� %s \n", mess);
					write_log(pFile, "%s \n", "..........next ......");
					cout << "..........next ......" << endl;
					break;
				}
			}			
			
		}
		int ii = 0;
		while (RecvID.empty())
		{
			if (ControlF)
			{
				write_log(pFile, "%s \n", "End of algorithm processing");
				cout << "End of algorithm processing" << endl;
				return;
			}
		
			ii++;
			Sleep(500);
			if ((ii % 6) == 0)
			{
				cout << "RecvImg is empty���ȴ������ͼ ..." << endl;
			}			
		}
		if (ControlF)
		{
			break;
		}
	}
	return;
}

/*************************************************************************
������:ResultProcess			
��  ��: ����ж�
�ļ���: Server.cpp
��  ��: strDefectDectResult��
����ֵ: ��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
int Server::ResultProcess( map<int, vector<string>>  &DefectInfoData, int Aflag, string Pid)
{	
	//�㷨����
	Defect_Detection_Result *pstDefectDetectResult = new Defect_Detection_Result;
	vector<string> Socketdata;     //������λ������·������ݷָ�����
	const char *t = "#@";
	char data[256];
	int NGNum = 0;
	//#���͹�λ��#������#����#��ƷID#���Է�ʽ@ 
	if (!Socketdata.empty())
	{
		Socketdata.clear();
	}
	string strinfo = ReRecvData.front();
	ReRecvData.pop();
	
	write_log(pFile, "%s %s\n", " ReRecvData.info = ", strinfo.c_str());
	strcpy(data, strinfo.c_str());
	char *p = strtok(data, t);
	while (p)
	{
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
		Socketdata.push_back(s);
		p = strtok(NULL, t);
	}
	if( (0 == DefectInfoData.size())&& (0 != Aflag ))
	{
		Json::Value root;	
		root[featureInfo[1]] = "1000"; //���1
		root[featureInfo[2]] = "1000"; //���2
		root[featureInfo[3]] = "1000"; //���
		root[featureInfo[4]] = "1000"; //����
		root[featureInfo[5]] = "1000"; //�Ҷ�
		root[featureInfo[6]] = "1000"; //�ҶȲ�
		root[featureInfo[7]] = "1000"; //Բ��
		root[featureInfo[8]] = "1000";	//Բ�ܱ���	

		Json::StyledWriter sw;
		std::string res_str = sw.write(root);

		string strQuery = "INSERT INTO table_station_result (StationNo, ProName, Batch, ProductID, Result,  DefectID, DefectName, DefectImgID, DefectCoordinate, DefectFeatures,InsertTime ) \
		VALUES ('" + Socketdata[0] + "','" + Socketdata[1] + "','" + Socketdata[2] + "','" + Pid + "','" + "NG" + "', " + to_string(0) + ",'" + "����" + "'," + to_string(0) + ",'" + "1000,1000" + "' ,'" + res_str + "', CURTIME());";
		write_log(pFile, "%s %s\n", " �㷨ʧ�ܷ���-1�� ����sql���Ϊ strQuery= ", strQuery.c_str());
		SaveOKNG(strQuery);
		return 0;
	}
	else if (0 == DefectInfoData.size())
	{
		return 0;
	}
	else
	{
		int QueryResult = 0;
		vector<string> m_vec;
		int num1 = 0;
		if (!Defectdefinecage.empty())
		{
			Defectdefinecage.clear();
		}
		map<int, vector<string>>::iterator ma_it = DefectInfoData.begin();   //map<int, vector<string>>�洢ȱ����Ϣ  ��ӡȱ����Ϣ��־
		for (; ma_it != DefectInfoData.end(); ma_it++)
		{			
			write_log(pFile, "%s %s\n", " defectInfoNum = ", to_string(ma_it->first).c_str());
			vector<string>::iterator it = ma_it->second.begin();
			for (int i = 0; i < ma_it->second.size(); i++)   //���1  ���2
			{
				write_log(pFile, "%s %s\n", " defectInfo = ", ma_it->second.at(i).c_str());
			}
		}
		map<int, vector<string>>::iterator map_it = DefectInfoData.begin();   //map<int, vector<string>>�洢ȱ����Ϣ
		for (; map_it != DefectInfoData.end(); map_it++)
		{
			//ÿһ��ȱ����Ϣ
			string json_key;
			string json_value;
			//int nnn = mDefine_json.size();
			map<string, string>::iterator map_json = mDefine_json.begin();   //map<int, vector<string>>ȱ�ݶ������Ϣ
			for (; map_json != mDefine_json.end(); map_json++)
			{
				json_key = map_json->first;    // ����
				json_value = map_json->second;				 //((ȱ�����1>30)&&(ȱ�����1<20))		
				//cout << "ȱ�ݶ�����Ϣ :" << json_key << "    " << map_json->second << endl;
				if (!m_vec.empty())
					m_vec.clear();

				string strData;
				vector<string>::iterator it = map_it->second.begin();
				for (int i = 0; i < map_it->second.size(); i++)   //���1  ���2
				{
					strData = map_it->second.at(i);
					m_vec.push_back(strData);
					//cout << "ȱ����Ϣ��" << strData << endl;
					//cout << "����ֵ:" << featureInfo[i] << endl;
					size_t pos = json_value.find(featureInfo[i]);
					if (pos != string::npos)
					{
						//cout << "pos= " << pos << endl;
						//cout << json_value << endl;
						json_value.replace(pos, featureInfo[i].length(), strData);
						//cout << json_value << endl;
					}					
				}
				m_vec.push_back(to_string(map_it->first));
				string strQuery = "SELECT " + json_value + ";";
				QueryResult = m_CDatabase.filtrate(strQuery.c_str());
				//write_log(pFile, "%s %s\n", " defectDefine SQL = ", strQuery.c_str());
				if (0 != QueryResult)
				{
					//write_log(pFile, "%s %s\n", " defectDefine  = ", json_key);
					Defectdefinecage.insert(pair<string, vector<string>>(json_key, m_vec));
					break;
				}
			}	
			num1++;
		}

		int Defectnum = 0;
		string OKORNG ="OK";
		int defectNum = 0; //ͬ��ȱ�ݸ���
		int defectDis = 0; //ͬ��ȱ�ݾ���

		string strQuery = "INSERT INTO table_station_result (StationNo, ProName, Batch, ProductID, Result,  DefectID, DefectName, DefectImgID, DefectCoordinate, DefectFeatures,InsertTime ) VALUES ";
		string strData ="";		

		multimap<string, vector<string>>::iterator  mm_json = Defectdefinecage.begin();   //map<int, string >ȱ��ɸѡ����Ϣ
		for (; mm_json != Defectdefinecage.end(); mm_json++)
		{
			//cout << " ȱ�ݷ���key=:" << mm_json->first << endl;  // "    ȱ�ݷ��ඨ��=:" << mm_json->second <<
			//ÿһ��ȱ����Ϣ
			string json_key;
			string json_value;
			string strDefectCoordinate;
			map<string, string>::iterator map_json = mScreening_json.begin();   //map<string, string>ȱ����ѡ����Ϣ
			for (; map_json != mScreening_json.end(); map_json++)
			{
				json_key = map_json->first;    // ����
				json_value = map_json->second;				 //((ȱ�����1>30)&&(ȱ�����1<20))		

				if (mm_json->first == map_json->first) //ȱ�ݷ��� �� ȱ��ɸѡ�ж�Ϊͬһ��ȱ��
				{
					if (map_json->second == "") //ɸѡ����Ϊ��,ǿ��OK
					{
						OKORNG = "OK";
						break;
					}

					//����ͬ��ȱ�ݸ���
					defectNum = Defectdefinecage.count(mm_json->first);
					//cout << "defectNum = "<< defectNum << endl;
					if (defectNum == 2)
					{
						string str[2];
						multimap<string, vector<string>>::iterator  json;
						json = Defectdefinecage.find(mm_json->first);
						for (int i = 0;i < defectNum; i++, json++)
						{
							str[i] = json->second.at(0);
							//cout << "str[i] =:"<< str[i] <<endl;
						}
						int x1, x2, y1, y2;
						x1 = stoi(str[0].substr(0, str[0].find(",")));
						y1 = stoi(str[0].substr(str[0].find(",") + 1, str[0].length()));
						x2 = stoi(str[1].substr(0, str[1].find(",")));
						y2 = stoi(str[1].substr(str[1].find(",") + 1, str[1].length()));
						//cout << x1 << " ,"<< y1 <<  endl;
						//cout << x2 << " ," << y2 << endl;
						defectDis = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
					}

					//cout << "ȱ�ݶ�����Ϣ :" << json_key << "    " << map_json->second << endl;
					string strData;
					vector<string>::iterator it = mm_json->second.begin();
					strDefectCoordinate = mm_json->second.at(0);
					for (int i = 1; i < mm_json->second.size()+1; i++)   //���1  ���2
					{								
						if (i < mm_json->second.size()-1)
						{
							strData = mm_json->second.at(i);							
						}
						else
						{
							if (i == mm_json->second.size()- 1)
							{								
								Defectnum = atoi(mm_json->second.at(i).c_str());
								//cout << "defectNum2 = " << defectNum << endl;
								strData = to_string(defectNum);
								//cout << "defectNum3 = " << strData << endl;
							}
							if (i == mm_json->second.size() )
							{
								strData = to_string(defectDis);
							}
						}
						
						size_t pos;
						pos = json_value.find(featureInfo[i]);
						while (pos != string::npos)
						{
							//cout << "------------------------------------------" << endl;
							json_value.replace(pos, featureInfo[i].length(), strData);
							//cout << json_value << endl;
							pos = json_value.find(featureInfo[i]);
						}
					}
					string strQuery = "SELECT " + json_value + ";";
					QueryResult = m_CDatabase.filtrate(strQuery.c_str());
					//write_log(pFile, "%s %s\n", " defectScreening SQL = ", strQuery.c_str());
					if (0 != QueryResult)
					{
						OKORNG = "NG";
						NGNum++;
						break;
					}
					else
						OKORNG = "OK";
					write_log(pFile, "%s %s\n", " defectScreening Result = ", OKORNG);
				}
			}
			
			Json::Value root;
			for (int i = 1; i < mm_json->second.size() - 1; i++)   //���1  ���2
			{
				//cout <<"featureInfo[i] =:" << featureInfo[i] << "map_it->second.at(i) = :" << mm_json->second.at(i) << endl;
				root[featureInfo[i]] = mm_json->second.at(i);
			}

			Json::StyledWriter sw;
			std::string res_str = sw.write(root);

			pstDefectDetectResult->nStationNo = Socketdata[0];
			pstDefectDetectResult->strProName = Socketdata[1];
			pstDefectDetectResult->strBatch = Socketdata[2];
			if ((m_server.m_CDatabase.m_StationNoInfo.SysOnLine != 1))
			{
				pstDefectDetectResult->strProductID = Socketdata[3];
			}
			else
				pstDefectDetectResult->strProductID = Pid;
			pstDefectDetectResult->strResult = OKORNG;

			string str_nDefectID = "null";
			string str_DefectImgID = "null";

			if (pstDefectDetectResult->strResult.compare("OK") == 0)
			{			
				pstDefectDetectResult->strDefectName = "null";			
				pstDefectDetectResult->strDefectCoordinate = "null";
				pstDefectDetectResult->strDefectFeatures = "null";
			}
			else
			{
				pstDefectDetectResult->nDefectID = Defectnum;
				pstDefectDetectResult->strDefectName = json_key;
				str_nDefectID = to_string(pstDefectDetectResult->nDefectID);
				str_DefectImgID = to_string(pstDefectDetectResult->nDefectImgID);		
				pstDefectDetectResult->nDefectImgID = Defectnum;
				pstDefectDetectResult->strDefectCoordinate = strDefectCoordinate;
				pstDefectDetectResult->strDefectFeatures = res_str;
			}

			write_log(pFile, "%s %s\n", " NG Small Img Num =", to_string(Defectnum).c_str());
			write_log(pFile, "%s %s\n", " current Img ProductID =", pstDefectDetectResult->strProductID.c_str());

			if (strData == "")
			{
				strData = "('" + pstDefectDetectResult->nStationNo + "','" + pstDefectDetectResult->strProName + "','" + pstDefectDetectResult->strBatch + "','" + pstDefectDetectResult->strProductID + "','" + pstDefectDetectResult->strResult \
					+ "', " + str_nDefectID + "   ,'" + pstDefectDetectResult->strDefectName + "'," + str_DefectImgID + ",'" + pstDefectDetectResult->strDefectCoordinate + "' ,'" + pstDefectDetectResult->strDefectFeatures + "' , CURTIME())";
			}
			else				
			{ 
				strData = ",('" + pstDefectDetectResult->nStationNo + "','" + pstDefectDetectResult->strProName + "','" + pstDefectDetectResult->strBatch + "','" + pstDefectDetectResult->strProductID + "','" + pstDefectDetectResult->strResult \
					+ "', " + str_nDefectID + "   ,'" + pstDefectDetectResult->strDefectName + "'," + str_DefectImgID + ",'" + pstDefectDetectResult->strDefectCoordinate + "' ,'" + pstDefectDetectResult->strDefectFeatures + "' , CURTIME())";
			}		
			strQuery += strData;		
		}
		//write_log(pFile, "%s %s\n", " NG SaveData strQuery = ", strQuery.c_str());
		m_CDatabase.SaveData(strQuery.c_str());
	}	
	RELEASE(pstDefectDetectResult);
	return  NGNum;
}
/*************************************************************************
������:SaveOKNG
��  ��: ����NG����OK���ݵ����ݿ�
�ļ���: Server.cpp
��  ��: 
����ֵ: ��
�޸���: ZHZ
��  ��: 2019/10/25
��  ��: ��
*************************************************************************/
void Server::SaveOKNG(string strQuery)
{
	m_CDatabase.SaveData(strQuery.c_str());
}

/*************************************************************************
������:initInfo			
��  ��: ��ʼ�����
�ļ���: Server.cpp
��  ��: ��
����ֵ: ��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
void Server::initInfo()
{
	//����json����	
	JsonReader reader;
	JsonValue value;

	if (!mDefine_json.empty()) mDefine_json.clear();
	if (!mScreening_json.empty()) mScreening_json.clear();
	if (!mToolPara_json.empty()) mToolPara_json.clear();
	if (!mLanguage_json.empty()) mLanguage_json.clear();
	if (!featureInfo.empty()) featureInfo.clear();	
	if (!m_strJson.empty()) m_strJson.clear();	

	string localIP = GetLocalIpAddress();
	write_log(pFile, "%s %s\n", "localIP = ", localIP.c_str());
	string strQuery;
	strQuery = "SELECT  table_station.ToolPara ,table_station.DefectNum , table_station.DefectDefine, table_station.DefectScreening, table_station.Features,  table_config.NowProName,   \
				table_config.NowBatchName, table_config.DiskSpaceAlarm, table_config.DiskSpaceDelete ,  table_config.SysOnLine, table_config.SysLang,  \
				table_config.SaveOKImg, table_config.SaveNGImg, table_config.SaveCompressedOKImg, table_config.SaveCompressedNGImg, table_config.AlgorithmOnOrOff, table_config.SamplingCompressedImg,	\
				table_config.StationDynamic FROM table_station, table_config  WHERE table_config.NowProName = table_station.ProName AND table_station.IP = '" + localIP +"'; ";
		
	m_CDatabase.ReadWorkerParam(strQuery.c_str());
	write_log(pFile, "%s %s\n", "ReadWorkerParam strQuery = ", strQuery.c_str());

	if (!reader.parse(m_CDatabase.m_StationNoInfo.strFeatures, value))//���Զ������
	{
		write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strFeatures  = ", m_CDatabase.m_StationNoInfo.strFeatures);
		return;
	}	//������ֵ	
	printDefine(value, mLanguage_json); //���Զ������

	string strLang;
	map<string, string>::iterator mlang_json = mLanguage_json.begin();
	switch (m_CDatabase.m_StationNoInfo.Syslangage)//ϵͳ���ԣ�2 - �������ģ�1 - Ӣ�ģ�0 - ��������
	{
	case 0:		
		mlang_json = mLanguage_json.find("Simplified");  //0 - ��������
		if (mlang_json != mLanguage_json.end())
		{
			strLang = mlang_json->second;
		}	
		break;
	case 1:		
		mlang_json = mLanguage_json.find("English");  //1 - Ӣ��
		if (mlang_json != mLanguage_json.end())
		{
			strLang = mlang_json->second;
		}
		break;
	case 2:		
		mlang_json = mLanguage_json.find("Traditional");  //2 - ��������
		if (mlang_json != mLanguage_json.end())
		{
			strLang = mlang_json->second;
		}
		break;		
	default:
		break;
	}

	const char *t = "#";
	char data[256];
	strcpy(data, strLang.c_str());
	//#���͹�λ��#������#����#��ƷID#���Է�ʽ@      ���յ�����Ϣ��ʽ
	char *p = strtok(data, t);
	while (p)
	{
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
		featureInfo.push_back(s);
		p = strtok(NULL, t);
	}
	write_log(pFile, "%s %s\n", "ReadFeaturePara strLang = ", strLang.c_str());

	strQuery = "SELECT table_programme.ResultMode FROM table_programme, table_config WHERE table_programme.ProName = table_config.NowProName;";
	nResultMode = m_CDatabase.filtrate(strQuery.c_str());
	
	if (!reader.parse(m_CDatabase.m_StationNoInfo.strDefectDefine, value))
	{
		write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strDefectDefine  = ", m_CDatabase.m_StationNoInfo.strDefectDefine);
		return ;
	}	//������ֵ	
	printDefine(value, mDefine_json); //ȱ�ݶ�����Ϣ
	
	if (!reader.parse(m_CDatabase.m_StationNoInfo.strDefectScreensing, value))
	{
		write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strDefectScreensing  = ", m_CDatabase.m_StationNoInfo.strDefectScreensing);
		return;
	}	//������ֵ	
	printDefine(value, mScreening_json);//ȱ��ɸѡ������Ϣ

	if (m_CDatabase.m_StationNoInfo.strToolPara != "null")
	{
		if (!reader.parse(m_CDatabase.m_StationNoInfo.strToolPara, value))
		{
			write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strToolPara  = ", m_CDatabase.m_StationNoInfo.strToolPara);
			return;
		}	//������ֵ	
		printJson(value);//���湤�߲���

		for (int i = 0; i < m_strJson.size(); i++)
		{
			if (!m_json.empty())
			{
				m_json.clear();
			}
			printDefine1(m_strJson[i]);
			mToolPara_json.push_back(m_json);
		}
	}
	return ;
}
/*************************************************************************
������:CreateFolder
��  ��: �����ļ���
�ļ���: Server.cpp
��  ��: ��
����ֵ: ��
�޸���: ZHZ
��  ��: 2019/9/25
��  ��: ��
*************************************************************************/
void Server::CreateFolder()
{
	//��������ͼƬ���ļ���·��  data[1]��������  data[2]��������
	if (("null"  == m_CDatabase.m_StationNoInfo.strProName) || ("null" == m_CDatabase.m_StationNoInfo.strBatchName))
	{
		write_log(pFile, "%s %s\n", "CreateFolder failure, StationNoInfo.strProName  = ", m_CDatabase.m_StationNoInfo.strProName);
		write_log(pFile, "%s %s\n", "CreateFolder failure, StationNoInfo.strBatchName  = ", m_CDatabase.m_StationNoInfo.strBatchName);
		return;
	}
	string command;
	ProcedureFolderPath		= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName;					//����ʼͼ
	DefectFolderPath		= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\Defect";	//ȱ��ͼ	
	CompressNGFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\CompressNG";	//ѹ��NGͼ�ļ���·��
	CompressOKFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\CompressOK";	//ѹ��OKͼ�ļ���·��
	OriginalFolderPath		= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\Original";	//ԭͼ�ļ���·��
	OriginalNGFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\OriginalNG";	//ԭNGͼ�ļ���·��
	OriginalOKFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\OriginalOK";	//ԭOKͼ�ļ���·��

	command = "mkdir " + ProcedureFolderPath;				//����ʼͼ
	//cout << "ProcedureFolderPath :=" << command  << endl;
	system(command.c_str());
	command = "mkdir " + DefectFolderPath;					//ȱ��ͼ
	//cout << "DefectFolderPath :=" << command << endl;
	system(command.c_str());
	command = "mkdir " + CompressNGFolderPath;				//ѹ��NGͼ�ļ���·��
	system(command.c_str());
	command = "mkdir " + CompressOKFolderPath;				//ѹ��OKͼ�ļ���·��
	system(command.c_str());	
	command = "mkdir " + OriginalFolderPath;				//ԭͼ�ļ���·��
	system(command.c_str());
	command = "mkdir " + OriginalNGFolderPath;				//ԭNGͼ�ļ���·��
	system(command.c_str());
	command = "mkdir " + OriginalOKFolderPath;				//ԭOKͼ�ļ���·��
	system(command.c_str());
}
/*************************************************************************
������:FindFreeSpace
��  ��: ��ѯ���̿��пռ�
�ļ���: Server.cpp
��  ��: ��
����ֵ: i64FreeBytesToCaller������ʣ��ռ�
�޸���: ZHZ
��  ��: 2019/10/9
��  ��: ��
*************************************************************************/
int FindFreeSpace()
{
	DWORDLONG i64FreeBytesToCaller = 0;
	DWORDLONG i64TotalBytes = 0;
	DWORDLONG i64FreeBytes = 0;

	GetDiskFreeSpaceExA("D:\\", (PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);

	//cout << " totalspace =" << (float)i64TotalBytes / 1024 / 1024 / 1024 << " GB" << endl;//����������
	//cout << " freespace = " << (float)i64FreeBytesToCaller / 1024 / 1024 / 1024 << " GB" << endl;//����ʣ��ռ�
	float freespace = (float)i64FreeBytesToCaller / 1024 / 1024 / 1024;
	return (int)freespace;
}
/*************************************************************************
������:GetLocalIpAddress
��  ��: ��ȡ����IP��ַ
�ļ���: Server.cpp
��  ��: ��
����ֵ: ����IP��ַ
�޸���: ZHZ
��  ��: 2019/10/9
��  ��: ��
*************************************************************************/
string Server::GetLocalIpAddress()
{
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//�õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	string localIP;
	if (ERROR_SUCCESS == nRel)
	{
		IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
		localIP = pIpAddrString->IpAddress.String;
	}
	//�ͷ��ڴ�ռ�
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	if (localIP == "0.0.0.0")
	{
		localIP = "127.0.0.1";
	}
	cout << "����IP��ַ���£�" << endl;
	cout << "IP ��ַ��" << localIP << endl;

	return localIP;

}
/*************************************************************************
������:ConnectCamera
��  ��: �������
�ļ���: Server.cpp
��  ��: ��
����ֵ: ��
�޸���: ZHZ
��  ��: 2019/10/9
��  ��: ��
*************************************************************************/
void Server::ConnectCamera()
{
	//����������ӳ�ʼ�������ݷ������͹�λ��ȥ��ѯ�����ccf��ROI���Զ���ROI·�����㷨����

	string strNewCcfPath = "";
	string strOldCcfOath = "";
	string strCameraType = "";

	map<string, string>::iterator mAlg_json = mToolPara_json[0].begin();

	mAlg_json = mToolPara_json[0].find("CcfConfig");
	 if (mAlg_json != mToolPara_json[0].end())
	 {
		 strOldCcfOath =  mAlg_json->second;
	 }
	 mAlg_json = mToolPara_json[0].find("CameraType");
	 if (mAlg_json != mToolPara_json[0].end())
	 {
		 strCameraType = mAlg_json->second;
	 }

	 char szFilePath[MAX_PATH + 1] = { 0 };
	 GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	 (strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�

	 strNewCcfPath = szFilePath;
	 strNewCcfPath += "\\1.ccf";
	 if (file_exists(strNewCcfPath))
	 {
		 remove(strNewCcfPath.c_str());
	 }
	 cout << "strOldCcfOath = "<< strOldCcfOath << endl;
	 cout << "strNewCcfPath = " << strNewCcfPath << endl;
	 if ( !CopyFile(strOldCcfOath.c_str(), strNewCcfPath.c_str(), false))
	 {
		 cout << "Copy ccf File failed! "<< endl;
		 write_log(pFile, "%s \n", "Copy ccf File failed! ");
		 getchar();
		 return;
	 }
	
	 cout << "Camera start connect... " << endl;
	 write_log(pFile, "%s \n", "Camera start connect... ");
	 CameraInterface(m_CDatabase.m_StationNoInfo.strToolPara, m_CDatabase.m_StationNoInfo.SaveOKImg);	
}

//�����ļ����������ļ���
void getFiles(const std::string & path, std::vector<std::string> & files, std::vector<std::string> & filesId, string format)
{
	if (!files.empty())
	{
		files.clear();
	}

	//�ļ����  		 
	intptr_t hFile = 0;
	//�ļ���Ϣ��_finddata_t��Ҫio.hͷ�ļ�  		  
	struct _finddata_t fileinfo;
	std::string p;
	//if ((hFile = _findfirst(p.assign(path).append("\\*"+ ".bmp").c_str(), &fileinfo)) != -1)
	if ((hFile = _findfirst(p.assign(path).append("/*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  							
			//�������,�����б�  							  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("/").append(fileinfo.name), files, filesId, format);
			}
			else
			{
				files.push_back(p.assign(path).append("/").append(fileinfo.name));
				string strfilename = fileinfo.name;
				int pos = strfilename.find_last_of("_");
				if (pos == string::npos)
				{
					pos = strfilename.find_last_of(".");
				}
				filesId.push_back(strfilename.substr(0, pos));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int Algorithm_Type()
{
	int m_flag = 0;
	string ToolPosition;
	map<string, string>::iterator mAlg_json = mToolPara_json[1].begin();
	mAlg_json = mToolPara_json[1].find("ToolPosition");
	if (mAlg_json != mToolPara_json[1].end())
	{
		ToolPosition = mAlg_json->second;
	}
	int pos = ToolPosition.find("-");
	m_flag = atoi(ToolPosition.substr(pos + 1, 1).c_str());
	return m_flag;
}

void InitAlgrithmPara_hjh(int i)
{
	map<string, string>::iterator mAlg_json = mToolPara_json[i - 1].begin();
	mAlg_json = mToolPara_json[i - 1].find("new_model");  //�Ƿ��½�ģ���ļ���0����������ģ��    1���½�ģ��
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para_hjh.new_model = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i - 1].find("aa_region_positive");  //(�����A��ƽ�������ϸ�ֵ)A������ֵ
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para_hjh.aa_region_positive = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i - 1].find("aa_region_negative");  //(�����A��ƽ�������³�ֵ)A������ֵ
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para_hjh.aa_region_negative = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_positive");  //(覴ü��ɽ��ܻҶȷ�Χ)覴ø���ֵ
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.grey_difference_positive = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_negative");  //(覴ü��ɽ��ܻҶȷ�Χ)覴õ���ֵ
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.grey_difference_negative = atoi((mAlg_json->second).c_str());
	}
	
	mAlg_json = mToolPara_json[i].find("max_area");  //(��һ覴��������)ȱ���������
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.max_area = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("min_area");  //����С��������ȱ���������
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.min_area = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("num_for_stop_inspection");  //(覴���Ŀ��ȡ����)ȱ�ݲɼ�����
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.num_for_stop_inspection = atoi((mAlg_json->second).c_str());
	}
}

void InitAlgrithmPara(int i)
{
 	map<string, string>::iterator mAlg_json = mToolPara_json[i-1].begin();
	mAlg_json = mToolPara_json[i-1].find("aa_region_value_min");  //(a����ȡ��������) ��ߵ���ֵ
	if (mAlg_json != mToolPara_json[i-1].end())
	{
		 m_server.m_Algorithm_Para.aa_reg_min = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i - 1].find("aa_region_value_max");  //  (a����ȡ��������)��ߵ���ֵ
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para.aa_reg_max = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_positive"); // (覴ü��ɽ��ܻҶȷ�Χ) ��ֵ��������
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.grey_dif_pos = atof((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_negative"); //(覴ü��ɽ��ܻҶȷ�Χ) ��ֵ����  ����
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.grey_dif_neg = atof((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("exception_dilate_element_width"); // (���Ͳ����˿�)  �˲���
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.excep_dilate_width = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("exception_dilate_element_height"); // (���Ͳ����˸�) �˲���
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.excep_dilate_height = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("loop_stride_to_decide_stantard_grey_value"); // (�ɽ��ܻҶ�ֵ��������)  �ҽ�ȡֵ����
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.loop_stride_to_decide = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("defect_area_for_stop_inspection"); //(��һ覴��������)��ߵ����
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.defect_area = atoi((mAlg_json->second).c_str());
	}
	mAlg_json = mToolPara_json[i].find("defect_num_for_stop_inspection"); //(覴���Ŀ��ȡ����) ȱ������ 
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.defect_num = atoi((mAlg_json->second).c_str());
	}
	mAlg_json = mToolPara_json[i].find("defect_acceptable_minimum_area"); //����С��������С�ߵ����
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.defect_accep_min_area = atoi((mAlg_json->second).c_str());
	}
}

void InitAlgrithmPara_ITO(int i)
{
	map<string, string>::iterator mAlg_json = mToolPara_json[i].begin();
	mAlg_json = mToolPara_json[i].find("set_length");  //�趨�ж�Ϊȱ�ݵĳ�����ֵ
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_length = atoi((mAlg_json->second).c_str());
	}
	
	mAlg_json = mToolPara_json[i].find("set_thresh");  //�����뱳��֮��ĻҶȲ���ڸ���ֵ�����ָ����
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_thresh = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_top");  //������Ե���Կ��
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_top = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_left");  //�󲿱�Ե���Կ��
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_left = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_right");  //�Ҳ���Ե���Կ��
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_right = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_bottom");  //�²���Ե���Կ��
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_bottom = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_limit");  //���ü������ȱ������
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_limit = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_iter");  //�����˲�ǿ��ֵ
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_iter = atoi((mAlg_json->second).c_str());
	}
}

