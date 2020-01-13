/***************************************************************
Copyright (C), 2019-2100, CFHS Tech. Co., Ltd.
文  件名：Server.cpp
描    述：创建socket，连接数据库，逻辑业务处理
函数列表: 1.Start()					启动运行程序
		  2.CloseMySocket			关闭连接
		  3.CreateSocket			创建socket
		  4.BandSocket				绑定本地地址和端口号
		  5.ListenSocket			初始化监听并设置最大连接等待数量
		  6.AcceptSocket			接受请求连接的请求，并返回句柄
		  7.RecvAndSendSocket		接受和发送数据给客服端
		  8.DefectDetect			缺陷检测
		  9.ResultProcess			结果判断
		  10.initInfo				初始化信息
		  11.CreateFolder			创建文件夹路径
		  12.FindFreeSpace			查询磁盘剩余空间
		  13.GetLocalIpAddress		获取本地IP地址
作    者：ZHZ
日    期：2019/9/23
版    本：1.0
历史版本：<修改人>  <日  期>  <版  本> <描  述>
		    ZHZ	    2019/9/23   1.0     创建文件
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
#include <opencv2/highgui.hpp>  //highgui 模块头文件
#include <opencv2/imgproc.hpp>  //图像处理头文件
#include "Algorithm.h"


#include <Iphlpapi.h>       //获取Ip地址函数所需头文件
#include <iostream>
#include <fstream>
#include <sstream>        //istringstream 必须包含这个头文件
#include <mutex>

#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库

#include "json.h"

typedef Json::Writer JsonWriter;
typedef Json::Reader JsonReader;
typedef Json::Value  JsonValue;

using namespace cv;
using namespace std;
// 释放指针宏
#define RELEASE(x)                      {if(NULL != x){delete x; x = NULL;}}
// 释放指针数组宏
#define RELEASE_ARRAY(x)                {if(NULL != x){delete[] x; x = NULL;}}

#define MYPROT	6000

//SOCKET sClient;
//日志文件
FILE	*pFile;	
string  CompressOKFolderPath;    //压缩OK图文件夹路径
string  CompressNGFolderPath;    //压缩NG图文件夹路径
string  DefectFolderPath;		//缺陷小图文件夹路径
string  OriginalOKFolderPath;    //原OK图文件夹路径
string  OriginalNGFolderPath;    //原NG图文件夹路径
string	OriginalFolderPath;		 //原图文件夹路径
string  ProcedureFolderPath;    //过程图文件夹路径
string  CcfFilePath;				//ccf文件路径

vector<string> AlgorithmData;  //算法数据
vector<string> ParameterData;  //参数数据
vector<string> CameraInfoData; //相机信息数据
vector<string> RoiInfoData;	   //ROi信息数据
vector<string> Offlinefiles;	//离线模式下保存图片文件
vector<string> OfflinefilesId;  //离线模式下保存图片的ID

vector<string> featureInfo;					//保存特性信息
multimap<string, vector<string>> Defectdefinecage; //缺陷定义分类

queue<string> RecvData;  //socket接收的数据
queue<string> ReRecvData; ////socket接收的数据
queue<string> RecvImg;
queue<string> RecvID;
vector<string> Socketdata;     //接受上位机软件下发的数据分隔保存
queue<Mat> ImgMat;


int	nResultMode;
bool ControlF;
Server m_server;
int currentImg;

void DefectDetect(SOCKET recvsocket); //缺陷检测
int FindFreeSpace();				//检测磁盘剩余空间
void FindCameraImg(SOCKET recvsocket); //查找图片进行分类处理
int ServerThread(SOCKET *ClientSocket); //启动各个socket服务
int SendDetect(SOCKET recvsocket, vector<string> &RecvInfo, int iflag);  //发送信息到上位机
int Algorithm_Type();
void InitAlgrithmPara_hjh(int i);
void InitAlgrithmPara(int i);
void InitAlgrithmPara_ITO(int i);

//相机部分
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

//命名空间
#pragma warning(disable: 4996)//屏蔽itoa的语法错误

//文件保存路径

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

//保存原图
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

		/*********从相机中取图读取数据**********/
		PUINT8 pData;
		Buffers->GetAddress((void**)&pData);
		Mat img = Mat::zeros(Buffers->GetHeight(), Buffers->GetWidth(), CV_8UC1);
		memcpy(img.data, pData, Buffers->GetHeight() * Buffers->GetWidth() * sizeof(char));
		if (img.empty())
		{
			write_log(pFile, "%s \n", "相机彩图为空或者异常!");
			imwrite("D:\\error.bmp", img);		
		}

		ImgMat.push(img);
		bCamera = true;

		/**********************测试使用代码******************************/
		//Mat image;
		//string strimg;
		//strimg = "D:\\20191220\\111\\" + to_string(iiii) + ".bmp";
		//image = imread(strimg);
		//cout << "strimg = " << strimg << endl;
		//iiii++;
		//Mat imgGray;
		////灰度化
		//cvtColor(image, imgGray, CV_BGR2GRAY);
		//ImgMat.push(imgGray);
		//bCamera = true;
		//cout << "bCamera = " << bCamera << endl;
		/**********************************************************************/

		//char* pImageData = (char*)malloc(width*height * sizeof(char));
		//memcpy(pImageData, pData, height *width * sizeof(char));
		//memcpy(img1.data, pImageData, height *width * sizeof(char));
		//namedWindow("img", 0); imshow("img", img); waitKey(1);
		//imwrite("D:\\打标\\4444.bmp", img);
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

