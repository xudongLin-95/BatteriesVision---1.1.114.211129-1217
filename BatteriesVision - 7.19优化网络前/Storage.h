#pragma once
#include "BaseTmpl.h"

class CStorage : public CBaseTmpl
{
public:
	enum eSaveType
	{
		SaveNone=0,		//�����¼
		SaveError,		//ֻ������¼
		SaveAll,		//�������м�¼
	};
	enum eOKImageSaveType
	{
		OKOriginal=0,	//ԭʼ����ͼ��
		OKCompressed,	//��������ͼ��
	};
	enum eNGImageSaveType
	{
		NGOriginal=0,	//ԭʼ����ͼ��
		NGCompressed,	//��������ͼ��
		NGPartOriginal,	//ȱ�ݲ�λԭͼ
	};
	struct tagImageSaveType
	{
		eSaveType eImageSaveType;
		eOKImageSaveType eOKSaveType;
		eNGImageSaveType eNGSaveType;
	};
	enum eImageFormat
	{
		Bmp=0,	//��������ͼ��
		Jpg,	//ѹ��ͼ��
		Png,	//��ѹ��ͼ��
		Tiff,	//ѹ��ͼ��
	};
	typedef std::vector<HDevThread*> ThreadList;
public:
	CStorage();
	~CStorage();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	void Enqueue(const int& nSN,const HTuple& MessageHandle, const HTuple& GenParamName=HTuple(), const HTuple& GenParamValue=HTuple());

	int		GetSaveThreadNum(){return m_nSaveThreadNum;}
	HTuple	GetStorageQueue(const int& nSN){return m_hvecStorageQueues[nSN].T();}

	BOOL	IsSaveCalibrationImage(){return m_bSaveCalibrationImage;}
	void	SetSaveCalibrationImage(const BOOL& bSave=FALSE){m_bSaveCalibrationImage = bSave;}

	BOOL	GetSaveErrorTag(){return m_bSaveErrorTag;}
	void	SetSaveErrorTag(const BOOL& bSave=FALSE){m_bSaveErrorTag = bSave;}

	CString GetMapFolderPath(){return m_strMapFolderPath;}
	void	SetMapFolderPath(const CString& strPath){m_strMapFolderPath = strPath;}

	int		GetImageSaveType(){return (int)m_tImageSaveType.eImageSaveType;}
	void	SetImageSaveType(const int& nType){m_tImageSaveType.eImageSaveType = (eSaveType)nType;}
	int		GetOKImageSaveType(){return (int)m_tImageSaveType.eOKSaveType;}
	void	SetOKImageSaveType(const int& nType){m_tImageSaveType.eOKSaveType = (eOKImageSaveType)nType;}
	int		GetNGImageSaveType(){return (int)m_tImageSaveType.eNGSaveType;}
	void	SetNGImageSaveType(const int& nType){m_tImageSaveType.eNGSaveType = (eNGImageSaveType)nType;}

	int		GetRecordSaveType(){return (int)m_eRecordSaveType;}
	void	SetRecordSaveType(const int& nType){m_eRecordSaveType = (eSaveType)nType;}

	int		GetOKImageSaveFormat(){return m_eOKImageFormat;}
	void	SetOKImageSaveFormat(const int& nFmt){m_eOKImageFormat = (eImageFormat)nFmt;}
	int		GetNGImageSaveFormat(){return m_eNGImageFormat;}
	void	SetNGImageSaveFormat(const int& nFmt){m_eNGImageFormat = (eImageFormat)nFmt;}

	BOOL	GetUseProgramStartSaveTime(){return m_bUseProgramStartSaveTime;}
	void	SetUseProgramStartSaveTime(const BOOL& bUse){m_bUseProgramStartSaveTime = bUse;}
	int		GetProgramStartSaveSeconds(){return m_nProgramStartSaveSecond;}
	void	SetProgramStartSaveSeconds(const int& nMinutes){ m_nProgramStartSaveSecond = nMinutes;}

	int		GetMinRemainStorageGB(){return m_nMinRemainStorage;}
	void	SetMinRemainStorageGB(const int& nGB){m_nMinRemainStorage = nGB;}

	BOOL	GetDeleteOldCaches(){return m_bDeleteOldCaches;}
	void	SetDeleteOldCaches(const BOOL& bDelete){m_bDeleteOldCaches = bDelete;}
	int		GetCacheSaveDays(){return m_nCacheSaveDays;}
	void	SetCacheSaveDays(const int& nDays){m_nCacheSaveDays = nDays;}

	//��ȡ�洢���ڴ��̿ռ��С
	double GetRemainDiskGB();

	//��ȡ�洢���ݵ�·��
	CString GetDataSaveFolderPath() { return m_strDataSaveFolder; }
	//��ȡ�洢ͼ���·��
	CString GetImageSaveFolderPath() { return m_strImageSaveFolder; }
	//���ɴ洢���ݵ�·��
	void GenSaveFolderPath();
	//��ȡ�ļ��洢��·��
	CString GetSaveDataFilePath() { return m_strSaveFilePath; }

	//����GRR��ʼ
	void SetGrrNewStart() { m_bGrrNewStart = TRUE; }

	//���������ļ��洢·��
	void GenDataFileSavePath();

	//���̣߳�ִ�л������
	void CachesOperate();
private:
	void InitQueues();
	void ReleaseQueues();
	BOOL InitThread();
	BOOL ReleaseThread();

