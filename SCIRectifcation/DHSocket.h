#pragma once
#include "DlgDHSocket.h"
#include "PController.h"
#include <vector>
#include "Language.h"
class CDlgDHSocket;
// CDHSocket 命令目标

typedef struct tagTPARAM//初始化Rectficaion纠偏类
{
	CString strSrcIP;
	int nSrcPort;
	CString strOffsetDestIP;
	int nOffsetDestPort;

	CString strPositionDestIP;
	int nPositionDestPort;

	
	double dMin,dMax;//最大值。最小值
	int    nMode;//当前纠偏模式

	double dOffsetMax;
	struct DIF
	{
		int nFrameSeg;//差异的帧数
		int nUseFrame;//使用的帧数
		BOOL bEnable;
	};
	DIF tDif;
	
// 	struct PID
// 	{
// 		double dKp;
// 		double dActionThresh;
// 		int    nDelay;
// 		BOOL   bEnable;
// 	};
// 	PID tPID;
}PARAM,*PPARAM;

class CDHSocket : public CSocket,public DHInterface
{
public:
	CDHSocket();
	virtual ~CDHSocket();
public:
	CDlgDHSocket* m_pMainDlg;
	virtual void OnReceive(int nErrorCode);

	///////////////////////////重载DHInterFace///////////////////////////////////////////////
	virtual int DoSomething(void* message,void* wParam,void* lParam);
	BOOL Init(PRECTIFICATIONINIT pInit);
	BOOL DisplayTestDlg(void);
	
	void WriteDiff(double dValue,int bMainThread);//写入宽度差异值
	void WriteOffset(double dValue,int& bSuccess);//写入offset值
	double ReadOffset(int bMainThread);
	void RecvOffset(double& dValue);//收到Offset值，需要依据PID算法更改基准值了

	void WritePositionL(double dLeftValue,int& bSuccess);//单侧左基准纠偏模式时候使用
	void WritePositionR(double dRightValue,int& bSuccess);//单侧右基准纠偏模式使用
	void WritePositionLR(double dLeftValue,double dRightValue,int& bSuccess);//居中纠偏时使用

	void WriteCommand(int nCommand);//0代表手动 1代表自动 2代表回中
	double ScaleOffset(double dDiff);
	
	void WriteMove(int nDirection);//设置移动的方向
	void ReadParam();
	void WriteParam();
	PARAM   m_tParam;
	RECTIFICATIONINIT m_InitParam;
	CPIDController m_PID;
	HTuple m_hvDiff;//收到的分条差异值
	double m_dCurrentOffset;
	CEvent m_evtGetOffset;
	int   m_nJumpFrame;//是否是刚刚发送过
	CLanguage m_Lang;
private:

	CString m_strIniFile;
	BOOL    m_bConnect;
	int SendTo(const void* lpBuf, int nBufLen,UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
private:
	BOOL KillTimeOut();
	BOOL SetTimeOut(int nTimeOut);
	int     m_nTimerID;//超时设置 10000操过这么久就直接返回 防止阻塞
public:
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual BOOL OnMessagePending();

};

extern CDHSocket* g_This;