//遍历文件夹下所有文件名
void getFiles(const std::string & path, std::vector<std::string> & files, std::vector<std::string> & filesId, string format);

map<string, string> mDefine_json;  //缺陷定义信息
map<string, string> mScreening_json;  //缺陷筛选信息

map<string, string> m_json; //中间参数
vector<map<string, string>> mToolPara_json; //保存工具参数
vector<JsonValue> m_strJson; //保存大分类的参数数据

map<string, string> mLanguage_json;	 //保存语言解析参数

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

//写文件日志
int write_log(FILE* pFile, const char *format, ...)
{
	va_list arg;
	int done;
	va_start(arg, format);
	SYSTEMTIME st = { 0 };  	
	GetLocalTime(&st);  
	//获取当前时间 可精确到ms	
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
	//获取当前时间 可精确到ms	

	printf("当前系统时间为：  %4d/%02d/%02d %02d:%02d:%02d.%03d \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	return;
}
//判断文件夹是存在
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
函数名: Start
描  述: 启动运行程序
文件名: Server.cpp
参  数: 无
返回值: void:无
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
void Server::Start()
{	
	//日志文件
	string CameraType;
	string CcfFilePath;
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);

	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串

	char strLogName[MAX_PATH];
	sprintf(strLogName, "%s\\%04d-%02d-%02d.log", szFilePath, tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday);

	pFile = fopen(strLogName, "a");
	if (!CreateSocket())   //socket连接文件
	{
		system("pause");
		return;
	}
	AcceptSocket();
}

