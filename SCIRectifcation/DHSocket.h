#pragma once
#include "DlgDHSocket.h"
#include "PController.h"
#include <vector>
#include "Language.h"
class CDlgDHSocket;
// CDHSocket ����Ŀ��

typedef struct tagTPARAM//��ʼ��Rectficaion��ƫ��
{
	CString strSrcIP;
	int nSrcPort;
	CString strOffsetDestIP;
	int nOffsetDestPort;

	CString strPositionDestIP;
	int nPositionDestPort;

	
	double dMin,dMax;//���ֵ����Сֵ
	int    nMode;//��ǰ��ƫģʽ

	double dOffsetMax;
	struct DIF
	{
		int nFrameSeg;//�����֡��
		int nUseFrame;//ʹ�õ�֡��
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

	///////////////////////////����DHInterFace///////////////////////////////////////////////
	virtual int DoSomething(void* message,void* wParam,void* lParam);
	BOOL Init(PRECTIFICATIONINIT pInit);
	BOOL DisplayTestDlg(void);
	
	void WriteDiff(double dValue,int bMainThread);//д���Ȳ���ֵ
	void WriteOffset(double dValue,int& bSuccess);//д��offsetֵ
	double ReadOffset(int bMainThread);
	void RecvOffset(double& dValue);//�յ�Offsetֵ����Ҫ����PID�㷨���Ļ�׼ֵ��

	void WritePositionL(double dLeftValue,int& bSuccess);//�������׼��ƫģʽʱ��ʹ��
	void WritePositionR(double dRightValue,int& bSuccess);//�����һ�׼��ƫģʽʹ��
	void WritePositionLR(double dLeftValue,double dRightValue,int& bSuccess);//���о�ƫʱʹ��

	void WriteCommand(int nCommand);//0�����ֶ� 1�����Զ� 2�������
	double ScaleOffset(double dDiff);
	
	void WriteMove(int nDirection);//�����ƶ��ķ���
	void ReadParam();
	void WriteParam();
	PARAM   m_tParam;
	RECTIFICATIONINIT m_InitParam;
	CPIDController m_PID;
	HTuple m_hvDiff;//�յ��ķ�������ֵ
	double m_dCurrentOffset;
	CEvent m_evtGetOffset;
	int   m_nJumpFrame;//�Ƿ��Ǹոշ��͹�
	CLanguage m_Lang;
private:

	CString m_strIniFile;
	BOOL    m_bConnect;
	int SendTo(const void* lpBuf, int nBufLen,UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);
private:
	BOOL KillTimeOut();
	BOOL SetTimeOut(int nTimeOut);
	int     m_nTimerID;//��ʱ���� 10000�ٹ���ô�þ�ֱ�ӷ��� ��ֹ����
public:
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual BOOL OnMessagePending();

};

extern CDHSocket* g_This;