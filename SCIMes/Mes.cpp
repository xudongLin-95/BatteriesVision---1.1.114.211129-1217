#pragma once
#include "StdAfx.h"
#include "Mes.h"
#include "WinNetWK.h"
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib, "shlwapi.lib")

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CMes();
}
CMesSendInfo *g_pCMesSendInfo = NULL;
HANDLE *g_pHThead = NULL;
CMes* g_This = NULL;

CMes::CMes()
{
	g_This = this;
	startCopyFileFlag = false;
}

CMes::~CMes()
{

}
//显示MES界面的设置参数
BOOL CMes::DisplayMesSet(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgMesSet dlg;
	dlg.m_RealMes = this;
	dlg.DoModal();
	return TRUE;
}
void CMes::GetFileNamePath(std::vector<CString>&vFilePathList, const CString&sPath, CString&sName)
{
	CFileFind finder;
	BOOL isNotEmpty = finder.FindFile(sPath + "\\" + _T("*.*"));//总文件夹，开始遍历?
	while (isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();//查找文件?
		CString filename = finder.GetFilePath();//获取文件的路径，可能是文件夹，可能是文件?
		if (!(finder.IsDirectory()))
		{
			//1.去除拷贝文件中的副本
			if (filename.Find("副本") >= 0)
			{
				continue;
			}
			int ret = filename.MakeLower().Find(sName.MakeLower());
			if (ret >= 0)
			{
				vFilePathList.push_back(filename);//将一个文件路径加入容器?
			}
		}
		else
		{
			//递归遍历用户文件夹，跳过非用户文件夹?
			if (!(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()))
			{
				GetFileNamePath(vFilePathList, sPath, sName);
			}
		}
	}
}

