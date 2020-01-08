/***************************************************************
Copyright (C), 2019-2100, CFHS Tech. Co., Ltd.
文  件名：Database.cpp
描    述：通过MySQL自己的API函数进行连接操作数据库
函数列表：	1. void InitRes			初始化资源
			2. void Release			释放资源
			3. bool ConnectDatabase	打开连接数据库
			4. void CloseDatabase	关闭数据库
			5. bool IsTableExist	数据表是否存在
			6. void ReadWorkerParam	读取工位信息
			7. bool SaveData		保存工位信息
			8. bool filtrate		筛选结果
			9. void ReadFeaturePara 读取特性值
作    者：ZHZ
日    期：2019/9/20
版    本：1.0
历史版本：<修改人>  <日  期>  <版  本> <描  述>
ZHZ	    2019/9/20   1.0     创建文件
***************************************************************/

#include "stdafx.h"
#include "Database.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "mysql.h" 
using namespace std;

#pragma comment(lib,"libmySQL.lib") 
MYSQL m_sqlCon;
char pQuery[2000]; //查询语句
MYSQL_RES* res; //一个结果集结构体
MYSQL_ROW row;  //char** 二维数组，存放一条条记录


extern void CurrentTime();

CDatabase::CDatabase()
{
	//InitRes();
}

CDatabase::~CDatabase()
{
	//Release();
}

/*************************************************************************
函数名: InitRes
描  述: 初始化资源
文件名: Database.cpp
参  数: void: 无
返回值: void: 无
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
void CDatabase::InitRes()
{
	char szParameter[16] = { 0 };
	char szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[1] = 0;
	strcat_s(szFilePath, "DataBaseIni.ini");

	//数据库IP
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("数据库参数", "DatabaseIP", NULL, szParameter, 16, szFilePath);
	pszIP = szParameter;

	//数据库用户名
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("数据库参数", "DatabaseUser", NULL, szParameter, 16, szFilePath);
	pszUser = szParameter;

	//数据库密码
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("数据库参数", "DatabasePwd", NULL, szParameter, 16, szFilePath);
	pszPwd = szParameter;

	// 数据库名称
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("数据库参数", "DatabaseName", NULL, szParameter, 16, szFilePath);
	pszDBName = szParameter;

	CurrentTime(); cout << "Database IP = " << pszIP << endl;

	mysql_init(&m_sqlCon);
	mysql_options(&m_sqlCon, MYSQL_SET_CHARSET_NAME, "gbk");
}
/*************************************************************************
函数名: Release
描  述: 释放资源
文件名: Database.cpp
参  数: void: 无
返回值: void: 无
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
void CDatabase::Release()
{

}
/*************************************************************************
函数名: ConnectDatabase
描  述: 连接数据库
文件名: Database.cpp
参  数: 无
返回值: bool: true  打开成功
false 打开失败
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
bool CDatabase::ConnectDatabase()
{
	InitRes();
	// localhost:服务器 root为账号密码 test为数据库名 3306为端口 
	if (!mysql_real_connect(&m_sqlCon, pszIP.c_str(), pszUser.c_str(), pszPwd.c_str(), pszDBName.c_str(), 3306, NULL, 0))
	{
		CurrentTime();  std::cout << "Database connection fail !" << std::endl;
		return false;
	}
	return true;
}
/*************************************************************************
函数名: CloseDatabase
描  述: 释放结果集，关闭数据库
文件名: Database.cpp
参  数: void: 无
返回值: void: 无
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
void CDatabase::CloseDatabase()
{
	CurrentTime(); std::cout << "Database close !" << std::endl;
	mysql_close(&m_sqlCon);
	return;
}
/*************************************************************************
函数名: IsTableExist
描  述: 数据表是否存在
文件名: Database.cpp
参  数: pszTableName: 表名
返回值: bool: true  存在
false 不存在
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
bool CDatabase::IsTableExist(const char* pszTableName)
{
	return true;
}
/*************************************************************************
函数名: ReadWorkerParam
描  述: 读取数据
文件名: Database.cpp
参  数: pQuery: 查询读取sql语句
返回值: void:无
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
void CDatabase::ReadWorkerParam(const char* pQuery)
{
	//cout << pQuery << endl;
	if(mysql_query(&m_sqlCon, pQuery))		
	{
		CurrentTime();  cout << " query database fail" << endl;
		CurrentTime();  std::cout << " pQuery=： " << pQuery << std::endl;
		return;
	}
	res = mysql_store_result(&m_sqlCon);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			m_StationNoInfo.strToolPara			= row[0];	
			m_StationNoInfo.strDefectDefine		= row[1];
			m_StationNoInfo.strDefectScreensing = row[2];
			m_StationNoInfo.strFeatures			= row[3];
			m_StationNoInfo.strProName			= row[4];
			m_StationNoInfo.strBatchName		= row[5];
			m_StationNoInfo.DiskSpaceAlarm		= atoi(row[6]);
			m_StationNoInfo.DiskSpaceDelete		= atoi(row[7]);	
			m_StationNoInfo.SysOnLine			= atoi(row[8]);
			m_StationNoInfo.Syslangage 			= atoi(row[9]);
			m_StationNoInfo.SaveOKImg			= atoi(row[10]);
			m_StationNoInfo.SaveNGImg			= atoi(row[11]);
			m_StationNoInfo.SaveCompressedOKImg = atoi(row[12]);
			m_StationNoInfo.SaveCompressedNGImg = atoi(row[13]);
			m_StationNoInfo.AlgorithmOnOrOff	= atoi(row[14]);
			m_StationNoInfo.SamplingCompressedImg = atoi(row[15]);
			m_StationNoInfo.StationDynamic		= atoi(row[16]);
		}
	}	
	mysql_free_result(res);
	CurrentTime();  cout << "config和station联合查询成功！" << endl;
	return;
}
/*************************************************************************
函数名: SaveData
描  述: 保存数据
文件名: Database.cpp
参  数: pQuery: sql语句
返回值: void: 无
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
bool CDatabase::SaveData(const char* pQuery)
{
	if (0 != mysql_query(&m_sqlCon, pQuery))
	{
		CurrentTime();  std::cout << " Save OK or NG data into database fail"<< std::endl;
		CurrentTime();  std::cout << " strQuery=： " << pQuery << std::endl;
		return false;
	}	
	return true;
}

/*************************************************************************
函数名: filtrate
描  述: 设置筛选结果
文件名: Database.cpp
参  数: 查询sql语句
返回值: void: 无
修改人: ZHZ
日  期: 2019/9/20
其  他: 无
*************************************************************************/
int CDatabase::filtrate(const char* pQuery)
{		
	int fiResult;
	if (mysql_query(&m_sqlCon, pQuery))
	{
		CurrentTime();  cout << "filtrate query =" << pQuery << endl;
		CurrentTime();  cout << " query database fail" << endl;
		CurrentTime();  cout << mysql_error(&m_sqlCon) << endl;
		return NULL;
	}
	res = mysql_store_result(&m_sqlCon);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{		
			fiResult = atoi( row[0]);
		}
	}
	mysql_free_result(res);
	//cout << "设置筛选结果1-正常结果，2-强制OK，3-强制NG 为::" << fiResult <<endl;
	return fiResult;
}

