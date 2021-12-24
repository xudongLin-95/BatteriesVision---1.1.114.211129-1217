#include "stdafx.h"
#include "Control.h"
#include "DlgTemperature.h"

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CControl();
}

CControl* g_This = NULL;

std::vector<uint16_t> CControl::m_vTempList; //Ĭ�������¿���������Ҫ���Ÿ���

CControl::CControl()
{
	//��������ԵļĴ�������
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
			//�ر�����
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
	m_tParam.sComName.ReleaseBuffer();//��ȡ��Ʒ����
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

	//��ȡ������Ϣ
	if (ReadParam())
	{
		//�����¿������������б�Ĵ�С
		m_vTempList.resize(m_tParam.nTempNum,0.0);
	}
	
	//��ȡ��ǰ���¶���Ϣ
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

//�򿪹�Դ����������
BOOL CControl:: Open() 
{
	CString strLog;
	try
	{		
		//���ɲ���ʼ��һ��modbus�ṹ���ڴ�����·��ʹ��RTUģʽͨѶ��E��ʾżУ��
		ctx = modbus_new_rtu(m_tParam.sComName, 9600, 'E', 8, 1);
		if (ctx == NULL)
		{
			return FALSE;
		}
	
		//��������ģʽ����־λ����ΪTRUE����ʾ����stdout��stderr����׼����豸������ʾ�ܶ��߳�����Ϣ��������ʾmodbus��Ϣ���ֽ�
		modbus_set_debug(ctx, TRUE);

		//����modbus���ӣ������ӳɹ����ú�������0�����򣬷���-1������errno����Ϊ�ײ�ϵͳ���õĶ���ֵ
		if (modbus_connect(ctx) == -1)
		{
			//ͨ������errno���������ʧ�ܵ�ԭ��
			
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
//�رտ���������
BOOL CControl::Close() 
{
	try
	{		
		//�ر�����
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
			//�ر�����
			modbus_close(ctx);
			modbus_free(ctx);
			m_nCurrentStatus = FALSE;
		}	
		return FALSE;
	}
}

//modbus ��ȡ��ǰ�¶�
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
				//���ôӻ��˵�ַ(ID),�򿪴��ڣ�ֱ���л��ӻ��������Ƿ����
				modbus_set_slave(ctx, i + 1); //0x01,ͳһ�㲥��ַ:0x00								
			    //���Ե����Ĵ����Ķ�ȡ(����ֵrcӦ���ǼĴ���������)
				int rc = modbus_read_registers(ctx, 0, 2, tab_reg); //��ȡ���ּĴ�������Ӧ����0x03
				if (rc == -1)
				{
					strLog.Format("CControl_GetTemperature: ERROR modbus_read_registers(%d)", rc);
					Log(strLog, ExcpLog);
					return FALSE;
				}
				else
				{
					vTemp[i] = tab_reg[1]; //��ȡ�¶�����					
				}
			}
			catch (const std::exception&)
			{}

			//ÿ���л��ӻ���Ԥ��200ms
			Sleep(200);
		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
}

//��ȡ��ǰ���¶���Ϣ
void CControl::GetCurrentTemperature()
{
	//�����̣߳�ѭ����ȡ״̬��Ϣ��
	AfxBeginThread((AFX_THREADPROC)ThreadGetTemperature, this);
}

UINT CControl::ThreadGetTemperature(LPVOID lpVoid) //����ѭ����ȡ�¶� �̺߳���
{
	//���򿪣��������߳�
	if (!g_This->m_tParam.nUse) return TRUE;
	g_This->Open();
	while (1)
	{
		try
		{
			if (g_This->m_nCurrentStatus)
			{
				CLockHelper lk(&g_This->m_csLock);
				//����ָ�� ��ȡ�¶��б�,�Ȼ�ȡһ��
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

		Sleep(g_This->m_tParam.nIntervalTime *1000);//5��ѭ��һ�Σ���������Կ��ų���
	}
	return 0;
}
