#include "StdAfx.h"
#include "Speed.h"

CSpeed::CSpeed()
{
	m_dSpeed = 0;
	CountSeconds(&m_hvTimeOld);
	CountSeconds(&m_hvTimeNew);
}

CSpeed::~CSpeed()
{

}

BOOL CSpeed::Init()
{
	ReadParam();

	return TRUE;
}

BOOL CSpeed::ReadParam()
{
	CString strFile;
	strFile.Format("%s\\Config\\Public\\Calibration\\%d\\CameraParma.ini", GetCurrentAppPath(), 0);
	m_dResultion = GetPrivateProfileDouble("CalibrationParam", "YResolution", 0.3, strFile);

	return TRUE;
}

BOOL CSpeed::WriteParam()
{
	return TRUE;
}