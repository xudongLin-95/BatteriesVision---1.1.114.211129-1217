// DlgCrashInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CrashRecord.h"
#include "DlgCrashInfo.h"
#include "afxdialogex.h"
#include "Crash.h"


// CDlgCrashInfo �Ի���

IMPLEMENT_DYNAMIC(CDlgCrashInfo, CDialogEx)

CDlgCrashInfo::CDlgCrashInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCrashInfo::IDD, pParent)
{

}

CDlgCrashInfo::~CDlgCrashInfo()
{
}

void CDlgCrashInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_LST_Info);
}


BEGIN_MESSAGE_MAP(CDlgCrashInfo, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgCrashInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgCrashInfo::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgCrashInfo ��Ϣ�������


BOOL CDlgCrashInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

CString CDlgCrashInfo::GetExceptionInfo(DWORD dwCode)
{
	switch(dwCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.�߳�Υ���дû���ʵ�Ȩ�޵������ַ��";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.�߳��ڵײ�Ӳ��֧�ֵı߽����·�������Խ�硣";
	case EXCEPTION_BREAKPOINT:
		return "A breakpoint was encountered.����һ���ϵ㡣";
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment.�߳���ͼ�ڲ�֧�ֶ����Ӳ���϶�дδ��������ݡ�";
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "One of the operands in a floating-point operation is denormal.���������һ�������������档 A denormal value is one that is too small to represent as a standard floating-point value.�������ֵ��ʾ̫С�����ܱ�ʾһ����׼�ĸ���ֵ��";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "The thread tried to divide a floating-point value by a floating-point divisor of zero.�̸߳�����������";
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.�������Ĳ�������׼ȷ�Ĵ���С����";
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "This exception represents any floating-point exception not included in this list.����쳣����������б���û���г������������쳣��";
	case EXCEPTION_FLT_OVERFLOW:
		return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.���������ָ����������Ӧ���͵����ֵ��";
	case EXCEPTION_FLT_STACK_CHECK:
		return "The stack overflowed or underflowed as the result of a floating-point operation.���������ջԽ����������";
	case EXCEPTION_FLT_UNDERFLOW:
		return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.���������ָ��û�г�����Ӧ���͵����ֵ��";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "The thread tried to execute an invalid instruction.�߳���ͼִ����Чָ�";
	case EXCEPTION_IN_PAGE_ERROR:
		return "The thread tried to access a page that was not present, and the system was unable to load the page.�߳���ͼ����һ�������ڵ�ҳ�����޷����ص�ҳ�� For example, this exception might occur if a network connection is lost while running a program over the network.���磬�쳣���������е��������ʧ���ӵ�ʱ������";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "The thread tried to divide an integer value by an integer divisor of zero.�߳���ͼ�������㡣";
	case EXCEPTION_INT_OVERFLOW:
		return "The result of an integer operation caused a carry out of the most significant bit of the result.���������Ľ��ռ���˽����������λ��";
	case EXCEPTION_INVALID_DISPOSITION:
		return "An exception handler returned an invalid disposition to the exception dispatcher.�쳣���������쳣������������һ����Ч���á� Programmers using a high-level language such as C should never encounter this exception.ʹ�ø߼����ԣ�����C���ԣ��ĳ�����Զ������������쳣��";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return "The thread tried to continue execution after a noncontinuable exception occurred.�߳���ͼ��һ�����ɼ���ִ�е��쳣���������ִ�С�";
	case EXCEPTION_PRIV_INSTRUCTION:
		return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.�߳���ͼִ�е�ǰ����ģʽ��֧�ֵ�ָ�";
	case EXCEPTION_SINGLE_STEP:
		return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.���򵥵�˵�ǣ�������";
	case  EXCEPTION_STACK_OVERFLOW:
		return "The thread used up its stack.���򵥵�˵�ǣ�ջ�����";

	default:
		return "δ֪����";
	}
}

void CDlgCrashInfo::InitUI()
{
	CString strInfo;
	strInfo.Format("������:0x%llx",g_CrashRecord->m_ExcpInfo.ExceptionPointers->ExceptionRecord->ExceptionCode);
	m_LST_Info.AddString(strInfo);
	strInfo.Format("�����ַ:0x%llx",g_CrashRecord->m_ExcpInfo.ExceptionPointers->ExceptionRecord->ExceptionAddress);
	m_LST_Info.AddString(strInfo);
	strInfo.Format("������Ϣ:%s",GetExceptionInfo(g_CrashRecord->m_ExcpInfo.ExceptionPointers->ExceptionRecord->ExceptionCode));
	m_LST_Info.AddString(strInfo);
	
}

void CDlgCrashInfo::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_CrashRecord->RestartProcess();
}


void CDlgCrashInfo::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_CrashRecord->m_bExit = TRUE;
	CDialogEx::OnCancel();
	exit(0);
	g_CrashRecord->Release();
}
