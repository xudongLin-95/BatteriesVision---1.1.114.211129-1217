// SCIRectifcation.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "SCIRectifcation.h"
#include "BCGCBProInc.h"
#include "DHSocket.h"

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

// CSCIRectifcationApp

BEGIN_MESSAGE_MAP(CSCIRectifcationApp, CWinApp)
END_MESSAGE_MAP()


// CSCIRectifcationApp ����

CSCIRectifcationApp::CSCIRectifcationApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSCIRectifcationApp ����

CSCIRectifcationApp theApp;


// CSCIRectifcationApp ��ʼ��

BOOL CSCIRectifcationApp::InitInstance()
{
	CWinApp::InitInstance();
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}

int CSCIRectifcationApp::ExitInstance()
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