/*************************************************************************
函数名:CreateSocket			
描  述: 创建socket
文件名: Server.cpp
参  数: 无
返回值: void:无
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
bool Server::CreateSocket()
{	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		cout << "Socket error" << endl;
	m_nServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//设定TCP协议接口;失败返回INVALID_SOCKET
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
	addrSrv.sin_port = htons(MYPROT); //1024以上的端口号  
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	int retVal  = ::bind(m_nServerSocket, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {   //绑定服务器地址和端口号;成功，返回0，否则为SOCKET_ERROR
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
	if (!m_server.m_CDatabase.ConnectDatabase())  //数据库连接
	{
		write_log(pFile, "%s \n", "Database connection fail !");
	}
	
	cout << "Database connection success !" << endl;
	write_log(pFile, "%s \n", "Database connection success !");

	m_server.initInfo();
	m_server.CreateFolder();
	if (m_server.m_CDatabase.m_StationNoInfo.SysOnLine == 0)//0为在线
		m_server.ConnectCamera();

	m_server.RecvAndSendSocket(*ClientSocket);//接受客户端的消息		
	closesocket(*ClientSocket);
	free(ClientSocket);
	m_server.m_CDatabase.CloseDatabase();
	return 0;
}

/*************************************************************************
函数名:.AcceptSocket			
描  述: 接受请求连接的请求，并返回句柄
文件名: Server.cpp
参  数: 无
返回值: SOCKET:句柄
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
SOCKET Server::AcceptSocket()
{	
	//设置Socket为非阻塞模式
	int iMode = 1;
	int retVal;//调用Socket函数的返回值
	retVal = ioctlsocket(m_nServerSocket, FIONBIO, (u_long FAR*) &iMode);
	if (retVal == SOCKET_ERROR)
	{
		printf("设置Socket为非阻塞模式失败!\n");
		WSACleanup();
		return -1;
	}

	while (1)
	{		
		//循环接收客户端连接请求并创建服务线程
		SOCKET *ClientSocket = new SOCKET;
		ClientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		sockaddr_in nClientSocket;
		//接收客户端连接请求
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
		cout << "一个客户端已连接到服务器，socket是：" << *ClientSocket << endl;
		thread ServerThd(ServerThread, ClientSocket);//CreateThread(NULL, 0, &ServerThread, ClientSocket, 0, NULL);
		ServerThd.detach();
	}
	fclose(pFile);
	if (m_nServerSocket != -1)
		closesocket(m_nServerSocket);	//关闭socket连接
	m_nServerSocket = -1;
	WSACleanup();	//终止ws2_32.lib的使用
	return 0;
}

/*************************************************************************
函数名:RecvAndSendSocket		
描  述: 接受和发送数据给客服端
文件名: Server.cpp
参  数: recvsocket：客户端句柄
返回值: 成功或失败
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
void Server::RecvAndSendSocket(SOCKET recvsocket)
{
	while (!ImgMat.empty()) ImgMat.pop();
	while (!RecvImg.empty()) RecvImg.pop();
	while (!RecvID.empty()) RecvID.pop();
	if (!Socketdata.empty()) Socketdata.clear();
	while (true)
	{
		  //1为相机连接成功			0为在线模式									1为离线模式											0为算法关闭，1为算法打开
		if( ((nReady == 1) && (0 == m_CDatabase.m_StationNoInfo.SysOnLine))||(1 == m_CDatabase.m_StationNoInfo.SysOnLine))
		{			
			char mess[100] = "#ReadyOK@"; // 回复值代表相机启动成功		
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
		// 从客户端接收数据
		char buff[256];	
		try {
			memset(buff, 0, sizeof(buff));
			std::lock_guard<std::mutex> my_lock_guard(my_mutex_1);
			int nRecv = recv(recvsocket, buff, 256, 0);//从客户端接受消息  #发送工位号#方案名#批次#产品ID@
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
		if (strbuf.compare("#Close@") == 0)//关机
		{
			char cmd[20] = "shutdown -s -t";
			system(cmd);
			break;
		}
		else if (strbuf.compare("#Reset@") == 0)//关机重启
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
			thread findImg(FindCameraImg, recvsocket); //启动流程以及相机拍图
			findImg.detach();
			ControlCame = false;
		}
	}
}

void FindCameraImg(SOCKET recvsocket)
{
	bool Control = true;			//算法启动控制
	
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
		//#发送工位号#方案名#批次#产品ID#测试方式@      接收到的信息格式
		char *p = strtok(data, t);
		while (p)
		{
			string s = p; //分割得到的字符串转换为string类型  
			Socketdata.push_back(s);
			p = strtok(NULL, t);
		}

		//正常测试流程0
		//导出图片：outimg
		//复测执行：retest
		//上一张：last
		//下一张：next
		//循环模式：loop
		//查找产品ID图：实际ID
		string str = Socketdata[4]; //测试方式         #发送工位号#方案名#批次#产品ID#测试方式@
		strProductID = Socketdata[3]; //产品ID         #发送工位号#方案名#批次#产品ID#测试方式@
		int flag = 1;
		string new_BmpName;

		if (m_server.m_CDatabase.m_StationNoInfo.SysOnLine == 1) //模式  软件离线：1-离线，0-在线；
		{
			if (("OutImg" == str))  //离线模式下的图片导出 后一位数据回复1
			{
				string strQuery = "SELECT DownFilePath FROM table_config  WHERE NowProName  = '" + Socketdata[1] + "'; ";
				m_server.m_CDatabase.ReadInAndOutImgPath(strQuery.c_str(), flag);   //flag = 1 为导出

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
			else if ("InImg" == str) //图像导入    每次收到信息重新读取数据库,获取路径,获取新图片  后一位数据回复1
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
			else if ("Retest" == str) //复测执行   单张图像时候  执行算法操作回复信息, 文件夹的时候,执行第一张图片,执行算法,回复信息   后一位数据回复1
			{
				new_BmpName = Offlinefiles[currentImg];
				cout << "new_BmpName  Retest == str" << new_BmpName << "::::" << currentImg << endl;
				RecvImg.push(new_BmpName);
				RecvID.push(OfflinefilesId[currentImg]);
				RecvData.pop();
			}
			else if ("Last" == str) //上一张  当前图片的上一张图,执行算法   后一位数据回复1
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
			else if ("Next" == str) //下一张 当前图片的下一张图,执行算法  后一位数据回复1
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
			else if ("Loop" == str) //循环模式  最后一位数据回复1,没执行完回复0
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
			else   //查询产品ID图   后一位数据回复1
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
		else if (("0" == str) && (m_server.m_CDatabase.m_StationNoInfo.SysOnLine != 1)) // 正常测试流程0  ；正常流程下，模式  软件离线：1-离线，0-在线；  
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
			cout << "接受到的数据格式不对！" << endl;
			write_log(pFile, "%s %s\n", "接受到的数据格式不对！: ", strInfo.c_str());
			return;
		}
		
		//公共部分代码	
		if (ControlDefect == true) 
		{
			thread mydataTest(DefectDetect, recvsocket); //启动算法
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
				cout << "socket RecvData is empty, 等待上位机下发数据 ... " << endl;
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
		strResult += "0";   // 加Id
	}
	else
	{
		strResult += RecvInfo[0];   // 加Id
	}
	
	strResult += "#0#";			//没有缺陷	
	strResult += "0#";			//磁盘空间大于下限值的时候，上报0,本来就是原磁盘上的图,所以不用磁盘检测						
	strResult += to_string(iflag);		
	strResult += "@";			//查询ID成功回复1,失败0
	char mess[100];				// "#产品ID#结果#磁盘结果@";   OK--0;  NG---实际数量
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
函数名:DefectDetect			
描  述: 缺陷检测
文件名: Server.cpp
参  数: strDefectDectResult：
返回值: 无
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
void DefectDetect(SOCKET recvsocket)
{	
	
	Mat src;
	string Pid;
	map<int, vector<string>> DefectInfoData;  //读取算法缺陷信息到map
	while (true)
	{
		write_log(pFile, "RecvID size =   %s \n",to_string( RecvID.size()).c_str());		
		if (!RecvID.empty())
		{		
			string strResult = "#";

			if (m_server.m_CDatabase.m_StationNoInfo.SysOnLine == 1) //離綫
			{
				write_log(pFile, "RecvImg size =   %s \n", to_string(RecvImg.size()).c_str());
				string strImg;
				strImg = RecvImg.front();
				RecvImg.pop();

				cout << "算法调用图片名字=" << strImg << endl;
				write_log(pFile, "%s  %s \n", "算法调用图片名字=", strImg.c_str());
				src = imread(strImg);
				if (src.empty())
				{
					write_log(pFile, "%s  %s \n", "输入到算法的图像为空,请检查原图是否存在或者采集正常！");
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
					write_log(pFile, "%s  %s \n", "输入到算法的图像为空,请检查原图是否存在或者采集正常！");
					return;
				}
			}			
					
			strResult += RecvID.front(); //产品ID
			Pid = RecvID.front();
			RecvID.pop();
			string new_BmpName;
			string new_JpgName;
			int result = 0;
			int AlgoriFlag = 0;
			Mat img;
			int iSaveBmpStatus = 0;
			CurrentTime();
			if (0 == m_server.m_CDatabase.m_StationNoInfo.AlgorithmOnOrOff) //0为算法关闭  1为算法打开
			{
				img = src.clone();
			}
			if ((1 == nResultMode) && (1 == m_server.m_CDatabase.m_StationNoInfo.AlgorithmOnOrOff))  //1-正常流程，2-强制OK，3-强制NG    0为算法关闭  1为算法打开
			{
				try
				{
					if (!DefectInfoData.empty())
					{
						DefectInfoData.clear();
					}
					int i = 2; //第三组vector保存的数据
					if (1 == Algorithm_Type())    //AA分区类型索引1--陈工，2--黄工
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
						cout << "算子配置错误，请核对" << endl;
						write_log(pFile, "%s  \n", "算子配置错误，请核对");
					}
				}
				catch (const cv::Exception& err)
				{
					AlgoriFlag = -1;
					imwrite("算法失败.bmp", src);
					cout << "算法失败:" << err.what() << endl;
				}
				CurrentTime();
				result = m_server.ResultProcess(DefectInfoData, AlgoriFlag, Pid); //返回NG的数量
				write_log(pFile, "%s \n", "结果检测筛选结束");

				if (0 != result) {
					iSaveBmpStatus = 1;
					new_BmpName = OriginalNGFolderPath + "\\" + Pid + "_NG.bmp";
					new_JpgName = CompressNGFolderPath + "\\" + Pid + "_NG.jpg"; //"_NG.jpg";
					strResult += "#" + to_string(result) + "#"; //有缺陷	
					cout << "缺陷检测结果为NG" << endl;	
				}
				else if ((0 == result) && (0 == AlgoriFlag))
				{		
					iSaveBmpStatus = 2;
					new_BmpName = OriginalOKFolderPath + "\\" + Pid + "_OK.bmp";
					new_JpgName = CompressOKFolderPath + "\\" + Pid + "_OK.jpg"; //"_OK.jpg";					
					strResult += "#0#"; //没有缺陷
					cout << "缺陷检测结果为OK" << endl;
				}
				else {
					iSaveBmpStatus = 1;
					new_BmpName = OriginalNGFolderPath + "\\" + Pid + "_NG.bmp";
					new_JpgName = CompressNGFolderPath + "\\" + Pid + "_NG.jpg"; //"_NG.jpg";
					strResult += "#1#"; //有缺陷	
					cout << "缺陷检测结果为NG" << endl;
				}
			}
		
			
			if ((1 == nResultMode)&&(0 == m_server.m_CDatabase.m_StationNoInfo.AlgorithmOnOrOff)) //0为算法关闭  1为算法打开
			{
				iSaveBmpStatus = 3;
				new_BmpName = OriginalFolderPath + "\\" + Pid + ".bmp"; //关闭算法只为采图
				new_JpgName = OriginalFolderPath + "\\" + Pid + ".jpg"; //".jpg";
				strResult += "#0#"; //没有缺陷 
			}
			else if (2 == nResultMode)  //强制OK
			{
				iSaveBmpStatus = 2;
				new_BmpName = OriginalOKFolderPath + "\\" + Pid + "_OK.bmp";
				new_JpgName = CompressOKFolderPath + "\\" + Pid + "_OK.jpg"; //"_OK.jpg";
				strResult += "#0#"; //没有缺陷   不管NG还是OK都回复0
			}
			else if (3 == nResultMode)  //强制NG
			{
				iSaveBmpStatus = 1;
				new_BmpName = OriginalNGFolderPath + "\\" + Pid + "_NG.bmp";
				new_JpgName = CompressNGFolderPath + "\\" + Pid + "_NG.jpg"; //"_NG.jpg";
				strResult += "#0#"; //没有缺陷   不管NG还是OK都回复0
			}			
		
			if (!img.empty())
			{				
				cout << "bmp_new_name = " << new_BmpName << endl;
				write_log(pFile, "%s %s\n", " bmp_new_name = ", new_BmpName.c_str());
				cout << "SaveCompressedOKImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveCompressedOKImg << "   SaveCompressedNGImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveCompressedNGImg << endl;
				cout << "SaveOKImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveOKImg << "   SaveNGImg = " << m_server.m_CDatabase.m_StationNoInfo.SaveNGImg << endl;

				if (3 == iSaveBmpStatus) // 算法关闭只为采图
				{
				    cout << "开始保存bmp原图" << endl;	write_log(pFile, "%s \n", "开始保存bmp原图");
					imwrite(new_BmpName, src);
					write_log(pFile, "%s \n", "保存bmp原图结束");	 cout << "保存bmp原图结束" << endl;
				}

				if ( (1 == m_server.m_CDatabase.m_StationNoInfo.SaveNGImg) && (0 == m_server.m_CDatabase.m_StationNoInfo.SysOnLine)&& (1 == iSaveBmpStatus))  //软件在离线 0--在线  1--离线
				{
					cout << "开始保存NG-bmp原图" << endl;	write_log(pFile, "%s \n", "开始保存NG-bmp原图");
					imwrite(new_BmpName, src);		
					write_log(pFile, "%s \n", "保存NG-bmp原图结束");	 cout << "保存NG-bmp原图结束" << endl;
				}

				if ((1 == m_server.m_CDatabase.m_StationNoInfo.SaveOKImg) && (0 == m_server.m_CDatabase.m_StationNoInfo.SysOnLine) && (2 == iSaveBmpStatus))   //软件在离线 0--在线  1--离线
				{
					cout << "开始保存OK-bmp原图" << endl;	write_log(pFile, "%s \n", "开始保存OK-bmp原图");
					imwrite(new_BmpName, src);
					write_log(pFile, "%s \n", "保存OK-bmp原图结束");	cout << "保存OK-bmp原图结束" << endl;
				}
				if (1 == m_server.m_CDatabase.m_StationNoInfo.SamplingCompressedImg) //1 - 保存，0 - 不保存
				{
					pyrDown(img, img);
				}
				if (1 == m_server.m_CDatabase.m_StationNoInfo.StationDynamic) //工位图来源，1-动态，0-静态
				{
					cout << "开始保存jpg压缩图" << endl;		write_log(pFile, "%s \n", "开始保存jpg压缩图");
					imwrite(new_JpgName, img); //动态情况下一定保存
					write_log(pFile, "%s \n", "结束保存jpg压缩图");		 cout << "结束保存jpg压缩图" << endl;
				}
				else
				{
					if ((1 == m_server.m_CDatabase.m_StationNoInfo.SaveCompressedOKImg) && (2 == iSaveBmpStatus)) // 1-NG; 2- OK
					{
						cout << "开始保存OK-jpg压缩图" << endl;	write_log(pFile, "%s \n", "开始保存OK-jpg压缩图");
						imwrite(new_JpgName, img);
						write_log(pFile, "%s \n", "结束保存OK-jpg压缩图");	cout << "结束保存OK-jpg压缩图" << endl;
					}
					if ((1 == m_server.m_CDatabase.m_StationNoInfo.SaveCompressedNGImg) && (1 == iSaveBmpStatus)) // 1-NG; 2- OK
					{
						cout << "开始保存NG-jpg压缩图" << endl;	write_log(pFile, "%s \n", "开始保存NG-jpg压缩图");
						imwrite(new_JpgName, img);
						write_log(pFile, "%s \n", "结束保存NG-jpg压缩图");	cout << "结束保存NG-jpg压缩图" << endl;
					}
				}
	
			}			

			if (m_server.m_CDatabase.m_StationNoInfo.DiskSpaceAlarm > FindFreeSpace())
			{
				strResult += "1#";//磁盘空间小于下限值的时候，上报1,然后上位机软件报警
			}
			else
				strResult += "0#";//磁盘空间大于下限值的时候，上报0
								  //发送数据给我客户端   //数据库操作
			if (RecvImg.size() == 0)
				strResult += "1@";	 //  执行完回复1
			else
				strResult += "0@";	 //  执行完回复1

			char mess[100];		  // "#产品ID#结果#磁盘结果@";   OK--0;  NG---实际数量
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
					cout << "Sending message data ：" << mess << endl;
					write_log(pFile, "Sending message data ： %s \n", mess);
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
				cout << "RecvImg is empty，等待相机采图 ..." << endl;
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
函数名:ResultProcess			
描  述: 结果判断
文件名: Server.cpp
参  数: strDefectDectResult：
返回值: 无
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
int Server::ResultProcess( map<int, vector<string>>  &DefectInfoData, int Aflag, string Pid)
{	
	//算法操作
	Defect_Detection_Result *pstDefectDetectResult = new Defect_Detection_Result;
	vector<string> Socketdata;     //接受上位机软件下发的数据分隔保存
	const char *t = "#@";
	char data[256];
	int NGNum = 0;
	//#发送工位号#方案名#批次#产品ID#测试方式@ 
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
		string s = p; //分割得到的字符串转换为string类型  
		Socketdata.push_back(s);
		p = strtok(NULL, t);
	}
	if( (0 == DefectInfoData.size())&& (0 != Aflag ))
	{
		Json::Value root;	
		root[featureInfo[1]] = "1000"; //面积1
		root[featureInfo[2]] = "1000"; //面积2
		root[featureInfo[3]] = "1000"; //宽度
		root[featureInfo[4]] = "1000"; //长度
		root[featureInfo[5]] = "1000"; //灰度
		root[featureInfo[6]] = "1000"; //灰度差
		root[featureInfo[7]] = "1000"; //圆度
		root[featureInfo[8]] = "1000";	//圆周比率	

		Json::StyledWriter sw;
		std::string res_str = sw.write(root);

		string strQuery = "INSERT INTO table_station_result (StationNo, ProName, Batch, ProductID, Result,  DefectID, DefectName, DefectImgID, DefectCoordinate, DefectFeatures,InsertTime ) \
		VALUES ('" + Socketdata[0] + "','" + Socketdata[1] + "','" + Socketdata[2] + "','" + Pid + "','" + "NG" + "', " + to_string(0) + ",'" + "划伤" + "'," + to_string(0) + ",'" + "1000,1000" + "' ,'" + res_str + "', CURTIME());";
		write_log(pFile, "%s %s\n", " 算法失败返回-1， 插入sql语句为 strQuery= ", strQuery.c_str());
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
		map<int, vector<string>>::iterator ma_it = DefectInfoData.begin();   //map<int, vector<string>>存储缺陷信息  打印缺陷信息日志
		for (; ma_it != DefectInfoData.end(); ma_it++)
		{			
			write_log(pFile, "%s %s\n", " defectInfoNum = ", to_string(ma_it->first).c_str());
			vector<string>::iterator it = ma_it->second.begin();
			for (int i = 0; i < ma_it->second.size(); i++)   //面积1  面积2
			{
				write_log(pFile, "%s %s\n", " defectInfo = ", ma_it->second.at(i).c_str());
			}
		}
		map<int, vector<string>>::iterator map_it = DefectInfoData.begin();   //map<int, vector<string>>存储缺陷信息
		for (; map_it != DefectInfoData.end(); map_it++)
		{
			//每一条缺陷信息
			string json_key;
			string json_value;
			//int nnn = mDefine_json.size();
			map<string, string>::iterator map_json = mDefine_json.begin();   //map<int, vector<string>>缺陷定义的信息
			for (; map_json != mDefine_json.end(); map_json++)
			{
				json_key = map_json->first;    // 脏污
				json_value = map_json->second;				 //((缺陷面积1>30)&&(缺陷面积1<20))		
				//cout << "缺陷定义信息 :" << json_key << "    " << map_json->second << endl;
				if (!m_vec.empty())
					m_vec.clear();

				string strData;
				vector<string>::iterator it = map_it->second.begin();
				for (int i = 0; i < map_it->second.size(); i++)   //面积1  面积2
				{
					strData = map_it->second.at(i);
					m_vec.push_back(strData);
					//cout << "缺陷信息：" << strData << endl;
					//cout << "特性值:" << featureInfo[i] << endl;
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
		int defectNum = 0; //同类缺陷个数
		int defectDis = 0; //同类缺陷距离

		string strQuery = "INSERT INTO table_station_result (StationNo, ProName, Batch, ProductID, Result,  DefectID, DefectName, DefectImgID, DefectCoordinate, DefectFeatures,InsertTime ) VALUES ";
		string strData ="";		

		multimap<string, vector<string>>::iterator  mm_json = Defectdefinecage.begin();   //map<int, string >缺陷筛选的信息
		for (; mm_json != Defectdefinecage.end(); mm_json++)
		{
			//cout << " 缺陷分类key=:" << mm_json->first << endl;  // "    缺陷分类定义=:" << mm_json->second <<
			//每一条缺陷信息
			string json_key;
			string json_value;
			string strDefectCoordinate;
			map<string, string>::iterator map_json = mScreening_json.begin();   //map<string, string>缺陷赛选的信息
			for (; map_json != mScreening_json.end(); map_json++)
			{
				json_key = map_json->first;    // 脏污
				json_value = map_json->second;				 //((缺陷面积1>30)&&(缺陷面积1<20))		

				if (mm_json->first == map_json->first) //缺陷分类 和 缺陷筛选判断为同一个缺陷
				{
					if (map_json->second == "") //筛选条件为空,强制OK
					{
						OKORNG = "OK";
						break;
					}

					//查找同类缺陷个数
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

					//cout << "缺陷定义信息 :" << json_key << "    " << map_json->second << endl;
					string strData;
					vector<string>::iterator it = mm_json->second.begin();
					strDefectCoordinate = mm_json->second.at(0);
					for (int i = 1; i < mm_json->second.size()+1; i++)   //面积1  面积2
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
			for (int i = 1; i < mm_json->second.size() - 1; i++)   //面积1  面积2
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
函数名:SaveOKNG
描  述: 保存NG或者OK数据到数据库
文件名: Server.cpp
参  数: 
返回值: 无
修改人: ZHZ
日  期: 2019/10/25
其  他: 无
*************************************************************************/
void Server::SaveOKNG(string strQuery)
{
	m_CDatabase.SaveData(strQuery.c_str());
}

