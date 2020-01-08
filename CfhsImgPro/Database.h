
#pragma once
#include <string>
#include <vector>

using namespace std;

typedef struct _StationNoInfo
{
	string	strToolPara;		 //���߲���
	string	strDefectDefine;	 //ȱ������
	string  strDefectScreensing; //ȱ��ɸѡ
	string	strFeatures;		 //���Զ���
	string  strProName;			 //������
	string  strBatchName;		 //������
	int		DiskSpaceAlarm;		 //����С�ڱ���
	int		DiskSpaceDelete;	 //����С�ڶ���ɾ��	
	int		SaveOKImg;			 //����OKԭͼƬ,1-���棬0-������
	int		SaveNGImg;			 //����NGԭͼƬ,1-���棬0-������
	int		SaveCompressedOKImg; //����ѹ��OKԭͼƬ,1-���棬0-������
	int		SaveCompressedNGImg; //����ѹ��NGԭͼƬ,1-���棬0-������
	int		SamplingCompressedImg;//�������ѹ��ͼ��1-����ѹ����0-������ѹ��
	int		StationDynamic;		 //��λͼ��Դ��1-��̬��0-��̬
	int		AlgorithmOnOrOff;	 //�㷨����
	int		Syslangage;			 //ϵͳ����  0--Ӣ��  1--����
	int		SysOnLine;			 //��������� 0--����  1--����
	string  strInputImgPath;	 //����ͼƬ·��
	string  strOutputImgPath;	 //����ͼƬ·��
}StationNoInfo;

//typedef struct _featuresInfo
//{
//	string CoorDinate;      //ȱ������
//	string DefectArea1;		//    ���1
//	string DefectArea2;		//    ���2
//	string DefectWidth;		//    ���
//	string DefectHight;		//    �߶�
//	string DefectGray;		//    �Ҷ�
//	string DefectGrayDiff;	//    �ҶȲ�
//	string DefectRundness;	//    Բ��
//	string CirferenceRatio;	//	  Բ�ܱ���
//
//}featuresInfo;


class CDatabase
{
public:
	CDatabase();
	~CDatabase();

public:
	//��ʼ����Դ
	void InitRes();
	//�ͷ���Դ
	void Release();
	//���������ݿ�
	bool ConnectDatabase();
	//�ر����ݿ�
	void CloseDatabase();
	//���ݱ��Ƿ����
	bool IsTableExist(const char* pszTableName);
	//��ȡ��λ��Ϣ
	void ReadWorkerParam(const char* pQuery);
	//���湤λ��Ϣ
	bool SaveData(const char* pQuery);
	//��ȡ�Ƿ񱣴�ԭͼ��־
	int filtrate(const char* pQuery);
	//��ѯ���ݿ���������
	void ReadFeaturePara(const char* pQuery, vector<string> &info);

	string ReadResultMode(const char* pQuery);

	void ReadInAndOutImgPath(const char* pQuery, int flag);


public:

	string pszIP;
	string pszUser;
	string pszPwd;
	string pszDBName;

	char* strLocalTime;
	StationNoInfo m_StationNoInfo;
	//featuresInfo m_featuresInfo;
	
	int nSaveFlag;
	
};