//新添加文件的拷贝
BOOL CMes::CopyFileToServer(CString strLocaFilePath, CString strNumber)
{
	try
	{
		if (!m_bUploadFileAuto) return TRUE;

		CString strServerFilePath, strServerPath;//服务器路径
		//时间
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);
		CString sTime;

		CString strFileName, strFilePath, strPath2;
		strFilePath = strLocaFilePath.Left(strLocaFilePath.ReverseFind('\\'));
		strFileName = strLocaFilePath.Right(strLocaFilePath.GetLength() - strLocaFilePath.ReverseFind('\\') - 1);
		strPath2 = strFilePath.Left(strFilePath.ReverseFind('\\'));

		//搜寻前7天卷号相同的文件，一并上传
		std::vector<CString> vecLocalFile;
		std::vector<CString> vecServerFile;
		std::vector<CString> vecServerPath;
		{

		}

		for (int i = 0; i < vecLocalFile.size(); i++)
		{
			if (!PathFileExistsA(vecServerPath[i])) CreateMultilevelPath(vecServerPath[i]);
			if (!CopyFileA(vecLocalFile[i], vecServerFile[i], FALSE))
			{
				CString strMess;
				strMess.Format("上传文件至服务器端失败,%s", strServerFilePath);
				m_sInit.Log(strMess, AlarmLog, SlightExcp);
			}
		}

		////上传地址(192.168.0.33/ccd数据)\\(年月) \\ 日 \\ 卷号 \\ 文件名(厂号_设备号_产品名_卷号.csv)
		//strServerFilePath.Format(_T("%s\\%s\\%s\\%s"), m_strFtpAddr, sTime, strNumber, strFileName);
		//strServerPath.Format(_T("%s\\%s\\%s\\"), m_strFtpAddr, sTime, strNumber);
		//BOOL bExist = FALSE;
		//if (!PathFileExistsA(strServerPath)) CreateMultilevelPath(strServerPath);

		//if (!CopyFileA(strLocaFilePath, strServerFilePath, bExist))
		//{
		//	CString strMess;
		//	strMess.Format("上传文件至服务器端失败,%s", strServerFilePath);
		//	m_sInit.Log(strMess, AlarmLog, SlightExcp);
		//	return FALSE;
		//}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return FALSE;
}
//新添加文件夹的拷贝
BOOL CMes::CopyFilePathToServer(CString strLocalPath, CString strNumber)
{
	try
	{
		CString strServerPath;//服务器路径

		//时间
		SYSTEMTIME	tSaveTime;
		::GetLocalTime(&tSaveTime);
		CString sTime;

		sTime.Format("%04d%02d\\%02d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay);
		//上传地址(192.168.0.33/ccd数据)\\(年月) \\ 日 \\ 卷号
		strServerPath.Format(_T("%s\\%s\\%s\\%s"), m_strFtpAddr, sTime, sTime, strNumber);

		if (!PathFileExists(strServerPath)); CreateMultilevelPath(strServerPath);

		return CopyDirectory(strLocalPath, strServerPath);
	}
	catch (const std::exception&)
	{
		return false;
	}
	catch (...)
	{
		return FALSE;
	}
	return FALSE;
}

BOOL CMes::CopyDirectory(CString strSrcPath, CString strDestPath)
{
	try
	{
		CFileFind m_sFileFind;
		if (strSrcPath.IsEmpty())
		{
			OutputDebugString("源文件名为空，无法进行拷贝!");
			return FALSE;
		}
		if (!m_sFileFind.FindFile(strDestPath))
		{
			CreateDirectory(strDestPath, NULL);//创建目标文件夹
		}
		CFileFind finder;
		CString path;
		path.Format("%s/*.*", strSrcPath);
		//AfxMessageBox(path);
		BOOL bWorking = finder.FindFile(path);
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory() && !finder.IsDots())//是文件夹 而且 名称不含 . 或 ..
			{
				CopyDirectory(finder.GetFilePath(), strDestPath + "/" + finder.GetFileName());//递归创建文件夹+"/"+finder.GetFileName()
			}
			else
			{//是文件，则直接复制
			 //AfxMessageBox("复制文件"+finder.GetFilePath());//+finder.GetFileName()
				CopyFileA(finder.GetFilePath(), strDestPath + "/" + finder.GetFileName(), FALSE);
			}
		}
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

void CMes::StartCopyFileToServerThread(void)
{
	AfxBeginThread((AFX_THREADPROC)ThreadCopyFileToServer, this);
}

UINT ThreadCopyFileToServer(LPVOID lpVoid)     //文件拷贝线程
{
	while (true)
	{
		try
		{
			if (g_This->startCopyFileFlag)
			{
				//收到上传信号，开始上传
				CString strLocaFilePath;

				g_This->CopyFileToServer(strLocaFilePath, "卷号");
				g_This->startCopyFileFlag = false;//上传完成关闭标志
			}
		}
		catch (const std::exception&)
		{

		}
		Sleep(50);
	}
}

//时间获取接口
CString CMes::GetCurrentLocalTime()
{
	time_t timep;
	time(&timep);
	struct tm *p;
	CString cstrCurrentLocalTime;
	p = localtime(&timep); /*转换为本地的tm结构的时间按*/
	cstrCurrentLocalTime.Format("%04d-%02d-%02dT%02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	return	cstrCurrentLocalTime;
}
CString CMes::GetCurrentLocalTime(const time_t & tTime)
{
	struct tm *p;
	CString cstrCurrentLocalTime;
	p = localtime(&tTime); /*转换为本地的tm结构的时间按*/
	cstrCurrentLocalTime.Format("%04d-%02d-%02dT%02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	return	cstrCurrentLocalTime;
}
CString CMes::GetSystemLocalTime()
{
	CString cstrCurrent;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	cstrCurrent.Format("%04d/%02d/%02d-%02d:%02d:%02d.%03d",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	return cstrCurrent;
}
//获取参数
bool CMes::ReadParams()
{
	CString strFilePath;
	strFilePath.Format("%s\\Config\\Public\\SCIMes", GetCurrentAppPath());
	if (!PathFileExists(strFilePath)); CreateMultilevelPath(strFilePath);

	m_strConfigFile.Format("%s\\Config\\Public\\SCIMes\\Mes.ini", GetCurrentAppPath());

	//获取MES数据的头
	mesHead.m_MsgNO = GetPrivateProfileInt("DataHead", "MsgNO", 0, m_strConfigFile);
	mesHead.m_NeedReply = GetPrivateProfileInt("DataHead", "NeedReply", 1, m_strConfigFile);
	mesHead.m_ProtocolNO = GetPrivateProfileCString("DataHead", "ProtocolNO", "1110", m_strConfigFile);
	mesHead.m_CmdID = GetPrivateProfileCString("DataHead", "CmdID", "0001A", m_strConfigFile);
	mesHead.m_EID = GetPrivateProfileCString("DataHead", "EID", "010920161010000000001", m_strConfigFile);

	//服务器参数
	m_nPort = GetPrivateProfileInt("ServerSetting", "Port", 6000, m_strConfigFile);
	m_strIP = GetPrivateProfileCString("ServerSetting", "IP", "127.0.0.1", m_strConfigFile);
	//设置
	m_nType = GetPrivateProfileInt("Set", "Type", 0, m_strConfigFile);
	m_nLogType = GetPrivateProfileInt("Set", "nLogType", 1, m_strConfigFile);



	m_strFtpAddr = GetPrivateProfileCString("FtpServer", "IP", "\\\\192.168.0.33\\ccd数据", m_strConfigFile);
	m_strReportAddr = GetPrivateProfileCString("FtpServer", "ReportAddr", "\\\\192.168.0.33\\caw2104\\Coating Weight Data\\CCDReport", m_strConfigFile);
	m_strFtpUserID = GetPrivateProfileCString("FtpServer", "User", "ATL-BM\NW211FS-Coating2", m_strConfigFile);
	m_strFtpPwd = GetPrivateProfileCString("FtpServer", "Password", "Nd-myname", m_strConfigFile);
	m_bUploadFileAuto = GetPrivateProfileInt("FtpServer", "bUploadFileAuto", FALSE, m_strConfigFile);

	tSelectUploadFile.bUploadFile = GetPrivateProfileInt("FtpServer", "bUploadFile", FALSE, m_strConfigFile);
	tSelectUploadFile.bUploadAllFile = GetPrivateProfileInt("FtpServer", "bUploadALLFile", FALSE, m_strConfigFile);
	tSelectUploadFile.bUploadDefectFile = GetPrivateProfileInt("FtpServer", "bUploadDefectFile", FALSE, m_strConfigFile);
	tSelectUploadFile.bUploadReportFile = GetPrivateProfileInt("FtpServer", "bUploadReportFile", FALSE, m_strConfigFile);

	//设备信息
	m_nDataSourceIndex = GetPrivateProfileInt("EquipmentInfo", "DataSourceIndex", 0, m_strConfigFile);

	m_strLineCode = GetPrivateProfileCString("EquipmentInfo", "LineCode", "", m_strConfigFile);
	m_strSectionCode = GetPrivateProfileCString("EquipmentInfo", "SectionCode", "", m_strConfigFile);
	m_strEquipmentCode = GetPrivateProfileCString("EquipmentInfo", "EquipmentCode", "", m_strConfigFile);
	m_strEquipmentName = GetPrivateProfileCString("EquipmentInfo", "EquipmentName", "", m_strConfigFile);
	m_strFactory = GetPrivateProfileCString("EquipmentInfo", "Factory", "", m_strConfigFile);
	m_strEQM = GetPrivateProfileCString("EquipmentInfo", "EQM", "", m_strConfigFile);
	m_cstrResourceCode = GetPrivateProfileCString("EquipmentInfo", "ResourceCode", "", m_strConfigFile);
	m_nHeartBeatTime = GetPrivateProfileInt("EquipmentInfo", "HeartBeatTime", 10, m_strConfigFile);
	//m_cstrUserName = GetPrivateProfileCString("EquipmentInfo", "UserName", "", m_strConfigFile);
	//m_cstrUserPassword = GetPrivateProfileCString("EquipmentInfo", "UserPassword", "", m_strConfigFile);
	//m_cstrOperatorID = GetPrivateProfileCString("EquipmentInfo", "OperatorID", "", m_strConfigFile);
	//m_cstrOperatorPassword = GetPrivateProfileCString("EquipmentInfo", "OperatorPassword", "", m_strConfigFile);

	//日志路径
	m_nLogSaveDays = GetPrivateProfileInt("LogPath", "LogSaveDays", 30, m_strConfigFile);
	//文件路径
	m_strLocalFilePath = GetPrivateProfileCString("Path", "LocalImagePath", "", m_strConfigFile);
	m_strServerFilePath = GetPrivateProfileCString("Path", "ServerResultPath", "", m_strConfigFile);
	//发送设置
	m_isSendHttpMSG = GetPrivateProfileInt("SendSet", "SendHttpMSG", 0, m_strConfigFile);
	m_intervalMinute = GetPrivateProfileInt("SendSet", "IntervalMinute", 5, m_strConfigFile);
	m_iThreadNumber = GetPrivateProfileInt("SendSet", "threadNumber", 1, m_strConfigFile);
	m_bSendOK = GetPrivateProfileInt("SendSet", "bSendOK", 0, m_strConfigFile);
	m_bSendNG = GetPrivateProfileInt("SendSet", "bSendNG", 0, m_strConfigFile);
	m_bSendNGCrop = GetPrivateProfileInt("SendSet", "bSendNGCrop", 0, m_strConfigFile);
	m_bSendCover = GetPrivateProfileInt("SendSet", "bSendCover", 0, m_strConfigFile);
	m_bRename = GetPrivateProfileInt("SendSet", "bRename", 0, m_strConfigFile);
	m_strLocalRootPath = GetPrivateProfileCString("SendSet", "LocalRootPath", "", m_strConfigFile);
	m_strRootPathSymbol = GetPrivateProfileCString("SendSet", "RootPathSymbol", "Map/", m_strConfigFile);
	m_strRenameSymbol = GetPrivateProfileCString("SendSet", "RenameSymbol", "-Upload", m_strConfigFile);
	//配方信息参数
	m_strRecipeCode = GetPrivateProfileCString("RecipeInfo", "RecipeCode", "", m_strConfigFile);
	m_strProductCode = GetPrivateProfileCString("RecipeInfo", "ProdctCode", "", m_strConfigFile);
	m_strRecipeVison = GetPrivateProfileCString("RecipeInfo", "RecipeVison", "", m_strConfigFile);
	return true;
}
//保存参数
bool CMes::WriteParams()
{
	//Mes 数据交互消息头部分
	WritePrivateProfileString("DataHead", "ProtocolNO", mesHead.m_ProtocolNO, m_strConfigFile);
	WritePrivateProfileString("DataHead", "CmdID", mesHead.m_CmdID, m_strConfigFile);
	WritePrivateProfileString("DataHead", "EID", mesHead.m_EID, m_strConfigFile);
	WritePrivateProfileInt("DataHead", "MsgNO", mesHead.m_MsgNO, m_strConfigFile);
	WritePrivateProfileInt("DataHead", "NeedReply", mesHead.m_NeedReply, m_strConfigFile);
	//服务器参数
	WritePrivateProfileInt("ServerSetting", "Port", m_nPort, m_strConfigFile);
	WritePrivateProfileString("ServerSetting", "IP", m_strIP, m_strConfigFile);

	//ftp
	WritePrivateProfileString("FtpServer", "IP", m_strFtpAddr, m_strConfigFile);
	WritePrivateProfileString("FtpServer", "ReportAddr", m_strReportAddr, m_strConfigFile);
	WritePrivateProfileString("FtpServer", "User", m_strFtpUserID, m_strConfigFile);
	WritePrivateProfileString("FtpServer", "Password", m_strFtpPwd, m_strConfigFile);
	WritePrivateProfileInt("FtpServer", "bUploadFileAuto", m_bUploadFileAuto, m_strConfigFile);

	WritePrivateProfileInt("FtpServer", "bUploadFile", tSelectUploadFile.bUploadFile, m_strConfigFile);
	WritePrivateProfileInt("FtpServer", "bUploadALLFile", tSelectUploadFile.bUploadAllFile, m_strConfigFile);
	WritePrivateProfileInt("FtpServer", "bUploadDEFECTFile", tSelectUploadFile.bUploadDefectFile, m_strConfigFile);
	WritePrivateProfileInt("FtpServer", "bUploadREPORTFile", tSelectUploadFile.bUploadReportFile, m_strConfigFile);

	//设置
	WritePrivateProfileInt("Set", "Type", m_nType, m_strConfigFile);
	WritePrivateProfileInt("Set", "nLogType", m_nLogType, m_strConfigFile);
	//设备信息  
	WritePrivateProfileInt("EquipmentInfo", "DataSourceIndex", m_nDataSourceIndex, m_strConfigFile);

	WritePrivateProfileString("EquipmentInfo", "LineCode", m_strLineCode, m_strConfigFile);
	WritePrivateProfileString("EquipmentInfo", "SectionCode", m_strSectionCode, m_strConfigFile);
	WritePrivateProfileString("EquipmentInfo", "EquipmentCode", m_strEquipmentCode, m_strConfigFile);
	WritePrivateProfileString("EquipmentInfo", "EquipmentName", m_strEquipmentName, m_strConfigFile);
	WritePrivateProfileString("EquipmentInfo", "Factory", m_strFactory, m_strConfigFile);
	WritePrivateProfileString("EquipmentInfo", "EQM", m_strEQM, m_strConfigFile);
	WritePrivateProfileString("EquipmentInfo", "ResourceCode", m_cstrResourceCode, m_strConfigFile);
	//WritePrivateProfileString("EquipmentInfo", "OperatorID", m_cstrOperatorID, m_strConfigFile);
	//WritePrivateProfileString("EquipmentInfo", "OperatorPassword", m_cstrOperatorPassword, m_strConfigFile);
	//WritePrivateProfileString("EquipmentInfo", "UserName", m_cstrUserName, m_strConfigFile);
	//WritePrivateProfileString("EquipmentInfo", "UserPassword", m_cstrUserPassword, m_strConfigFile);

	//日志路径
	WritePrivateProfileInt("LogPath", "LogSaveDays", m_nLogSaveDays, m_strConfigFile);

	//MES界面设置参数
	WritePrivateProfileInt("SendSet", "SendHttpMSG", m_isSendHttpMSG, m_strConfigFile);  //变量 参与图形交互
	WritePrivateProfileInt("SendSet", "IntervalMinute", m_intervalMinute, m_strConfigFile);
	WritePrivateProfileInt("SendSet", "bSendOK", m_bSendOK, m_strConfigFile);
	WritePrivateProfileInt("SendSet", "bSendNG", m_bSendNG, m_strConfigFile);
	WritePrivateProfileInt("SendSet", "bSendNGCrop", m_bSendNGCrop, m_strConfigFile);
	WritePrivateProfileInt("SendSet", "bSendCover", m_bSendCover, m_strConfigFile);
	WritePrivateProfileInt("SendSet", "bRename", m_bRename, m_strConfigFile);
	WritePrivateProfileString("SendSet", "LocalRootPath ", m_strLocalRootPath, m_strConfigFile);
	WritePrivateProfileString("SendSet", "RootPathSymbol  ", m_strRootPathSymbol, m_strConfigFile);
	WritePrivateProfileString("SendSet", "RenameSymbol", m_strRenameSymbol, m_strConfigFile);

	//配方信息参数

	return true;
}

BOOL CMes::Log2(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_sInit.Log == NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

int CMes::Init(PMESINIT pInit)//加载配置参数
{
	m_sInit = *pInit;

	if (ReadParams() == false)
	{
		return -1;//参数加载失败
	}

	//开启文件拷贝线程
	//StartCopyFileToServerThread();

	////创建一个IO队列
	//CreateMessageQueue(&m_hvQueue);
	////设置IO队列的最大容量 25
	//SetMessageQueueParam(m_hvQueue, "max_message_num", 25);

	//HDevThreadContext Context;
	//HTuple ProcThreads;
	//HDevThread * pThread = new HDevThread(Context, (void*)IOThread, 0, 0);//开启一个具有2个输入的线程;	
	//pThread->ParStart(&ProcThreads);

	return true;
}
void* CMes::IOThread(void *hcthread)//io线程
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		g_This->Loop();
		hcppthread->Exit();
		delete hcppthread;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("io_thread出现异常\r\n异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		//g_This->m_sInit.pLogView->DoSomething("EXCEPTION",(LPSTR)(LPCSTR)strException,NULL);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		if (is_direct_call)
			throw e;
	}
	return NULL;
}
void CMes::Loop()
{
	return;

	//初始化
	if (0 == m_nType) return;
	//建立与MES服务器的连接
	OpenSocket();

	HTuple hv_MessageRecv, hv_StopProcInfo, hv_TimeStart, hv_TimeEnd, hv_ErrorFlag;
	HTuple hv_Queue = m_hvQueue;//队列句柄，类似于指针的东西，hvQueue、m_hvQueue指向同一块数据
	while (TRUE)
	{
		DequeueMessage(hv_Queue, "timeout", "infinite", &hv_MessageRecv);
		GetMessageParam(hv_MessageRecv, "key_exists", "stop_processing", &hv_StopProcInfo);
		if (0 != (HTuple(hv_StopProcInfo[0])))
		{
			break;
		}
		CountSeconds(&hv_TimeStart);//开始计时
		//定义综合结果数据结构体变量
		//C_ALLOCATORWORKSHOPOUTPUT Output;    //仍要包含define.h  ！！！！

		tagMesProductInfo tInfo;
		m_sInit.Main(MT_Mes, "GetMesProductInfo", "", NULL, &tInfo);

		try
		{
			//Output.ReadMessage(hv_MessageRecv);
		}
		catch (HException& e)
		{
			//需要定义异常编号，获取结果数据异常0001
		}
		//过程数据上传之一：常规数据
		//UploadResultFileToMES(); //上传结果文件数据 逻辑尚未实现
		CountSeconds(&hv_TimeEnd); //结束计时
	}
}

int CMes::PushMessage(const HTuple& hvMessageSend)//放置消息进入队列,通过Sonmthing()结构供平台传入结果数据消息
{
	return FALSE;
	if (!m_nType) return FALSE;
	try
	{
		EnqueueMessage(m_hvQueue, hvMessageSend, HTuple(), HTuple());//将消息投递到剔废车间
	}
	catch (HException& e)
	{
		if (H_ERR_MQOVL == e.ErrorCode())
		{
			CString strException;
			strException.Format("Mse队列已满,整合线程已退出,请排查原因");
			//m_sInit.pLogView->DoSomething("EXCEPTION", (LPSTR)(LPCSTR)strException, NULL);
		}
		else
		{
			throw e;
		}
		return FALSE;
	}
	return TRUE;
}

int CMes::DoSomething(void* message, void* wParam, void* lParam)
{
	CString strMessage;
	strMessage.Format("%s", (char*)message);

	if ("USEMES" == strMessage)
	{
		m_useMes = *(int*)wParam;
		m_nType = m_useMes;
	}
	if ("ONLINEINFO" == strMessage)
	{
		int *pRet = (int*)lParam;

		*pRet = m_iConnectStauts;

		return TRUE;
	}
	if ("INITIALIZE" == strMessage)
	{
		return Init((PMESINIT)wParam);
	}
	if (0 == m_useMes)
	{
		return FALSE;
	}
	if ("PUSH_MESSAGE" == strMessage)//放置消息进入队列
	{
		return PushMessage(*(HTuple*)wParam);
	}
	else if ("UploadDeviceInfo" == strMessage)
	{
		return UploadDeviceInfo(*(tagMesDeviceInfo*)wParam);
	}
	else if ("SHOW" == strMessage)
	{
		return DisplayMesSet();
	}
	else if ("STARTTHREAD" == strMessage)
	{
		//文件上传和心跳监测
		return StartUploadFileThread();
	}
	else if ("BATCH" == strMessage)
	{
		CString strBatch;
		if (wParam)
		{
			strBatch.Format("%s", (char*)wParam);
		}
		//return Batch(strBatch);
	}
	else if ("RECIPEINFO" == strMessage)//更新化配方版本信息
	{
		if (wParam)
		{
			//return UpdateRecipeInfo((PRECIPEINFO)wParam);
		}
		else
		{
			return FALSE;
		}
	}
	else if ("INITRECIPE" == strMessage)//初始化配方版本信息
	{
		if (wParam) //瑕疵相关配方，需求不明确，暂时屏蔽
		{
			CString cstrRecipePath;
			cstrRecipePath.Format(_T("%s"), (char*)wParam);
			//return InitRecipe(cstrRecipePath);
		}
		else
		{
			return FALSE;
		}
	}
	else if ("REPORTCOUNTINFO" == strMessage)
	{
		if (wParam)
		{
			//CLockHelper lockHelp(&m_csSoapProcessParam);
			CString cstrLParam;
			cstrLParam.Format("%s", (char*)lParam);
			if ("DEFECT" == cstrLParam)
			{
				//s_Count* pCount = (s_Count*)wParam;
				if (FALSE)
				{
					//	return  ReportDefectCountInfo(*(s_Count*)wParam);
				}
				else
				{
					//	return Soap_DefectCountInfo(*(s_Count*)wParam);
				}
			}
			if ("SIZE" == cstrLParam)
			{
				//	s_Count_Size* pCount = (s_Count_Size*)wParam;
				if (FALSE)
				{
					//return  ReportSizeCountInfo(*(s_Count_Size*)wParam);
				}
				else
				{
					//return Soap_SizeCountInfo(*(s_Count_Size*)wParam);
				}
			}
			if ("TATOLCOUNT" == cstrLParam)
			{
				if (FALSE)
				{
					//return ReportCountInfo(*(Total_Count *)wParam);
				}
				else
				{
					//return Soap_CountInfo(*(Total_Count *)wParam);
				}
			}
		}
	}
	else if ("CHECKRECIPEVERSION" == strMessage)
	{
		int * pRet = (int*)lParam;
		CString * pCstrResponse = (CString*)wParam;
		std::string strResponse;	//返回值
		if (FALSE)
		{
			*pRet = CHeckPrecipeVersion(strResponse);		//返回200 表示校验成功
		}
		else
		{
			*pRet = Soap_CheckRecipeVersion(strResponse);		//返回200 表示校验成功
		}
		(*pCstrResponse).Format(_T("%s"), strResponse.c_str());

		return TRUE;
	}
	else if ("EXIT" == strMessage) //程序运行状态
	{
		m_bExit = *(BOOL*)wParam;
		return TRUE;
	}
	else if ("CopyCcdFileToServer" == strMessage)
	{
		try
		{
			if (!m_bUploadFileAuto) return TRUE;
			CString  fileName = *(CString*)wParam;
			CString  number = *(CString*)lParam;

			return UpdateFileToServer(fileName, number);

			//CString strFileName;      //文件名字-带扩展
			//CString strFileExtendName;//仅扩展名 .csv
			//CString strName;          //仅文件名 
			//CString strPath;          //仅路径，无文件名
			//strFileExtendName = fileName.Right(fileName.GetLength() - fileName.ReverseFind('.'));
			//strFileName = fileName.Right(fileName.GetLength() - fileName.ReverseFind('\\') - 1);
			//strName = strFileName.Left(strFileName.ReverseFind('.'));
			//strPath = fileName.Left(fileName.GetLength() - strFileName.GetLength() - 1);

			//std::vector<CString> vFilePathList;
			//GetFileNamePath(vFilePathList, strPath, strName);

			//for (int i = 0; i < vFilePathList.size(); i++)
			//{
			//	CopyFileToServer(vFilePathList[i], number);//直接传文件路径进来
			//}
		}
		catch (const std::exception&)
		{
			return FALSE;
		}
		catch (...)
		{
			return FALSE;
		}

		//return UploadCcdFileOnly(fileName); //FTP格式上传
		return TRUE;
	}
	else if ("CopyCcdFileDirectoryToServer" == strMessage)
	{
		CString  pathName = *(CString*)wParam;
		CString  number = *(CString*)lParam;
		return CopyFilePathToServer(pathName, number);;
	}
	return FALSE;
}
//建立与MES服务器的连接
BOOL CMes::OpenSocket(CString strProtocol)
{
	return true;// 仅线下测试使用，正常使用需屏蔽

	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().ConnectToServer(m_strIP, m_nPort);
	}
	return true;
}
//发送心跳
BOOL CMes::Send_Heatbeat(CString strProtocol)
{
	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().Send_Heatbeat(mesHead);
	}
	return true;
}

//状态数据的采集及上传
BOOL CMes::Send_EquipmentStateData(CString strProtocol)
{
	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().Send_EquipmentStateData(mesHead);
	}
	return true;
}

//产品过程参数上传-常规数据
BOOL CMes::Send_equipmentProcessData(CString strProtocol)
{
	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().Send_ProcessData1(mesHead);
	}
	return true;
}

