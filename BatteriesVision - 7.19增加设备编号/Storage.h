#pragma once
#include "BaseTmpl.h"

class CStorage : public CBaseTmpl
{
public:
	enum eSaveType
	{
		SaveNone=0,		//不存记录
		SaveError,		//只存错误记录
		SaveAll,		//保存所有记录
	};
	enum eOKImageSaveType
	{
		OKOriginal=0,	//原始整张图像
		OKCompressed,	//缩略整张图像
	};
	enum eNGImageSaveType
	{
		NGOriginal=0,	//原始整张图像
		NGCompressed,	//缩略整张图像
		NGPartOriginal,	//缺陷部位原图
	};
	struct tagImageSaveType
	{
		eSaveType eImageSaveType;
		eOKImageSaveType eOKSaveType;
		eNGImageSaveType eNGSaveType;
	};
	enum eImageFormat
	{
		Bmp=0,	//常用无损图像
		Jpg,	//压缩图像
		Png,	//可压缩图像
		Tiff,	//压缩图像
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

	//获取存储所在磁盘空间大小
	double GetRemainDiskGB();

	//获取存储数据的路径
	CString GetDataSaveFolderPath() { return m_strDataSaveFolder; }
	//获取存储图像的路径
	CString GetImageSaveFolderPath() { return m_strImageSaveFolder; }
	//生成存储数据的路径
	void GenSaveFolderPath();
	//获取文件存储的路径
	CString GetSaveDataFilePath() { return m_strSaveFilePath; }

	//设置GRR开始
	void SetGrrNewStart() { m_bGrrNewStart = TRUE; }

	//生成数据文件存储路径
	void GenDataFileSavePath();

	//开线程，执行缓存操作
	void CachesOperate();
private:
	void InitQueues();
	void ReleaseQueues();
	BOOL InitThread();
	BOOL ReleaseThread();

	//获取要写入文件的时间数据
	CString GetSaveTimeS();
	CString GetSaveTimeS2();
	//保存数据
	void SaveData(C_MergeWork& tOutput);
	//保存图片
	void SaveImage(C_MergeWork& tOutput);
	//保存瑕疵局部缩略图片
	void SaveDefectThumbnailImage(C_MergeWork& tOutput);
	//获取有效的记录保存名
	CString GetRecordSaveFileName(const CString& strSaveDir,const CString& strNameOri,const CString& strNameNew);
	//创建检测记录文件
	BOOL CreateRecordFile(const CString& strFilePath);
	BOOL CreateRecordFile_wet(const CString& strFilePath);
	//写入检测记录文件
	void WriteRecordFile(const CString& strFilePath, C_MergeWork& tOutput);
	//写入检测记录文件,所有数据
	void WriteRecordFileAll(const CString& strFilePath, C_MergeWork& tOutput);
	//写入湿膜的数据，不加在线补偿的数据
	void WriteRecordFileWet(const CString& strFilePath, C_MergeWork& tOutput);
	//判断空间是否够用
	BOOL IsDiskRemainEnough();
	//判断停止存图时间是否到了
	BOOL IsStopSaveTimeCome();

	//写Grr数据
	BOOL WriteGRRFile(C_MergeWork& tOutput);
	//写点检数据
	BOOL WriteSpotInspectionFile(C_MergeWork& tOutput);
	//创建首件数据文件
	BOOL CreateFirstArticleFile(const CString& strFilePath);
	//写首件数据
	BOOL WriteFirstArticleFile(C_MergeWork& tOutput);

	//创建瑕疵文件
	BOOL CreateDefectFile(const CString& strFilePath);
	//写瑕疵数据(客户要求单独输出)
	BOOL WriteDefectFile(const CString& strFilePath, C_MergeWork& tOutput);

	//检测记录保存线程
	static void* HThreadStorage(void *hcthread);
	//保存线程函数
	static void T_Save(HTuple hv_SaveQueue, HTuple hv_ThreadIndex);
	//缓存线程
	static void* HThreadCache(void *hcthread);

	//删除过期缓存
	void DeleteOldCaches();
	//定时备份数据
	void BackupData();

	CString GetRecoredSaveFileName(const CString& strSaveDir, const CString& strOriName, const CString& strNewName);
	//根据所给的文件名，找到同目录下序号最大的那个同卷号文件名
	CString GetMaxBatchNumFile(const CString&strFileName);
	//递归遍历文件夹下满足条件的文件
	void GetFileNamePath(std::vector<CString>&vFilePathList, const CString&sPath, const CString&sName);
	//递归遍历文件夹下所有文件
	void TraverseDir(const CString& strDir, std::vector<CString>& vecFiles, std::vector<CString>& vecPaths, std::vector<CString>& vecFileNames);
private:
	int				m_nSaveThreadNum;				//保存线程数量
	HTupleVector	m_hvecStorageQueues;			//存储队列句柄
	tagImageSaveType m_tImageSaveType;				//图像保存选择
	eSaveType		m_eRecordSaveType;				//检测记录保存选择
	int				m_nSingleRecordFileSize;		//单个检测文件大小MB
	BOOL			m_bSaveErrorTag;				//是否对缺陷图像标记

	eImageFormat	m_eOKImageFormat;				//OK图像保存格式
	eImageFormat	m_eNGImageFormat;				//NG图像保存格式

	double			m_dImageCompressScale;			//图像压缩系数

	CString			m_strMapFolderPath;				//记录图像文件夹路径
	CString			m_strDataSaveFolder;			//数据记录路径
	CString			m_strImageSaveFolder;			//图像记录路径
	CString			m_strThumbImageSaveFolder;		//缩略图像记录路径
	BOOL			m_bUseProgramStartSaveTime;		//是否使用开机定时存图像
	int				m_nProgramStartSaveSecond;		//开机保存图像的秒数
	int				m_nMinRemainStorage;			//磁盘最小剩余空间GB
	BOOL			m_bDeleteOldCaches;				//是否删除过期缓存
	int				m_nCacheSaveDays;				//保存的记录缓存天数
	BOOL			m_bSaveCalibrationImage;		//是否保存标定图片

	ThreadList      m_vecThreadHandle;

	SYSTEMTIME		m_tSaveTime;

	BOOL			m_bGrrNewStart;						//Grr新开始
	CString			m_strSaveFilePath;					//保存数据文件路径
	CString			m_strSaveFilePathAll;				//保存全部数据文件路径

	CStdioFile		m_fileRecord;
	CStdioFile		m_fileRecordAll;
	BOOL			m_strFileRecoredOpened;
	BOOL			m_strFileRecoredAllOpened;
	CString			m_strLastFileRecordName;			//上次保存的文件名
	BOOL			m_isFirstGetLastRecored;
	CString			m_strLastFileRecordAllName;			//上次保存的文件名
	BOOL			m_isFirstGetLastRecoredAll;

	//湿膜数据
	CString			m_strSaveFilePath_wet;				//保存数据文件路径
	CStdioFile		m_fileRecord_wet;
	BOOL			m_strFileRecoredOpened_wet;
	CString			m_strLastFileRecordName_wet;		//上次保存的文件名
	BOOL			m_isFirstGetLastRecored_wet;
};