#pragma once
#include "DHInterFace.h"
#include "StdAfx.h"
//#include "DH_HalconEx.h"
#include "DH_HalconEx.h"
#include "Log.in.h"
#include "SCIMeasure.in.h"
#include "SCIDataFlow.in.h"
#include "HDevThread.h"
#include "CallBack.in.h"
#include "Marking.in.h"
#include <vector>
#include <map>
using std::map;
using std::vector;
//说明：图片裁切高度不同，每张图按照200mm截出，不足的也算一张；
class CMarkingCtr : DHInterface
{
public:
	typedef struct tagSOCKETSET
	{
		int     nUseCommu;       //是否使用与喷码机socket通信
		CString strIP;           //IP地址
		int     nPort;           //端口
		int     nImgDiff;        //图像号延迟帧数
		int     nUseCheckDefeat; //是否使用检测插值
		int     nUseCheckSize;   //是否使用检测尺寸
		int     nMarkingWidth;   //喷码器喷涂宽度(200的倍数)
		int     nMarkingHeight;  //喷码器喷涂高度(200的倍数),大于一则每200mm算一份
		int     nDivImageNum;    //每张图划分为几个区域，默认2个
	}TSOCKETSET, *PTSOCKETSET;

	typedef struct tagTMARKINGRES//初始化
	{
		int				nImgNum;			//当前图像序号
		CString			sResult;			//有瑕疵结果字符串，没有瑕疵时为空
		int				nImgSNTarget;		//当前有瑕疵时要在图像号多少的时候发送结果
		vector<CString> vSaveResult;
		double			dTotalMeter;		//当前记米
		double			dSpeed;				//当前速度
		double			dWholeWidth;		//总宽
	}MARKINGRES, *PMARKINGRES;

	typedef struct tagPartitionMsg
	{		
		HTuple  hv_PartEdgeCol;   //区域边缘
		HObject ho_PartAll;		//分区

		HTuple  hv_SigleStripeSN;//分条
	}PARTITIONMSG;
	
	typedef struct tagRegionMsg
	{
		HObject ho_NewtRegion; //重新划分后的瑕疵区域
		HTuple  hv_NewStrip;   //重新划分后的瑕疵区域，对应分条
		HTuple  hv_NewType;    //重新划分后的瑕疵区域，对应类型
		HTuple  hv_NewSide;   //重新划分后的瑕疵区域，对应面
		
	}NEWREGIONMSG;

	struct tagDefect
	{
		//vector<int> nStripeSN; //分条序号(分区序号)
		vector<int> nType;      //瑕疵类型
		vector<double> nRow1;   //瑕疵区域起点列坐标
		vector<double> nCol1;   //瑕疵区域起点行坐标
		vector<double> nRow2;   //瑕疵区域终点列坐标
		vector<double> nCol2;   //瑕疵区域终点行坐标
	};	

	struct tagResolution
	{
		double dRow;
		double dColumn;
	};
	vector<tagResolution> m_vResolutionList;

	struct tagCameraBaseMsg
	{
		int    nCameraCount;     //相机个数
		double dHResolutionSideA;//A面相机水平分辨率
		double dHResolutionSideB;//B面相机水平分辨率
		double dVResolutionSideA;//A面相机垂直分辨率
		double dVResolutionSideB;//B面相机垂直分辨率
		HTuple hv_ColumnOverA;   //A面重叠部分信息
		HTuple hv_ColumnOverB;   //B面重叠部分信息
	};
	tagCameraBaseMsg t_CameraBaseMsg;
public:
	CMarkingCtr(void);
	virtual ~CMarkingCtr(void);
	virtual int DoSomething(void* message, void* wParam, void* lParam);

	void ReadParam();
	void WriteParam();

	//socket sec
	BOOL SocketConnect(const CString& strIP, const int& nPort);
	BOOL SocketDisconnect();
	BOOL SocketReconnect();
	BOOL GetIsConnected(int& res);
	
	BOOL SendResultToMarking(C_MergeWork& Result);
	BOOL ReceiveStringResult(CString& sResult);
	BOOL SendStrToMarking(CString Result, int sendSN=0,int newSN=0);

	//水平获取分区
	BOOL GetPartitionInHorizontalDirection(C_MergeWork& Result, vector<PARTITIONMSG>& vPartitionMsg);
	//获取瑕疵在新划分的八分区上
	BOOL GetDefectInNewRegion(C_MergeWork& Result,int SideNum, vector<PARTITIONMSG>& vPartitionMsg, vector<tagDefect>& mtagDefacts);
	//获取尺寸NG项在把分区上
	BOOL GetDefectSize(C_MergeWork& Result, int SideNum, vector<PARTITIONMSG>& vPartitionMsg,vector<tagDefect>& mtagDefacts);

	BOOL SetCurrentBatch(CString strBatch);
public:
	CString m_strCurrentBatch;
	HTuple m_hv_Socket;
	TSOCKETSET m_tSocketSet;
	bool m_bIsConnected;
	CBCGPDialog* m_pDlg;//主界面指针
	std::vector<MARKINGRES> m_vMarkingRes;
	HObject m_ho_RegionSegment;//用来存储图像垂直方向划分后的区域
		
private:
	MARKINGINIT m_sInit;   //测试先声明为public,后面改为私有
	HTuple m_hvDefectToMarkigQueue;//队列句柄   瑕疵结果队列，发送给喷码机
	CString m_strConfigMarkingFile;

	HTupleVector m_hvec_Data;
	static void* HThreadMarking(void *hcthread);
	void marking_thread();
	BOOL SaveDataMarking(int nImgNum, double dTM, double dSpeed, vector<CString> saveResults);

	//static CCriticalSection m_csLock;
	CRITICAL_SECTION m_csLock;
	BOOL CreateMarkingFile(const CString& strFilePath, int nPartNum);
	BOOL GetCameraSetting(vector<tagResolution>& vResolutionList, tagCameraBaseMsg& cameraBaseMsg);
	BOOL GetCameraResolution(vector<tagResolution>& ResolutionList, tagCameraBaseMsg& tCameraBaseMsg);
private:
	int Init(MARKINGINIT &Init);
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
	int PushMessage(const HTuple& hvMessageSend);//放置消息进入队列
	BOOL ShowParamDlg();
	int m_nNGCount;
	CString strFileName;

	int m_nIOCount;
	
	vector<int> vACameraSnList; //A面相机SN列表
	vector<int> vBCameraSnList; //B面相机SN列表

	map<CString, CString> NgDescribe;//喷码器的错误代码和对应描述
	void initNgDescribe() { //初始化
		NgDescribe.insert(map<CString, CString>::value_type("a", "漏金属"));
		NgDescribe.insert(map<CString, CString>::value_type("b", "接带"));
		NgDescribe.insert(map<CString, CString>::value_type("c", "脱碳"));
		NgDescribe.insert(map<CString, CString>::value_type("d", "汽包"));
		NgDescribe.insert(map<CString, CString>::value_type("e", "划痕"));
		NgDescribe.insert(map<CString, CString>::value_type("f", "条痕(打皱)"));
		NgDescribe.insert(map<CString, CString>::value_type("g", "气泡"));
		NgDescribe.insert(map<CString, CString>::value_type("h", "极片黑点"));
		NgDescribe.insert(map<CString, CString>::value_type("i", "亮点"));
		NgDescribe.insert(map<CString, CString>::value_type("j", "极片尺寸不良"));
	};
};

extern CMarkingCtr* g_This;