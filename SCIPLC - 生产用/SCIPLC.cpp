// SCIPLC.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "SCIPLC.h"
#include "PLC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CSCIPLCApp

BEGIN_MESSAGE_MAP(CSCIPLCApp, CWinApp)
END_MESSAGE_MAP()


// CSCIPLCApp ����

CSCIPLCApp::CSCIPLCApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSCIPLCApp ����

CSCIPLCApp theApp;


// CSCIPLCApp ��ʼ��

BOOL CSCIPLCApp::InitInstance()
{
	CWinApp::InitInstance();
	try
	{
		SetHcppInterfaceStringEncodingIsUtf8(FALSE);
	}
	catch (const HException&){}
	return TRUE;
}


int CSCIPLCApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	if (g_This!=NULL)
	{
		delete g_This;
		g_This = NULL;
	}
	BCGCBProCleanUp();
	return CWinApp::ExitInstance();
}