//报警数据的采集及上传(需要汇总设备的报警信息，以表格形式呈现)
BOOL CMes::Send_EquipmentAlarmData(CString strProtocol)
{
	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().Send_EquipmentAlarmMessage(mesHead);
	}
	return true;
}

//设备配方数据的请求(传出从MES获取的配方数据->转发至设备进行设置)
//设备通过按钮自行触发，以回去当前已选择工单的配方参数，也支持上位机在开机前自行去获取配方
BOOL CMes::GetRequestRecipes(CString strProtocol)
{
	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().Get_EquipmentRecipes(mesHead);
	}
	return true;
}

//设备开机配方参数校验
BOOL CMes::Send_CheckRecipeVersion(CString strProtocol)
{
	if (strProtocol == "TCP")
	{
		return CTcpMes::getInstance().Check_EquipmentRecipes(mesHead);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//启动上传线程
BOOL CMes::StartUploadFileThread(void)
{
	m_ThreadFtpUpload = AfxBeginThread((AFX_THREADPROC)ThreadFtpUpload, this);// 文件上传线程
	AfxBeginThread((AFX_THREADPROC)ThreadOnInfo, this);                       // 心跳监测线程
	return TRUE;
}

//文件上传 (线程函数为非成员函数)
void  ThreadFtpUpload(PVOID * pParam)
{
	CMes* pCMes = (CMes*)pParam;
	SYSTEMTIME currentTime;
	Sleep(2000);
	while (!pCMes->m_bExit)//程序运行状态
	{
		if (1 != pCMes->m_nType || 0 == pCMes->m_nType)
		{
			Sleep(2000);
			continue;
		}

		pCMes->m_vFilePathList.clear();
		pCMes->GetFilePath(pCMes->m_vFilePathList, pCMes->m_strLocalRootPath, pCMes->m_bSendOK, pCMes->m_bSendNG, pCMes->m_bSendNGCrop, pCMes->m_bSendCover);
		pCMes->m_hvCurrentDisk.Clear();	     //清空上一次磁盘扫描结果
		int  iCount = 0;
		try
		{
			pCMes->GetFilePath(pCMes->m_vFilePathList, pCMes->m_strLocalRootPath, pCMes->m_bSendOK, pCMes->m_bSendNG, pCMes->m_bSendNGCrop);
			HTuple hvUploadFile;	         //需要上传的文件集合
			HTuple hvDeletFile;		         //已经删除的文件集合
			TupleDifference(pCMes->m_hvUploaded, pCMes->m_hvCurrentDisk, &hvDeletFile);	            //找出已经删除的文件
			TupleDifference(pCMes->m_hvUploaded, hvDeletFile, &pCMes->m_hvUploaded);	            //去除已经删除的文件
			TupleDifference(pCMes->m_hvCurrentDisk, pCMes->m_hvUploaded, &pCMes->m_hvCurrentDisk);	//过滤已经上传的文件
			iCount = pCMes->m_hvCurrentDisk.Length();
			pCMes->m_vFilePathList.clear();	//清空上次上传文件列表
			for (int i = 0; i < iCount; i++)
			{
				pCMes->m_vFilePathList.push_back(pCMes->m_hvCurrentDisk[i].S().Text());
			}
		}
		catch (HException& e)
		{
			CString strMsg, strFunc;
			strMsg.Format("扫盘处理tuple异常\t%s", e.ErrorMessage().Text());
			pCMes->m_pSoapMesLog->AssignmentLog(strMsg);
			continue;
		}
		//线程数固定为1
		pCMes->m_iThreadNumber = 1;
		//////////////////////////////////////////////////
		CString strMessage;
		int iLenght = 0;
		int iMod = 0;
		int iSum = 0;
		int iThreadCreate = 0;
		iLenght = iCount / pCMes->m_iThreadNumber;
		iMod = iCount % pCMes->m_iThreadNumber;
		if (0 == iLenght && iLenght < pCMes->m_iThreadNumber)
		{
			iThreadCreate = iMod;
		}
		else
		{
			iThreadCreate = pCMes->m_iThreadNumber;
		}
		GetLocalTime(&currentTime);
		strMessage.Format(_T("线程总数：\t%d\t文件数目：\t%d"), iThreadCreate, iCount);
		pCMes->m_pSoapMesLog->AssignmentLog(strMessage);
		if (0 == iCount)
		{
			Sleep(pCMes->m_intervalMinute * 60 * 1000);
			continue;
		}
		g_pCMesSendInfo = new CMesSendInfo[iThreadCreate];
		g_pHThead = new HANDLE[iThreadCreate];
		memset(g_pHThead, 0, sizeof(HANDLE)* iThreadCreate);
		for (int i = 0; i < iThreadCreate; i++)
		{
			g_pCMesSendInfo[i].pCMes = pCMes;
			g_pCMesSendInfo[i].id = i;
			g_pCMesSendInfo[i].vFilePathList = pCMes->m_vFilePathList;
			if (iMod > 0)
			{
				g_pCMesSendInfo[i].iLenght = iLenght + 1;
			}
			else
			{
				g_pCMesSendInfo[i].iLenght = iLenght;
			}
			iMod--;
			iSum += g_pCMesSendInfo[i].iLenght;
			g_pCMesSendInfo[i].iSum = iSum;
			g_pHThead[i] = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProcDynamic_1, &g_pCMesSendInfo[i], NULL, NULL);
		}
		WaitForMultipleObjects(iThreadCreate, g_pHThead, TRUE, INFINITE);
		for (int i = 0; i < iThreadCreate; i++)
		{
			CloseHandle(g_pHThead[i]);
		}
		if (NULL != g_pCMesSendInfo)
		{
			delete[] g_pCMesSendInfo;
			g_pCMesSendInfo = NULL;
		}
		if (NULL != g_pHThead)
		{
			delete[] g_pHThead;

			g_pHThead = NULL;
		}
		///////////////////////////////////////////////////
		Sleep(pCMes->m_intervalMinute * 60 * 1000);
	}
}
UINT ThreadProcDynamic_1(LPVOID lpVoid)
{
	CMesSendInfo *lpSendInfo = (CMesSendInfo*)lpVoid;
	std::vector<CString>::iterator iterStart, iterEnd;
	iterStart = lpSendInfo->vFilePathList.begin() + lpSendInfo->iSum - lpSendInfo->iLenght;
	iterEnd = iterStart + lpSendInfo->iLenght;
	CString strMessage;
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);
	int iStart, iLenght;
	double dCPUUserRate = 0; //CPU使用的百分比 
	iStart = iterStart - lpSendInfo->vFilePathList.begin();
	iLenght = iterEnd - iterStart;
	strMessage.Format(_T("线程号：\t%d\t起始：\t%d\t长度：\t%d"), lpSendInfo->id, iStart, iLenght);
	lpSendInfo->pCMes->m_pSoapMesLog->AssignmentLog(strMessage);
	int i = lpSendInfo->id;
	for (std::vector<CString>::iterator iter = iterStart; iter != iterEnd; iter++)
	{
		//暂时屏蔽，不清楚这个变量的用意
		if (0 == lpSendInfo->pCMes->m_nType)
		{
			continue;
		}
		if (lpSendInfo->pCMes->m_bSendCover)			    //是否过滤已上传文件
		{
			if (!lpSendInfo->pCMes->IsUpload(*iter))		//是否上传检测
			{
				continue;
			}
		}
		while (!lpSendInfo->pCMes->m_bExit)                 //程序运行状态
		{
			lpSendInfo->pCMes->picProcessInfoCollect.GetCPUUserRate(dCPUUserRate);
			strMessage.Format(_T("CPU Rate：\t%0.3f\n"), dCPUUserRate);
			lpSendInfo->pCMes->m_record.writeMSG(strMessage);
			if (dCPUUserRate > 30)   //CPU 消耗大于25%  暂停上传ftp文件
			{
				Sleep(1000 * lpSendInfo->pCMes->m_intervalMinute);
			}
			else
			{
				break;
			}
		}
		if (lpSendInfo->pCMes->m_bExit)
		{
			break;
		}
		lpSendInfo->pCMes->FtpUploadFile(*iter);
	}
	lpSendInfo->pCMes->WriteUploadTuple();
	return	0;
}
//文件上传心跳监测
UINT ThreadOnInfo(LPVOID lpVoid)
{
	CMes* pCMes = (CMes*)lpVoid;
	int nStauts(0);

	while (!pCMes->m_bExit)
	{
		if (FALSE)
		{
			//pCMes->m_iConnectStauts = pCMes->ReportOnlieInfo();
		}
		else
		{
			int iRes(-100);
			int nCode;
			neb::CJsonObject CJsonRcv;
			iRes = pCMes->Send_Heatbeat();// 默认TCP协议
			CJsonRcv.Get("code", nCode);
			if (SOAP_OK == iRes && 1 == nCode)
			{
				pCMes->m_iConnectStauts = 200;
			}
			else
			{
				pCMes->m_iConnectStauts = 0;
			}
		}
		Sleep(1000 * pCMes->m_nHeartBeatTime);
	}
	return	0;
}
//本地写Tuple文件
BOOL CMes::WriteUploadTuple(void)
{
	try
	{
		CLockHelper cLock(&m_csWriteTuple);
		//	EnterCriticalSection(&m_csWriteTuple);

		WriteTuple(m_hvUploaded, (HTuple)m_cstrUploadTuple);

		//	LeaveCriticalSection(&m_csWriteTuple);
	}
	catch (HException &e)
	{
		CString strException;
		strException.Format("写tuple异常:%s\n", e.ErrorMessage().Text());
		m_record.writeMSG(strException);
	}
	catch (...)
	{
		//	LeaveCriticalSection(&m_csWriteTuple);

		CString strMsg;

		strMsg.Format("写tuple异常\t");

		m_record.writeMSG(strMsg);

		return FALSE;
	}
	return TRUE;
}
//本地文件 服务器文件  路径转换
BOOL CMes::LocalFilePathToServerFilePath(CString const & strLocalFilePath, CString & strServerFilePath, CString const & strSymbol)
{
	//本地文件路径
	//Map/年/月/日/卷号/产品名称/正面/NG/WIP条码_（OK/NG）_yyyyMMddHHmmssfff.jpg；图片
	//Map/年/月/日/卷号/产品名称/WIP条码_yyyyMMddHHmmssfff.csv ；          结果文件
	//服务器文件路径
	//工厂/工序/设备厂家/设备编号/Data/年/月/日/WIP条码_yyyyMMddHHmmssfff.csv
	//工厂/工序/设备厂家/设备编号/Image/年/月/日/WIP条码_（OK/NG）_yyyyMMddHHmmssfff.jpg
	CString strRemoteFile;	//服务器文件路径
	CString strFileExtendName;	//文件扩展名
	CString strFileName;		//文件名，带扩展名
	CString strDate;			//截取时间路径
	CString strFileType;		//文件类型

	if (strLocalFilePath.Find(strSymbol) < 0)
	{
		return FALSE;
	}
	//if (m_sInit.nProductType == 1)		//瑕疵
	{
		strFileName = strLocalFilePath.Right(strLocalFilePath.GetLength() - strLocalFilePath.ReverseFind('\\') - 1);
		strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.Find('.') - 1);
		strDate = strLocalFilePath.Right(strLocalFilePath.GetLength() - strLocalFilePath.Find(strSymbol) - strSymbol.GetLength());
		strDate = strDate.Left(strDate.Find("日\\") + 2);
		if (strFileExtendName == _T("csv") || strFileExtendName == _T("xls"))
		{
			strFileType.Format(_T("Data"));
		}
		else if (strFileExtendName == _T("jpg") || strFileExtendName == _T("bmp") || strFileExtendName == _T("png"))
		{
			strFileType.Format(_T("Image"));
		}
		else
		{
			strFileType.Format(_T("未知文件类型"));
		}
		//根地址 + 设备编号 + 数据类型 + 日期 + 文件名
		strServerFilePath.Format(_T("%s/%s/%s/%s/%s"), m_strFtpAddr, m_strEquipmentCode, strFileType, strDate, strFileName);
		strServerFilePath.Replace("\\", "/");
	}
	//else if (m_sInit.nProductType == 2)		//尺寸
	{
		strFileName = strLocalFilePath.Right(strLocalFilePath.GetLength() - strLocalFilePath.ReverseFind('\\') - 1);
		strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.Find('.') - 1);
		strDate = strLocalFilePath.Right(strLocalFilePath.GetLength() - strLocalFilePath.Find(strSymbol) - strSymbol.GetLength());
		strDate = strDate.Left(strDate.Find("日\\") + 2);
		strDate.Insert(strDate.Find(_T("年")) + 2, "\\");
		if (strFileExtendName == _T("csv") || strFileExtendName == _T("xls"))
		{
			strFileType.Format(_T("Data"));
		}
		else if (strFileExtendName == _T("jpg") || strFileExtendName == _T("bmp") || strFileExtendName == _T("png"))
		{
			strFileType.Format(_T("Image"));
		}
		else
		{
			strFileType.Format(_T("未知文件类型"));
		}
		//根地址 + 设备编号 + 数据类型 + 日期 + 文件名
		strServerFilePath.Format(_T("%s/%s/%s/%s/%s"), m_strFtpAddr, m_strEquipmentCode, strFileType, strDate, strFileName);
		strServerFilePath.Replace("\\", "/");
	}
	return TRUE;
}

