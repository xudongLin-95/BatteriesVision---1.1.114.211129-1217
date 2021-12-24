#pragma once
class CSoapMesLog
{
private:
	CRITICAL_SECTION m_csAssignment;
	CRITICAL_SECTION m_csUploadRecord;
	CRITICAL_SECTION m_csHeatBeat;
	CRITICAL_SECTION m_csEquipmentProductProcess;
	CRITICAL_SECTION m_csRecipe;
	CRITICAL_SECTION m_csGetRecipeList;
	CRITICAL_SECTION m_csGetRecipe;
	CRITICAL_SECTION m_csRecipeVersionExamine;
	CRITICAL_SECTION m_csCCDFileUploadComplete;
private:
	CSoapMesLog(void);
	static CSoapMesLog* m_instance;
	void	Init();
	void	GetFilePath(std::vector<CString>& vFilePathList, const CString &strDir);
	void	DeleteOldLog(const CString & cstrLogFile);
	void	doDeleteFile(const CString & cstrFile);
private: 
	CString m_cstrAssignmentPath;
	CString m_cstrUploadRecordPath;
	CString m_cstrHeatBeatPath;
	CString m_cstrEquipmentProductProcessPath;
	CString m_cstrRecipePath;
	CString m_cstrGetRecipeListPath;
	CString m_cstrGetRecipePath;
	CString m_cstrRecipeVersionExaminePath;
	CString m_cstrCCDFileUploadCompletePath;
	int m_nSaveDay;
	int m_nCount;
public:
	

public:
	
	~CSoapMesLog(void);
static	CSoapMesLog* getInstance();
	void WriteLog( const CString& cstrMsg ,const CString& cstrLogFile);
	void AssignmentLog( const CString& cstrMsg);
	void UploadFileLog( const CString& cstrMsg);
	void HeatBeatLog( const CString& cstrMsg);
	void EquipmentProductProcessLog( const CString& cstrMsg);
	void RecipeLog( const CString& cstrMsg);
	void GetRecipeListLog( const CString& cstrMsg);
	void GetRecipeLog( const CString& cstrMsg);
	void RecipeVersionExamineLog( const CString& cstrMsg);
	void CCDFileUploadCompleteLog( const CString& cstrMsg);
	void SetSaveDay( const int& nDay);

};