/*************************************************************************
函数名:initInfo			
描  述: 初始化相机
文件名: Server.cpp
参  数: 无
返回值: 无
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
void Server::initInfo()
{
	//解析json数据	
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

	if (!reader.parse(m_CDatabase.m_StationNoInfo.strFeatures, value))//语言定义解析
	{
		write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strFeatures  = ", m_CDatabase.m_StationNoInfo.strFeatures);
		return;
	}	//遍历键值	
	printDefine(value, mLanguage_json); //语言定义解析

	string strLang;
	map<string, string>::iterator mlang_json = mLanguage_json.begin();
	switch (m_CDatabase.m_StationNoInfo.Syslangage)//系统语言：2 - 繁体中文，1 - 英文，0 - 简体中文
	{
	case 0:		
		mlang_json = mLanguage_json.find("Simplified");  //0 - 简体中文
		if (mlang_json != mLanguage_json.end())
		{
			strLang = mlang_json->second;
		}	
		break;
	case 1:		
		mlang_json = mLanguage_json.find("English");  //1 - 英文
		if (mlang_json != mLanguage_json.end())
		{
			strLang = mlang_json->second;
		}
		break;
	case 2:		
		mlang_json = mLanguage_json.find("Traditional");  //2 - 繁体中文
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
	//#发送工位号#方案名#批次#产品ID#测试方式@      接收到的信息格式
	char *p = strtok(data, t);
	while (p)
	{
		string s = p; //分割得到的字符串转换为string类型  
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
	}	//遍历键值	
	printDefine(value, mDefine_json); //缺陷定义信息
	
	if (!reader.parse(m_CDatabase.m_StationNoInfo.strDefectScreensing, value))
	{
		write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strDefectScreensing  = ", m_CDatabase.m_StationNoInfo.strDefectScreensing);
		return;
	}	//遍历键值	
	printDefine(value, mScreening_json);//缺陷筛选定义信息

	if (m_CDatabase.m_StationNoInfo.strToolPara != "null")
	{
		if (!reader.parse(m_CDatabase.m_StationNoInfo.strToolPara, value))
		{
			write_log(pFile, "%s %s\n", "Json Parse failure, StationNoInfo.strToolPara  = ", m_CDatabase.m_StationNoInfo.strToolPara);
			return;
		}	//遍历键值	
		printJson(value);//保存工具参数

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
函数名:CreateFolder
描  述: 创建文件夹
文件名: Server.cpp
参  数: 无
返回值: 无
修改人: ZHZ
日  期: 2019/9/25
其  他: 无
*************************************************************************/
void Server::CreateFolder()
{
	//创建保存图片的文件夹路径  data[1]：方案名  data[2]：批次名
	if (("null"  == m_CDatabase.m_StationNoInfo.strProName) || ("null" == m_CDatabase.m_StationNoInfo.strBatchName))
	{
		write_log(pFile, "%s %s\n", "CreateFolder failure, StationNoInfo.strProName  = ", m_CDatabase.m_StationNoInfo.strProName);
		write_log(pFile, "%s %s\n", "CreateFolder failure, StationNoInfo.strBatchName  = ", m_CDatabase.m_StationNoInfo.strBatchName);
		return;
	}
	string command;
	ProcedureFolderPath		= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName;					//过程始图
	DefectFolderPath		= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\Defect";	//缺陷图	
	CompressNGFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\CompressNG";	//压缩NG图文件夹路径
	CompressOKFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\CompressOK";	//压缩OK图文件夹路径
	OriginalFolderPath		= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\Original";	//原图文件夹路径
	OriginalNGFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\OriginalNG";	//原NG图文件夹路径
	OriginalOKFolderPath	= "D:\\Image\\" + m_CDatabase.m_StationNoInfo.strProName + "\\" + m_CDatabase.m_StationNoInfo.strBatchName + "\\OriginalOK";	//原OK图文件夹路径

	command = "mkdir " + ProcedureFolderPath;				//过程始图
	//cout << "ProcedureFolderPath :=" << command  << endl;
	system(command.c_str());
	command = "mkdir " + DefectFolderPath;					//缺陷图
	//cout << "DefectFolderPath :=" << command << endl;
	system(command.c_str());
	command = "mkdir " + CompressNGFolderPath;				//压缩NG图文件夹路径
	system(command.c_str());
	command = "mkdir " + CompressOKFolderPath;				//压缩OK图文件夹路径
	system(command.c_str());	
	command = "mkdir " + OriginalFolderPath;				//原图文件夹路径
	system(command.c_str());
	command = "mkdir " + OriginalNGFolderPath;				//原NG图文件夹路径
	system(command.c_str());
	command = "mkdir " + OriginalOKFolderPath;				//原OK图文件夹路径
	system(command.c_str());
}
/*************************************************************************
函数名:FindFreeSpace
描  述: 查询磁盘空闲空间
文件名: Server.cpp
参  数: 无
返回值: i64FreeBytesToCaller：磁盘剩余空间
修改人: ZHZ
日  期: 2019/10/9
其  他: 无
*************************************************************************/
int FindFreeSpace()
{
	DWORDLONG i64FreeBytesToCaller = 0;
	DWORDLONG i64TotalBytes = 0;
	DWORDLONG i64FreeBytes = 0;

	GetDiskFreeSpaceExA("D:\\", (PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);

	//cout << " totalspace =" << (float)i64TotalBytes / 1024 / 1024 / 1024 << " GB" << endl;//磁盘总容量
	//cout << " freespace = " << (float)i64FreeBytesToCaller / 1024 / 1024 / 1024 << " GB" << endl;//磁盘剩余空间
	float freespace = (float)i64FreeBytesToCaller / 1024 / 1024 / 1024;
	return (int)freespace;
}
/*************************************************************************
函数名:GetLocalIpAddress
描  述: 获取本地IP地址
文件名: Server.cpp
参  数: 无
返回值: 本机IP地址
修改人: ZHZ
日  期: 2019/10/9
其  他: 无
*************************************************************************/
string Server::GetLocalIpAddress()
{
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
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
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	if (localIP == "0.0.0.0")
	{
		localIP = "127.0.0.1";
	}
	cout << "网卡IP地址如下：" << endl;
	cout << "IP 地址：" << localIP << endl;

	return localIP;

}
/*************************************************************************
函数名:ConnectCamera
描  述: 连接相机
文件名: Server.cpp
参  数: 无
返回值: 无
修改人: ZHZ
日  期: 2019/10/9
其  他: 无
*************************************************************************/
void Server::ConnectCamera()
{
	//相机部分连接初始化，根据方案名和工位号去查询相机、ccf、ROI手自动、ROI路径、算法参数

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
	 (strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串

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

//遍历文件夹下所有文件名
void getFiles(const std::string & path, std::vector<std::string> & files, std::vector<std::string> & filesId, string format)
{
	if (!files.empty())
	{
		files.clear();
	}

	//文件句柄  		 
	intptr_t hFile = 0;
	//文件信息，_finddata_t需要io.h头文件  		  
	struct _finddata_t fileinfo;
	std::string p;
	//if ((hFile = _findfirst(p.assign(path).append("\\*"+ ".bmp").c_str(), &fileinfo)) != -1)
	if ((hFile = _findfirst(p.assign(path).append("/*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  							
			//如果不是,加入列表  							  
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
	mAlg_json = mToolPara_json[i - 1].find("new_model");  //是否新建模板文件，0：调用已有模板    1：新建模板
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para_hjh.new_model = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i - 1].find("aa_region_positive");  //(相对于A区平均亮度上浮值)A区高阈值
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para_hjh.aa_region_positive = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i - 1].find("aa_region_negative");  //(相对于A区平均亮度下沉值)A区低阈值
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para_hjh.aa_region_negative = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_positive");  //(瑕疵检测可接受灰度范围)瑕疵高阈值
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.grey_difference_positive = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_negative");  //(瑕疵检测可接受灰度范围)瑕疵低阈值
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.grey_difference_negative = atoi((mAlg_json->second).c_str());
	}
	
	mAlg_json = mToolPara_json[i].find("max_area");  //(单一瑕疵面积上限)缺陷面积上限
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.max_area = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("min_area");  //（最小检测面积）缺陷面积下限
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.min_area = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("num_for_stop_inspection");  //(瑕疵数目提取上限)缺陷采集数量
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_hjh.num_for_stop_inspection = atoi((mAlg_json->second).c_str());
	}
}

void InitAlgrithmPara(int i)
{
 	map<string, string>::iterator mAlg_json = mToolPara_json[i-1].begin();
	mAlg_json = mToolPara_json[i-1].find("aa_region_value_min");  //(a区提取亮度下限) 提边低阈值
	if (mAlg_json != mToolPara_json[i-1].end())
	{
		 m_server.m_Algorithm_Para.aa_reg_min = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i - 1].find("aa_region_value_max");  //  (a区提取亮度上限)提边低阈值
	if (mAlg_json != mToolPara_json[i - 1].end())
	{
		m_server.m_Algorithm_Para.aa_reg_max = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_positive"); // (瑕疵检测可接受灰度范围) 阈值区间上限
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.grey_dif_pos = atof((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("grey_difference_negative"); //(瑕疵检测可接受灰度范围) 阈值区间  下限
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.grey_dif_neg = atof((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("exception_dilate_element_width"); // (膨胀操作核宽)  滤波宽
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.excep_dilate_width = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("exception_dilate_element_height"); // (膨胀操作核高) 滤波高
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.excep_dilate_height = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("loop_stride_to_decide_stantard_grey_value"); // (可接受灰度值遍历步长)  灰阶取值段数
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.loop_stride_to_decide = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("defect_area_for_stop_inspection"); //(单一瑕疵面积上限)大斑点面积
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.defect_area = atoi((mAlg_json->second).c_str());
	}
	mAlg_json = mToolPara_json[i].find("defect_num_for_stop_inspection"); //(瑕疵数目提取上限) 缺陷数量 
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.defect_num = atoi((mAlg_json->second).c_str());
	}
	mAlg_json = mToolPara_json[i].find("defect_acceptable_minimum_area"); //（最小检测面积）小斑点面积
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para.defect_accep_min_area = atoi((mAlg_json->second).c_str());
	}
}

void InitAlgrithmPara_ITO(int i)
{
	map<string, string>::iterator mAlg_json = mToolPara_json[i].begin();
	mAlg_json = mToolPara_json[i].find("set_length");  //设定判定为缺陷的长度阈值
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_length = atoi((mAlg_json->second).c_str());
	}
	
	mAlg_json = mToolPara_json[i].find("set_thresh");  //网格与背景之间的灰度差大于该阈值将被分割出来
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_thresh = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_top");  //顶部边缘忽略宽度
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_top = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_left");  //左部边缘忽略宽度
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_left = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_right");  //右部边缘忽略宽度
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_right = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_bottom");  //下部边缘忽略宽度
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_bottom = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_limit");  //设置检测的最大缺陷数量
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_limit = atoi((mAlg_json->second).c_str());
	}

	mAlg_json = mToolPara_json[i].find("set_iter");  //设置滤波强度值
	if (mAlg_json != mToolPara_json[i].end())
	{
		m_server.m_Algorithm_Para_ITO.detector_iter = atoi((mAlg_json->second).c_str());
	}
}