//日志接口(功能待完善)
void CMes::Log(const CString &cstrMessage, const CString &cstrType)
{
	//	return; //删除弹窗提示
	if (cstrType == "EXCEPTION")
	{
		return;
	}
	try
	{
		//switch (/*m_sInit.nProductType*/)
		//{
		//	case 1:
		//	{
		//		//暂时屏蔽，功能完善后开放日志功能
		//		//m_sInit.pLogView->DoSomething((void*)(LPCSTR)cstrType, (void*)(LPCSTR)cstrMessage, NULL);
		//		break;
		//	}
		//	case 2:
		//	{
		//		//暂时屏蔽，功能完善后开放日志功能
		//		//m_sInit.plog(cstrMessage, cstrType);
		//		break;
		//	}
		//}
	}
	catch (...)
	{

	}
}

//文件上传完成报告Http
int CMes::MesReportFileUploadCompleted(CString & strFilePath)
{
	int iRet;
	CString strSendFilePath, strMessage;
	CString cstrTime;
	std::string strRespon;
	std::string strSendMessage;
	std::string strSendMessageUTF8;
	GetLocalTime(&m_currentTime);
	strSendFilePath.Format(_T("%s"), strFilePath);
	if (!GetSendFileJson(strSendFilePath, strSendMessage, m_currentTime))
	{
		iRet = -1;
		strMessage.Format(_T("MESReportCcdFilesUploadCompleted\tCJeson转换失败！\t%文件路径：%s"), strSendFilePath);
		m_record.writeHttpFile(strMessage);
		return iRet;
	}
	strSendMessageUTF8 = AsciiToUtf8(strSendMessage);
	iRet = MESReportCcdFilesUploadCompleted((CString)strSendMessageUTF8.c_str(), strRespon);
	if (200 != iRet)
	{
		strMessage.Format(_T("MESReportCcdFilesUploadCompleted;返回码：%d\t返回信息：%s\n发送信息：%s"), iRet, strRespon.c_str(), strSendMessage.c_str());
		//	m_sInit.pLogView->DoSomething("EXCEPTION", (void*)(LPCSTR)strMessage,NULL);
		Log(strMessage); //日志功能后面完善
		m_record.writeHttpFile(strMessage);
	}

	strMessage.Format(_T("MESReportCcdFilesUploadCompleted;返回码：%d\t\n发送信息：%s"), iRet, strSendMessage.c_str());

	m_record.writeHttpFile(strMessage);

	return iRet;
}
int CMes::MESReportCcdFilesUploadCompleted(CString& strData, std::string &strRespons)
{
	CString strUrl;
	strUrl.Format("http://%s:%d/api/ccd/ccd", m_strIP, m_nPort);
	return http_request(LPCSTR(strUrl), strRespons, (std::string)strData.GetString());
}
int CMes::http_request(const char* url, std::string& strResponse, const std::string postdata, const char* headers, bool bReserveHeaders, int timeout)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	char chout[1024 * 50];

	memset(chout, 0, 1024 * 50);

	if (NULL == curl) return false;

	curl_easy_setopt(curl, CURLOPT_URL, url);

	//响应结果中保留头部信息
	if (bReserveHeaders)
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_callback);//设置回调函数
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, chout);//设置写数据  chout   (void*)&strResponse
	curl_easy_setopt(curl, CURLOPT_POST, 1);

	//设置超时时间
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

	//添加自定义头信息
	if (headers != NULL)
	{
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, headers);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	}

	//设置发送消息
	if (postdata != "")
	{
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postdata.size());
	}

	res = curl_easy_perform(curl);//会阻塞，需要等到完成才返回
	int nCode = 0;
	if (res == CURLE_OK)
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &nCode);
	else
		nCode = res;	// nCode = 0;

	strResponse = chout;

	curl_easy_cleanup(curl);

	return nCode;
}