/*************************************************************************
函数名: ReadFeaturePara
描  述: 读取特性参数
文件名: Database.cpp
参  数: 查询sql语句
返回值: void: 无
修改人: ZHZ
日  期: 2019/10/18
其  他: 无
*************************************************************************/
void CDatabase::ReadFeaturePara(const char* pQuery, vector<string> &info)
{	
	if (mysql_query(&m_sqlCon, pQuery))
	{
		CurrentTime();  cout << " query database fail" << endl;
		CurrentTime();  cout << "ReadFeaturePara pQuery  = " << pQuery << endl;
		return;
	}
	res = mysql_store_result(&m_sqlCon);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			for (int t = 0; t<mysql_num_fields(res); t++)
			{
				info.push_back(row[t]);
			}
		}
	}
	mysql_free_result(res);
	CurrentTime();  cout << "查询系统语言成功 "<< endl;
	return;
}

//*************************************************************************
//函数名: ReadResultMode
//描  述: 读取特性参数
//文件名: Database.cpp
//参  数: 查询sql语句
//返回值: void: 无
//修改人: ZHZ
//日  期: 2019/10/18
//其  他: 无
//*************************************************************************/
string CDatabase::ReadResultMode(const char* pQuery)
{
	if (mysql_query(&m_sqlCon, pQuery))
	{
		CurrentTime();  cout << " query database fail" << endl;
		CurrentTime();  cout << "ReadResultMode pQuery  = " << pQuery << endl;
		return NULL;
	}
	string strPara;
	res = mysql_store_result(&m_sqlCon);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			//for (int t = 0; t<mysql_num_fields(res); t++)
			//{
			//	info.push_back(row[t]);
			//}
			strPara = row[0];
		}
	}
	mysql_free_result(res);
	return strPara;
}
//*************************************************************************
//函数名: ReadInAndOutImgPath
//描  述: 读取特性参数
//文件名: Database.cpp
//参  数: 查询sql语句,查询导入导出路径的标志
//返回值: void: 无
//修改人: ZHZ
//日  期: 2019/10/23
//其  他: 无
//*************************************************************************/
void CDatabase::ReadInAndOutImgPath(const char* pQuery, int flag)
{
	if (mysql_query(&m_sqlCon, pQuery))
	{
		CurrentTime();  cout << " query database fail" << endl;
		CurrentTime();  cout << "ReadResultMode pQuery  = " << pQuery << endl;
		return ;
	}
	res = mysql_store_result(&m_sqlCon);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			if (0 == flag) //导入路径
			{
				m_StationNoInfo.strInputImgPath		= row[0];				
			}
			else //导出路径
				m_StationNoInfo.strOutputImgPath = row[0];
		}
	}
	mysql_free_result(res);
	return ;
}

