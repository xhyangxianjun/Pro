#pragma once



#include <WinSock2.h>

#include <vector>
#include <map>
#include "Database.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll


//定义算法结果结构体
typedef struct _Defect_Detection_Result
{
	string	nStationNo;			//工位
	string	strProName;			//方案
	string	strBatch;			//批次
	string	strProductID;		//产品ID
	string	strResult;			//检测结果0：OK    >=1:NG   
	int 	nDefectID;			//缺陷ID
	string	strDefectName;		//缺陷名
	int 	nDefectImgID;		//缺陷小图ID
	string	strDefectCoordinate;//缺陷坐标
	string	strDefectFeatures;	//缺陷特征

	void DefectResultInit()
	{
		nStationNo = "";
		strProName = "";
		strBatch = "";
		strProductID = "";
		strResult = "OK";  //结果0：OK    >=1:NG
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
	//下面是缺陷检测，上面是分区
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
	int	new_model;					//是否新建模板文件，0：调用已有模板    1：新建模板
	int	aa_region_positive;			// (相对于A区平均亮度上浮值)A区高阈值
	int aa_region_negative;			// (相对于A区平均亮度下沉值)A区低阈值

	int grey_difference_positive;	//  (瑕疵检测可接受灰度范围)瑕疵高阈值
	int grey_difference_negative;	//(瑕疵检测可接受灰度范围)瑕疵低阈值
	int max_area;					//(单一瑕疵面积上限)缺陷面积上限
	int min_area;						//（最小检测面积）缺陷面积下限
	int num_for_stop_inspection;			//(瑕疵数目提取上限)缺陷采集数量
}Algorithm_Para_hjh;

typedef struct _Algorithm_Para_ITO
{
	int detector_length;			// (统计平均阈值的线条长度)		缺陷长度
	int detector_thresh;			//(灰度突变判定阈值)判定阈值	分割阈值
	int detector_top;				//（顶部边缘忽略宽度）			上边缘
	int detector_left;				//（左部边缘忽略宽度）			左边缘
	int detector_right;				//（右部边缘忽略宽度）			右边缘
	int detector_bottom;			//（下部边缘忽略宽度）			下边缘
	int	detector_limit;				//  缺陷数量					设置检测的最大缺陷数量
	int detector_iter;				//	滤波强度					设置滤波强度值
}Algorithm_Para_ITO;





class Server
{
public:
	Server();
	~Server();

public:
	bool CreateSocket();												//创建socket
	SOCKET AcceptSocket();												//接受请求连接的请求，并返回句柄
	void RecvAndSendSocket(SOCKET recvsocket);							//接受和发送数据给客服端

	void Start();														//启动运行程序

	int ResultProcess(map<int, vector<string>>  &DefectInfoData, int Aflag, string Pid);		//检测结果处理
	void SaveOKNG(string strQuery);
	void initInfo();													//连接相机接口
	void CreateFolder();												//创建文件夹路径
	
	string GetLocalIpAddress();											//获取本地IP地址
	void ConnectCamera();												//连接启动相机

public:
	SOCKET m_nServerSocket;												//绑定本地地址和端口号的套接口
	
	int							nStationNo;
	//bool						initCameraflag;
	CDatabase					m_CDatabase;
	Defect_Detection_Result		m_Defect_Detection_Result;				//检测参数
	//算法部分
	Algorithm_Para				m_Algorithm_Para;
	Algorithm_Para_hjh			m_Algorithm_Para_hjh;
	Algorithm_Para_ITO			m_Algorithm_Para_ITO;
};

