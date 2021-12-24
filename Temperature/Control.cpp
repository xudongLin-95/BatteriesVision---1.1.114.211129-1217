#include "stdafx.h"
#include "Control.h"
#include "DlgTemperature.h"

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CControl();
}

CControl* g_This = NULL;

std::vector<uint16_t> CControl::m_vTempList; //默认两个温控器，有需要开放个数

CControl::CControl()
{
	//计算需测试的寄存器个数
	regNum = ADDRESS_END - ADDRESS_START;
		
	g_This = this;
	m_nCurrentStatus = FALSE;
	InitializeCriticalSection(&m_csLock);
}

CRITICAL_SECTION CControl::m_csLock;

CControl::~CControl()
{
	try
	{
		if (!m_bModbusClosed)
		{
			//关闭连接
			modbus_close(ctx);
			modbus_free(ctx);

			m_nCurrentStatus = FALSE;
		}
	}
	catch (const std::exception&)
	{

	}
	catch (...)
	{

	}
}

int CControl::DoSomething(void* message, void* wParam, void* lParam)
{
	CString strMessage;
	strMessage.Format("%s", (char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{
		return Init(*(TEMPERATUREINIT*)wParam);
	}
	else if ("PUSH_MESSAGE" == strMessage || "Push_Message" == strMessage)
	{
		//return PushMessage(*(HTuple*)wParam);
	}
	else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		return ShowParamDlg();
	}
	else if ("GETCURRENTTEMPERATURE" == strMessage || "GetCurrentTemperature" == strMessage)
	{
		*(std::vector<uint16_t>*)wParam = m_vTempList;
		return TRUE;
	}
	else if ("GETCONNECTION" == strMessage || "GetConnection" == strMessage)
	{
		*(BOOL*)wParam = m_nCurrentStatus;
		return TRUE;
	}
	return FALSE;
}

BOOL CControl::ReadParam()
{
	m_tParam.nComIndex = GetPrivateProfileInt("COM", "ComIndex", 0, m_strSerialFile);
	GetPrivateProfileString("COM", "ComName", "", m_tParam.sComName.GetBuffer(MAX_PATH), MAX_PATH, m_strSerialFile);
	m_tParam.sComName.ReleaseBuffer();//读取产品名称
	m_tParam.nUse = GetPrivateProfileInt("Config", "UseCmmu", 0, m_strSerialFile);
	m_tParam.nTempNum = GetPrivateProfileInt("Config", "TempNum", 1, m_strSerialFile);
	m_tParam.nIntervalTime = GetPrivateProfileInt("Config", "IntervalTime", 5, m_strSerialFile);
	m_tParam.nResponseTimeout = GetPrivateProfileInt("Config", "ResponseTimeout", 150000, m_strSerialFile);
	return TRUE;
}

BOOL CControl::SaveParam()
{
	WritePrivateProfileInt("COM", "ComIndex", m_tParam.nComIndex, m_strSerialFile);
	WritePrivateProfileString("COM", "ComName", m_tParam.sComName, m_strSerialFile);
	WritePrivateProfileInt("Config", "UseCmmu", m_tParam.nUse, m_strSerialFile);
	WritePrivateProfileInt("Config", "TempNum", m_tParam.nTempNum, m_strSerialFile);
	WritePrivateProfileInt("Config", "IntervalTime", m_tParam.nIntervalTime, m_strSerialFile);
	WritePrivateProfileInt("Config", "ResponseTimeout", m_tParam.nResponseTimeout, m_strSerialFile);
	return TRUE;
}

BOOL CControl::Log(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_sInit.Log == NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

int CControl::Init(TEMPERATUREINIT &Init)
{
	m_sInit = Init;

	m_strSerialFilePath.Format("%s\\Config\\Public\\Temperature", GetCurrentAppPath());
	m_strSerialFile.Format("%s\\ComSetting.ini", m_strSerialFilePath);
	if (!PathFileExists(m_strSerialFilePath))
		CreateMultilevelPath(m_strSerialFilePath);

	//读取参数信息
	if (ReadParam())
	{
		//根据温控器个数设置列表的大小
		m_vTempList.resize(m_tParam.nTempNum,0.0);
	}
	
	//获取当前的温度信息
	GetCurrentTemperature();
	return TRUE;
}

BOOL CControl::ShowParamDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgTemperature dlg;
	dlg.m_pOwner = this;
	dlg.DoModal();
	return TRUE;
}

//打开光源控制器串口
BOOL CControl:: Open() 
{
	CString strLog;
	try
	{		
		//生成并初始化一个modbus结构体在串行线路中使用RTU模式通讯，E表示偶校验
		ctx = modbus_new_rtu(m_tParam.sComName, 9600, 'E', 8, 1);
		if (ctx == NULL)
		{
			return FALSE;
		}
	
		//启动调试模式，标志位设置为TRUE，表示会在stdout和stderr（标准输出设备）上显示很多冗长的信息，用于显示modbus消息的字节
		modbus_set_debug(ctx, TRUE);

		//建立modbus连接，若连接成功，该函数返回0，否则，返回-1，并将errno设置为底层系统调用的定义值
		if (modbus_connect(ctx) == -1)
		{
			//通过调用errno，捕获程序失败的原因
			
			strLog.Format("CControl_Open: Connection failed :%s", modbus_strerror(errno));
			Log(strLog, ExcpLog);
			return FALSE;
		}

		struct timeval t;
		t.tv_sec = 0;
		t.tv_usec = g_This->m_tParam.nResponseTimeout;
		int ret = modbus_set_response_timeout(ctx, t.tv_sec, t.tv_usec);
		strLog.Format("CControl_Open: modbus_set_response_timeout return:%d", ret);
		Log(strLog, ExcpLog);

		m_nCurrentStatus = TRUE;
		return TRUE;
	}
	catch (const std::exception& e)
	{		
		strLog.Format("CControl_Open: Connection failed(exception) :%s",e);
		Log(strLog, ExcpLog);
		return FALSE;
	}
	catch (...)
	{
		strLog.Format("CControl_Open: Connection failed(exception)");
		Log(strLog, ExcpLog);
		return FALSE;
	}
}
//关闭控制器串口
BOOL CControl::Close() 
{
	try
	{		
		//关闭连接
		modbus_close(ctx);
		modbus_free(ctx);
		m_nCurrentStatus = FALSE;
		m_bModbusClosed = TRUE;
		return TRUE;
	}
	catch (const std::exception&)
	{
		if (!m_bModbusClosed)
		{		
			//关闭连接
			modbus_close(ctx);
			modbus_free(ctx);
			m_nCurrentStatus = FALSE;
		}	
		return FALSE;
	}
}

//modbus 获取当前温度
BOOL CControl::GetTemp(std::vector<uint16_t>& vTemp)
{
	try
	{		
		CString strLog;	
		for (int i = 0; i < vTemp.size(); i++)
		{
			uint16_t tab_reg[2];
			try
			{
				//设置从机端地址(ID),打开串口，直接切换从机，测试是否可行
				modbus_set_slave(ctx, i + 1); //0x01,统一广播地址:0x00								
			    //测试单个寄存器的读取(返回值rc应该是寄存器的数量)
				int rc = modbus_read_registers(ctx, 0, 2, tab_reg); //读取保持寄存器，对应功能0x03
				if (rc == -1)
				{
					strLog.Format("CControl_GetTemperature: ERROR modbus_read_registers(%d)", rc);
					Log(strLog, ExcpLog);
					return FALSE;
				}
				else
				{
					vTemp[i] = tab_reg[1]; //获取温度数据					
				}
			}
			catch (const std::exception&)
			{}

			//每次切换从机，预留200ms
			Sleep(200);
		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
}

//获取当前的温度信息
void CControl::GetCurrentTemperature()
{
	//启动线程，循环读取状态信息，
	AfxBeginThread((AFX_THREADPROC)ThreadGetTemperature, this);
}

UINT CControl::ThreadGetTemperature(LPVOID lpVoid) //串口循环获取温度 线程函数
{
	//不打开，则不启动线程
	if (!g_This->m_tParam.nUse) return TRUE;
	g_This->Open();
	while (1)
	{
		try
		{
			if (g_This->m_nCurrentStatus)
			{
				CLockHelper lk(&g_This->m_csLock);
				//串口指令 读取温度列表,先获取一个
				g_This->GetTemp(g_This->m_vTempList);
			}									
		}
		catch (const std::exception&)
		{
			g_This->m_nCurrentStatus = false;
		}
		catch (...)
		{
			g_This->m_nCurrentStatus = false;
		}

		if (!g_This->m_nCurrentStatus)
		{
			g_This->Close();
			g_This->Open();
		}

		Sleep(g_This->m_tParam.nIntervalTime *1000);//5秒循环一次，有需求可以开放出来
	}
	return 0;
}
