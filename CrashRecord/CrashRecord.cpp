// CrashRecord.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "CrashRecord.h"
#include "Crash.h"

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

// CCrashRecordApp

BEGIN_MESSAGE_MAP(CCrashRecordApp, CWinApp)
END_MESSAGE_MAP()


// CCrashRecordApp ����

CCrashRecordApp::CCrashRecordApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CCrashRecordApp ����

CCrashRecordApp theApp;


// CCrashRecordApp ��ʼ��

BOOL CCrashRecordApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


int CCrashRecordApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	if (g_CrashRecord!=NULL)
	{
		delete g_CrashRecord;
		g_CrashRecord = NULL;
	}
	return CWinApp::ExitInstance();
}
