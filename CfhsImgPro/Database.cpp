/***************************************************************
Copyright (C), 2019-2100, CFHS Tech. Co., Ltd.
��  ������Database.cpp
��    ����ͨ��MySQL�Լ���API�����������Ӳ������ݿ�
�����б�	1. void InitRes			��ʼ����Դ
			2. void Release			�ͷ���Դ
			3. bool ConnectDatabase	���������ݿ�
			4. void CloseDatabase	�ر����ݿ�
			5. bool IsTableExist	���ݱ��Ƿ����
			6. void ReadWorkerParam	��ȡ��λ��Ϣ
			7. bool SaveData		���湤λ��Ϣ
			8. bool filtrate		ɸѡ���
			9. void ReadFeaturePara ��ȡ����ֵ
��    �ߣ�ZHZ
��    �ڣ�2019/9/20
��    ����1.0
��ʷ�汾��<�޸���>  <��  ��>  <��  ��> <��  ��>
ZHZ	    2019/9/20   1.0     �����ļ�
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
char pQuery[2000]; //��ѯ���
MYSQL_RES* res; //һ��������ṹ��
MYSQL_ROW row;  //char** ��ά���飬���һ������¼


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
������: InitRes
��  ��: ��ʼ����Դ
�ļ���: Database.cpp
��  ��: void: ��
����ֵ: void: ��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
void CDatabase::InitRes()
{
	char szParameter[16] = { 0 };
	char szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[1] = 0;
	strcat_s(szFilePath, "DataBaseIni.ini");

	//���ݿ�IP
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("���ݿ����", "DatabaseIP", NULL, szParameter, 16, szFilePath);
	pszIP = szParameter;

	//���ݿ��û���
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("���ݿ����", "DatabaseUser", NULL, szParameter, 16, szFilePath);
	pszUser = szParameter;

	//���ݿ�����
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("���ݿ����", "DatabasePwd", NULL, szParameter, 16, szFilePath);
	pszPwd = szParameter;

	// ���ݿ�����
	ZeroMemory(szParameter, sizeof(szParameter));
	GetPrivateProfileString("���ݿ����", "DatabaseName", NULL, szParameter, 16, szFilePath);
	pszDBName = szParameter;

	CurrentTime(); cout << "Database IP = " << pszIP << endl;

	mysql_init(&m_sqlCon);
	mysql_options(&m_sqlCon, MYSQL_SET_CHARSET_NAME, "gbk");
}
/*************************************************************************
������: Release
��  ��: �ͷ���Դ
�ļ���: Database.cpp
��  ��: void: ��
����ֵ: void: ��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
void CDatabase::Release()
{

}
/*************************************************************************
������: ConnectDatabase
��  ��: �������ݿ�
�ļ���: Database.cpp
��  ��: ��
����ֵ: bool: true  �򿪳ɹ�
false ��ʧ��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
bool CDatabase::ConnectDatabase()
{
	InitRes();
	// localhost:������ rootΪ�˺����� testΪ���ݿ��� 3306Ϊ�˿� 
	if (!mysql_real_connect(&m_sqlCon, pszIP.c_str(), pszUser.c_str(), pszPwd.c_str(), pszDBName.c_str(), 3306, NULL, 0))
	{
		CurrentTime();  std::cout << "Database connection fail !" << std::endl;
		return false;
	}
	return true;
}
/*************************************************************************
������: CloseDatabase
��  ��: �ͷŽ�������ر����ݿ�
�ļ���: Database.cpp
��  ��: void: ��
����ֵ: void: ��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
void CDatabase::CloseDatabase()
{
	CurrentTime(); std::cout << "Database close !" << std::endl;
	mysql_close(&m_sqlCon);
	return;
}
/*************************************************************************
������: IsTableExist
��  ��: ���ݱ��Ƿ����
�ļ���: Database.cpp
��  ��: pszTableName: ����
����ֵ: bool: true  ����
false ������
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
bool CDatabase::IsTableExist(const char* pszTableName)
{
	return true;
}
/*************************************************************************
������: ReadWorkerParam
��  ��: ��ȡ����
�ļ���: Database.cpp
��  ��: pQuery: ��ѯ��ȡsql���
����ֵ: void:��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
void CDatabase::ReadWorkerParam(const char* pQuery)
{
	//cout << pQuery << endl;
	if(mysql_query(&m_sqlCon, pQuery))		
	{
		CurrentTime();  cout << " query database fail" << endl;
		CurrentTime();  std::cout << " pQuery=�� " << pQuery << std::endl;
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
	CurrentTime();  cout << "config��station���ϲ�ѯ�ɹ���" << endl;
	return;
}
/*************************************************************************
������: SaveData
��  ��: ��������
�ļ���: Database.cpp
��  ��: pQuery: sql���
����ֵ: void: ��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
*************************************************************************/
bool CDatabase::SaveData(const char* pQuery)
{
	if (0 != mysql_query(&m_sqlCon, pQuery))
	{
		CurrentTime();  std::cout << " Save OK or NG data into database fail"<< std::endl;
		CurrentTime();  std::cout << " strQuery=�� " << pQuery << std::endl;
		return false;
	}	
	return true;
}

/*************************************************************************
������: filtrate
��  ��: ����ɸѡ���
�ļ���: Database.cpp
��  ��: ��ѯsql���
����ֵ: void: ��
�޸���: ZHZ
��  ��: 2019/9/20
��  ��: ��
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
	//cout << "����ɸѡ���1-���������2-ǿ��OK��3-ǿ��NG Ϊ::" << fiResult <<endl;
	return fiResult;
}

/*************************************************************************
������: ReadFeaturePara
��  ��: ��ȡ���Բ���
�ļ���: Database.cpp
��  ��: ��ѯsql���
����ֵ: void: ��
�޸���: ZHZ
��  ��: 2019/10/18
��  ��: ��
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
	CurrentTime();  cout << "��ѯϵͳ���Գɹ� "<< endl;
	return;
}

//*************************************************************************
//������: ReadResultMode
//��  ��: ��ȡ���Բ���
//�ļ���: Database.cpp
//��  ��: ��ѯsql���
//����ֵ: void: ��
//�޸���: ZHZ
//��  ��: 2019/10/18
//��  ��: ��
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
//������: ReadInAndOutImgPath
//��  ��: ��ȡ���Բ���
//�ļ���: Database.cpp
//��  ��: ��ѯsql���,��ѯ���뵼��·���ı�־
//����ֵ: void: ��
//�޸���: ZHZ
//��  ��: 2019/10/23
//��  ��: ��
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
			if (0 == flag) //����·��
			{
				m_StationNoInfo.strInputImgPath		= row[0];				
			}
			else //����·��
				m_StationNoInfo.strOutputImgPath = row[0];
		}
	}
	mysql_free_result(res);
	return ;
}