size_t CMes::write_data_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string data((const char*)ptr, (size_t)size * nmemb);

	data = Utf8ToAscii(data);

	strcpy((char*)stream, data.c_str());

	return size * nmemb;
}
//生成文件上传报告json信息(这里要确认下：如果文件只有图片的话，是否需要转换为JSON格式，另外文件传输是否采用FAT协议！！！)
BOOL CMes::GetSendFileJson(CString const& strFilePath, std::string & strSendJson, SYSTEMTIME const& iLocalTime)
{
	neb::CJsonObject oJsonPackage;
	neb::CJsonObject oJsonFile;
	CString strServerFilePath;
	CString strFileName;
	CString strFileType;
	CString strProductBarcode;
	CString strTimestamp;
	CString strIsOK = _T("");
	//LocalFilePathToServerFilePath(strLocalFilePath,strServerFilePath);
	strServerFilePath = strFilePath;
	strFileType = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('.') - 1);
	strFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('/') - 1);
	if (strFileName.Find('_') > 0)
	{
		strProductBarcode = strFileName.Left(strFileName.Find('_'));
	}
	else
	{
		strProductBarcode.Format(_T("33333"));
	}
	if (strFileType == "jpg" || strFileType == "bmp")
	{
		strIsOK.Format(_T("true"));
	}
	//2020-03-23T15:01:14.6810138+08:00
	strTimestamp.Format(_T("%04d-%02d-%02dT%02d:%02d:%02d.%03d+08:00"),
		iLocalTime.wYear, iLocalTime.wMonth, iLocalTime.wDay, iLocalTime.wHour, iLocalTime.wMinute, iLocalTime.wSecond, iLocalTime.wMilliseconds);

	oJsonFile.Add("FilePath", strServerFilePath.GetString());
	if (strIsOK != _T(""))
	{
		oJsonFile.Add("IsOk", strIsOK.GetString());
	}
	oJsonFile.Add("ProductBarcode", strProductBarcode.GetString());
	oJsonFile.Add("Timestamp", strTimestamp.GetString());
	oJsonPackage.AddEmptySubArray("Files");
	oJsonPackage["Files"].Add(oJsonFile);
	oJsonPackage.Add("LineCode", m_strLineCode.GetString());
	oJsonPackage.Add("SectionCode", m_strSectionCode.GetString());
	oJsonPackage.Add("EquipmentCode", m_strEquipmentCode.GetString());
	oJsonPackage.Add("Timestamp", strTimestamp.GetString());
	strSendJson.empty();
	strSendJson = "[" + oJsonPackage.ToString() + "]";
	return TRUE;
}
//获取上传文件 CCD相关
int CMes::GetParamCCDUploadFiles(const CString& cstrURI, std::vector<ParamCCDUploadFile>& vecParam)
{
	vecParam.clear();
	ParamCCDUploadFile par;
	par.cstrSFC.Format("%s", m_strCurrentBatch);
	par.cstrURI.Format("%s", cstrURI);
	if (cstrURI.Find(_T("_OK_")) > 0 || cstrURI.Find(_T(".csv")) > 0)		//OK 图片 或csv 文件  
	{
		par.nPassed = 0;
	}
	else
	{
		par.nPassed = 1;
	}
	vecParam.push_back(par);
	return 0;
}
//文件上传完成
int CMes::Soap_CCDFileUploadComplete(const std::vector<ParamCCDUploadFile> & vecParamCCDUpload, neb::CJsonObject & nebCJsonReciev)
{
	neb::CJsonObject nebCJsonSend;
	int res = -101;
	res = CMesGSoap::getInstance().CCDFileUploadComplete(vecParamCCDUpload, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		m_pSoapMesLog->CCDFileUploadCompleteLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		m_pSoapMesLog->CCDFileUploadCompleteLog(cstrMsg);

		nebCJsonReciev.Get("RcvTime", strTime);
		nebCJsonReciev.Get("Res", nRes);
		nebCJsonReciev.Get("code", nCode);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		m_pSoapMesLog->CCDFileUploadCompleteLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		m_pSoapMesLog->CCDFileUploadCompleteLog(cstrMsg);
	}
	return res;
}

BOOL CMes::UploadDeviceInfo(tagMesDeviceInfo& tInfo)
{
	//上传设备状态

	return TRUE;
}

BOOL CMes::UploadCcdFileOnly(CString strFullFilePath)
{
	int nRet = 0;
	CString strServerFilePath;//
	CString strFileName;
	//strFullFilePath.Format("%s", m_strLocalFilePath);
	strFileName = strFullFilePath.Right(strFullFilePath.GetLength() - strFullFilePath.ReverseFind('\\') - 1);
	strServerFilePath.Format("%s/%s", m_strFtpAddr, strFileName);
	nRet = FtpUpload(strServerFilePath, strFullFilePath/*本地路径*/);
	if (nRet > 0)
	{
		CString strException;
		strException.Format(_T("FtpUpload错误！\t错误码：%d"), nRet);
		//m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCTSTR)strException,NULL);
		Log(strException); //日志待功能完善后再考虑
		return nRet;
	}
}

