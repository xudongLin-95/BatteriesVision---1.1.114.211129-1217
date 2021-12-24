// DlgCrashInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrashRecord.h"
#include "DlgCrashInfo.h"
#include "afxdialogex.h"
#include "Crash.h"


// CDlgCrashInfo 对话框

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


// CDlgCrashInfo 消息处理程序


BOOL CDlgCrashInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

CString CDlgCrashInfo::GetExceptionInfo(DWORD dwCode)
{
	switch(dwCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.线程违规读写没有适当权限的虚拟地址。";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.线程在底层硬件支持的边界检查下访问数组越界。";
	case EXCEPTION_BREAKPOINT:
		return "A breakpoint was encountered.遇到一个断点。";
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment.线程试图在不支持对齐的硬件上读写未对齐的数据。";
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "One of the operands in a floating-point operation is denormal.浮点操作的一个操作数不正规。 A denormal value is one that is too small to represent as a standard floating-point value.不正规的值表示太小而不能表示一个标准的浮点值。";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "The thread tried to divide a floating-point value by a floating-point divisor of zero.线程浮点除零操作。";
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.浮点结果的操作不能准确的代替小数。";
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "This exception represents any floating-point exception not included in this list.这个异常代表了这个列表里没有列出的其他浮点异常。";
	case EXCEPTION_FLT_OVERFLOW:
		return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.浮点操作的指数超过了相应类型的最大值。";
	case EXCEPTION_FLT_STACK_CHECK:
		return "The stack overflowed or underflowed as the result of a floating-point operation.浮点操作的栈越界或下溢出。";
	case EXCEPTION_FLT_UNDERFLOW:
		return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.浮点操作的指数没有超过相应类型的最大值。";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "The thread tried to execute an invalid instruction.线程试图执行无效指令。";
	case EXCEPTION_IN_PAGE_ERROR:
		return "The thread tried to access a page that was not present, and the system was unable to load the page.线程试图访问一个不存在的页或者无法加载的页。 For example, this exception might occur if a network connection is lost while running a program over the network.例如，异常可能在运行的网络程序丢失连接的时候发生。";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "The thread tried to divide an integer value by an integer divisor of zero.线程试图整数除零。";
	case EXCEPTION_INT_OVERFLOW:
		return "The result of an integer operation caused a carry out of the most significant bit of the result.整数操作的结果占用了结果的最大符号位。";
	case EXCEPTION_INVALID_DISPOSITION:
		return "An exception handler returned an invalid disposition to the exception dispatcher.异常处理程序给异常调度器返回了一个无效配置。 Programmers using a high-level language such as C should never encounter this exception.使用高级语言（例如C语言）的程序永远不会遇到这个异常。";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return "The thread tried to continue execution after a noncontinuable exception occurred.线程试图在一个不可继续执行的异常发生后继续执行。";
	case EXCEPTION_PRIV_INSTRUCTION:
		return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.线程试图执行当前机器模式不支持的指令。";
	case EXCEPTION_SINGLE_STEP:
		return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.（简单的说是）单步。";
	case  EXCEPTION_STACK_OVERFLOW:
		return "The thread used up its stack.（简单的说是）栈溢出。";

	default:
		return "未知错误";
	}
}

void CDlgCrashInfo::InitUI()
{
	CString strInfo;
	strInfo.Format("错误码:0x%llx",g_CrashRecord->m_ExcpInfo.ExceptionPointers->ExceptionRecord->ExceptionCode);
	m_LST_Info.AddString(strInfo);
	strInfo.Format("错误地址:0x%llx",g_CrashRecord->m_ExcpInfo.ExceptionPointers->ExceptionRecord->ExceptionAddress);
	m_LST_Info.AddString(strInfo);
	strInfo.Format("错误信息:%s",GetExceptionInfo(g_CrashRecord->m_ExcpInfo.ExceptionPointers->ExceptionRecord->ExceptionCode));
	m_LST_Info.AddString(strInfo);
	
}

void CDlgCrashInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	g_CrashRecord->RestartProcess();
}


void CDlgCrashInfo::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	g_CrashRecord->m_bExit = TRUE;
	CDialogEx::OnCancel();
	exit(0);
	g_CrashRecord->Release();
}
