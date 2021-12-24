//////////////////////////////////////////////////////////////////////////
///Global.h 全局类，所有需要全局存放的变量都可以放于此处
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "DHInterFace.h"
#include "BaseTmpl.h"
#include "Permission.h"
#include "Project.h"
#include "Counter.h"
#include "GrabberTool.h"
#include "AlgorithmTool.h"
#include "ProductTool.h"
#include "Process.h"
#include "Judge.h"
#include "Storage.h"
#include "Language.h"
#include "CommunicationTool.h"
#include "DataFilter.h"
#include "BatteriesVisionDlg.h"
#include "SubDispScope.h"
#include "SubDispList.h"
#include "Upload.h"
#include "FoolProofing.h"
#include "ErrorInfo.h"
#include "DataCalculate.h"
#include "DlgAlarm.h"
#include "DlgTip.h"
#include "Callback.h"


extern int      WM_DISP;		//显示结果消息
extern int      WM_GRABBER;		//显示帧计数
extern int      WM_DETECT;		//显示检测计数
extern int      WM_RESTARTPROCESS;	//重启
extern int		WM_REFRESH_DISP;	//刷新显示
extern int		WM_CHECK_PMSN;		//检查权限
extern int      WM_CHANGEBATCH;
extern int      WM_UPDATE_PRODUCT;
extern int      WM_UPDATE_ONLINEOFFSET;
const  int      nImageZoomWidth  = 1000;
const  int      nImageZoomHeight = 240;
const  double   dImageDilation = 0.2;
class CDlgLoading;
class CDlgAlarm;
class CGlobal : public CBaseTmpl
{
public:
	enum eHistoryType
	{
		NoHistory=0,	//队列不存
		ErrorHistory,	//队列只存错误历史
		AllHistory,		//队列保存所有历史
	};
	enum eDeviceType//设备类型
	{
		DT_SingleLayer = 0,		//单层涂布机
		DT_DoubleLayer,			//双层涂布机，五架式涂布机
	};
	enum eStationSN//工位
	{
		SN_Head=1,	//工位1，机头
		SN_2nd,		//工位2，五架式二涂，单层机尾
		SN_3rd,		//工位3，五架式机尾
		SN_4th,		//工位4
		SN_5th,		//工位5
	};
	struct tagSystemSet
	{
		eDeviceType		eDevType;				//设备类型，0单层，1双层
		eStationSN		eSN;					//1代表位于1涂  2代表2涂 3代表位于机尾

		CString			strWorkshopName;		//工厂区域编号
		CString			strDeviceID;			//设备编号

		int				nHistoryCount;			//保留的历史图片的个数
		eHistoryType	eHistoryType;			//仅保留错误的还是全部保留,还是不保留

		BOOL			bGrabAlign;				//相机采图对位
		int				nCameraDistance;		//前后相机距离

		int				nActionMode;			//系统检测模式

		double			dEnableOffsetSpeed;		//高于这个速度开启闭环控制

		BOOL			bUseChangeBatchAuto;	//是否使用自动换卷
		double			dChangeBatchDelayMeter;	//换卷延迟米数，延迟积累米数后才换卷
		BOOL			bCheckProductInfoByChangeBatch;//换卷时是否使用多工位配方校验
		
		inline void Init()
		{
			eDevType = DT_SingleLayer;
			eSN = SN_Head;

			nHistoryCount = 10;
			eHistoryType = NoHistory;
			bGrabAlign = FALSE;
			nCameraDistance = 1000;

			strWorkshopName = "";
			strDeviceID = "";

			nActionMode = RunMode;

			bUseChangeBatchAuto = FALSE;
			dChangeBatchDelayMeter = 0;
			bCheckProductInfoByChangeBatch = FALSE;
		}
	};
	struct tagResolution
	{
		double dRow;
		double dColumn;
	};
	typedef std::vector<tagResolution> ResolutionList;
public:
	CGlobal();
	~CGlobal();
public:
	CProject			m_Project;			//项目方案类，配置几个相机几面几个产品结果
	CGrabberTool		m_GrabberTool;		//采集库管理
	CCounter			m_Counter;			//统计计数
	CSpeed              m_Speed;            //计算设备速度
	CAlgorithmTool		m_AlgTool;			//算法管理
	CProductTool		m_ProductTool;		//品种管理
	CProcess			m_Process;			//检测处理
	CStorage			m_Storage;			//存储
	CJudge				m_Judge;			//判断检测类
	CLog				m_Log;				//日志
	CPermission			m_Permission;		//权限
	CLanguage			m_Language;			//语言类
	CCommunicationTool	m_CommunicationTool;//通信管理
	CDataFilter			m_DataFilter;		//数据滤波
	CDataCalculate		m_DataCalculate;	//数据计算
	CFoolProofing		m_FoolProofing;		//防呆模块
	CErrorInfo			m_ErrorInfo;		//错误信息
	CCallback			m_Callback;			//用于模块调用系统回调执行操作
	