//FTP上传文件
int CMes::FtpUploadFile(CString & strFullFilePath)
{
	int nRet = 0;
	CString strServerFilePath;
	if (!LocalFilePathToServerFilePath(strFullFilePath, strServerFilePath, m_strRootPathSymbol))
	{
		nRet = -2;	//文件路径转换错误
	}
	nRet = FtpUpload(strServerFilePath, strFullFilePath/*本地路径*/);  //C:\Users\xuxh\Desktop\111\0.bmp
	if (nRet > 0)
	{
		CString strException;
		strException.Format(_T("FtpUpload错误！\t错误码：%d"), nRet);
		//m_sInit.pLogView->DoSomething("EXCEPTION",(void*)(LPCTSTR)strException,NULL);
		Log(strException); //日志待功能完善后再考虑
		return nRet;
	}
	if (m_isSendHttpMSG)
	{
		if (FALSE)			//1期
		{
			MesReportFileUploadCompleted(strServerFilePath);
		}
		if (TRUE)			//2期
		{
			//const std::vector<ParamCCDUploadFile> & vecParamCCDUpload,neb::CJsonObject & nebCJsonReciev

			std::vector<ParamCCDUploadFile> vecParamCCDUpload;
			GetParamCCDUploadFiles(strServerFilePath, vecParamCCDUpload);
			Soap_CCDFileUploadComplete(vecParamCCDUpload, neb::CJsonObject());
		}
	}
	return nRet;
}
BOOL CMes::IsUpload(const CString & cstrFile)           //已经上传 返回FALS，没有上传 返回TRUE
{
	HTuple hvIndex;

	int iRet = -1;

	EnterCriticalSection(&m_csWriteTuple);		        //m_csUploadTUple 线程锁

	TupleFind(m_hvUploaded, HTuple(cstrFile), &hvIndex);//Halcon算子:获取某个元素在另一个数组里的所有位置值，并就这个位置返回一个数组；

	LeaveCriticalSection(&m_csWriteTuple);

	if (hvIndex.Length() > 0)
	{
		iRet = hvIndex[0].I();
	}
	if (iRet >= 0)		//找到了，表示已经上传
	{
		return FALSE;
	}
	else				//没有找到表示没有上传
	{
		return TRUE;
	}
}
int CMes::FtpUpload(CString& strRemoteFilePath, CString& strLocalFilePath)
{
	int iRet = -1;
	CString cstrStatus, cstrMessage;
	std::string strPath = strRemoteFilePath.GetString();
	std::string strPathUtf8 = AsciiToUtf8(strPath);
	//此接口通过cURL的FTP协议上传，需要先确认文件上传的格式
	iRet = ftp_upload(strPathUtf8.c_str(), LPCSTR(strLocalFilePath), LPCSTR(m_strFtpUserID), LPCSTR(m_strFtpPwd));
	//iRet = ftp_upload("ftp://192.168.0.33/ccd数据/1.csv", "D:\\1.csv", "ATL-BM\NW211FS-Coating2", LPCSTR(m_strFtpPwd));

	if (0 != iRet)		//失败重新上传
	{
		Sleep(200);
		iRet = ftp_upload(strPathUtf8.c_str(), LPCSTR(strLocalFilePath), LPCSTR(m_strFtpUserID), LPCSTR(m_strFtpPwd));
	}
	////暂时屏蔽，后面开启
	//if (0 == iRet)
	//{
	//	cstrStatus.Format(_T("TRUE"));
	//	AppendUploadTuple(strLocalFilePath);		//记录已上传文件
	//}
	//else
	//{
	//	cstrStatus.Format(_T("FALSE"));
	//}
	//cstrMessage.Format(_T("ftp_upload:%s\t状态码:%d\t上传文件:%s"), cstrStatus, iRet, strLocalFilePath);
	//m_pSoapMesLog->UploadFileLog(cstrMessage);
	if (0 == iRet && m_bRename)
	{
		RenameFile(strLocalFilePath, m_strRenameSymbol);
	}
	return iRet;
}
//上传文件 
int CMes::ftp_upload(const char *remote_file_path, const char *local_file_path, const char *username, const char *password, long timeout, long tries)
{
	// init curl handle
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curlhandle = curl_easy_init();

	// get user_key pair
	char user_key[1024] = { 0 };
	sprintf(user_key, "%s:%s", username, password);

	FILE *file;
	long uploaded_len = 0;
	CURLcode ret = CURLE_GOT_NOTHING;
	file = fopen(local_file_path, "rb");
	if (file == NULL)
	{
		perror(NULL);
		return 0;
	}
	//获取源文件大小
	fseek(file, 0, SEEK_END);
	int resource_file_size = ftell(file);

	curl_easy_setopt(curlhandle, CURLOPT_URL, remote_file_path);
	curl_easy_setopt(curlhandle, CURLOPT_USERPWD, user_key);
	curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);//libcurl会输出通讯过程中的一些细节，上传出错会用到
	if (timeout)
		curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);
	curl_easy_setopt(curlhandle, CURLOPT_READDATA, file); //没有专门写读源文件回调函数，CURLOPT_READDATA会自动读取
	curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(curlhandle, CURLOPT_INFILESIZE, resource_file_size);
	curl_easy_setopt(curlhandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);

	// upload: 断点续传
	for (int c = 0; (ret != CURLE_OK) && (c < tries); c++)
	{
		/* are we resuming? */
		if (c)
		{ /* yes */
		  /* determine the length of the file already written */
		  /*
		  * With NOBODY and NOHEADER, libcurl will issue a SIZE
		  * command, but the only way to retrieve the result is
		  * to parse the returned Content-Length header. Thus,
		  * getContentLengthfunc(). We need discardfunc() above
		  * because HEADER will dump the headers to stdout
		  * without it.
		  */
			curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 1L);
			curl_easy_setopt(curlhandle, CURLOPT_HEADER, 1L);
			ret = curl_easy_perform(curlhandle);
			if (ret != CURLE_OK)
				continue;
			curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 0L);
			curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0L);
			fseek(file, uploaded_len, SEEK_SET);
			curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1L);
		}
		else
			curl_easy_setopt(curlhandle, CURLOPT_APPEND, 0L);

		ret = curl_easy_perform(curlhandle);
	}
	fclose(file);
	//exit curl handle
	curl_easy_cleanup(curlhandle);
	curl_global_cleanup();
	return ret;
}
//上传，重命名文件
BOOL CMes::RenameFile(CString const& strLocalFilePath, CString const & strSymbol)
{
	CString	strFullFilePath;	//带路径文件全名
	CString strFilePath;		//文件夹路径
	CString strFileExtendName;	//文件扩展名
	CString strFileName;		//文件名，带扩展名
	CString strFile;			//文件名，不带扩展名
	CString strNewFilePath;		//新文件 带路径全名
	CString strNewFile;			//新文件名
	strFullFilePath = strLocalFilePath;
	strFilePath = strFullFilePath.Left(strFullFilePath.ReverseFind('\\') + 1);
	strFileName = strFullFilePath.Right(strFullFilePath.GetLength() - strFullFilePath.ReverseFind('\\') - 1);
	strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1);
	strFile = strFileName.Left(strFileName.Find('.'));
	strNewFile.Format(_T("%s%s"), strFile, strSymbol);
	strNewFilePath.Format(_T("%s%s.%s"), strFilePath, strNewFile, strFileExtendName);
	try
	{
		CFile::Rename(strFullFilePath, strNewFilePath);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
//记录已上传文件 (文件成功上传后，需要将文件路径添加到已上传列表)
BOOL CMes::AppendUploadTuple(CString & cstrFile)
{
	EnterCriticalSection(&m_csWriteTuple); //m_csUploadTUple

	m_hvUploaded.Append(HTuple(cstrFile)); //已经上传的文件

	LeaveCriticalSection(&m_csWriteTuple);

	return TRUE;
}
//检测文件是否上传过(根据已上传标志判断文件是否已上传)
BOOL CMes::HasBeenSent(CString const& strLocalFilePath)//FALSE  未发送过，TRUE 已发送过
{
	BOOL bRet = FALSE;
	CString strFileName;
	strFileName = strLocalFilePath.Right(strLocalFilePath.GetLength() - strLocalFilePath.ReverseFind('\\') - 1);

	if (strFileName.Find(m_strRenameSymbol) < 0)
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}
	return bRet;   //FALSE  未发送过，TRUE 已发送过
}
BOOL CMes::UnRenameFile(CString const & strLocalFilePath, CString const & strSymbol, CString& strOldFilePath)
{
	CString	strFullFilePath;	//带路径文件全名
	CString strFilePath;		//文件夹路径
	CString strFileExtendName;	//文件扩展名
	CString strFileName;		//文件名，带扩展名
	CString strFile;			//文件名，不带扩展名
	CString strNewFilePath;		//新文件 带路径全名
	CString strNewFile;			//新文件名
	strFullFilePath = strLocalFilePath;
	strFilePath = strFullFilePath.Left(strFullFilePath.ReverseFind('\\') + 1);
	strFileName = strFullFilePath.Right(strFullFilePath.GetLength() - strFullFilePath.ReverseFind('\\') - 1);
	strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1);
	strFile = strFileName.Left(strFileName.Find('.'));
	strNewFile = strFileName.Left(strFileName.Find(strSymbol));
	strOldFilePath.Format(_T("%s%s.%s"), strFilePath, strNewFile, strFileExtendName);
	try
	{
		CFile::Rename(strFullFilePath, strOldFilePath);
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}
//扫描磁盘文件
void CMes::GetFilePath(std::vector<CString>& vFilePathList, CString &strDir, BOOL & bSendOK, BOOL &bSendNG, BOOL & bSendNGC, BOOL & bSendCover)
{
	CFileFind finder;
	CString strFilePath;
	strFilePath.Format(_T("%s"), strDir);
	if (strFilePath == _T(""))
	{
		return;
	}
	else
	{
		if (strFilePath.Right(1) != _T(""))
		{
			strFilePath += _T("\\");
		}
		strFilePath += _T("*.*");
	}

	BOOL isNotEmpty = finder.FindFile(strFilePath);//总文件夹，开始遍历
	while (isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();        //查找文件
		CString filename = finder.GetFilePath();   //获取文件的路径，可能是文件夹，可能是文件?
		if (!(finder.IsDirectory()))
		{
			//如果是文件则加入文件列表
			CString strOldFileName;
			if (HasBeenSent(filename))	//已发送过
			{
				if (!bSendCover)	    //已发送文件再发送
				{
					CString strFileName;
					CString strFileExtendName;
					CString strOldFileName;
					strFileName = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);
					strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.Find('.') - 1);
					if (strFileExtendName == _T("bmp") || strFileExtendName == _T("jpg"))
					{
						int iOKImg = 0;
						int iNGImg = 0;
						int iNGCImg = 0;
						iOKImg = strFileName.Find(_T("_OK_"));
						iNGImg = strFileName.Find(_T("_NG_"));
						iNGCImg = strFileName.Find(_T("_NGC_"));
						if (iOKImg >= 0 && bSendOK)
						{
							UnRenameFile(filename, m_strRenameSymbol, strOldFileName);
							vFilePathList.push_back(strOldFileName);//将一个文件路径加入容器
						}
						else if (iNGImg >= 0 && bSendNG)
						{
							UnRenameFile(filename, m_strRenameSymbol, strOldFileName);
							vFilePathList.push_back(strOldFileName);
						}
						else if (iNGCImg >= 0 && bSendNGC)
						{
							UnRenameFile(filename, m_strRenameSymbol, strOldFileName);
							vFilePathList.push_back(strOldFileName);
						}
						else
						{
							;
						}
					}
					else
					{
						if (strFileName.Find(m_strCurrentBatch) < 0)
						{
							UnRenameFile(filename, m_strRenameSymbol, strOldFileName);
							vFilePathList.push_back(strOldFileName);
						}
					}
				}
				else //已发送文件不再发送
				{
					//continue;
				}
			}
			else//未发送过
			{
				CString strFileName;
				CString strFileExtendName;
				strFileName = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);
				strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.Find('.') - 1);
				if (strFileExtendName == _T("bmp") || strFileExtendName == _T("jpg"))
				{
					int iOKImg = 0;
					int iNGImg = 0;
					int iNGCImg = 0;
					iOKImg = strFileName.Find(_T("_OK_"));
					iNGImg = strFileName.Find(_T("_NG_"));
					iNGCImg = strFileName.Find(_T("_NGC_"));
					if (iOKImg >= 0 && bSendOK)
					{
						vFilePathList.push_back(filename);//将一个文件路径加入容器
					}
					else if (iNGImg >= 0 && bSendNG)
					{
						vFilePathList.push_back(filename);
					}
					else if (iNGCImg >= 0 && bSendNGC)
					{
						vFilePathList.push_back(filename);
					}
					else
					{
						//	continue;
					}
				}
				else
				{
					if (strFileName.Find(m_strCurrentBatch) < 0)
					{
						vFilePathList.push_back(filename);
					}
				}
			}
		}
		else
		{
			//递归遍历用户文件夹，跳过非用户文件夹
			if (!(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()))
			{
				GetFilePath(vFilePathList, filename + _T("\\"), bSendOK, bSendNG, bSendNGC, bSendCover);
			}
		}
	}
}

