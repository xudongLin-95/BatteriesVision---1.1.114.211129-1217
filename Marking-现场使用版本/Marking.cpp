// Marking.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Marking.h"
#include "MarkingCtr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
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
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CMarkingApp

BEGIN_MESSAGE_MAP(CMarkingApp, CWinApp)
END_MESSAGE_MAP()


// CMarkingApp ����

CMarkingApp::CMarkingApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMarkingApp ����

CMarkingApp theApp;


// CMarkingApp ��ʼ��

BOOL CMarkingApp::InitInstance()
{
	CWinApp::InitInstance();
	try
	{
		SetHcppInterfaceStringEncodingIsUtf8(FALSE);
	}
	catch (const HException&) {}

	return TRUE;
}

int CMarkingApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	if (g_This != NULL)
	{
		delete g_This;
		g_This = NULL;
	}
	BCGCBProCleanUp();
	return CWinApp::ExitInstance();
}
