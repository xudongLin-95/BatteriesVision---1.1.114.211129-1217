#pragma once
#include "PIO24BLib.h"
#include "SCIIO.in.h"
#include "Log.in.h"
#include <vector>
#include <setupapi.h>
#include "rpcdce.h"
#include "BCGCBProInc.h"
#include "Language.h"
const int MAX_LANE = 2;
const int DEF_LASTLABLEPOS = -99999;//默认情况下的标签位置


typedef struct tagDeviceInfo
{
	//设备友好名称,很友好……
	CString szDeviceName;
	//设备类
	CString szDeviceClass;
	//设备显示名
	CString szDeviceDesc;
	//设备驱动
	CString szDriverName;
	//设备实例
	DWORD dwDevIns;
	//设备类标志
	GUID Guid;
	//按类名排序
	bool operator < (const tagDeviceInfo &tmp) const
	{
		if (tmp.szDeviceClass != szDeviceClass)
		{
			return tmp.szDeviceClass.CompareNoCase (szDeviceClass) > 0;
		}
		else
		{
			return tmp.szDeviceDesc.CompareNoCase (szDeviceDesc) > 0;
		}
	}
}DeviceInfo;
#define TB_CHANNEL 2
class CIOCommunicate : DHInterface
{
public:
	CIOCommunicate(void);
	virtual ~CIOCommunicate(void);
	virtual int DoSomething(void* message,void* wParam,void* lParam);
public:
	
	int m_nMarkerLength;//打标机标的长度（行）,0 相机0 ,1 相机1

	CString		m_strProductName;
	CString strProductName;

	CString				m_strConfigCommonFile;
	CString				m_strConfigSystemFile;
	CString				m_strConfigCommunicateFile;
	CString				m_strConfigCalibrationFile;
	COMMUICATEPARAM		m_Commuicate;//踢废相关参数

	ALARMS				m_Alarms;//报警相关参数
	COMMONPARAM			m_Common;//公共参数
	static CPIO24BLib   m_IOCard[PIO24_COUNT];//大恒图像IO卡
	int					m_nProductType;
	
    HWND				m_OutputhWnd;
	int					m_nCurrentStatus;//当前状态
	CLanguage			m_Lang;

	int					m_nCamRelation;//相机关系，0 相机0对应上带路(正机)，1 相机1对应上带路(反机)
	int                 m_nIn[2];
	BOOL                m_bIn[2];
	BOOL                m_bLXTB[TB_CHANNEL];//4个带路是否进行连续贴标测试
	BOOL m_bExit;//是否退出
private:
	void InitThread();
	void ReadParam();
	void WriteParam();
	BOOL SendMeasureIO(C_MergeWork& tOutput);
	BOOL SendLabelIO(const BOOL& bFlag, const int& nPictureSN, const int& nChannel, int& nBias);
	BOOL GetDeviceList(LPGUID lpGuid,BOOL bsFlag);
	BOOL SetDeviceStatus(DeviceInfo &theDevice,BOOL bStatusFlag);

	BOOL SetAlarmStatus(const e_AlarmStatus& nParam);//设置报警状态
	BOOL IO_heartBeat (BOOL useHeartBeat = true);
private:
	
	IOINIT m_tInit;

	HTuple m_hvQueue;//队列句柄

	std::vector<HDevThread*> m_vecThreadHandle;

	HTuple m_hvLabelPointRow[2];//贴标列坐标数组
	HTuple m_hvDecreaseFrames[2];//延迟帧数递减处理数组
	HTuple m_hvResultFlag[2];//结果标志位数组，随贴标延迟
	HTuple m_hvOriImageSN[2];//原始图像号，随贴标延迟
private:
	static void* HThreadIO(void *hcthread);//io线程
	void IO_thread(HTuple hvQueue);
	int Init(PIOINIT pInit);//初始化
	int PushMessage(const HTuple& hvMessageSend);//放置消息进入队列
	BOOL ReadyOutput(CString & strParam);//Ready信号
	BOOL Log(const CString& strLog,const int& nType,const int&nExcpLevel=NoExcp);
};

extern CIOCommunicate* g_This;