void CMes::GetFilePath(std::vector<CString>& vFilePathList, CString &strDir, BOOL & bSendOK, BOOL &bSendNG, BOOL & bSendNGC)
{
	CFileFind finder;
	CString strFilePath;
	strFilePath.Format(_T("%s"), strDir);
	if (strFilePath == _T(""))
	{
		return;
	}
	else
	{
		if (strFilePath.Right(1) != _T(""))
		{
			strFilePath += _T("\\");
		}
		strFilePath += _T("*.*");
	}
	BOOL isNotEmpty = finder.FindFile(strFilePath);//总文件夹，开始遍历
	while (isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();//查找文件?
		CString filename = finder.GetFilePath();//获取文件的路径，可能是文件夹，可能是文件?
		if (!(finder.IsDirectory()))
		{
			//如果是jpg.csv文件则加入文件列表			
			CString strFileName;
			CString strFileExtendName;
			strFileName = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);
			strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.Find('.') - 1);
			if (strFileExtendName == _T("bmp") || strFileExtendName == _T("jpg") || strFileExtendName == _T("csv"))
			{
				int iOKImg = 0;
				int iNGImg = 0;
				int iNGCImg = 0;
				iOKImg = strFileName.Find(_T("_OK_"));
				iNGImg = strFileName.Find(_T("_NG_"));
				iNGCImg = strFileName.Find(_T("_NGC_"));
				if (iOKImg >= 0 && bSendOK)
				{
					vFilePathList.push_back(filename);//将一个文件路径加入容器
					m_hvCurrentDisk.Append(HTuple(filename));
				}
				else if (iNGImg >= 0 && bSendNG)
				{
					vFilePathList.push_back(filename);
					m_hvCurrentDisk.Append(HTuple(filename));
				}
				else if (iNGCImg >= 0 && bSendNGC)
				{
					vFilePathList.push_back(filename);
					m_hvCurrentDisk.Append(HTuple(filename));
				}
				else
				{
					if ((strFileExtendName == _T("csv")) && (strFileName.Find(m_strCurrentBatch) < 0))
					{
						vFilePathList.push_back(filename);
						m_hvCurrentDisk.Append(HTuple(filename));
					}
				}
			}
		}
		else
		{
			//递归遍历用户文件夹，跳过非用户文件夹
			if (!(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()))
			{
				GetFilePath(vFilePathList, filename + _T("\\"), bSendOK, bSendNG, bSendNGC);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void CMes::SetMesAddrAndPort()
{
	CMesGSoap::getInstance().setMesAddrAndPort(m_strIP, m_nPort);
}

BOOL CMes::UNRename(CString & strFilePath)//删除文件名标记
{
	std::vector<CString>vecFileList;

	GetFilePath(vecFileList, strFilePath);

	for (std::vector<CString>::iterator iter = vecFileList.begin(); iter != vecFileList.end(); iter++)
	{
		if (HasBeenSent(*iter))
		{
			CString strOldFilePath;

			UnRenameFile(*iter, m_strRenameSymbol, strOldFilePath);
		}
	}
	return TRUE;
}

void CMes::GetFilePath(std::vector<CString>& vFilePathList, CString & strDir)
{
	CFileFind finder;

	if (strDir == _T(""))
	{
		return;
	}
	else
	{
		if (strDir.Right(1) != _T(""))
		{
			strDir += _T("\\");
		}
		strDir += _T("*.*");
	}

	BOOL isNotEmpty = finder.FindFile(strDir);//总文件夹，开始遍历?
	while (isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();//查找文件?
		CString filename = finder.GetFilePath();//获取文件的路径，可能是文件夹，可能是文件?
		if (!(finder.IsDirectory()))
		{
			//如果是文件则加入文件列表
			vFilePathList.push_back(filename);//将一个文件路径加入容器
		}
		else
		{
			//递归遍历用户文件夹，跳过非用户文件夹
			if (!(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()))
			{
				GetFilePath(vFilePathList, filename + _T("\\"));
			}
		}
	}
}

int CMes::CHeckPrecipeVersion(std::string & strResponse)
{
	int iRet = 0;

	neb::CJsonObject cJsonSend;

	CString cstrTimeStamp, cstrMessage;

	std::string strSendMessage, strSendUTF8, strRespon;

	SYSTEMTIME iLocalTime;

	GetLocalTime(&iLocalTime);

	cstrTimeStamp.Format(_T("%04d-%02d-%02dT%02d:%02d:%02d.%03d+08:00"),
		iLocalTime.wYear, iLocalTime.wMonth, iLocalTime.wDay, iLocalTime.wHour, iLocalTime.wMinute, iLocalTime.wSecond, iLocalTime.wMilliseconds);

	cJsonSend.Add("RecipeCode", m_strRecipeCode.GetString());

	cJsonSend.Add("ProductCode", m_strProductCode.GetString());

	cJsonSend.Add("Version", m_strRecipeVison.GetString());

	cJsonSend.Add("LineCode", m_strLineCode.GetString());

	cJsonSend.Add("SectionCode", m_strSectionCode.GetString());

	cJsonSend.Add("EquipmentCode", m_strEquipmentCode.GetString());

	cJsonSend.Add("Timestamp", cstrTimeStamp.GetString());

	strSendMessage = cJsonSend.ToString();

	strSendUTF8 = AsciiToUtf8(strSendMessage);

	iRet = MESRequest((CString)strSendUTF8.c_str(), strRespon, CheckEquipmentRecipeVersion);

	strResponse = strRespon;

	if (200 != iRet)
	{
		cstrMessage.Format(_T("CHeckPrecipeVersion;返回码：%d\t返回信息：%s\n发送信息：%s"), iRet, strRespon.c_str(), strSendMessage.c_str());

		//	m_sInit.pLogView->DoSomething("EXCEPTION", (void*)(LPCSTR)cstrMessage,NULL);

		Log(cstrMessage);

		m_record.writeHttpFile(cstrMessage);
	}

	cstrMessage.Format(_T("CHeckPrecipeVersion;返回码：%d\t返回信息：%s\n发送信息：%s"), iRet, strRespon.c_str(), strSendMessage.c_str());

	m_record.writeHttpFile(cstrMessage);

	return iRet;
}

int CMes::MESRequest(CString& strData, std::string& strRespons, eHttpAPI eApi)
{
	CString strUrl;


	strUrl.Format("http://%s:%d/api/edc/%s", m_strIP, m_nPort, g_strAPIFuncName[eApi]);

	//	strUrl.Format("http://%s:%d/api/edc/",m_strIP,m_nPort);

	return http_request(LPCSTR(strUrl), strRespons, (std::string)strData.GetString());
}

int CMes::Soap_CheckRecipeVersion(std::string &strResponse)
{
	ParamRecipeVersionExamine paramRecipeVersion;
	neb::CJsonObject cJsonRcv;

	paramRecipeVersion.cstrRecipeCode.Format("%s", m_strRecipeCode);
	paramRecipeVersion.cstrVersion.Format("%s", m_strRecipeVison);

	int iRes(-1), nCode(-11);


	iRes = Soap_recipeVersionExamine(paramRecipeVersion, cJsonRcv);
	cJsonRcv.Get("code", nCode);
	cJsonRcv.Get("msg", strResponse);

	if (SOAP_OK == iRes && 1 == nCode)
	{
		return 200;
	}
	else
	{
		return nCode;
	}
}

int CMes::Soap_recipeVersionExamine(const ParamRecipeVersionExamine& paramRecipeVersion, neb::CJsonObject & nebCJsonReciev)
{
	neb::CJsonObject nebCJsonSend;
	int res = -101;
	res = CMesGSoap::getInstance().RecipeVersionExamine(paramRecipeVersion, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		m_pSoapMesLog->RecipeVersionExamineLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		m_pSoapMesLog->RecipeVersionExamineLog(cstrMsg);

		nebCJsonReciev.Get("RcvTime", strTime);
		nebCJsonReciev.Get("Res", nRes);
		nebCJsonReciev.Get("code", nCode);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		m_pSoapMesLog->RecipeVersionExamineLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		m_pSoapMesLog->RecipeVersionExamineLog(cstrMsg);
	}
	return res;
}

int CMes::Soap_getRecipeList(const CString& cstrPruoductCode, neb::CJsonObject & nebCJsonReciev)
{
	neb::CJsonObject nebCJsonSend;
	int res = -101;
	res = CMesGSoap::getInstance().GetRecipeList(cstrPruoductCode, nebCJsonSend, nebCJsonReciev);

	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		m_pSoapMesLog->GetRecipeListLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		m_pSoapMesLog->GetRecipeListLog(cstrMsg);

		nebCJsonReciev.Get("RcvTime", strTime);
		nebCJsonReciev.Get("Res", nRes);
		nebCJsonReciev.Get("code", nCode);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		m_pSoapMesLog->GetRecipeListLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		m_pSoapMesLog->GetRecipeListLog(cstrMsg);
	}

	return res;
}

int CMes::GetRequestRecipeParameters(CString const &strRecipe, std::string &strRespons)
{
	CString strUrl;

	strUrl.Format(_T("http://%s:%d/api/edc/recipeparameterrequest?%s"), m_strIP, m_nPort, strRecipe);

	return http_getquest(LPCSTR(strUrl), strRespons);
}

int CMes::http_getquest(const char* url, std::string& strResponse, const char* headers, bool bReserveHeaders, int timeout)
{
	CURLcode res;
	//curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();
	struct MemoryStruct MemoryChunk;
	MemoryChunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	MemoryChunk.size = 0;    /* no data at this point */

	if (NULL == curl)
	{
		curl_easy_cleanup(curl);

		//curl_global_cleanup();
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);

	//响应结果中保留头部信息
	if (bReserveHeaders)
	{
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	}

	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);				//
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);//设置回调函数
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&MemoryChunk);//设置写数据	//chout		(void*)&strResponse
	curl_easy_setopt(curl, CURLOPT_POST, 0);			//http   Get  ,post 设为1														//设置超时时间
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

	//添加自定义头信息
	if (headers != NULL)
	{
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, headers);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	}
	res = curl_easy_perform(curl);
	int nCode = 0;

	if (res == CURLE_OK)		//查询错误码
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &nCode);
	else
		nCode = res;

	strResponse = MemoryChunk.memory;
	curl_easy_cleanup(curl);
	free(MemoryChunk.memory);

	//curl_global_cleanup();

	return nCode;
}

size_t CMes::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{

	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)stream;
	char *ptr1 = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
		/* out of memory! */
		//	printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr1;
	memcpy(&(mem->memory[mem->size]), ptr, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;

	/*

	std::string data((const char*) ptr, (size_t) size * nmemb);

	data = Utf8ToAscii(data);

	strcpy((char*)stream,data.c_str());

	return size * nmemb;

	*/

	/*
	if (stream == NULL || ptr == NULL || size == 0)
	return 0;

	size_t realsize = size * nmemb;

	std::string *buffer = (std::string*)stream;

	if (buffer != NULL)
	{
	buffer->append((const char *)ptr, realsize);

	*buffer = Utf8ToAscii(*buffer);
	}
	return realsize;
	*/
}

