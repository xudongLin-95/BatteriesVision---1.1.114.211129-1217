// SCISeparate.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "SCISeparate.h"
#include "Measure.h"

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

// CSCISeparateApp

BEGIN_MESSAGE_MAP(CSCISeparateApp, CWinApp)
END_MESSAGE_MAP()


// CSCISeparateApp ����

CSCISeparateApp::CSCISeparateApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSCISeparateApp ����

CSCISeparateApp theApp;


// CSCISeparateApp ��ʼ��
void CPPExpDefaultExceptionHandler(const HException& except)
{
	throw except;
}
BOOL CSCISeparateApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}


int CSCISeparateApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���

	return CWinApp::ExitInstance();
}
