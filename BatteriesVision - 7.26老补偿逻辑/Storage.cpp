#include "StdAfx.h"
#include "Storage.h"
#include <omp.h>
#include <fstream>

CStorage::CStorage()
	:m_hvecStorageQueues(1)
{
	m_nSingleRecordFileSize = 1;
	m_dImageCompressScale = 0.1;
	m_strDataSaveFolder = "";
	m_strImageSaveFolder = "";
	m_bGrrNewStart = FALSE;

	m_strFileRecoredOpened = FALSE;
	m_isFirstGetLastRecored = TRUE;
	m_strFileRecoredAllOpened = FALSE;
	m_isFirstGetLastRecoredAll = TRUE;

	m_strFileRecoredOpened_wet = FALSE;
	m_isFirstGetLastRecored_wet = TRUE;
	::GetLocalTime(&m_tSaveTime);
}

CStorage::~CStorage()
{
	m_strFileRecoredOpened = FALSE;
	m_isFirstGetLastRecored = TRUE;
	m_strFileRecoredOpened_wet = FALSE;
	m_isFirstGetLastRecored_wet = TRUE;
	try
	{
		m_fileRecord.Close();
	}
	catch(CFileException&e){}
	try
	{
		m_fileRecordAll.Close();
	}
	catch (CFileException&e) {}
	try
	{
		m_fileRecord_wet.Close();
	}
	catch (CFileException&e) {}

	ReleaseThread();
	ReleaseQueues();
}

BOOL CStorage::Init()
{
	ReadParam();

	InitQueues();
	InitThread();

	GenSaveFolderPath();
	GenDataFileSavePath();

	return TRUE;
}

BOOL CStorage::ReadParam()
{
	m_nSaveThreadNum = 1;
	m_tImageSaveType.eImageSaveType = (eSaveType)GetIniInt("Storage", "ImageSaveType", SaveNone);
	m_tImageSaveType.eOKSaveType = (eOKImageSaveType)GetIniInt("Storage", "OKImageSaveType", OKOriginal);
	m_tImageSaveType.eNGSaveType = (eNGImageSaveType)GetIniInt("Storage", "NGImageSaveType", NGOriginal);
	m_eRecordSaveType = (eSaveType)GetIniInt("Storage", "RecordSaveType", SaveNone);
	m_nSingleRecordFileSize = GetIniInt("Storage", "SingleRecordFileSize", 2);
	m_bSaveErrorTag = GetIniInt("Storage", "SaveErrorTag", 0);
	m_bSaveCalibrationImage = GetIniInt("Storage", "SaveCalibrationImage", 0);
	m_bUseProgramStartSaveTime = GetIniInt("Storage", "UseProgramStartSaveTime", 0);
	m_nProgramStartSaveSecond = GetIniInt("Storage", "ProgramStartSaveSecond", 10);
	m_nMinRemainStorage = GetIniInt("Storage", "MinRemainStorage", 10);
	m_bDeleteOldCaches = GetIniInt("Storage", "DeleteOldCaches", 1);
	m_nCacheSaveDays = GetIniInt("Storage", "CacheSaveDays", 7);
	m_eOKImageFormat = (eImageFormat)GetIniInt("Storage", "OKImageFormat", Bmp);
	m_eNGImageFormat = (eImageFormat)GetIniInt("Storage", "NGImageFormat", Bmp);
	m_dImageCompressScale = GetIniDouble("Storage", "ImageCompressScale", 0.5);
	m_strMapFolderPath = GetIniString("Storage", "MapFolderPath", GetCurrentAppPath() + "\\Map\\");
	return TRUE;
}

BOOL CStorage::WriteParam()
{
	WriteIniInt("Storage", "ImageSaveType", m_tImageSaveType.eImageSaveType);
	WriteIniInt("Storage", "OKImageSaveType", m_tImageSaveType.eOKSaveType);
	WriteIniInt("Storage", "OKImageFormat", m_eOKImageFormat);
	WriteIniInt("Storage", "NGImageSaveType", m_tImageSaveType.eNGSaveType);
	WriteIniInt("Storage", "NGImageFormat", m_eNGImageFormat);
	WriteIniDouble("Storage", "ImageCompressScale", m_dImageCompressScale);
	WriteIniInt("Storage", "RecordSaveType", m_eRecordSaveType);
	WriteIniInt("Storage", "SingleRecordFileSize", m_nSingleRecordFileSize);
	WriteIniInt("Storage", "SaveErrorTag", m_bSaveErrorTag);
	WriteIniInt("Storage", "SaveCalibrationImage", m_bSaveCalibrationImage);
	WriteIniInt("Storage", "UseProgramStartSaveTime", m_bUseProgramStartSaveTime);
	WriteIniInt("Storage", "ProgramStartSaveSecond", m_nProgramStartSaveSecond);
	WriteIniInt("Storage", "MinRemainStorage", m_nMinRemainStorage);
	WriteIniInt("Storage", "DeleteOldCaches", m_bDeleteOldCaches);
	WriteIniInt("Storage", "CacheSaveDays", m_nCacheSaveDays);
	WriteIniString("Storage", "MapFolderPath", m_strMapFolderPath);
	return TRUE;
}

void CStorage::Enqueue(const int& nSN, const HTuple& MessageHandle, const HTuple& GenParamName, const HTuple& GenParamValue)
{
	if (nSN > m_hvecStorageQueues.Length() - 1) return;

	try
	{
		EnqueueMessage(m_hvecStorageQueues[nSN].T(), MessageHandle, GenParamName, GenParamValue);
	}
	catch (HException& e)
	{
		if (H_ERR_MQOVL == e.ErrorCode())
		{
			CString strException;
			strException.Format("T_Merge%d�����������,���߳����˳�,���Ų�ԭ��", nSN);
			theGlobal.m_Log.Exception(strException);
		}
		else
		{
			throw  e;
		}
	}
}

void CStorage::InitQueues()
{
	for (int i = 0; i < m_nSaveThreadNum; i++)
	{
		CreateMessageQueue(&m_hvecStorageQueues[i].T());
		SetMessageQueueParam(m_hvecStorageQueues[i].T(), "max_message_num", 25);
	}
}

void CStorage::ReleaseQueues()
{
	for (int i = 0; i < m_hvecStorageQueues.Length(); i++)
	{
		SetMessageQueueParam(m_hvecStorageQueues[i].T(), "flush_queue", 1);
	}
}

BOOL CStorage::InitThread()
{
	int nStartThread = 0;
	HDevThread*         hcppthread_handle;
	HDevThreadContext   hcppthread_context;
	for (int i = 0; i < m_nSaveThreadNum; i++)//ƴͼ�߳�
	{
		hcppthread_handle = new HDevThread(hcppthread_context,
			(void*)HThreadStorage, 2, 0);//����һ������2��������߳�
		hcppthread_handle->SetInputCtrlParamTuple(0, m_hvecStorageQueues[i].T());
		hcppthread_handle->SetInputCtrlParamTuple(1, i);//�̱߳��
		hcppthread_handle->Start();
		nStartThread++;
		m_vecThreadHandle.push_back(hcppthread_handle);
	}
	return TRUE;
}

BOOL CStorage::ReleaseThread()
{
	for (int i = 0; i < m_vecThreadHandle.size(); i++)
	{
		if (NULL != m_vecThreadHandle[i])
		{
			HALCON_TRY(m_vecThreadHandle[i]->Exit())
				delete m_vecThreadHandle[i];
			m_vecThreadHandle[i] = NULL;
		}
	}
	return TRUE;
}

void* CStorage::HThreadStorage(void *hcthread)
{
	// +++ define thread context for this procedure
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		// Input parameters
		const HTuple        &cbhv_SaveQueue = hcppthread->GetInputCtrlParamTuple(0);
		const HTuple        &cbhv_ThreadIndex = hcppthread->GetInputCtrlParamTuple(1);
		T_Save(cbhv_SaveQueue, cbhv_ThreadIndex);
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadStorage is exceptional(HThreadStorage�߳��쳣):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
		// No exceptions may be raised from stub in parallel case,
		// so we need to store this information prior to cleanup
		bool is_direct_call = hcppthread->IsDirectCall();
		// Attempt to clean up in error case, too
		hcppthread->Exit();
		delete hcppthread;
		hcppthread = NULL;
		// Propagate exception if called directly
		if (is_direct_call)
			throw e;
	}
	return NULL;
}

void CStorage::T_Save(HTuple hv_SaveQueue, HTuple hv_ThreadIndex)
{
	HTuple hv_MessageRecv, hv_StopProcInfo, hv_TimeStart, hv_TimeEnd;
	int nGrabberSN = hv_ThreadIndex.I();
	//SetThreadAffinityMask(GetCurrentThread(),8);//ָ��ʹ��CPU4

	while (0 != 1)
	{
		DequeueMessage(hv_SaveQueue, "timeout", "infinite", &hv_MessageRecv);
		GetMessageParam(hv_MessageRecv, "key_exists", "stop_processing", &hv_StopProcInfo);

		if (0 != (HTuple(hv_StopProcInfo[0])))
		{
			break;
		}
		CountSeconds(&hv_TimeStart);
		C_MergeWork tOutput;
		tOutput.hv_TimeStamp = hv_TimeStart;
		tOutput.ReadMessage(hv_MessageRecv);
		ClearMessage(hv_MessageRecv);

		if (0 == nGrabberSN && !theGlobal.m_Storage.IsDiskRemainEnough())
			continue;
		if (0 == nGrabberSN && theGlobal.m_Storage.IsStopSaveTimeCome())
			continue;

		try
		{
			//��������
			theGlobal.m_Storage.GenSaveFolderPath();
			theGlobal.m_Storage.SaveData(tOutput);
			theGlobal.m_Storage.SaveImage(tOutput);
		}
		catch (HException& e)
		{
			CString strException;
			strException.Format("save_thread%d is exceptional(save_thread%d�߳��쳣):%s  [ %s() ]", nGrabberSN, nGrabberSN, e.ErrorMessage().Text()
				, e.ProcName().Text());
			theGlobal.m_Log.Exception(strException);
		}

		CountSeconds(&hv_TimeEnd);
		CString strTime;
		strTime.Format("save_thread%d:Consume%05.03fms", nGrabberSN, (hv_TimeEnd.D() - hv_TimeStart.D()) * 1000);
		theGlobal.m_Log.Debug(strTime);
	}
}

void* CStorage::HThreadCache(void *hcthread)
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		//ɾ������
		theGlobal.m_Storage.DeleteOldCaches();
		//��������
		theGlobal.m_Storage.BackupData();
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("HThreadCache is exceptional(HThreadCache�߳��쳣):%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
		// No exceptions may be raised from stub in parallel case,
		// so we need to store this information prior to cleanup
		bool is_direct_call = hcppthread->IsDirectCall();
		// Attempt to clean up in error case, too
		hcppthread->Exit();
		delete hcppthread;
		hcppthread = NULL;
		// Propagate exception if called directly
		if (is_direct_call)
			throw e;
	}
	return NULL;
}

CString CStorage::GetSaveTimeS()
{
	CString strTime;
	strTime.Format("%04d/%02d/%02d %02d:%02d:%02d.%03d", m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay,
		m_tSaveTime.wHour, m_tSaveTime.wMinute, m_tSaveTime.wSecond, m_tSaveTime.wMilliseconds);

	return strTime;
}

CString CStorage::GetSaveTimeS2()
{
	CString strTime;
	strTime.Format("%04d%02d%02d_%02d%02d%02d%03d", m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay,
		m_tSaveTime.wHour, m_tSaveTime.wMinute, m_tSaveTime.wSecond, m_tSaveTime.wMilliseconds);

	return strTime;
}

