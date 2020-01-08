#pragma once



#include <WinSock2.h>

#include <vector>
#include <map>
#include "Database.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll


//�����㷨����ṹ��
typedef struct _Defect_Detection_Result
{
	string	nStationNo;			//��λ
	string	strProName;			//����
	string	strBatch;			//����
	string	strProductID;		//��ƷID
	string	strResult;			//�����0��OK    >=1:NG   
	int 	nDefectID;			//ȱ��ID
	string	strDefectName;		//ȱ����
	int 	nDefectImgID;		//ȱ��СͼID
	string	strDefectCoordinate;//ȱ������
	string	strDefectFeatures;	//ȱ������

	void DefectResultInit()
	{
		nStationNo = "";
		strProName = "";
		strBatch = "";
		strProductID = "";
		strResult = "OK";  //���0��OK    >=1:NG
		nDefectID = 0;
		strDefectName = "";
		nDefectImgID = 0;
		strDefectCoordinate = "";
		strDefectFeatures = "";
	}

}Defect_Detection_Result, *PDefect_Detection_Result;

typedef struct _Algorithm_Para
{
	int aa_reg_min;
	int aa_reg_max;
	//������ȱ�ݼ�⣬�����Ƿ���
	float grey_dif_pos;
	float grey_dif_neg;
	int excep_dilate_width;
	int excep_dilate_height;
	int loop_stride_to_decide;
	int defect_area;
	int defect_num;
	int defect_accep_min_area;

}Algorithm_Para;



typedef struct _Algorithm_Para_hjh
{
	int	new_model;					//�Ƿ��½�ģ���ļ���0����������ģ��    1���½�ģ��
	int	aa_region_positive;			// (�����A��ƽ�������ϸ�ֵ)A������ֵ
	int aa_region_negative;			// (�����A��ƽ�������³�ֵ)A������ֵ

	int grey_difference_positive;	//  (覴ü��ɽ��ܻҶȷ�Χ)覴ø���ֵ
	int grey_difference_negative;	//(覴ü��ɽ��ܻҶȷ�Χ)覴õ���ֵ
	int max_area;					//(��һ覴��������)ȱ���������
	int min_area;						//����С��������ȱ���������
	int num_for_stop_inspection;			//(覴���Ŀ��ȡ����)ȱ�ݲɼ�����
}Algorithm_Para_hjh;

typedef struct _Algorithm_Para_ITO
{
	int detector_length;			// (ͳ��ƽ����ֵ����������)		ȱ�ݳ���
	int detector_thresh;			//(�Ҷ�ͻ���ж���ֵ)�ж���ֵ	�ָ���ֵ
	int detector_top;				//��������Ե���Կ�ȣ�			�ϱ�Ե
	int detector_left;				//���󲿱�Ե���Կ�ȣ�			���Ե
	int detector_right;				//���Ҳ���Ե���Կ�ȣ�			�ұ�Ե
	int detector_bottom;			//���²���Ե���Կ�ȣ�			�±�Ե
	int	detector_limit;				//  ȱ������					���ü������ȱ������
	int detector_iter;				//	�˲�ǿ��					�����˲�ǿ��ֵ
}Algorithm_Para_ITO;





class Server
{
public:
	Server();
	~Server();

public:
	bool CreateSocket();												//����socket
	SOCKET AcceptSocket();												//�����������ӵ����󣬲����ؾ��
	void RecvAndSendSocket(SOCKET recvsocket);							//���ܺͷ������ݸ��ͷ���

	void Start();														//�������г���

	int ResultProcess(map<int, vector<string>>  &DefectInfoData, int Aflag, string Pid);		//���������
	void SaveOKNG(string strQuery);
	void initInfo();													//��������ӿ�
	void CreateFolder();												//�����ļ���·��
	
	string GetLocalIpAddress();											//��ȡ����IP��ַ
	void ConnectCamera();												//�����������

public:
	SOCKET m_nServerSocket;												//�󶨱��ص�ַ�Ͷ˿ںŵ��׽ӿ�
	
	int							nStationNo;
	//bool						initCameraflag;
	CDatabase					m_CDatabase;
	Defect_Detection_Result		m_Defect_Detection_Result;				//������
	//�㷨����
	Algorithm_Para				m_Algorithm_Para;
	Algorithm_Para_hjh			m_Algorithm_Para_hjh;
	Algorithm_Para_ITO			m_Algorithm_Para_ITO;
};