	ResolutionList		m_vecResolution;	//图像比例尺

	CBatteriesVisionDlg* m_pMainDlg;		//主对话框指针
	CSubDispScope*		m_pSubDispScope;	//折线图子对话指针
	CSubDispList*		m_pSubDispList;		//显示列表子对话指针

	HObjectVector		m_hovecHistroyImage;

	tagSystemSet		m_tSystemSet;		//系统设置

	BOOL                m_bMSA;

	CUpload m_Upload;
	HTuple  m_hOffsetSocket;//发送offset值使用
	BOOL    m_bExitThread;
	CString m_strVersion;

	double m_dCurSpeed;
public:
	//开启崩溃捕获
	void CrashRecord();	
	//验证key
	void Verify();
	//显示验证库到期时间
	void ShowVerifyInfo();
	//初始化
	BOOL Init();
	BOOL InitAll();
	//读参数
	BOOL ReadParam();
	BOOL ReadParamAll();
	//写参数
	BOOL WriteParam();
	BOOL WriteParamAll();

	//操作加载对话框
	void InitLoadingDlg();
	void SetLoadingDlgInfo(const CString& strInfoCN,const CString& strInfoEN);
	void SetLoadingDlgInfo(const CString& strInfoCN,const CString& strInfoEN,double dPercent);
	void CloseLoadingDlg();
	//报警对话框
	void InitAlarmDlg();
	void SetAlarmInfo(const CString& strInfo);
	void SetAlarmInfo(const int& nErrorCode);
	void SetAlarmInfo(CErrorInfo::tagErrorInfo tInfo);
	void CloseAlarmDlg();
	//提示对话框
	void InitTipDlg();
	void SetTipInfo(eTipType eType);
	void CloseTipDlg();

	//获取系统设置参数
	tagSystemSet GetSystemSet() const { return m_tSystemSet; }

	//主题参数
	int  GetTheme(){return m_nTheme;}
	void SetTheme(int nTheme){m_nTheme=nTheme;}

	//卷号调用接口
	CString GetCurrentBatch() {return m_Counter.GetCurrentBatch().strName;}
	void	StartCurrentBatch(const CString& strBatch){ m_Counter.StartBatch(strBatch);}
	void	EndCurrentBatch() { m_Counter.EndBatch(); }

	//工号调用接口
	CString GetCurrentJobNum() {return m_Permission.GetUserInfo().strJobNumber;}

	//历史记录调用接口
	int	 GetHistoryCount()const {return m_tSystemSet.nHistoryCount;}
	void SetHistoryCount(const int& nCount){m_tSystemSet.nHistoryCount = nCount;}
	void SetHistoryType(eHistoryType eType){m_tSystemSet.eHistoryType = eType;}
	eHistoryType GetHistoryType()const {return m_tSystemSet.eHistoryType;}

	//获取相机比例尺
	const ResolutionList & GetResolutionList()const {return m_vecResolution;}
	//获取程序打开的时间
	const SYSTEMTIME & GetProgramOpenTime()const {return m_tmProgramOpen;}

	//给主界面发送消息
	BOOL PostMessageToMainDlg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL SendMessageToMainDlg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	//程序启动标志位调用接口
	BOOL IsStart(){return m_bStart;}
	void SetStart(const BOOL& bStart){m_bStart=bStart;}

	//获取软件版本
	const CString & GetVersion() { return m_strVersion; }

	double GetCPURate();
	double GetRAMUseMB();
private:
	
private:
	CString m_strHelpFile;			//帮助文件路径
	int		m_nTheme;				//BCG主题序号

	CString m_strCalibrationPath;	//标定参数文件夹路径
	CString m_strPositionParamFile;	//位置参数文件路径
	
	SYSTEMTIME m_tmProgramOpen;		//程序打开的时间
	BOOL	m_bStart;				//程序是否开始检测

	CDlgLoading* m_pDlgLoading;		//加载对话框
	CDlgAlarm* m_pDlgAlarmInfo;		//报警对话框
	CDlgTip* m_pDlgTip;				//提示对话框

	DHInterface* m_pCrash;
protected:
};

extern CGlobal theGlobal;