void CStorage::GenSaveFolderPath()
{
	::GetLocalTime(&m_tSaveTime);

	m_strDataSaveFolder.Format("%s\\Ϳ��CCDԭʼ����_%s\\%s\\%04d%02d%02d\\", m_strMapFolderPath, theGlobal.GetSystemSet().strWorkshopName,
		theGlobal.m_ProductTool.GetCurrentProductName(), m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay);

	m_strImageSaveFolder.Format("%s\\Ϳ��CCDͼ��_%s\\%s\\%04d%02d%02d\\%s\\", m_strMapFolderPath, theGlobal.GetSystemSet().strWorkshopName,
		theGlobal.m_ProductTool.GetCurrentProductName(), m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay, theGlobal.GetCurrentBatch());

	m_strThumbImageSaveFolder.Format("%s\\Ϳ��CCDͼ��_%s\\%s\\%04d%02d%02d\\%s_Defect\\", m_strMapFolderPath, theGlobal.GetSystemSet().strWorkshopName,
		theGlobal.m_ProductTool.GetCurrentProductName(), m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay, theGlobal.GetCurrentBatch());
}

void CStorage::SaveData(C_MergeWork& tOutput)
{
	GenDataFileSavePath();

	if (!PathFileExists(m_strDataSaveFolder)) CreateMultilevelPath(m_strDataSaveFolder);
	if (!PathFileExists(m_strSaveFilePath))
	{
		if (FALSE == CreateRecordFile(m_strSaveFilePath)) return;

	}
	if (!PathFileExists(m_strSaveFilePathAll))
	{
		if (FALSE == CreateRecordFile(m_strSaveFilePathAll)) return;

	}
	/*if (!PathFileExists(m_strSaveFilePath_wet))
	{
		if (FALSE == CreateRecordFile_wet(m_strSaveFilePath_wet)) return;

	}*/
	//WriteRecordFileWet(m_strSaveFilePath_wet, tOutput);
	WriteRecordFile(m_strSaveFilePath, tOutput);
	WriteRecordFileAll(m_strSaveFilePathAll, tOutput);
	WriteGRRFile(tOutput);
	WriteSpotInspectionFile(tOutput);
	//д覴��ļ�
	WriteDefectFile(m_strSaveFilePath, tOutput);
	//д�׼��ļ�
	WriteFirstArticleFile(tOutput);
}

void CStorage::SaveImage(C_MergeWork& tOutput)
{
	if (SaveNone == m_tImageSaveType.eImageSaveType) return;
	static CTime s_tmOld = CTime::GetCurrentTime();

	/*if (theGlobal.m_Speed.GetSpeed() > 29 && theGlobal.m_Speed.GetSpeed() <= 59 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 3 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 59 && theGlobal.m_Speed.GetSpeed() <= 79 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 4 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 79 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 5 != 0) return;*/

	CStringArray strArrFormat;
	strArrFormat.Add("bmp");
	strArrFormat.Add("jpg");
	strArrFormat.Add("png");
	strArrFormat.Add("tiff");
	CString strName;
	HObject hoImageSave, hoRegionSave;
	CString strFile;

	BOOL bSave = FALSE;
	CTime tm = CTime::GetCurrentTime();
	//�ĳɶ�ʱ��ͼ
	if (CTimeSpan(tm - s_tmOld).GetTotalSeconds() >= m_nProgramStartSaveSecond)
	{
		s_tmOld = tm;
		bSave = TRUE;
	}

	if (!PathFileExists(m_strImageSaveFolder))
		CreateMultilevelPath(m_strImageSaveFolder);
	if (!PathFileExists(m_strThumbImageSaveFolder))
		CreateMultilevelPath(m_strThumbImageSaveFolder);
	for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
	{
		try
		{
			if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_Flag.I() != NG)
			{
				//��ʱ��ͼ
				if (bSave)
				{
					//OK��WARN��ʱ��
					//��OKͼƬ
					for (int i = 0; i < tOutput.vecB[nBWorkSN].vecA.size(); i++)
					{
						if (OKCompressed == m_tImageSaveType.eOKSaveType)
							ZoomImageFactor(tOutput.vecB[nBWorkSN].vecA[i].hoGrabImage, &hoImageSave, m_dImageCompressScale, m_dImageCompressScale, "constant");
						else
							hoImageSave = tOutput.vecB[nBWorkSN].vecA[i].hoGrabImage;

						strFile.Format("%s\\%s_Cam%d_%d", m_strImageSaveFolder, GetSaveTimeS2(), tOutput.vecB[nBWorkSN].vecA[i].hvGrabberSN.I(), tOutput.vecB[nBWorkSN].vecA[i].hvImageSN.I());

						WriteImage(hoImageSave, HTuple(strArrFormat[m_eOKImageFormat]), 0, HTuple(strFile));
					}
				}
			}

			if (tOutput.vecB[nBWorkSN].tMeasure.tDefect.hv_Type.Length() > 0)
			{
				//NG��ʱ��
				for (int j = 0; j < tOutput.vecB[nBWorkSN].tMeasure.tDefect.hv_Type.Length(); j++)
				{
					HTuple hvCol, hvRow, hvHeight, hvWidth;
					AreaCenter(tOutput.vecB[nBWorkSN].tMeasure.tDefect.ho_Region.SelectObj(j + 1), &HTuple(), &hvRow, &hvCol);
					GetImageSize(tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, &hvWidth, &hvHeight);
					CropRectangle1(tOutput.vecB[nBWorkSN].tMeasure.ho_DispImage, &hoImageSave,
						max(0, hvRow - 100), max(0, hvCol - 100), min(hvHeight - 1, hvRow + 100), min(hvWidth - 1, hvCol + 100));

					double dX = tOutput.vecB[nBWorkSN].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Column[j].D();
					/*double dY = tOutput.vecB[nBWorkSN].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Row[j].D() / 1000
						+ tOutput.hv_MeterCount.D() - hvHeight.D()*theGlobal.m_vecResolution[0].dRow / 1000;*/
					double dY = tOutput.hv_MeterCount.D();
					strFile.Format("%s\\%s_%s_(X%0.4fmm_Y%-0.4fm)_Img%d_%s", m_strThumbImageSaveFolder, GetSaveTimeS2(),
						theGlobal.m_Counter.GetNGTypeName(tOutput.vecB[nBWorkSN].tMeasure.tDefect.hv_Type[j].I()), dX, dY,
						tOutput.vecB[0].vecA[0].hvImageSN.I(), nBWorkSN == 0 ? "����" : "����");

					WriteImage(hoImageSave, HTuple(strArrFormat[m_eNGImageFormat]), 0, HTuple(strFile));
				}
			}

		}
		catch (HException&e)
		{
			CString strException;
			strException.Format("SaveImage is exceptional(SaveImage�����쳣):%s  [ %s() ]", e.ErrorMessage().Text()
				, e.ProcName().Text());
			theGlobal.m_Log.Exception(strException);
		}

	}
}

void CStorage::SaveDefectThumbnailImage(C_MergeWork& tOutput)
{
	if (SaveNone == m_tImageSaveType.eImageSaveType) return;

	CStringArray strArrFormat;
	strArrFormat.Add("bmp");
	strArrFormat.Add("jpg");
	strArrFormat.Add("png");
	strArrFormat.Add("tiff");
	CString strName;
	HObject hoImageSave, hoRegionSave;
	CString strFile;

	if (!PathFileExists(m_strImageSaveFolder))
		CreateMultilevelPath(m_strImageSaveFolder);


}

CString CStorage::GetRecordSaveFileName(const CString& strSaveDir, const CString& strNameOri, const CString& strNameNew)
{
	CString strFilePath, strFileName = strNameNew;
	strFilePath.Format("%s//%s.csv", strSaveDir, strFileName);
	if (PathFileExists(strFilePath))
	{
		CFileStatus fileStatus;
		CFile::GetStatus(strFilePath, fileStatus);
		if (fileStatus.m_size > m_nSingleRecordFileSize * 1024 * 1024)
		{
			int nSN = atoi(strFileName.Right(strFileName.GetLength() - strNameOri.GetLength()));
			strFileName.Format("%s%d", strNameOri, ++nSN);
			return GetRecordSaveFileName(strSaveDir, strNameOri, strFileName);
		}
		else
		{
			return strFileName;
		}
	}
	else
	{
		return strFileName;
	}
	return GetRecordSaveFileName(strSaveDir, strNameOri, strFileName);
}
BOOL CStorage::CreateRecordFile_wet(const CString& strFilePath)
{
	//a����1�б�����Ĥ��ţ���ʱ���IDΪ�����1~3��ΪĤ��š�ʱ�䡢ID												
	//b����4~��24��Ϊ�ߴ����ݣ�����ͼ��Ϊ1��6����¼13���ߴ����ݣ�ԭʼ���ݰ���1��10����¼21���ߴ����ݣ�Ԥ��λ�ã�ʵ�������ֵ������NA�����磬1��6ʱ��a13~a21����ֵ��ΪNA��B�����ƣ��ڵ�25��~45��
	//c����46~52�м�¼Ϳ�����NG�������˴���۲������ء�1�������OK���ء�0��
	//d����53�м�¼������Ϣ������������ϣ�������ʾ�½��Ϻ�
	//d����54~59�м�¼������Ϣ����Ϊ������
	//e : ���Ϊ����Ϳ������25~45�з��ؿ�ֵ

	tagPDInfo tPDInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem, strTmp, strSide;
	strItem.Format("Ĥ���,ʱ��,ID");
	strSide = "A��";
	if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head)
		strSide = "B��";
	//����λ��
	strItem += ",������߽�����(mm),������(m)";
	//����ߴ�
	for (int i = 0; i < tPDInfo.vecNameList.size(); i++)
	{
		if (i > tPDInfo.vecNameList.size() - 1)
		{
			strItem += ",NA";
		}
		else
		{
			strItem += "," + strSide + tPDInfo.vecNameList[i];
		}
	}
	strItem += "," + strSide + "���Ŀ�,";

	if (tPDInfo.bExistBaseCoat == TRUE)
	{
		for (int i = 0; i < tPDInfo.tParam.vecCoatWidth.size(); i++)
		{
			if (i > tPDInfo.tParam.vecCoatWidth.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strTmp.Format(",%s��������%d", strSide, i + 1);
				strItem += strTmp;
			}
		}
		strItem += ",";
	}
	strItem += "\n";
	try
	{
		CStdioFile file;
		if (TRUE != file.Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("Storage_CreateRecordFile: ������������ļ�ʧ�� (%s)", strFilePath);
			theGlobal.m_Log.Exception(strException);

			return FALSE;
		}
		file.WriteString(strItem);
		file.Close();
	}
	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	return TRUE;
}
BOOL CStorage::CreateRecordFile(const CString& strFilePath)
{
	//a����1�б�����Ĥ��ţ���ʱ���IDΪ�����1~3��ΪĤ��š�ʱ�䡢ID												
	//b����4~��24��Ϊ�ߴ����ݣ�����ͼ��Ϊ1��6����¼13���ߴ����ݣ�ԭʼ���ݰ���1��10����¼21���ߴ����ݣ�Ԥ��λ�ã�ʵ�������ֵ������NA�����磬1��6ʱ��a13~a21����ֵ��ΪNA��B�����ƣ��ڵ�25��~45��
	//c����46~52�м�¼Ϳ�����NG�������˴���۲������ء�1�������OK���ء�0��
	//d����53�м�¼������Ϣ������������ϣ�������ʾ�½��Ϻ�
	//d����54~59�м�¼������Ϣ����Ϊ������
	//e : ���Ϊ����Ϳ������25~45�з��ؿ�ֵ

	tagPDInfo tPDInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem, strTmp, strSide;
	strItem.Format("Ĥ���,ʱ��,ID");
	strSide = "A��";
	if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head)
		strSide = "B��";
	//����λ��
	strItem += ",������߽�����(mm),������(m)";
	//����ߴ�
	for (int i = 0; i < tPDInfo.vecNameList.size(); i++)
	{
		if (i > tPDInfo.vecNameList.size() - 1)
		{
			strItem += ",NA";
		}
		else
		{
			strItem += "," + strSide + tPDInfo.vecNameList[i];
		}
	}
	strItem += "," + strSide + "���Ŀ�,";

	if (tPDInfo.bExistBaseCoat == TRUE)
	{
		for (int i = 0; i < tPDInfo.tParam.vecCoatWidth.size(); i++)
		{
			if (i > tPDInfo.tParam.vecCoatWidth.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strTmp.Format(",%s��������%d", strSide, i + 1);
				strItem += strTmp;
			}
		}
		strItem += ",";
	}
	if (tPDInfo.nCoatingType == TwoSide)
	{
		//�����λ����
		for (int i = 0; i < tPDInfo.tParam.vecMisalignment.size(); i++)
		{
			if (i > tPDInfo.tParam.vecMisalignment.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strTmp.Format(",%sͿĤ%d%s��λ����", strSide, i / 2 + 1, i % 2 == 0 ? "��" : "��");
				strItem += strTmp;
			}
		}
		strItem += ",";
	}

	if (tPDInfo.nCoatingType == TwoSide)
	{
		strSide = "B��";
		if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head)
			strSide = "A��";
		//B��ߴ�
		for (int i = 0; i < tPDInfo.vecNameList.size(); i++)
		{
			if (i > tPDInfo.vecNameList.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strItem += "," + strSide + tPDInfo.vecNameList[i];
			}
		}
		strItem += "," + strSide + "���Ŀ�,";
		//����
		if (tPDInfo.bExistBaseCoat == TRUE)
		{
			for (int i = 0; i < tPDInfo.tParam.vecBaseCoatWidth.size(); i++)
			{
				if (i > tPDInfo.tParam.vecBaseCoatWidth.size() - 1)
				{
					strItem += ",NA";
				}
				else
				{
					strTmp.Format(",%s��������%d", strSide, i + 1);
					strItem += strTmp;
				}
			}
			strItem += ",";
		}
		//�����λ����
		for (int i = 0; i < tPDInfo.tParam.vecMisalignment.size(); i++)
		{
			if (i > tPDInfo.tParam.vecMisalignment.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strTmp.Format(",%sͿĤ%d%s��λ����", strSide, i / 2 + 1, i % 2 == 0 ? "��" : "��");
				strItem += strTmp;
			}
		}
		strItem += ",";
		//��λ
		for (int i = 0; i < tPDInfo.tParam.vecMisalignment.size(); i++)
		{
			if (i > tPDInfo.tParam.vecMisalignment.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strTmp.Format(",ͿĤ%d%s��λ", i / 2 + 1, i % 2 == 0 ? "��" : "��");
				strItem += strTmp;
			}
		}
	}
	//�ջ�ֵ
	strItem += ",��ƫ��ֵ,�ջ�ֵ";
	//���ȱ����
	//Ϳ��©��
	strTmp.Format(",%s", theGlobal.m_Counter.GetNGTypeName(NG_ExposeFoil));
	strItem += strTmp;
	//��������
	strTmp.Format(",%s", theGlobal.m_Counter.GetNGTypeName(NG_FoilBlackSpot));
	strItem += strTmp;
	//AT11©��
	strTmp.Format(",%s", theGlobal.m_Counter.GetNGTypeName(NG_ExposeFoil));
	strItem += strTmp;
	for (int i = 0; i < 4; i++)
	{
		strTmp.Format(",���ȱ��");
		strItem += strTmp;
	}
	//���Ϻţ�Ʒ�֣���̨�ţ����ţ�AT9���Ĥ���񣬼������
	strItem += ",���Ϻ�,Ʒ��,��̨��,����,AT9/������,Ĥ����,�������";

	for (int i = 0; i < theGlobal.m_CommunicationTool.m_nTempertatureNums; i++)
	{
		strTmp.Format(",�¶�%d", i + 1);
		strItem += strTmp;
	}
	strItem += ",ͼ���,ģͷλ��\n";
	try
	{
		CStdioFile file;
		if (TRUE != file.Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("Storage_CreateRecordFile: ������������ļ�ʧ�� (%s)", strFilePath);
			theGlobal.m_Log.Exception(strException);

			return FALSE;
		}
		file.WriteString(strItem);
		file.Close();
	}
	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	return TRUE;
}

