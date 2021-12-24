#include "StdAfx.h"
#include "LineToolPara.h"


CLineToolPara::CLineToolPara(void)
{
	m_nDirect=0;
	m_nSelect=0;
	m_nThr=20;
	m_dSigma=1.0;
	m_dWidth=20;
	m_dHeight=80;
	m_nMeasureNum=20;
	m_dStartRow=100;
	m_dStartCol=100;
	m_dEndRow=300;
	m_dEndCol=300;
	m_nChoice=0;
}


CLineToolPara::~CLineToolPara(void)
{
}



CLineToolPara CLineToolPara::AffineTrans(const HTuple& HomMat2d)
{
	CLineToolPara tNewLine;
	tNewLine.m_dSigma      = m_dSigma;
	tNewLine.m_nThr        = m_nThr;
	tNewLine.m_nDirect     = m_nDirect;
	tNewLine.m_nSelect     = m_nSelect;
	tNewLine.m_dWidth      = m_dWidth;
	tNewLine.m_dHeight     = m_dHeight;
	tNewLine.m_nMeasureNum = m_nMeasureNum;
	tNewLine.m_nChoice     = m_nChoice;
	HTuple RowTrans,ColTrans;
	AffineTransPixel(HomMat2d,m_dStartRow,m_dStartCol,&RowTrans,&ColTrans);
	tNewLine.m_dStartRow = RowTrans.D();
	tNewLine.m_dStartCol = ColTrans.D();
	AffineTransPixel(HomMat2d,m_dEndRow,m_dEndCol,&RowTrans,&ColTrans);
	tNewLine.m_dEndRow = RowTrans.D();
	tNewLine.m_dEndCol = ColTrans.D();
	return tNewLine;
}
int CLineToolPara::ReadParam(CString strName)
{
	try
	{
		char tmpchar[MAX_PATH];

		GetPrivateProfileString("param","m_nDirect","1",tmpchar,MAX_PATH,strName);
		m_nDirect=atoi(tmpchar);
		GetPrivateProfileString("param","m_nSelect","1",tmpchar,MAX_PATH,strName);
		m_nSelect=atoi(tmpchar);
		GetPrivateProfileString("param","m_nThr","20",tmpchar,MAX_PATH,strName);
		m_nThr=atoi(tmpchar);
		GetPrivateProfileString("param","m_dSigma","1.0",tmpchar,MAX_PATH,strName);
		m_dSigma=atof(tmpchar);

		GetPrivateProfileString("param","m_dWidth","20.0",tmpchar,MAX_PATH,strName);
		m_dWidth=atof(tmpchar);
		GetPrivateProfileString("param","m_dHeight","200.0",tmpchar,MAX_PATH,strName);
		m_dHeight=atof(tmpchar);

		GetPrivateProfileString("param","m_nMeasureNum","15",tmpchar,MAX_PATH,strName);
		m_nMeasureNum=atoi(tmpchar);
		GetPrivateProfileString("param","m_dStartRow","100.0",tmpchar,MAX_PATH,strName);
		m_dStartRow=atof(tmpchar);
		GetPrivateProfileString("param","m_dStartCol","100.0",tmpchar,MAX_PATH,strName);
		m_dStartCol=atof(tmpchar);
		GetPrivateProfileString("param","m_dEndRow","300.0",tmpchar,MAX_PATH,strName);
		m_dEndRow=atof(tmpchar);
		GetPrivateProfileString("param","m_dEndCol","300.0",tmpchar,MAX_PATH,strName);
		m_dEndCol=atof(tmpchar);
		GetPrivateProfileString("param","m_nChoice","0",tmpchar,MAX_PATH,strName);
		m_nChoice=atoi(tmpchar);
		return 0;
	}
	catch (...)
	{
		return 1;
	}
}
int CLineToolPara::WriteParam(CString strName)
{
	try
	{
		CString tmpstr;

		tmpstr.Format("%d",m_nDirect);
		WritePrivateProfileString("param","m_nDirect",tmpstr,strName);
		tmpstr.Format("%d",m_nSelect);
		WritePrivateProfileString("param","m_nSelect",tmpstr,strName);
		tmpstr.Format("%d",m_nThr);
		WritePrivateProfileString("param","m_nThr",tmpstr,strName);
		tmpstr.Format("%f",m_dSigma);
		WritePrivateProfileString("param","m_dSigma",tmpstr,strName);

		tmpstr.Format("%f",m_dWidth);
		WritePrivateProfileString("param","m_dWidth",tmpstr,strName);
		tmpstr.Format("%f",m_dHeight);
		WritePrivateProfileString("param","m_dHeight",tmpstr,strName);

		tmpstr.Format("%d",m_nMeasureNum);
		WritePrivateProfileString("param","m_nMeasureNum",tmpstr,strName);
		tmpstr.Format("%f",m_dStartRow);
		WritePrivateProfileString("param","m_dStartRow",tmpstr,strName);
		tmpstr.Format("%f",m_dStartCol);
		WritePrivateProfileString("param","m_dStartCol",tmpstr,strName);
		tmpstr.Format("%f",m_dEndRow);
		WritePrivateProfileString("param","m_dEndRow",tmpstr,strName);
		tmpstr.Format("%f",m_dEndCol);
		WritePrivateProfileString("param","m_dEndCol",tmpstr,strName);
		tmpstr.Format("%d",m_nChoice);
		WritePrivateProfileString("param","m_nChoice",tmpstr,strName);
		return 0;
	}
	catch (...)
	{
		return 1;
	}	
}
