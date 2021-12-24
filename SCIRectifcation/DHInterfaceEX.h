#pragma once
#include "SCIRectifcation.in.h"
class DialogUdp;

#define   RECTIFCATION_COUNT 2
typedef struct tagTREADPARAM//初始化Rectficaion纠偏类
{
	int nRectifcationSN;		//纠偏器编号 从1开始
	int nSrcPort;
	int nDestPort;
	int nUse;
	CString strSrcIP;
	CString strDestIP;
	int nDelay;
	double nKp;
	double nActionThresh;
	double nOffset;
}READPARAM,*PREADPARAM;
class DHInterfaceEX:public DHInterface
{
public:
	DialogUdp* m_pDlgUdp;
	CPIDController* m_CPIDController;
	READPARAM m_readParam;
	DHInterfaceEX(void);
	~DHInterfaceEX(void);
private:
	RECTIFICATIONINIT m_InitParam;
public:
	virtual int DoSomething(void* message,void* wParam,void* lParam);
	BOOL Init(PRECTIFICATIONINIT pInit);
	BOOL Show(void);
	BOOL m_isInit;
	BOOL ReadParam(void);
	BOOL WriteParam(void);
	CString m_strRectifcationIni;
	BOOL SendMSG(double dValue,int * pSendOk);

	BOOL SetInit(PRECTIFICATIONINIT pInitParam);
	int m_nConnection;
};
CString GetCurrentAppPath();
BOOL CreateMultilevelPath(CString pstrFolder);
BOOL DeleteFolder(CString pstrFolder);
BOOL IsDirectory(CString pstrPath);
double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName);
CString GetPrivateProfileCString(CString lpAppName,LPCTSTR lpKeyName,CString lpDefault,LPCTSTR lpFileName);
void WritePrivateProfileInt(CString lpAppName,CString lpKeyName,int Value, CString lpFileName);
void WritePrivateProfileDouble(CString lpAppName,CString lpKeyName,double Value, CString lpFileName);