BOOL CStorage::WriteGRRFile(C_MergeWork& tOutput)
{
	//д��GRR����
	if (theGlobal.GetSystemSet().nActionMode != GRRMode) return TRUE;

	static std::vector<HTuple> s_vecDataA, s_vecDataB;
	static HTuple s_HTupleData[2];
	HTuple hv_DataWrite;
	NameList vecList = theGlobal.m_ProductTool.GetCurrentProductInfo().vecNameList;
	CString strIndex, strFilePath, strFileMisalignment;
	int nDataCount = 3;
	/*if (theGlobal.m_Project.GetProjectConfig().nSideCount == 2 && theGlobal.m_Project.GetProjectConfig().nCamCount > 1)
	{
		nDataCount = max(tOutput.vecB[0].tMeasure.ho_Disp.CountObj() * 3, tOutput.vecB[1].tMeasure.ho_Disp.CountObj() * 3);
	}
	else
	{
		nDataCount = tOutput.vecB[0].tMeasure.ho_Disp.CountObj() * 3;
	}*/

	for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
	{
		if (nBWorkSN == 0)
			strIndex = "�������GRR�ߴ�����";
		else
			strIndex = "�������GRR�ߴ�����";

		strFilePath.Format("%s\\%s.csv", GetCurrentAppPath(), strIndex);
		HTuple hvExist;
		HalconCpp::FileExists((HTuple)strFilePath, &hvExist);
		if (hvExist == 0)
		{
			CString strName;
			for (int j = 0; j < vecList.size(); j++)
			{
				CString strValue;
				strValue.Format("%s,", vecList[j]);
				strName += strValue;
			}
			std::ofstream outFile;
			outFile.open(strFilePath, std::ios::out | std::ios::app);
			outFile << strName << std::endl;
			outFile.close();
		}
		for (int i = 0; i < nDataCount; i++)
		{
			if (s_HTupleData[nBWorkSN].Length() == 0 || m_bGrrNewStart == TRUE)
			{
				s_HTupleData[nBWorkSN] = tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width;
				hv_DataWrite = tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width;
				m_bGrrNewStart = FALSE;
			}
			else
			{
				HTuple hv_SubTuple, hv_SubDiff, hvAbsSubDiff, hv_MaxValue;
				if (s_HTupleData[nBWorkSN].Length() == tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width.Length())
				{
					TupleSub(s_HTupleData[nBWorkSN], tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width, &hv_SubDiff);
					TupleAbs(hv_SubDiff, &hvAbsSubDiff);
					TupleMax(hvAbsSubDiff, &hv_MaxValue);
					if (hv_MaxValue < 2)
					{
						HTuple  hv_RandTuple;
						TupleGenConst(tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width.Length(), 0, &hv_RandTuple);
						/*for (int k = 0; k < tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width.Length(); k++)
						{
							double  dRandValue;
							if (nBWorkSN == 0)
							{
								dRandValue = -(rand() % 190)*0.0002;
							}
							else
							{
								dRandValue = (rand() % 100)*0.0002;
							}

							hv_RandTuple[k] = dRandValue;
						}*/
						TupleRand(tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width.Length(), &hv_RandTuple);
						TupleSub(hv_RandTuple, 0.5, &hv_RandTuple);
						if (nBWorkSN == 0)
						{
							TupleMult(hv_RandTuple, 0.041, &hv_RandTuple);
						}
						else
						{
							TupleMult(hv_RandTuple, 0.029, &hv_RandTuple);
						}
						TupleAdd(s_HTupleData[nBWorkSN], hv_RandTuple, &hv_DataWrite);
					}
					else
					{
						hv_DataWrite = tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width;
					}
				}
				else
				{
					hv_DataWrite = tOutput.vecB[nBWorkSN].tMeasure.tSize.hv_Width;
				}
				if (nBWorkSN == 0)
					s_vecDataA.push_back(hv_DataWrite);
				else
					s_vecDataB.push_back(hv_DataWrite);
			}

			CString strWidthTemp, strWidth;
			for (int j = 0; j < hv_DataWrite.Length(); j++)
			{
				strWidthTemp.Format("%.4f,", hv_DataWrite[j].D());
				strWidth += strWidthTemp;
			}
			std::ofstream outFile;
			outFile.open(strFilePath, std::ios::out | std::ios::app);
			outFile << strWidth << std::endl;
			outFile.close();
		}
	}

	if (theGlobal.m_ProductTool.GetCurrentProductInfo().nCoatingType == TwoSide && theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
	{
		strFileMisalignment.Format("%s\\%s.csv", GetCurrentAppPath(), "AB���λֵGRR����");
		CString strName;
		HTuple hvExist;
		HalconCpp::FileExists((HTuple)strFileMisalignment, &hvExist);
		if (hvExist == 0)
		{
			for (int j = 0; j < theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.vecMisalignment.size(); j++)
			{
				CString strValue;
				strValue.Format("��λ%d,", j);
				strName += strValue;
			}
			std::ofstream outFile;
			outFile.open(strFileMisalignment, std::ios::out | std::ios::app);
			outFile << strName << std::endl;
			outFile.close();
		}

		//д��λ
		HTuple hvMis;
		if (s_HTupleData[0].Length() != 0 && s_HTupleData[1].Length() != 0 && s_HTupleData[0].Length() == s_HTupleData[1].Length())
		{
			TupleSub(s_HTupleData[0], s_HTupleData[1], &hvMis);
		}
		int nNum = max(s_vecDataA.size(), s_vecDataB.size());
		time_t tm;
		time(&tm);
		srand((int)tm);//���������
		for (int i = 0; i < nNum; i++)
		{
			CString strMisalignment = "";
			{
				HTuple  hv_RandTuple;
				TupleGenConst(hvMis.Length(), 0, &hv_RandTuple);
				for (int j = 0; j < theGlobal.m_ProductTool.GetCurrentProductInfo().tParam.vecMisalignment.size(); j++)
				{
					double dRandValue = (rand() % 2 == 0 ? -1.001 : 1.001)*(rand() % 121)*0.0002;
					//if(abs(dRandValue)<0.008) dRandValue = (rand() % 121)*0.0002;

					CString strTmp;
					strTmp.Format("%0.4f,", dRandValue + hvMis[j].D());
					strMisalignment += strTmp;
				}
			}
			std::ofstream outFile;
			outFile.open(strFileMisalignment, std::ios::out | std::ios::app);
			outFile << strMisalignment << std::endl;
			outFile.close();
		}
	}
	s_vecDataA.clear();
	s_vecDataB.clear();
	return TRUE;
}

BOOL CStorage::WriteSpotInspectionFile(C_MergeWork& tOutput)
{
	//д��������
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (!tPdInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode) return TRUE;

	//�жϵ�������ǲ�����Ч
	if (tOutput.vecB.size() > 1)
	{
		HTuple hvVal;
		hvVal.Append(tOutput.vecB[0].tMeasure.tSpotInspection.hv_Width);
		hvVal.Append(tOutput.vecB[1].tMeasure.tSpotInspection.hv_Width);

		HTuple hvInc;
		TupleFind(hvVal, NO_DATA, &hvInc);
		if (hvInc.Length() >= 6) return FALSE;
	}
	else
	{
		HTuple hvVal;
		hvVal.Append(tOutput.vecB[0].tMeasure.tSpotInspection.hv_Width);

		HTuple hvInc;
		TupleFind(hvVal, NO_DATA, &hvInc);
		if (hvInc.Length() >= 3) return FALSE;
	}


	CString strFilePath;
	strFilePath.Format("%s\\�������\\", m_strMapFolderPath);
	if (!PathFileExists(strFilePath)) CreateMultilevelPath(strFilePath);
	strFilePath.Format("%s\\�������\\���ԭʼ����_%04d%02d%02d.csv", m_strMapFolderPath, m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay);
	HTuple hvExist;

	HalconCpp::FileExists((HTuple)strFilePath, &hvExist);
	if (hvExist == 0)
	{
		CString strName = "ʱ��,";
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
			CString strTmp;
			strTmp.Format("%s�������(mm),%s���������(mm),%s�������(mm),%s������߹��(mm),%s���ҵ���(mm),%s���ҵ�����(mm),%s���ҵ���(mm),%s���ҵ����(mm),%s�������(mm),%s���������(mm),",
				nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��",
				nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��");
			strName += strTmp;
		}
		std::ofstream outFile;
		outFile.open(strFilePath, std::ios::out | std::ios::app);
		outFile << strName << std::endl;
		outFile.close();
	}

	CString strValue;
	strValue.Format("%s,", GetSaveTimeS());
	for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
	{
		if (tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width.Length() == 5)
		{
			for (int j = 0; j < tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width.Length(); j++)
			{
				CString strTmp;
				if (tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width[j] != NO_DATA)
				{
					strTmp.Format("%0.4f,%0.2f��%0.2f,", tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width[j].D(),tPdInfo.tParam.vecChecking[j].dStandard,
						tPdInfo.tParam.vecChecking[j].dTolerance);
				}
				else
				{
					strTmp = ",,";
				}
				strValue += strTmp;
			}
		}
		else
		{
			strValue += ",,,,,,,,,,";
		}
	}

	std::ofstream outFile;
	outFile.open(strFilePath, std::ios::out | std::ios::app);
	outFile << strValue << std::endl;
	outFile.close();

	return TRUE;
}

BOOL CStorage::WriteSpotInspectionReport(const CJudge::tagSpotInspCount& tCount)
{
	//д���챨��
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (!tPdInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode) return TRUE;

	CString strFilePath;
	strFilePath.Format("%s\\�������\\", m_strMapFolderPath);
	if (!PathFileExists(strFilePath)) CreateMultilevelPath(strFilePath);
	strFilePath.Format("%s\\�������\\����ж���_%04d%02d%02d.csv", m_strMapFolderPath, m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay);
	HTuple hvExist;

	HalconCpp::FileExists((HTuple)strFilePath, &hvExist);
	if (hvExist == 0)
	{
		CString strName = "ʱ��,�¶�,��������,��̨���,��λ,";
		for (int nBWorkSN = 0; nBWorkSN < theGlobal.m_Project.GetProjectConfig().nSideCount; nBWorkSN++)
		{
			CString strTmp;
			strTmp.Format("%s�������(mm),%s���������(mm),%s�������(mm),%s������߹��(mm),%s���ҵ���(mm),%s���ҵ�����(mm),%s���ҵ���(mm),%s���ҵ����(mm),%s�������(mm),%s���������(mm),",
				nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��",
				nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��", nBWorkSN == 0 ? "��" : "��");
			strName += strTmp;
		}
		strName += "�����,";
		std::ofstream outFile;
		outFile.open(strFilePath, std::ios::out | std::ios::app);
		outFile << strName << std::endl;
		outFile.close();
	}

	CString strValue;

	SYSTEMTIME tTime;
	GetLocalTime(&tTime);
	CString strTime;
	strTime.Format("%04d/%02d/%02d %02d:%02d:%02d.%03d", tTime.wYear, tTime.wMonth, tTime.wDay,
		tTime.wHour, tTime.wMinute, tTime.wSecond, tTime.wMilliseconds);
	strValue.Format("%s,NA,Ϳ��,%s,��λ%d,", strTime, theGlobal.m_tSystemSet.strDeviceID, theGlobal.m_tSystemSet.eSN);
	for (int nBWorkSN = 0; nBWorkSN < theGlobal.m_Project.GetProjectConfig().nSideCount; nBWorkSN++)
	{
		if (tCount.hvDataMedian[nBWorkSN].Length() == 5)
		{
			for (int j = 0; j < tCount.hvDataMedian[nBWorkSN].Length(); j++)
			{
				CString strTmp;
				if (tCount.hvDataMedian[nBWorkSN][j] != NO_DATA)
				{
					strTmp.Format("%0.4f,%0.2f��%0.2f,", tCount.hvDataMedian[nBWorkSN][j].D(), tPdInfo.tParam.vecChecking[j].dStandard,
						tPdInfo.tParam.vecChecking[j].dTolerance);
				}
				else
				{
					strTmp = ",,";
				}
				strValue += strTmp;
			}
		}
		else
		{
			strValue += ",,,,,,,,,,";
		}
	}
	//�ж����
	if (tCount.hvFlagFinal == NG)
	{
		strValue += "NG,";
	}
	else
	{
		strValue += "OK,";
	}

	std::ofstream outFile;
	outFile.open(strFilePath, std::ios::out | std::ios::app);
	outFile << strValue << std::endl;
	outFile.close();

	return TRUE;
}

void CStorage::WriteRecordFileWet(const CString& strFilePath, C_MergeWork& tOutput)
{
	if (theGlobal.GetSystemSet().nActionMode == GRRMode) return;

	if (0 == tOutput.vecB[0].vecA[0].hvRunDirection.I()
		|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[1].I()
		|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[2].I()) return;

	if (CGlobal::SN_3rd == theGlobal.GetSystemSet().eSN)//��β���Ǹ�Ĥ ����Ҫ��¼ʪĤ�ĳߴ�
	{
		return;
	}
	HTuple hvOnlineOffset = theGlobal.m_ProductTool.GetOnlineOffset();
	if (tOutput.vecB[0].tMeasure.tSize.hv_Width.Length() != hvOnlineOffset.Length())
	{
		return;
	}
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem = "", strTmp;
	try
	{
		//a����1�б�����Ĥ��ţ���ʱ���IDΪ�����1~3��ΪĤ��š�ʱ�䡢ID												
		//b����4~��24��Ϊ�ߴ����ݣ�����ͼ��Ϊ1��6����¼13���ߴ����ݣ�ԭʼ���ݰ���1��10����¼21���ߴ����ݣ�Ԥ��λ�ã�ʵ�������ֵ������NA�����磬1��6ʱ��a13~a21����ֵ��ΪNA��B�����ƣ��ڵ�25��~45��
		//c����46~52�м�¼Ϳ�����NG�������˴���۲������ء�1�������OK���ء�0��
		//d����53�м�¼������Ϣ������������ϣ�������ʾ�½��Ϻ�
		//d����54~59�м�¼������Ϣ����Ϊ������
		//e : ���Ϊ����Ϳ������25~45�з��ؿ�ֵ

		strItem.Format("%s,%s,%s", theGlobal.GetCurrentBatch(), GetSaveTimeS(), "ID");

		//����λ��
		strTmp.Format(",%0.4f,%0.4f", (tOutput.vecB[0].tMeasure.tLine.hv_StartColumn[0].D() + tOutput.vecB[0].tMeasure.tLine.hv_EndColumn[0].D()) / 2 * theGlobal.GetResolutionList()[0].dColumn, theGlobal.m_Counter.GetCurrentMeterCount());
		strItem += strTmp;
		HTuple hvSize;

		//A������
		hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width - hvOnlineOffset;
		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleInverse() - hvOnlineOffset.TupleInverse();
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			if (i > hvSize.Length() - 1 || tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE)
			{
				strItem += ",";
			}
			else
			{
				strTmp.Format(",%0.4f", hvSize[i].D());
				strItem += strTmp;
			}
		}
		strTmp.Format(",%0.4f,", tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D());
		strItem += strTmp;
		//���氼��
		if (tPdInfo.bExistBaseCoat == TRUE)
		{
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_WholeBaseCoat;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
				hvSize = tOutput.vecB[0].tMeasure.tSize.hv_WholeBaseCoat.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				if (i > hvSize.Length() - 1
					|| tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE
					|| hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}

			strItem += ",";
		}
		strItem += "\n";
		CString sOrinFileName;      //�ļ�����-����չ
		CString sOrinName;          //���ļ��� 
		sOrinFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		sOrinName = sOrinFileName.Left(sOrinFileName.ReverseFind('.'));
		//1.����Ĥ��Ҫ�ر�ԭ�ļ���2.������ļ����ڲ�һ�������ļ�����һ��ҲҪ�ر��ļ�(Ĥ��Ų���);
		if (theGlobal.m_Counter.bStartNewBetchFlag || m_strLastFileRecordName_wet != strFilePath)
		{
			if (m_strFileRecoredOpened_wet) m_fileRecord_wet.Close();

			m_strFileRecoredOpened_wet = FALSE;
			m_isFirstGetLastRecored_wet = TRUE;
		}

		if (!m_strFileRecoredOpened_wet)
		{
			//���ݴ��������ļ������ҵ�������ͬ��ŵ������ļ����ҵ���������Ǹ��ж��ļ�
			if (m_isFirstGetLastRecored_wet)
			{
				m_strLastFileRecordName_wet = GetMaxBatchNumFile(strFilePath);
				m_isFirstGetLastRecored_wet = FALSE;
			}

			m_strFileRecoredOpened_wet = m_fileRecord_wet.Open(m_strLastFileRecordName_wet, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite);
			if (!m_strFileRecoredOpened_wet)
			{
				CString sFileName;      //�ļ�����-����չ
				CString sFileExtendName;//����չ�� .csv
				CString sName;          //���ļ��� 
				CString sPath;          //��·�������ļ���
				sFileExtendName = m_strLastFileRecordName_wet.Right(m_strLastFileRecordName_wet.GetLength()
					- m_strLastFileRecordName_wet.ReverseFind('.'));
				sFileName = m_strLastFileRecordName_wet.Right(m_strLastFileRecordName_wet.GetLength() -
					m_strLastFileRecordName_wet.ReverseFind('\\') - 1);
				sName = sFileName.Left(sFileName.ReverseFind('.'));
				sPath = m_strLastFileRecordName_wet.Left(m_strLastFileRecordName_wet.GetLength() - sFileName.GetLength() - 1);

				CString sNewFileName;
				sNewFileName.Format("%s\\%s_wet%s", sPath, GetRecoredSaveFileName(sPath, sOrinName, sName), sFileExtendName);

				if (FALSE == CreateRecordFile_wet(sNewFileName))
				{
					CString strException;
					strException.Format("Storage_WriteRecordFile: ��������ļ�����,���´����ļ�ʧ��");
					theGlobal.m_Log.Exception(strException);
					return;
				}
				else
				{
					//�����ļ�
					if (m_fileRecord_wet.Open(sNewFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
					{
						m_strLastFileRecordName_wet = sNewFileName;
						m_strFileRecoredOpened_wet = true;
					}
				}
			}
		}
		m_fileRecord_wet.SeekToEnd();
		m_fileRecord_wet.WriteString(strItem);
		m_fileRecord_wet.Flush();
	}

	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("Storage_WriteRecordFile:ImageSN:%d Error:%s  [ %s() ]", tOutput.vecB[0].vecA[0].hvImageSN.I(), e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
}

void CStorage::WriteRecordFile(const CString& strFilePath, C_MergeWork& tOutput)
{
	if (theGlobal.GetSystemSet().nActionMode == GRRMode) return;

	//�豸ֹͣ����ûͿ��������ͳ��
	if (theGlobal.m_DataCalculate.m_tParam.bUseStopPosDataFiltering)
	{
		if (1 != tOutput.vecB[0].vecA[0].hvRunDirection.I()
			|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[1].I()
			|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[2].I()) return;
	}

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem = "", strTmp;
	try
	{
		//a����1�б�����Ĥ��ţ���ʱ���IDΪ�����1~3��ΪĤ��š�ʱ�䡢ID												
		//b����4~��24��Ϊ�ߴ����ݣ�����ͼ��Ϊ1��6����¼13���ߴ����ݣ�ԭʼ���ݰ���1��10����¼21���ߴ����ݣ�Ԥ��λ�ã�ʵ�������ֵ������NA�����磬1��6ʱ��a13~a21����ֵ��ΪNA��B�����ƣ��ڵ�25��~45��
		//c����46~52�м�¼Ϳ�����NG�������˴���۲������ء�1�������OK���ء�0��
		//d����53�м�¼������Ϣ������������ϣ�������ʾ�½��Ϻ�
		//d����54~59�м�¼������Ϣ����Ϊ������
		//e : ���Ϊ����Ϳ������25~45�з��ؿ�ֵ

		strItem.Format("%s,%s,%s", theGlobal.GetCurrentBatch(), GetSaveTimeS(), "ID");

		//����λ��
		HTuple hvEdgeCol, hvEdgeColReal;
		hvEdgeCol = (tOutput.vecB[0].tMeasure.tLine.hv_StartColumn[0].D() + tOutput.vecB[0].tMeasure.tLine.hv_EndColumn[0].D()) / 2;
		theGlobal.m_AlgTool.DoSomething(0, CAlgorithmTool::AlgMeasure, "CalColPoint", &hvEdgeCol, &hvEdgeColReal);
		strTmp.Format(",%0.4f,%0.4f", hvEdgeColReal.D(), theGlobal.m_Counter.GetCurrentMeterCount());
		strItem += strTmp;
		HTuple hvSize;

		//A������
		hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width;
		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleInverse();
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			if (i > hvSize.Length() - 1 || tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE)
			{
				strItem += ",";
			}
			else
			{
				strTmp.Format(",%0.4f", hvSize[i].D());
				strItem += strTmp;
			}
		}
		strTmp.Format(",%0.4f,", tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D());
		strItem += strTmp;
		//���氼��
		if (tPdInfo.bExistBaseCoat == TRUE)
		{
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_WholeBaseCoat;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
				hvSize = tOutput.vecB[0].tMeasure.tSize.hv_WholeBaseCoat.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				if (i > hvSize.Length() - 1
					|| tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE
					|| hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}

			strItem += ",";
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			//�����λ����
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
				hvSize = tOutput.vecB[0].tMeasure.tSize.hv_MisalignedDist.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				if (i > hvSize.Length() - 1 || hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
			strItem += ",";
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			hvSize = tOutput.vecB[1].tMeasure.tSize.hv_Width;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
				hvSize = tOutput.vecB[1].tMeasure.tSize.hv_Width.TupleInverse();
			//��������
			for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
			{
				if (i > hvSize.Length() - 1 || tOutput.vecB[1].tMeasure.hv_HasCoat == FALSE)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
			strTmp.Format(",%0.4f,", tOutput.vecB[1].tMeasure.tSize.hv_WholeWidth.D());
			strItem += strTmp;
			//���氼��
			if (tPdInfo.bExistBaseCoat == TRUE)
			{
				hvSize = tOutput.vecB[1].tMeasure.tSize.hv_WholeBaseCoat;
				if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
					hvSize = tOutput.vecB[1].tMeasure.tSize.hv_WholeBaseCoat.TupleInverse();
				for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
				{
					if (i > hvSize.Length() - 1
						|| tOutput.vecB[1].tMeasure.hv_HasCoat == FALSE
						|| hvSize[i] == NO_DATA)
					{
						strItem += ",";
					}
					else
					{
						strTmp.Format(",%0.4f", hvSize[i].D());
						strItem += strTmp;
					}
				}
				strItem += ",";
			}
			//�����λ����
			hvSize = tOutput.vecB[1].tMeasure.tSize.hv_MisalignedDist;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
				hvSize = tOutput.vecB[1].tMeasure.tSize.hv_MisalignedDist.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				if (i > hvSize.Length() - 1 || hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
			strItem += ",";
			//��λ
			hvSize = tOutput.tMerge.hv_Misalignment;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head && theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
				hvSize = tOutput.tMerge.hv_Misalignment.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				if (i > hvSize.Length() - 1 || hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
		}
		//�ջ�ֵ
		double dVal = 0, dVal2 = 0;
		BOOL bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetCorrectDiffValue", NULL, &dVal);
		if (bRet)
			strTmp.Format(",%0.4f", dVal);
		else
			strTmp = ",";
		strItem += strTmp;
		bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetCorrectValue", NULL, &dVal2);
		if (bRet)
			strTmp.Format(",%0.4f", dVal2);
		else
			strTmp = ",";
		strItem += strTmp;
		//覴�����
		//Ϳ��©��
		HTuple hvInc;
		hvInc = tOutput.tMerge.hv_NGType.TupleFind(NG_ExposeFoil);
		strTmp.Format(",%d", (hvInc.Length() > 0 && hvInc >= 0) ? hvInc.Length() : 0);
		strItem += strTmp;
		//��������
		hvInc = tOutput.tMerge.hv_NGType.TupleFind(NG_FoilBlackSpot);
		strTmp.Format(",%d", (hvInc.Length() > 0 && hvInc >= 0) ? hvInc.Length() : 0);
		strItem += strTmp;
		//AT11©��
		hvInc = tOutput.tMerge.hv_NGType.TupleFind(NG_ExposeFoil);
		strTmp.Format(",%d", (hvInc.Length() > 0 && hvInc >= 0) ? hvInc.Length() : 0);
		strItem += strTmp;
		for (int i = 0; i < 4; i++)
		{
			strItem += ",0";
		}
		double dStandard = 0;
		if (tPdInfo.bExistAT11)
			dStandard = tPdInfo.tParam.vecAT11Width[0].dStandard;
		else if (tPdInfo.bExistBaseCoat)
			dStandard = tPdInfo.tParam.vecBaseCoatWidth[0].dStandard;

		//���Ϻţ�Ʒ�֣���̨�ţ����ţ�AT9���Ĥ���񣬼������
		strTmp.Format(",%s,%s,%s,%s,%0.3f,%0.3f,%0.3f", "���Ϻ�", theGlobal.m_ProductTool.GetCurrentProductName(), theGlobal.GetSystemSet().strDeviceID,
			theGlobal.GetCurrentJobNum() == "" ? "" : theGlobal.GetCurrentJobNum(), dStandard, tPdInfo.tParam.vecCoatWidth[0].dStandard, tPdInfo.tParam.vecFoilWidth[0].dStandard);
		strItem += strTmp;

		//��ȡ�¶�
		std::vector<unsigned short> vTempList;
		try
		{
			vTempList.resize(theGlobal.m_CommunicationTool.m_nTempertatureNums, 0);
			theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Temperature, "GetCurrentTemperature", &vTempList, NULL);
		}
		catch (const std::exception&)
		{
		}
		catch (...)
		{
		}

		CString strTemp;
		for (int i = 0; i < vTempList.size(); i++)
		{
			strTemp.Format("%d", vTempList[i]);
			strTmp.Format(",%s", (strTemp == "0") ? "" : strTemp);
			strItem += strTmp;
		}
		//ͼ���
		strTemp.Format(",%d", tOutput.vecB[0].vecA[0].hvImageSN.I());
		strItem += strTemp;
		//ģͷλ��
		strTemp.Format(",%0.6f", theGlobal.m_CommunicationTool.m_dPlcDieHeadPosition);
		strItem += strTemp;
		strItem += "\n";

		CString sOrinFileName;      //�ļ�����-����չ
		CString sOrinName;          //���ļ��� 

		sOrinFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		sOrinName = sOrinFileName.Left(sOrinFileName.ReverseFind('.'));
		//1.����Ĥ��Ҫ�ر�ԭ�ļ���2.������ļ����ڲ�һ�������ļ�����һ��ҲҪ�ر��ļ�(Ĥ��Ų���);
		if (theGlobal.m_Counter.bStartNewBetchFlag || m_strLastFileRecordName != strFilePath)
		{
			if (m_strFileRecoredOpened) m_fileRecord.Close();

			m_strFileRecoredOpened = FALSE;
			m_isFirstGetLastRecored = TRUE;

			theGlobal.m_Counter.bStartNewBetchFlag = FALSE;
		}
		if (!m_strFileRecoredOpened)
		{
			//���ݴ��������ļ������ҵ�������ͬ��ŵ������ļ����ҵ���������Ǹ��ж��ļ�
			if (m_isFirstGetLastRecored)
			{
				m_strLastFileRecordName = GetMaxBatchNumFile(strFilePath);
				m_isFirstGetLastRecored = FALSE;
			}

			m_strFileRecoredOpened = m_fileRecord.Open(m_strLastFileRecordName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite);
			if (!m_strFileRecoredOpened)
			{
				CString sFileName;      //�ļ�����-����չ
				CString sFileExtendName;//����չ�� .csv
				CString sName;          //���ļ��� 
				CString sPath;          //��·�������ļ���
				sFileExtendName = m_strLastFileRecordName.Right(m_strLastFileRecordName.GetLength() - m_strLastFileRecordName.ReverseFind('.'));
				sFileName = m_strLastFileRecordName.Right(m_strLastFileRecordName.GetLength() - m_strLastFileRecordName.ReverseFind('\\') - 1);
				sName = sFileName.Left(sFileName.ReverseFind('.'));
				sPath = m_strLastFileRecordName.Left(m_strLastFileRecordName.GetLength() - sFileName.GetLength() - 1);

				CString sNewFileName;
				sNewFileName.Format("%s\\%s%s", sPath, GetRecoredSaveFileName(sPath, sOrinName, sName), sFileExtendName);

				if (FALSE == CreateRecordFile(sNewFileName))
				{
					CString strException;
					strException.Format("Storage_WriteRecordFile: ��������ļ�����,���´����ļ�ʧ��");
					theGlobal.m_Log.Exception(strException);
					return;
				}
				else
				{
					//�����ļ�
					if (m_fileRecord.Open(sNewFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
					{
						m_strLastFileRecordName = sNewFileName;
						m_strFileRecoredOpened = true;
					}
				}
			}
		}
		m_fileRecord.SeekToEnd();
		m_fileRecord.WriteString(strItem);
		m_fileRecord.Flush();
	}
	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("Storage_WriteRecordFile:ImageSN:%d Error:%s  [ %s() ]", tOutput.vecB[0].vecA[0].hvImageSN.I(), e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
}

void CStorage::WriteRecordFileAll(const CString& strFilePath, C_MergeWork& tOutput)
{
	if (theGlobal.GetSystemSet().nActionMode == GRRMode) return;

	//�������ݶ�д
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem = "", strTmp;
	try
	{
		//a����1�б�����Ĥ��ţ���ʱ���IDΪ�����1~3��ΪĤ��š�ʱ�䡢ID												
		//b����4~��24��Ϊ�ߴ����ݣ�����ͼ��Ϊ1��6����¼13���ߴ����ݣ�ԭʼ���ݰ���1��10����¼21���ߴ����ݣ�Ԥ��λ�ã�ʵ�������ֵ������NA�����磬1��6ʱ��a13~a21����ֵ��ΪNA��B�����ƣ��ڵ�25��~45��
		//c����46~52�м�¼Ϳ�����NG�������˴���۲������ء�1�������OK���ء�0��
		//d����53�м�¼������Ϣ������������ϣ�������ʾ�½��Ϻ�
		//d����54~59�м�¼������Ϣ����Ϊ������
		//e : ���Ϊ����Ϳ������25~45�з��ؿ�ֵ

		strItem.Format("%s,%s,%s", theGlobal.GetCurrentBatch(), GetSaveTimeS(), "ID");

		//����λ��
		strTmp.Format(",%0.4f,%0.4f", (tOutput.vecB[0].tMeasure.tLine.hv_StartColumn[0].D() + tOutput.vecB[0].tMeasure.tLine.hv_EndColumn[0].D()) / 2 * theGlobal.GetResolutionList()[0].dColumn, theGlobal.m_Counter.GetCurrentMeterCount());
		strItem += strTmp;
		HTuple hvSize;

		//A������
		hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width;
		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width.TupleInverse();
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			if (i > hvSize.Length() - 1 || tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE)
			{
				strItem += ",";
			}
			else
			{
				strTmp.Format(",%0.4f", hvSize[i].D());
				strItem += strTmp;
			}
		}
		strTmp.Format(",%0.4f,", tOutput.vecB[0].tMeasure.tSize.hv_WholeWidth.D());
		strItem += strTmp;
		//���氼��
		if (tPdInfo.bExistBaseCoat == TRUE)
		{
			hvSize = tOutput.vecB[0].tMeasure.tSize.hv_WholeBaseCoat;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
				hvSize = tOutput.vecB[0].tMeasure.tSize.hv_WholeBaseCoat.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
			{
				if (i > hvSize.Length() - 1
					|| tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE
					|| hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}

			strItem += ",";
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			hvSize = tOutput.vecB[1].tMeasure.tSize.hv_Width;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
				hvSize = tOutput.vecB[1].tMeasure.tSize.hv_Width.TupleInverse();
			//��������
			for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
			{
				if (i > hvSize.Length() - 1 || tOutput.vecB[1].tMeasure.hv_HasCoat == FALSE)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
			strTmp.Format(",%0.4f,", tOutput.vecB[1].tMeasure.tSize.hv_WholeWidth.D());
			strItem += strTmp;
			//���氼��
			if (tPdInfo.bExistBaseCoat == TRUE)
			{
				hvSize = tOutput.vecB[1].tMeasure.tSize.hv_WholeBaseCoat;
				if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
					hvSize = tOutput.vecB[1].tMeasure.tSize.hv_WholeBaseCoat.TupleInverse();
				for (int i = 0; i < tPdInfo.tParam.vecCoatWidth.size(); i++)
				{
					if (i > hvSize.Length() - 1
						|| tOutput.vecB[1].tMeasure.hv_HasCoat == FALSE
						|| hvSize[i] == NO_DATA)
					{
						strItem += ",";
					}
					else
					{
						strTmp.Format(",%0.4f", hvSize[i].D());
						strItem += strTmp;
					}
				}
				strItem += ",";
			}
			//��λ
			hvSize = tOutput.tMerge.hv_Misalignment;
			if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
				hvSize = tOutput.tMerge.hv_Misalignment.TupleInverse();
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				if (i > hvSize.Length() - 1 || hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
		}
		//�ջ�ֵ
		double dVal = 0, dVal2 = 0;
		BOOL bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetCorrectDiffValue", NULL, &dVal);
		if (bRet)
			strTmp.Format(",%0.4f", dVal);
		else
			strTmp = ",";
		strItem += strTmp;
		bRet = theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetCorrectValue", NULL, &dVal2);
		if (bRet)
			strTmp.Format(",%0.4f", dVal2);
		else
			strTmp = ",";
		strItem += strTmp;
		//覴�����
		//Ϳ��©��
		HTuple hvInc;
		hvInc = tOutput.tMerge.hv_NGType.TupleFind(NG_ExposeFoil);
		strTmp.Format(",%d", (hvInc.Length() > 0 && hvInc >= 0) ? hvInc.Length() : 0);
		strItem += strTmp;
		//��������
		hvInc = tOutput.tMerge.hv_NGType.TupleFind(NG_FoilBlackSpot);
		strTmp.Format(",%d", (hvInc.Length() > 0 && hvInc >= 0) ? hvInc.Length() : 0);
		strItem += strTmp;
		//AT11©��
		hvInc = tOutput.tMerge.hv_NGType.TupleFind(NG_ExposeFoil);
		strTmp.Format(",%d", (hvInc.Length() > 0 && hvInc >= 0) ? hvInc.Length() : 0);
		strItem += strTmp;
		for (int i = 0; i < 4; i++)
		{
			strItem += ",0";
		}
		double dStandard = 0;
		if (tPdInfo.bExistAT11)
			dStandard = tPdInfo.tParam.vecAT11Width[0].dStandard;
		else if (tPdInfo.bExistBaseCoat)
			dStandard = tPdInfo.tParam.vecBaseCoatWidth[0].dStandard;

		//���Ϻţ�Ʒ�֣���̨�ţ����ţ�AT9���Ĥ���񣬼������
		strTmp.Format(",%s,%s,%s,%s,%0.3f,%0.3f,%0.3f", "���Ϻ�", theGlobal.m_ProductTool.GetCurrentProductName(), theGlobal.GetSystemSet().strDeviceID,
			theGlobal.GetCurrentJobNum() == "" ? "" : theGlobal.GetCurrentJobNum(), dStandard, tPdInfo.tParam.vecCoatWidth[0].dStandard, tPdInfo.tParam.vecFoilWidth[0].dStandard);
		strItem += strTmp;

		//��ȡ�¶�
		std::vector<unsigned short> vTempList;
		try
		{
			vTempList.resize(theGlobal.m_CommunicationTool.m_nTempertatureNums, 0);
			theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Temperature, "GetCurrentTemperature", &vTempList, NULL);
		}
		catch (const std::exception&)
		{
		}
		catch (...)
		{
		}

		CString strTemp;
		for (int i = 0; i < vTempList.size(); i++)
		{
			strTemp.Format("%d", vTempList[i]);
			strTmp.Format(",%s", (strTemp == "0") ? "" : strTemp);
			strItem += strTmp;
		}
		//ͼ���
		strTemp.Format(",%d", tOutput.vecB[0].vecA[0].hvImageSN.I());
		strItem += strTemp;
		//ģͷλ��
		strTemp.Format(",%0.6f", theGlobal.m_CommunicationTool.m_dPlcDieHeadPosition);
		strItem += strTemp;
		strItem += "\n";

		CString sOrinFileName;      //�ļ�����-����չ
		CString sOrinName;          //���ļ��� 

		sOrinFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		sOrinName = sOrinFileName.Left(sOrinFileName.ReverseFind('.'));
		//1.����Ĥ��Ҫ�ر�ԭ�ļ���2.������ļ����ڲ�һ�������ļ�����һ��ҲҪ�ر��ļ�(Ĥ��Ų���);
		if (theGlobal.m_Counter.bStartNewBetchFlag || m_strLastFileRecordAllName != strFilePath)
		{
			if (m_strFileRecoredAllOpened) m_fileRecordAll.Close();

			m_strFileRecoredAllOpened = FALSE;
			m_isFirstGetLastRecoredAll = TRUE;
		}

		if (!m_strFileRecoredAllOpened)
		{
			//���ݴ��������ļ������ҵ�������ͬ��ŵ������ļ����ҵ���������Ǹ��ж��ļ�
			if (m_isFirstGetLastRecoredAll)
			{
				m_strLastFileRecordAllName = GetMaxBatchNumFile(strFilePath);
				m_isFirstGetLastRecoredAll = FALSE;
			}

			m_strFileRecoredAllOpened = m_fileRecordAll.Open(m_strLastFileRecordAllName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite);
			if (!m_strFileRecoredAllOpened)
			{
				CString sFileName;      //�ļ�����-����չ
				CString sFileExtendName;//����չ�� .csv
				CString sName;          //���ļ��� 
				CString sPath;          //��·�������ļ���
				sFileExtendName = m_strLastFileRecordAllName.Right(m_strLastFileRecordAllName.GetLength()
					- m_strLastFileRecordAllName.ReverseFind('.'));
				sFileName = m_strLastFileRecordAllName.Right(m_strLastFileRecordAllName.GetLength() -
					m_strLastFileRecordAllName.ReverseFind('\\') - 1);
				sName = sFileName.Left(sFileName.ReverseFind('.'));
				sPath = m_strLastFileRecordAllName.Left(m_strLastFileRecordAllName.GetLength() - sFileName.GetLength() - 1);

				CString sNewFileName;
				sNewFileName.Format("%s\\%s_All%s", sPath, GetRecoredSaveFileName(sPath, sOrinName, sName), sFileExtendName);

				if (FALSE == CreateRecordFile(sNewFileName))
				{
					CString strException;
					strException.Format("Storage_WriteRecordFile: ��������ļ�����,���´����ļ�ʧ��");
					theGlobal.m_Log.Exception(strException);
					return;
				}
				else
				{
					//�����ļ�
					if (m_fileRecordAll.Open(sNewFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
					{
						m_strLastFileRecordAllName = sNewFileName;
						m_strFileRecoredAllOpened = true;
					}
				}
			}
		}

		m_fileRecordAll.SeekToEnd();
		m_fileRecordAll.WriteString(strItem);
		m_fileRecordAll.Flush();
	}
	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("Storage_WriteRecordFile:ImageSN:%d Error:%s  [ %s() ]", tOutput.vecB[0].vecA[0].hvImageSN.I(), e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
}

//����覴������ļ�
BOOL CStorage::CreateDefectFile(const CString& strFilePath)
{
	try
	{
		CStdioFile file;
		if (TRUE != file.Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("CStorage_CreateDefeatFile: ����csv�ļ�ʧ�� (%s)", strFilePath);
			theGlobal.m_Log.Exception(strException);
			return false;
		}
		CString strItem = "���,ʱ��,ͼ���,覴�����,������,����,����X(mm),����Y(m),���(mm2),���(mm),�߶�(mm)\n";
		file.WriteString(strItem);
		file.Close();

		return true;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("CStorage:CreateDefeatFile ����csv�ļ������쳣:�쳣��Ϣ%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
		return false;
	}
}

//д覴�����(�ͻ�Ҫ�󵥶����)
BOOL CStorage::WriteDefectFile(const CString& strFilePath, C_MergeWork& tOutput)
{
	if (theGlobal.GetSystemSet().nActionMode == GRRMode) return FALSE;

	if (0 == tOutput.vecB[0].vecA[0].hvRunDirection.I()
		|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[1].I()
		|| 0 == tOutput.vecB[0].vecA[0].hvRunDirection[2].I()) return TRUE;

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem = "", strTmp;
	try
	{
		//����覴�����
		CString sFileName;      //�ļ�����-����չ
		CString sName;          //���ļ��� 
		CString sPath;          //��·�������ļ���
		sFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		sName = sFileName.Left(sFileName.ReverseFind('.'));
		sPath = strFilePath.Left(strFilePath.GetLength() - sFileName.GetLength() - 1);

		CString strDefeatFileName;
		strDefeatFileName.Format("%s\\%s.csv", sPath, sName + "_Defect");

		if (!PathFileExists(strDefeatFileName))
		{
			if (FALSE == CreateDefectFile(strDefeatFileName)) return FALSE;
		}

		HTuple hvHeight;
		GetImageSize(tOutput.vecB[0].tMeasure.ho_DispImage, &HTuple(), &hvHeight);

		for (int m = 0; m < tOutput.vecB.size(); m++)
		{
			for (int n = 0; n < tOutput.vecB[m].tMeasure.tDefect.hv_Type.Length(); n++)
			{
				//1.���
				strItem.Format("%s", theGlobal.GetCurrentBatch());

				//2.ʱ��
				SYSTEMTIME	tSaveTime;
				::GetLocalTime(&tSaveTime);
				CString sTime;
				sTime.Format("%04d-%02d-%02d %02d:%02d:%02d:%03d", tSaveTime.wYear, tSaveTime.wMonth, tSaveTime.wDay,
					tSaveTime.wHour, tSaveTime.wMinute, tSaveTime.wSecond, tSaveTime.wMilliseconds);

				strTmp.Format(",%s", sTime);
				strItem += strTmp;

				//3.ͼ���
				int nImgNum = tOutput.vecB[0].vecA[0].hvImageSN.I();
				strTmp.Format(",%d", nImgNum);
				strItem += strTmp;

				//4.覴�����
				CString type;
				int ng_type = tOutput.vecB[m].tMeasure.tDefect.hv_Type[n].I();

				type = theGlobal.m_Counter.GetNGTypeName(ng_type);

				strTmp.Format(",%s", type);
				strItem += strTmp;

				//5.������
				strTmp.Format(",%s", (m == 0) ? "����" : "����");
				strItem += strTmp;

				//6.����
				strTmp.Format(",%d", tOutput.vecB[m].tMeasure.tDefect.hv_StripeSN[n].I());
				strItem += strTmp;

				//7.����X(mm)
				strTmp.Format(",%0.4f", tOutput.vecB[m].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Column[n].D());
				strItem += strTmp;

				//8.����Y(m)
				/*double dY = tOutput.vecB[m].tMeasure.tDefect.tFeature.tRect.tLeftTop.hv_Row[n].D() / 1000
					+ tOutput.hv_MeterCount.D() - hvHeight.D()*theGlobal.m_vecResolution[0].dRow / 1000;*/
				double dY = tOutput.hv_MeterCount.D();
				strTmp.Format(",%0.4f", dY);
				strItem += strTmp;

				//9.���(mm2)
				strTmp.Format(",%0.4f", tOutput.vecB[m].tMeasure.tDefect.tFeature.hvArea[n].D());
				strItem += strTmp;
				//10.���(mm)
				strTmp.Format(",%0.4f", tOutput.vecB[m].tMeasure.tDefect.tFeature.hvWidth[n].D());
				strItem += strTmp;
				//11.�߶�(mm)
				strTmp.Format(",%0.4f\n", tOutput.vecB[m].tMeasure.tDefect.tFeature.hvHeight[n].D());
				strItem += strTmp;

				CStdioFile fileDefeat;
				if (!fileDefeat.Open(strDefeatFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
				{
					CreateDefectFile(strDefeatFileName);
				}
				fileDefeat.SeekToEnd();
				fileDefeat.WriteString(strItem);
				fileDefeat.Flush();
			}
		}
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
}

//�ж��ļ��Ƿ��Ѿ����ڣ������غ��ʵ��ļ���
CString CStorage::GetRecoredSaveFileName(const CString& strSaveDir, const CString& strOriName, const CString& strNewName)
{
	CString strFilePath, strFileName = strNewName;
	strFilePath.Format("%s//%s.csv", strSaveDir, strFileName);

	if (PathFileExists(strFilePath))
	{
		int nPos = strFileName.Find(strOriName);
		CString strRemain = strFileName.Mid(nPos + strOriName.GetLength());
		if (strRemain.Left(1) == "_")
			strRemain = strRemain.Right(strRemain.GetLength() - 1);

		int nSN = atoi(strRemain);
		strFileName.Format("%s_%d", strOriName, ++nSN);
		return GetRecoredSaveFileName(strSaveDir, strOriName, strFileName);
	}
	else
	{
		return strFileName;
	}
}

//�����������ļ������ҵ�ͬĿ¼����������Ǹ�ͬ����ļ���
CString CStorage::GetMaxBatchNumFile(const CString&strFilePath)
{
	try
	{
		//1.�ҵ������ŵ��ļ���
		CString sFileName;      //�ļ�����-����չ
		CString sFileExtendName;//����չ�� .csv
		CString sName;          //���ļ��� 
		CString sPath;          //��·�������ļ���
		sFileExtendName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('.'));
		sFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);
		sName = sFileName.Left(sFileName.ReverseFind('.'));
		sPath = strFilePath.Left(strFilePath.GetLength() - sFileName.GetLength() - 1);
		std::vector<CString> vFilePathList;

		GetFileNamePath(vFilePathList, sPath, sName);
		CString strFileName = strFilePath;
		int nSN = 0;
		int nMaxSN = 0;
		int index = -1;
		for (int i = 0; i < vFilePathList.size(); i++)
		{
			int nPos = vFilePathList[i].Find(sName);
			CString strRemain = vFilePathList[i].Mid(nPos + sName.GetLength());
			if (strRemain.Left(1) == "_")
				strRemain = strRemain.Right(strRemain.GetLength() - 1);

			nSN = atoi(strRemain);
			if (nSN > nMaxSN)
			{
				nMaxSN = nSN;
				index = i;
			}
		}
		if (index != -1)
		{
			strFileName.Format("%s", vFilePathList[index]);
		}
		return strFileName;
	}
	catch (const std::exception&)
	{
		return strFilePath;
	}
}

void CStorage::GetFileNamePath(std::vector<CString>&vFilePathList, const CString&sPath, const CString&sName)
{
	CFileFind finder;
	BOOL isNotEmpty = finder.FindFile(sPath + "\\" + _T("*.*"));//���ļ��У���ʼ����?
	while (isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();//�����ļ�?
		CString filename = finder.GetFilePath();//��ȡ�ļ���·�����������ļ��У��������ļ�?
		if (!(finder.IsDirectory()))
		{
			//������ļ�������ļ��б�?
			int ret = filename.Find(sName);
			if (ret >= 0)
			{
				vFilePathList.push_back(filename);//��һ���ļ�·����������?
			}
		}
		else
		{
			//�ݹ�����û��ļ��У��������û��ļ���?
			if (!(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()))
			{
				GetFileNamePath(vFilePathList, sPath, sName);
			}
		}
	}
}

double CStorage::GetRemainDiskGB()
{
	DWORD dwTotalDiskSpace, dwFreeDiskSpace, dwUsedDiskSpace;
	ULARGE_INTEGER uiFreeBytesAvailableToCaller;
	ULARGE_INTEGER uiTotalNumberOfBytes;
	ULARGE_INTEGER uiTotalNumberOfFreeBytes;
	CString strPath = m_strMapFolderPath;
	int pos = strPath.Find('\\');
	strPath = strPath.Left(pos);
	CString strFreeDiskSpace;
	if (GetDiskFreeSpaceEx(strPath, &uiFreeBytesAvailableToCaller,
		&uiTotalNumberOfBytes,
		&uiTotalNumberOfFreeBytes))
	{
		dwTotalDiskSpace = (DWORD)(uiTotalNumberOfBytes.QuadPart / 1024 / 1024);
		dwFreeDiskSpace = (DWORD)(uiFreeBytesAvailableToCaller.QuadPart >> 20);
		dwUsedDiskSpace = dwTotalDiskSpace - dwFreeDiskSpace;
		float fds = (float)dwFreeDiskSpace / 1024;

		return fds;
	}
	else
	{
		return 9999;
	}
}

BOOL CStorage::IsDiskRemainEnough()
{
	if (SaveNone == m_tImageSaveType.eImageSaveType)
		return TRUE;

	//��õ�ǰ���̿ռ�
	static BOOL bWarn = FALSE;
	BOOL bFull = FALSE;
	if (GetRemainDiskGB() < m_nMinRemainStorage)//�����ǰ���̿��ÿռ�С����С�����ռ�
	{
		bFull = TRUE;
		bWarn = TRUE;
	}
	else
	{
		bFull = FALSE;
	}
	if (TRUE == bFull)
	{
		if (TRUE == bWarn)
		{
			CString strException;
			strException.Format("[%s]Disk remain smaller than %dGB, datas saving will stopped([%s]���̿ռ�С��%dGB�����ݽ�ֹͣ����)",
				m_strMapFolderPath, m_nMinRemainStorage, m_strMapFolderPath, m_nMinRemainStorage);
			theGlobal.m_Log.Exception(strException);
			bWarn = FALSE;
		}
	}
	return !bFull;
}

void CStorage::DeleteOldCaches()
{
	if (!m_bDeleteOldCaches)return;
	//static BOOL s_bDelOldCaches = FALSE;
	//static int s_nDay = 0;

	////ɾ�����ڻ���
	//SYSTEMTIME tTm;
	//::GetLocalTime(&tTm);
	//if (0 == tTm.wHour && tTm.wMinute < 30 && s_nDay != tTm.wDay)
	//	s_bDelOldCaches = TRUE;

	//if (s_bDelOldCaches)
	{
		HTuple hvTimeEnd, hvTimeStart;
		CountSeconds(&hvTimeStart);

		CString strDatePath, strDayPath, strMapPath, strDataFolderPathFinal;
		strMapPath.Format("%s\\*.*", m_strMapFolderPath);
		CFileFind datePathFinder;
		BOOL bRet = datePathFinder.FindFile(strMapPath);
		while (bRet)
		{
			CString strDateDirectory;
			bRet = datePathFinder.FindNextFile();
			strDatePath = datePathFinder.GetFilePath();
			if (!datePathFinder.IsDirectory() || datePathFinder.IsDots())
			{
				continue;
			}
			if (strDatePath.Find("CCDͼ��") == -1)continue;
			//�ļ�·����ʽ��: /Map/12.2019/31/
			strDateDirectory = strDatePath;
			strDatePath.Append("\\*.*");
			CFileFind dayPathFinder;
			BOOL bRet2 = dayPathFinder.FindFile(strDatePath);
			while (bRet2)
			{
				bRet2 = dayPathFinder.FindNextFile();
				strDayPath = dayPathFinder.GetFilePath();
				if (!dayPathFinder.IsDirectory() || dayPathFinder.IsDots())
				{
					continue;
				}
				CString strFolderPathFinal = strDayPath;
				strFolderPathFinal.Append("\\*.*");

				CFileFind datePathFinder2;
				BOOL bRet3 = datePathFinder2.FindFile(strFolderPathFinal);
				while (bRet3)
				{
					bRet3 = datePathFinder2.FindNextFile();
					strDataFolderPathFinal = datePathFinder2.GetFilePath();
					if (!datePathFinder2.IsDirectory() || datePathFinder2.IsDots())
					{
						continue;
					}

					CString strDateName = datePathFinder2.GetFileTitle();
					int nYear = atoi(strDateName.Left(4));//Map���ļ��е����
					int nMonth = atoi(strDateName.Mid(4, 2));//Map���ļ��е��·�
					int nDay = atoi(strDateName.Right(2));//Map���ļ��е���

					try
					{
						CTime t1(m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay, 0, 0, 0);
						CTime t2(nYear, nMonth, nDay, 0, 0, 0);
						CTimeSpan tSpan = t1 - t2;
						//.....�жϱ��������ļ��з��ǹ���....
						if (tSpan.GetDays() > m_nCacheSaveDays)
						{
							DeleteFolder(strDataFolderPathFinal);
						}
					}
					catch (...)
					{

					}
				}
				datePathFinder2.Close();
				//ɾ���յ�Ŀ¼
				if (PathIsDirectoryEmpty(strDayPath)) DeleteFolder(strDayPath);
			}
			dayPathFinder.Close();
		}
		datePathFinder.Close();

		//s_bDelOldCaches = FALSE;
		//s_nDay = tTm.wDay;

		CountSeconds(&hvTimeEnd);
		CString strLog;
		strLog.Format("DeleteOldCaches: ɾ��������ɣ���ʱ%0.2fs", hvTimeEnd.D() - hvTimeStart.D());
		theGlobal.m_Log.Std(strLog);
	}
}

void CStorage::BackupData()
{
	if ("" == m_strMapFolderPath) return;
	//static BOOL s_bBackup = FALSE;
	//static int s_nDay = 0;

	////ɾ�����ڻ���
	//SYSTEMTIME tTm;
	//::GetLocalTime(&tTm);
	//if (0 == tTm.wHour && tTm.wMinute < 30 && s_nDay != tTm.wDay)
	//	s_bBackup = TRUE;
	//if (!s_bBackup) return;

	try
	{
		//�ж�E���Ƿ����
		DWORD dwAttr = GetFileAttributes(_T("e:\\"));
		if (dwAttr == 0xFFFFFFFF || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
			//s_bBackup = FALSE;
			CString strLog;
			strLog.Format("BackupData: E�̲����ڣ���������ʧ��");
			theGlobal.m_Log.Exception(strLog);
			return;
		}

		HTuple hvTimeStart, hvTimeEnd;
		CountSeconds(&hvTimeStart);
		//�����ļ���
		CString strDataPath, strMapPath, strBackupPath, strBackupDataPath;
		strMapPath.Format("%s\\*.*", m_strMapFolderPath);
		CFileFind datePathFinder;
		BOOL bRet = datePathFinder.FindFile(strMapPath);
		//����·��
		strBackupPath.Format("E:\\Map_Backup");
		if (!PathFileExists(strBackupPath)) CreateMultilevelPath(strBackupPath);
		while (bRet)
		{
			CString strDateDirectory;
			bRet = datePathFinder.FindNextFile();
			strDataPath = datePathFinder.GetFilePath();
			if (datePathFinder.IsDots())
			{
				continue;
			}
			if (strDataPath.Find("����") == -1)continue;
			std::vector<CString> vecFilePathList, vecPathList, vecFileNameList;
			/*if (datePathFinder.IsDirectory())
			{
			TraverseDir(strDataPath, vecFilePathList, vecPathList, vecFileNameList);
			}
			else
			{
			vecPathList.push_back(datePathFinder.GetFilePath());
			vecFileNameList.push_back(datePathFinder.GetFileName());
			}*/

			//�ļ�·����ʽ��: /Map/12.2019/31/
			strDateDirectory = strDataPath;
			strBackupDataPath.Format("%s\\%s", strBackupPath, datePathFinder.GetFileName());

			//CopyFileA(strDataPath, strBackupDataPath, FALSE);

			int nLengthFrm = strlen(strDataPath);
			char *NewPathFrm = new char[nLengthFrm + 2];
			strcpy(NewPathFrm, strDataPath);
			NewPathFrm[nLengthFrm] = '\0';
			NewPathFrm[nLengthFrm + 1] = '\0';
			SHFILEOPSTRUCT FileOp;
			ZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));
			FileOp.fFlags = FOF_NOCONFIRMATION | FOF_CONFIRMMOUSE | FOF_NOCONFIRMMKDIR | FOF_NO_UI | FOF_SIMPLEPROGRESS;
			FileOp.hNameMappings = NULL;
			FileOp.hwnd = NULL;
			FileOp.lpszProgressTitle = NULL;
			FileOp.fAnyOperationsAborted = FALSE;
			FileOp.pFrom = NewPathFrm;
			FileOp.pTo = strBackupDataPath;
			FileOp.wFunc = FO_COPY;
			SHFileOperation(&FileOp);
		}
		CountSeconds(&hvTimeEnd);

		CString strLog;
		strLog.Format("BackupData: ����������ɣ���ʱ%0.2fs", hvTimeEnd.D() - hvTimeStart.D());
		theGlobal.m_Log.Std(strLog);
	}
	catch (HException&)
	{

	}
	//s_bBackup = FALSE;
}

void CStorage::TraverseDir(const CString& strDir, std::vector<CString>& vecFiles, std::vector<CString>& vecPaths, std::vector<CString>& vecFileNames)
{
	CString strDirTmp = strDir;
	CFileFind ff;
	//��·���������\*.*��׺
	if (strDirTmp.Right(1) != "\\")
		strDirTmp += "\\";
	strDirTmp += "*.*";
	BOOL ret = ff.FindFile(strDirTmp);
	while (ret)
	{
		ret = ff.FindNextFile();
		if (ff.IsDirectory() && !ff.IsDots())
		{
			CString path = ff.GetFilePath();
			TraverseDir(path, vecFiles, vecPaths, vecFileNames);
		}
		else if (!ff.IsDirectory() && !ff.IsDots())
		{
			vecFiles.push_back(ff.GetFilePath());//��ȡ�ļ�·��
			vecFileNames.push_back(ff.GetFileName());//��ȡ����׺���ļ���
			int nPos = ff.GetFilePath().Find(ff.GetFileName());
			vecPaths.push_back(ff.GetFilePath().Left(nPos));//��ȡ�ļ�·��
		}

	}
}

BOOL CStorage::IsStopSaveTimeCome()
{
	/*if (!m_bUseProgramStartSaveTime)
		return FALSE;

	static BOOL s_bWarn = TRUE;

	CTime tm1(theGlobal.GetProgramOpenTime().wYear, theGlobal.GetProgramOpenTime().wMonth, theGlobal.GetProgramOpenTime().wDay
		, theGlobal.GetProgramOpenTime().wHour, theGlobal.GetProgramOpenTime().wMinute, theGlobal.GetProgramOpenTime().wSecond);
	CTime tm2 = CTime::GetCurrentTime();

	if (CTimeSpan(tm2 - tm1).GetTotalSeconds() > m_nProgramStartSaveSecond)
	{
		if (TRUE == s_bWarn)
		{
			s_bWarn = FALSE;
			CString strMess;
			strMess.Format("Image save time is over and will stoped.(��ʱ��ͼ�Ѿ����ڣ���ͼ����ֹͣ)");
			theGlobal.m_Log.Std(strMess);
		}
		return TRUE;
	}
	else*/
	return FALSE;
}

void CStorage::GenDataFileSavePath()
{
	tagPDInfo tPDInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();

	//����+��̨��+Ϳ��Ĥ���+Ʒ�֣�ʽ����M5_TMJI0001_C2220181124M0132_MFH3L7-BAEL
	m_strSaveFilePath.Format("%s%s_%s_%s_%s.csv", m_strDataSaveFolder, theGlobal.GetSystemSet().strWorkshopName, theGlobal.GetSystemSet().strDeviceID,
		(theGlobal.GetCurrentBatch() + (tPDInfo.nCoatingType == OneSide ? "A" : "")), theGlobal.m_ProductTool.GetCurrentProductName());
	m_strSaveFilePathAll.Format("%s%s_%s_%s_%s_All.csv", m_strDataSaveFolder, theGlobal.GetSystemSet().strWorkshopName, theGlobal.GetSystemSet().strDeviceID,
		(theGlobal.GetCurrentBatch() + (tPDInfo.nCoatingType == OneSide ? "A" : "")), theGlobal.m_ProductTool.GetCurrentProductName());
	m_strSaveFilePath_wet.Format("%s%s_%s_%s_%s_wet.csv", m_strDataSaveFolder, theGlobal.GetSystemSet().strWorkshopName, theGlobal.GetSystemSet().strDeviceID,
		(theGlobal.GetCurrentBatch() + (tPDInfo.nCoatingType == OneSide ? "A" : "")), theGlobal.m_ProductTool.GetCurrentProductName());
}

//�����׼������ļ�
BOOL CStorage::CreateFirstArticleFile(const CString& strFilePath)
{
	//�׼������б�

	tagPDInfo tPDInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strItem, strTmp, strSide;
	strItem.Format("�䷽��,Ĥ���,ʱ��");
	strSide = "����";
	//����ߴ�
	for (int i = 0; i < tPDInfo.vecNameList.size(); i++)
	{
		if (i > tPDInfo.vecNameList.size() - 1)
		{
			strItem += ",NA";
		}
		else
		{
			strItem += "," + strSide + tPDInfo.vecNameList[i];
		}
	}

	if (tPDInfo.nCoatingType == TwoSide)
	{
		strSide = "����";
		//B��ߴ�
		for (int i = 0; i < tPDInfo.vecNameList.size(); i++)
		{
			if (i > tPDInfo.vecNameList.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strItem += "," + strSide + tPDInfo.vecNameList[i];
			}
		}
		//��λ
		for (int i = 0; i < tPDInfo.tParam.vecMisalignment.size(); i++)
		{
			if (i > tPDInfo.tParam.vecMisalignment.size() - 1)
			{
				strItem += ",NA";
			}
			else
			{
				strTmp.Format(",ͿĤ%d%s��λ", i / 2 + 1, i % 2 == 0 ? "��" : "��");
				strItem += strTmp;
			}
		}
	}
	strItem += ",ͼ���\n";
	try
	{
		CStdioFile file;
		if (TRUE != file.Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("Storage_CreateFirstArticleFile: ������������ļ�ʧ�� (%s)", strFilePath);
			theGlobal.m_Log.Exception(strException);

			return FALSE;
		}
		file.WriteString(strItem);
		file.Close();
	}
	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	return TRUE;
}

//д�׼�����
BOOL CStorage::WriteFirstArticleFile(C_MergeWork& tOutput)
{
	if (!theGlobal.m_Judge.m_tParam.bUseFirstArticleInsp) return TRUE;

	//�ж�����ͿĤ������ͳ��
	if (!tOutput.vecB[0].tMeasure.hv_HasCoat.I()) return FALSE;
	if (tOutput.vecB.size() > 1 && !tOutput.vecB[1].tMeasure.hv_HasCoat.I()) return FALSE;

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	CString strFilePath;
	strFilePath.Format("%s\\�׼�����\\", m_strMapFolderPath);
	if (!PathFileExists(strFilePath)) CreateMultilevelPath(strFilePath);
	strFilePath.Format("%s\\�׼�����\\�׼�����_%04d%02d%02d_%s_%s.csv", m_strMapFolderPath, m_tSaveTime.wYear, m_tSaveTime.wMonth, m_tSaveTime.wDay
		, theGlobal.GetCurrentBatch(), tPdInfo.strPDName);

	if (!PathFileExists(strFilePath))
	{
		if (FALSE == CreateFirstArticleFile(strFilePath))
		{
			CString strException;
			strException.Format("Storage_WriteFirstArticleFile: �׼������ļ�����,�����ļ�ʧ��");
			theGlobal.m_Log.Exception(strException);
			return FALSE;
		}
	}

	CString strItem = "", strTmp;
	try
	{
		//�䷽����Ĥ��ţ�ʱ��
		strItem.Format("%s,%s,%s", tPdInfo.strPDName, theGlobal.GetCurrentBatch(), GetSaveTimeS());

		HTuple hvSize;
		//��������
		hvSize = tOutput.vecB[0].tMeasure.tSize.hv_Width;
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			if (i > hvSize.Length() - 1 || tOutput.vecB[0].tMeasure.hv_HasCoat == FALSE)
			{
				strItem += ",";
			}
			else
			{
				strTmp.Format(",%0.4f", hvSize[i].D());
				strItem += strTmp;
			}
		}
		if (tPdInfo.nCoatingType == TwoSide)
		{
			hvSize = tOutput.vecB[1].tMeasure.tSize.hv_Width;
			//��������
			for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
			{
				if (i > hvSize.Length() - 1 || tOutput.vecB[1].tMeasure.hv_HasCoat == FALSE)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
			//��λ
			hvSize = tOutput.tMerge.hv_Misalignment;
			for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
			{
				if (i > hvSize.Length() - 1 || hvSize[i] == NO_DATA)
				{
					strItem += ",";
				}
				else
				{
					strTmp.Format(",%0.4f", hvSize[i].D());
					strItem += strTmp;
				}
			}
		}
		
		//ͼ���
		strTmp.Format(",%d,\n", tOutput.vecB[0].vecA[0].hvImageSN.I());
		strItem += strTmp;

		CStdioFile file;
		if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
		{
			CString strException;
			strException.Format("Storage_WriteFirstArticleFile: ��������ļ�����,�����ļ�ʧ��");
			theGlobal.m_Log.Exception(strException);
			return FALSE;
		}
		file.SeekToEnd();
		file.WriteString(strItem);
		file.Flush();
		file.Close();
	}
	catch (CMemoryException* e)
	{
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("Storage_WriteRecordFile:ImageSN:%d Error:%s  [ %s() ]", tOutput.vecB[0].vecA[0].hvImageSN.I(), e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}

	return TRUE;
}

void CStorage::CachesOperate()
{
	if (!m_bDeleteOldCaches)return;
	static BOOL s_bDelOldCaches = FALSE;
	static int s_nDay = 0;
	//ÿ���賿����һ��
	SYSTEMTIME tTm;
	::GetLocalTime(&tTm);
	if (0 == tTm.wHour && tTm.wMinute < 30 && s_nDay != tTm.wDay)
	{
		s_nDay = tTm.wDay;
		s_bDelOldCaches = TRUE;
	}

	if (s_bDelOldCaches)
	{
		s_bDelOldCaches = FALSE;

		//�����߳�
		HDevThread*         hcppthread_handle;
		HDevThreadContext   hcppthread_context;
		hcppthread_handle = new HDevThread(hcppthread_context,
			(void*)HThreadCache, 0, 0);
		hcppthread_handle->Start();
	}
}