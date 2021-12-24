#include "StdAfx.h"
#include "GrabberTool.h"
#include "SCIGrabber.in.h"

CGrabberTool::CGrabberTool()
{
	m_nGrabberCount = 0;
	m_vecGrabber.clear();
}

CGrabberTool::~CGrabberTool()
{
}

BOOL CGrabberTool::Init()
{
	ReadParam();
	m_vecGrabber.clear();
	CString strSection;
	for (int i=0;i<m_nGrabberCount;i++)
	{
		strSection.Format("Grabber%d",i);
		int nGrabberType = GetIniInt(strSection,"Type",0);
		emImageCardType eCardType=Ethernet;
		CString strDeviceMark,strDeviceFile,strDeviceName;
		if (0==nGrabberType)
		{			
			strDeviceFile = GetIniString(strSection,"SimulateInitFile","(local)");
			strDeviceMark = GetIniString(strSection, "SimulateCamSN", "(local)");
			eCardType = SimulateCard;
		}else if (1==nGrabberType)
		{
			strDeviceFile = GetIniString(strSection,"DalsaInitFile","(local)");
			strDeviceMark = GetIniString(strSection,"DalsaCamSN","(local)");
			if (-1!=strDeviceMark.Find("Xcelera-CL_PX4"))
				eCardType = Xcelera;
			else
				eCardType = Xtium;
		}
		else if (2==nGrabberType)
		{
			strDeviceFile = GetIniString(strSection,"AVTInitFile","(local)");
			strDeviceMark = GetIniString(strSection,"AVTCamSN","(local)");
			eCardType = Ethernet;
		}
		else if (3==nGrabberType)
		{
			strDeviceFile = GetIniString(strSection,"MerInitFile","(local)");
			strDeviceMark = GetIniString(strSection,"MerCamSN","(local)");
			eCardType = Ethernet;
		}
		int nGrabberSN   = i;
		CString strDeviceNameDef;
		strDeviceNameDef.Format("Camera%d",nGrabberSN+1);//默认值
		strDeviceName = GetIniString(strSection,"Name",strDeviceNameDef);

		s_GBINITSTRUCT InitParam;
		InitParam.nGrabberSN = nGrabberSN;
		InitParam.CallBackFunc = theGlobal.m_Process.AcquisitionCallBack;
		InitParam.Context = this;
		InitParam.iReserve1 = GetIniInt(strSection, "CameraMode", CL_Full);
		CString strCameraLinkSN;
		strCameraLinkSN.Format("%c", strDeviceMark.GetAt(strDeviceMark.GetLength() - 1));
		int nCameraLinkSN = atoi(strCameraLinkSN);
		InitParam.iReserve2 = GetIniInt(strSection, "CameraLinkSN", nCameraLinkSN);
		InitParam.iGrabberTypeSN = nGrabberType;
		sprintf_s(InitParam.strDeviceMark, "%s",strDeviceMark);//序列号
		sprintf_s(InitParam.strDeviceName, "%s",strDeviceName);
		sprintf_s(InitParam.strGrabberFile,"%s\\Config\\Public\\SCIGrabber\\%s",GetCurrentAppPath(),strDeviceFile);//设备初始化文件名
		//相机类型:0为模拟采集卡，1 dalsa 线阵2 为AVT相机， 3  MER
		DHInterface* pGrabber = NULL;
		switch (InitParam.iGrabberTypeSN)
		{
		case 0:
			pGrabber = LoadDHInterFaceDll("SCIGrabberSG");
			break;
		case 1:
			pGrabber = LoadDHInterFaceDll("SCIGrabberDS");
			break;
		case 2:
			pGrabber = LoadDHInterFaceDll("SCIGrabberAVT");
			break;
		case 3:
			pGrabber = LoadDHInterFaceDll("SCIGrabberMER");
			break;
		}
		tagGrabber gbr;
		gbr.eGrabberType = emGrabberType(nGrabberType);
		gbr.eCardType	= eCardType;
		if (pGrabber!=NULL 
			&&TRUE == pGrabber->DoSomething("INITIALIZE",&InitParam,NULL))
		{
			gbr.pGrabber = pGrabber;
			m_vecGrabber.push_back(gbr);
		}else
		{
			CString strMessageBox;
			strMessageBox.Format("Fail to init camera %2d, program exits.\r\n相机%d初始化失败，程序将终止",i,i);
			AfxMessageBox(strMessageBox);
			exit(1);
		}
	}


	return TRUE;
}

BOOL CGrabberTool::ReadParam()
{
	m_nGrabberCount = GetIniInt("Project", "GrabberCount", 1);
	return TRUE;
}

BOOL CGrabberTool::WriteParam()
{
	return TRUE;
}

BOOL CGrabberTool::StartGrab()
{
	for (int i=0;i<m_vecGrabber.size();i++)
	{
		if (NULL==m_vecGrabber[i].pGrabber)
			continue;
		m_vecGrabber[i].pGrabber->DoSomething("STARTGRAB",NULL,NULL);
	}
	return TRUE;
}

BOOL CGrabberTool::StopGrab()
{
	for (int i=0;i<m_vecGrabber.size();i++)
	{
		if (NULL==m_vecGrabber[i].pGrabber)
			continue;
		m_vecGrabber[i].pGrabber->DoSomething("STOPGRAB",NULL,NULL);
	}
	return TRUE;
}

BOOL CGrabberTool::StopSimGrab()
{
	for (int i=0;i<m_vecGrabber.size();i++)
	{
		if (NULL==m_vecGrabber[i].pGrabber || Simulate!=m_vecGrabber[i].eGrabberType)
			continue;
		m_vecGrabber[i].pGrabber->DoSomething("STOPGRAB",NULL,NULL);
	}
	return TRUE;
}

BOOL CGrabberTool::Snapshot()
{
	for (int i=0;i<m_vecGrabber.size();i++)
	{
		if (NULL==m_vecGrabber[i].pGrabber)
			continue;
		m_vecGrabber[i].pGrabber->DoSomething("SNAPSHOT",NULL,NULL);
	}
	return TRUE;
}

BOOL CGrabberTool::CloseGrab()
{
	for (int i=0;i<m_vecGrabber.size();i++)
	{
		m_vecGrabber[i].pGrabber->DoSomething("CLOSE",NULL,NULL);
	}
	return TRUE;
}

int	CGrabberTool::DoSomething(const int& nCamSN,void *message,void *wParam,void *lParam)
{
	if (nCamSN >= m_vecGrabber.size() || NULL==m_vecGrabber[nCamSN].pGrabber)
		return -1;

	return m_vecGrabber[nCamSN].pGrabber->DoSomething(message,wParam,lParam);
}

int CGrabberTool::ShowParamDlg(const int& nCamSN,void* wParam)
{
	if (nCamSN >= m_vecGrabber.size() || NULL==m_vecGrabber[nCamSN].pGrabber)
		return -1;

	return m_vecGrabber[nCamSN].pGrabber->DoSomething("SHOW",wParam,NULL);
}