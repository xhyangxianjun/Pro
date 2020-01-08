
#pragma once
#include <string>
#include <vector>

using namespace std;

typedef struct _StationNoInfo
{
	string	strToolPara;		 //工具参数
	string	strDefectDefine;	 //缺陷数据
	string  strDefectScreensing; //缺陷筛选
	string	strFeatures;		 //语言定义
	string  strProName;			 //方案名
	string  strBatchName;		 //批次名
	int		DiskSpaceAlarm;		 //磁盘小于报警
	int		DiskSpaceDelete;	 //磁盘小于多少删除	
	int		SaveOKImg;			 //保存OK原图片,1-保存，0-不保存
	int		SaveNGImg;			 //保存NG原图片,1-保存，0-不保存
	int		SaveCompressedOKImg; //保存压缩OK原图片,1-保存，0-不保存
	int		SaveCompressedNGImg; //保存压缩NG原图片,1-保存，0-不保存
	int		SamplingCompressedImg;//保存抽样压缩图，1-抽样压缩，0-不抽样压缩
	int		StationDynamic;		 //工位图来源，1-动态，0-静态
	int		AlgorithmOnOrOff;	 //算法开关
	int		Syslangage;			 //系统语言  0--英文  1--中文
	int		SysOnLine;			 //软件在离线 0--在线  1--离线
	string  strInputImgPath;	 //导入图片路径
	string  strOutputImgPath;	 //导出图片路径
}StationNoInfo;

//typedef struct _featuresInfo
//{
//	string CoorDinate;      //缺陷坐标
//	string DefectArea1;		//    面积1
//	string DefectArea2;		//    面积2
//	string DefectWidth;		//    宽度
//	string DefectHight;		//    高度
//	string DefectGray;		//    灰度
//	string DefectGrayDiff;	//    灰度差
//	string DefectRundness;	//    圆度
//	string CirferenceRatio;	//	  圆周比率
//
//}featuresInfo;


class CDatabase
{
public:
	CDatabase();
	~CDatabase();

public:
	//初始化资源
	void InitRes();
	//释放资源
	void Release();
	//打开连接数据库
	bool ConnectDatabase();
	//关闭数据库
	void CloseDatabase();
	//数据表是否存在
	bool IsTableExist(const char* pszTableName);
	//读取工位信息
	void ReadWorkerParam(const char* pQuery);
	//保存工位信息
	bool SaveData(const char* pQuery);
	//读取是否保存原图标志
	int filtrate(const char* pQuery);
	//查询数据库特性数据
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

