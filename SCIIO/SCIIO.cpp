// SCICommunicate.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "SCIIO.h"
#include "IOCommunicate.h"

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

// CSCIIOApp

BEGIN_MESSAGE_MAP(CSCIIOApp, CWinApp)
END_MESSAGE_MAP()


// CSCIIOApp ����

CSCIIOApp::CSCIIOApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSCIIOApp ����

CSCIIOApp theApp;


// CSCIIOApp ��ʼ��

BOOL CSCIIOApp::InitInstance()
{
	CWinApp::InitInstance();
	try
	{
		SetHcppInterfaceStringEncodingIsUtf8(FALSE);
	}
	catch (const HException&){}
	return TRUE;
}

int CSCIIOApp::ExitInstance()
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
