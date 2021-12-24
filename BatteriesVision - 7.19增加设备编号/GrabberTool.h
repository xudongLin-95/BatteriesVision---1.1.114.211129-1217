//////////////////////////////////////////////////////////////////////////
///GrabberTool.h 用于采集库管理
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CGrabberTool : public CBaseTmpl
{
public:
	enum emGrabberType
	{
		Simulate=0,		//模拟采集
		DalsaLine,		//dalsa线阵
		AVT,			//AVT相机
		Mer,			//Mer水星相机
	};
	enum emImageCardType
	{
		SimulateCard=0,	//模拟采集卡
		Xcelera,		//Xcelera采集卡
		Xtium,			//Xtium采集卡
		Ethernet,		//有线网卡
		USB,			//USB口
		Card1394,		//1394采集卡
	};
	struct tagGrabber
	{
		DHInterface* pGrabber;
		emGrabberType eGrabberType;
		emImageCardType eCardType;
	};
	typedef std::vector<tagGrabber> GrabberList;
public:
	CGrabberTool();
	~CGrabberTool();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//开始采集
	BOOL StartGrab();
	//停止采集
	BOOL StopGrab();
	//停止模拟采集
	BOOL StopSimGrab();//停止模拟采集，不影响实时采集
	//单帧采集
	BOOL Snapshot();
	//关闭采集
	BOOL CloseGrab();

	int GetGrabberCount(){return m_nGrabberCount;}

	int	DoSomething(const int& nCamSN,void *message,void *wParam,void *lParam);

	int ShowParamDlg(const int& nCamSN,void* wParam=NULL);

	int GetGrabberType(const int& nCamSN){
		if(nCamSN<0 || nCamSN>=m_vecGrabber.size())
			return 0;
		return (int)m_vecGrabber[nCamSN].eGrabberType;
	}

private:
	int m_nGrabberCount;
	GrabberList m_vecGrabber;
	
};