	//��ȡҪд���ļ���ʱ������
	CString GetSaveTimeS();
	CString GetSaveTimeS2();
	//��������
	void SaveData(C_MergeWork& tOutput);
	//����ͼƬ
	void SaveImage(C_MergeWork& tOutput);
	//����覴þֲ�����ͼƬ
	void SaveDefectThumbnailImage(C_MergeWork& tOutput);
	//��ȡ��Ч�ļ�¼������
	CString GetRecordSaveFileName(const CString& strSaveDir,const CString& strNameOri,const CString& strNameNew);
	//��������¼�ļ�
	BOOL CreateRecordFile(const CString& strFilePath);
	BOOL CreateRecordFile_wet(const CString& strFilePath);
	//д�����¼�ļ�
	void WriteRecordFile(const CString& strFilePath, C_MergeWork& tOutput);
	//д�����¼�ļ�,��������
	void WriteRecordFileAll(const CString& strFilePath, C_MergeWork& tOutput);
	//д��ʪĤ�����ݣ��������߲���������
	void WriteRecordFileWet(const CString& strFilePath, C_MergeWork& tOutput);
	//�жϿռ��Ƿ���
	BOOL IsDiskRemainEnough();
	//�ж�ֹͣ��ͼʱ���Ƿ���
	BOOL IsStopSaveTimeCome();

	//дGrr����
	BOOL WriteGRRFile(C_MergeWork& tOutput);
	//д�������
	BOOL WriteSpotInspectionFile(C_MergeWork& tOutput);
	//�����׼������ļ�
	BOOL CreateFirstArticleFile(const CString& strFilePath);
	//д�׼�����
	BOOL WriteFirstArticleFile(C_MergeWork& tOutput);

	//����覴��ļ�
	BOOL CreateDefectFile(const CString& strFilePath);
	//д覴�����(�ͻ�Ҫ�󵥶����)
	BOOL WriteDefectFile(const CString& strFilePath, C_MergeWork& tOutput);

	//����¼�����߳�
	static void* HThreadStorage(void *hcthread);
	//�����̺߳���
	static void T_Save(HTuple hv_SaveQueue, HTuple hv_ThreadIndex);
	//�����߳�
	static void* HThreadCache(void *hcthread);

	//ɾ�����ڻ���
	void DeleteOldCaches();
	//��ʱ��������
	void BackupData();

	CString GetRecoredSaveFileName(const CString& strSaveDir, const CString& strOriName, const CString& strNewName);
	//�����������ļ������ҵ�ͬĿ¼����������Ǹ�ͬ����ļ���
	CString GetMaxBatchNumFile(const CString&strFileName);
	//�ݹ�����ļ����������������ļ�
	void GetFileNamePath(std::vector<CString>&vFilePathList, const CString&sPath, const CString&sName);
	//�ݹ�����ļ����������ļ�
	void TraverseDir(const CString& strDir, std::vector<CString>& vecFiles, std::vector<CString>& vecPaths, std::vector<CString>& vecFileNames);
private:
	int				m_nSaveThreadNum;				//�����߳�����
	HTupleVector	m_hvecStorageQueues;			//�洢���о��
	tagImageSaveType m_tImageSaveType;				//ͼ�񱣴�ѡ��
	eSaveType		m_eRecordSaveType;				//����¼����ѡ��
	int				m_nSingleRecordFileSize;		//��������ļ���СMB
	BOOL			m_bSaveErrorTag;				//�Ƿ��ȱ��ͼ����

	eImageFormat	m_eOKImageFormat;				//OKͼ�񱣴��ʽ
	eImageFormat	m_eNGImageFormat;				//NGͼ�񱣴��ʽ

	double			m_dImageCompressScale;			//ͼ��ѹ��ϵ��

	CString			m_strMapFolderPath;				//��¼ͼ���ļ���·��
	CString			m_strDataSaveFolder;			//���ݼ�¼·��
	CString			m_strImageSaveFolder;			//ͼ���¼·��
	CString			m_strThumbImageSaveFolder;		//����ͼ���¼·��
	BOOL			m_bUseProgramStartSaveTime;		//�Ƿ�ʹ�ÿ�����ʱ��ͼ��
	int				m_nProgramStartSaveSecond;		//��������ͼ�������
	int				m_nMinRemainStorage;			//������Сʣ��ռ�GB
	BOOL			m_bDeleteOldCaches;				//�Ƿ�ɾ�����ڻ���
	int				m_nCacheSaveDays;				//����ļ�¼��������
	BOOL			m_bSaveCalibrationImage;		//�Ƿ񱣴�궨ͼƬ

	ThreadList      m_vecThreadHandle;

	SYSTEMTIME		m_tSaveTime;

	BOOL			m_bGrrNewStart;						//Grr�¿�ʼ
	CString			m_strSaveFilePath;					//���������ļ�·��
	CString			m_strSaveFilePathAll;				//����ȫ�������ļ�·��

	CStdioFile		m_fileRecord;
	CStdioFile		m_fileRecordAll;
	BOOL			m_strFileRecoredOpened;
	BOOL			m_strFileRecoredAllOpened;
	CString			m_strLastFileRecordName;			//�ϴα�����ļ���
	BOOL			m_isFirstGetLastRecored;
	CString			m_strLastFileRecordAllName;			//�ϴα�����ļ���
	BOOL			m_isFirstGetLastRecoredAll;

	//ʪĤ����
	CString			m_strSaveFilePath_wet;				//���������ļ�·��
	CStdioFile		m_fileRecord_wet;
	BOOL			m_strFileRecoredOpened_wet;
	CString			m_strLastFileRecordName_wet;		//�ϴα�����ļ���
	BOOL			m_isFirstGetLastRecored_wet;
};