BOOL CMes::UpdateFileToServer(CString strFilePath, CString strNum)
{
	try
	{
		if (!m_bUploadFileAuto) return TRUE;

		//判断是否能连接共享文件夹
		//TCHAR szPasswd[] = TEXT("");          //共享资源授权用户的密码
		//TCHAR szUserName[] = TEXT("Administrator");        //共享资源授权的用户
		NETRESOURCE net_Resource;
		net_Resource.dwDisplayType = RESOURCEDISPLAYTYPE_DIRECTORY;
		net_Resource.dwScope = RESOURCE_CONNECTED;
		net_Resource.dwType = RESOURCETYPE_ANY;
		net_Resource.dwUsage = 0;
		net_Resource.lpComment = TEXT("");
		net_Resource.lpLocalName = TEXT("");  //映射成本地驱动器:
		net_Resource.lpProvider = NULL;
		net_Resource.lpRemoteName = (LPSTR)(LPCTSTR)m_strFtpAddr; // \\servername\共享资源名

		DWORD dwFlags = CONNECT_TEMPORARY;
		/*DWORD dw = WNetAddConnection2A(&net_Resource, m_strFtpPwd, m_strFtpUserID, dwFlags);
		if (dw != ERROR_SUCCESS)
		{
			CString strMess;
			strMess.Format("上传文件失败(错误码%d),无法连接服务器%s", dw, m_strFtpAddr);
			m_sInit.Log(strMess, AlarmLog, SlightExcp);
			return FALSE;
		}*/

		CString strFileName;      //文件名字-带扩展
		CString strFileExtendName;//仅扩展名 .csv
		CString strName;          //仅文件名 
		CString strPath, strPath2;          //仅路径，无文件名
		strFileExtendName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('.'));
		strFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		strName = strFileName.Left(strFileName.ReverseFind('.'));
		strPath = strFilePath.Left(strFilePath.GetLength() - strFileName.GetLength() - 1);
		strPath2 = strPath.Left(strPath.ReverseFind('\\'));

		CString sTime;
		SYSTEMTIME tSaveTime;
		::GetLocalTime(&tSaveTime);

		std::vector<CString> vecServerPath, vecServerFile, vecLocalFile;
		std::vector<CString> vFilePathList;
		//去除系统时间后新的遍历
		CString strServerFilePath;
		CString strServerPath;
		CString strSelectFile;

		vFilePathList.clear();
		GetFileNamePath(vFilePathList, strPath, strName);
		for (int i = 0; i < vFilePathList.size(); i++)
		{
			strFileName = vFilePathList[i].Right(vFilePathList[i].GetLength() - vFilePathList[i].ReverseFind('\\') - 1);
			//0811去掉了时间
			int	nPos = strFileName.ReverseFind('_');
			strSelectFile = strFileName.Mid(nPos + 1, strFileName.GetLength() - 1);
			if (strSelectFile == "双面测试.csv"&&tSelectUploadFile.bUploadFile == false)
			{
				continue;
			}
			if (strSelectFile == "all.csv"&&tSelectUploadFile.bUploadAllFile == false)
			{
				continue;
			}
			if (strSelectFile == "defect.csv"&&tSelectUploadFile.bUploadDefectFile == false)
			{
				continue;
			}
			if (strSelectFile == "report.csv"&&tSelectUploadFile.bUploadReportFile == true)
			{
				strServerFilePath.Format(_T("%s\\%s\\%s"), m_strReportAddr, strNum, strFileName);
				strServerPath.Format(_T("%s\\%s\\"), m_strReportAddr, strNum);
				vecServerPath.push_back(strServerPath);
				vecServerFile.push_back(strServerFilePath);
				vecLocalFile.push_back(vFilePathList[i]);
			}

			strServerFilePath.Format(_T("%s\\%s\\%s"), m_strFtpAddr, strNum, strFileName);
			strServerPath.Format(_T("%s\\%s\\"), m_strFtpAddr, strNum);
			vecServerPath.push_back(strServerPath);
			vecServerFile.push_back(strServerFilePath);
			vecLocalFile.push_back(vFilePathList[i]);
		}

		//if (tSaveTime.wDay < 7)
		//{
		//	//本月的文件遍历
		//	for (int i = 0; i <= tSaveTime.wDay - 1; i++)
		//	{
		//		sTime.Format("%04d%02d\\%02d", tSaveTime.wYear, tSaveTime.wMonth, i + 1);
		//		CString strPath;
		//		strPath.Format("%s\\%04d%02d%02d", strPath2, tSaveTime.wYear, tSaveTime.wMonth, i + 1);
		//		CString strServerFilePath;
		//		CString strServerPath;

		//		vFilePathList.clear();
		//		GetFileNamePath(vFilePathList, strPath, strName);
		//		for (int i = 0; i < vFilePathList.size(); i++)
		//		{
		//			strFileName = vFilePathList[i].Right(vFilePathList[i].GetLength() - vFilePathList[i].ReverseFind('\\') - 1);

		//			strServerFilePath.Format(_T("%s\\%s\\%s\\%s"), m_strFtpAddr, sTime, strNum, strFileName);
		//			strServerPath.Format(_T("%s\\%s\\%s\\"), m_strFtpAddr, sTime, strNum);

		//			vecServerPath.push_back(strServerPath);
		//			vecServerFile.push_back(strServerFilePath);
		//			vecLocalFile.push_back(vFilePathList[i]);
		//		}
		//	}
		//	//上个月的最后7天的遍历
		//	if (tSaveTime.wMonth == 1)
		//	{
		//		for (int i = 0; i < 7; i++)
		//		{
		//			sTime.Format("%04d%02d\\%02d", tSaveTime.wYear - 1, 12, 31 - i);
		//			CString strPath;
		//			strPath.Format("%s\\%04d%02d%02d", strPath2, tSaveTime.wYear - 1, 12, 31 - i);
		//			CString strServerFilePath;
		//			CString strServerPath;

		//			vFilePathList.clear();
		//			GetFileNamePath(vFilePathList, strPath, strName);
		//			for (int i = 0; i < vFilePathList.size(); i++)
		//			{
		//				strFileName = vFilePathList[i].Right(vFilePathList[i].GetLength() - vFilePathList[i].ReverseFind('\\') - 1);

		//				strServerFilePath.Format(_T("%s\\%s\\%s\\%s"), m_strFtpAddr, sTime, strNum, strFileName);
		//				strServerPath.Format(_T("%s\\%s\\%s\\"), m_strFtpAddr, sTime, strNum);

		//				vecServerPath.push_back(strServerPath);
		//				vecServerFile.push_back(strServerFilePath);
		//				vecLocalFile.push_back(vFilePathList[i]);
		//			}
		//		}
		//	}
		//	else
		//	{
		//		for (int i = 0; i < 7; i++)
		//		{
		//			sTime.Format("%04d%02d\\%02d", tSaveTime.wYear, tSaveTime.wMonth - 1, 31 - i);
		//			CString strPath;
		//			strPath.Format("%s\\%04d%02d%02d", strPath2, tSaveTime.wYear, tSaveTime.wMonth - 1, 31 - i);
		//			CString strServerFilePath;
		//			CString strServerPath;

		//			vFilePathList.clear();
		//			GetFileNamePath(vFilePathList, strPath, strName);
		//			for (int i = 0; i < vFilePathList.size(); i++)
		//			{
		//				strFileName = vFilePathList[i].Right(vFilePathList[i].GetLength() - vFilePathList[i].ReverseFind('\\') - 1);

		//				strServerFilePath.Format(_T("%s\\%s\\%s\\%s"), m_strFtpAddr, sTime, strNum, strFileName);
		//				strServerPath.Format(_T("%s\\%s\\%s\\"), m_strFtpAddr, sTime, strNum);

		//				vecServerPath.push_back(strServerPath);
		//				vecServerFile.push_back(strServerFilePath);
		//				vecLocalFile.push_back(vFilePathList[i]);
		//			}
		//		}
		//	}
		//}
		//else
		//{
		//	for (int i = 0; i < 7; i++)
		//	{
		//		sTime.Format("%04d%02d\\%02d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay - i);

		//		CString strPath;          //仅路径，无文件名
		//		strPath.Format("%s\\%04d%02d%02d", strPath2, tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay - i);
		//		CString strServerFilePath;
		//		CString strServerPath;
		//		
		//		vFilePathList.clear();
		//		GetFileNamePath(vFilePathList, strPath, strName);
		//		for (int i = 0; i < vFilePathList.size(); i++)
		//		{
		//			strFileName = vFilePathList[i].Right(vFilePathList[i].GetLength() - vFilePathList[i].ReverseFind('\\') - 1);

		//			strServerFilePath.Format(_T("%s\\%s\\%s\\%s"), m_strFtpAddr, sTime, strNum, strFileName);
		//			strServerPath.Format(_T("%s\\%s\\%s\\"), m_strFtpAddr, sTime, strNum);

		//			vecServerPath.push_back(strServerPath);
		//			vecServerFile.push_back(strServerFilePath);
		//			vecLocalFile.push_back(vFilePathList[i]);
		//		}
		//	}
		//}

		for (int i = 0; i < vecLocalFile.size(); i++)
		{
			if (i > vecServerFile.size() - 1 || i > vecServerPath.size() - 1) continue;
			BOOL bExist = FALSE;
			if (!PathFileExistsA(vecServerPath[i])) CreateMultilevelPath(vecServerPath[i]);

			if (!CopyFileA(vecLocalFile[i], vecServerFile[i], bExist))
			{
				CString strMess;
				strMess.Format("上传文件至服务器端失败,%s", vecServerFile[i]);
				m_sInit.Log(strMess, AlarmLog, SlightExcp);
				return FALSE;
			}
		}
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}

	//return UploadCcdFileOnly(fileName); //FTP格式上传
	return TRUE;
}

int CMes::Soap_getRecipe(const CString& cstrRecipeCode, neb::CJsonObject & nebCJsonReciev)
{
	neb::CJsonObject nebCJsonSend;
	int res = -101;
	res = CMesGSoap::getInstance().GetRecipe(cstrRecipeCode, nebCJsonSend, nebCJsonReciev);

	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		m_pSoapMesLog->GetRecipeLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		m_pSoapMesLog->GetRecipeLog(cstrMsg);

		nebCJsonReciev.Get("RcvTime", strTime);
		nebCJsonReciev.Get("Res", nRes);
		nebCJsonReciev.Get("code", nCode);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		m_pSoapMesLog->GetRecipeLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		m_pSoapMesLog->GetRecipeLog(cstrMsg);
	}

	return res;
}

int CMes::MESReportEquipmentRecipeParametersInfo(CString& strData, std::string &strRespons)
{
	CString strUrl;

	strUrl.Format("http://%s:%d/api/edc/recipeparameter", m_strIP, m_nPort);

	return http_request(LPCSTR(strUrl), strRespons, (std::string)strData.GetString(), "Content-Type:application/json;charset=UTF-8", false);
}

int CMes::Soap_recipe(const ParamRecipe& pRecipe, neb::CJsonObject & nebCJsonReciev)
{
	neb::CJsonObject nebCJsonSend;
	int res = -101;
	res = CMesGSoap::getInstance().Recipe(pRecipe, nebCJsonSend, nebCJsonReciev);

	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		m_pSoapMesLog->RecipeLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		m_pSoapMesLog->RecipeLog(cstrMsg);

		nebCJsonReciev.Get("RcvTime", strTime);
		nebCJsonReciev.Get("Res", nRes);
		nebCJsonReciev.Get("code", nCode);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		m_pSoapMesLog->RecipeLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		m_pSoapMesLog->RecipeLog(cstrMsg);
	}
	return res;
}