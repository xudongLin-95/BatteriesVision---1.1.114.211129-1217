#ifndef __Mes_Data_Type__
#define __Mes_Data_Type__

#include "afx.h"

struct	ParamCCDUploadFile
{
	int	nPassed;				//是否合格，空为合格
	CString cstrSFC;				//条码
	CString cstrURI;				//上传完成路径

	ParamCCDUploadFile():
		cstrSFC(""),
		nPassed(0),
		cstrURI("")
	{
	}
	ParamCCDUploadFile(const ParamCCDUploadFile & a):
		cstrSFC(a.cstrSFC),
		cstrURI(a.cstrURI),
		nPassed(a.nPassed)
	{
	}
	ParamCCDUploadFile(const CString & strSFC ,const CString& strURL = "",const int & iPassed =0 ):
		cstrSFC(strSFC),
		cstrURI(strURL),
		nPassed(iPassed)
	{
	}

};

struct	ParamProductProcessBase
{
	CString	cstrParamCode;				//代码
	CString cstrParamValue;				//值
	time_t timeStamp;					//时间戳

	ParamProductProcessBase():
		cstrParamCode(""),
		cstrParamValue("")	
	{
		time(&timeStamp);
	}
	ParamProductProcessBase(const ParamProductProcessBase & a):
		cstrParamCode(a.cstrParamCode),
		cstrParamValue(a.cstrParamValue),
		timeStamp(a.timeStamp)
	{
	}
	ParamProductProcessBase(const CString & strCode,const time_t & tTime ,const CString& strValue = ""):
		cstrParamCode(strCode),
		cstrParamValue(strValue),
		timeStamp(tTime)
	{
	}
	ParamProductProcessBase(const CString & strCode,const CString& strValue = ""):
		cstrParamCode(strCode),
		cstrParamValue(strValue)
	{
		time(&timeStamp);
	}
};
struct	ParamEquipmentProductProcess
{
	CString cstrSFC;
	std::vector<ParamProductProcessBase> vecParamList;
	ParamEquipmentProductProcess():
		cstrSFC("")
	{
		vecParamList.clear();
	}

	ParamEquipmentProductProcess(const ParamEquipmentProductProcess& pP):
		cstrSFC(pP.cstrSFC)
	{
		for (std::vector<ParamProductProcessBase>::const_iterator itr = pP.vecParamList.begin();itr != pP.vecParamList.end();itr++)
		{
			vecParamList.push_back(*itr);
		}
	}
	ParamEquipmentProductProcess(const CString strSFC):
		cstrSFC(strSFC)
	{
		vecParamList.clear();
		
	}
	ParamEquipmentProductProcess(std::vector<ParamProductProcessBase> tParamList):
	cstrSFC("")
	{
		for (std::vector<ParamProductProcessBase>::const_iterator itr = tParamList.begin();itr != tParamList.end();itr++)
		{
			vecParamList.push_back(*itr);
		}
	}
	ParamEquipmentProductProcess(const CString strSFC,std::vector<ParamProductProcessBase> tParamList):
	cstrSFC(strSFC)
	{
		for (std::vector<ParamProductProcessBase>::const_iterator itr = tParamList.begin();itr != tParamList.end();itr++)
		{
			vecParamList.push_back(*itr);
		}
	}

};
struct	ParamRecipeVersionExamine
{
	CString cstrRecipeCode;
	CString cstrVersion;
	ParamRecipeVersionExamine():cstrRecipeCode(""),cstrVersion("")
	{
	}
	ParamRecipeVersionExamine(const ParamRecipeVersionExamine & tRecipeVersion):cstrRecipeCode(tRecipeVersion.cstrRecipeCode),cstrVersion(tRecipeVersion.cstrVersion)
	{
	}
	ParamRecipeVersionExamine(const CString & strRecipeCode, const CString & strRecipeVersion = ("")):cstrRecipeCode(strRecipeCode),cstrVersion(strRecipeVersion)
	{
	}

};

struct	ParamRecipeBase
{
	CString	cstrParamCode;				
	CString cstrParamValue;	
	CString cstrParamUpper;
	CString cstrParamLower;
	time_t timeStamp;				

	ParamRecipeBase():cstrParamCode(""),cstrParamValue(""),cstrParamUpper(""),cstrParamLower("")
	{
		time(&timeStamp);
	}
	ParamRecipeBase(const ParamRecipeBase & a):cstrParamCode(a.cstrParamCode),cstrParamValue(a.cstrParamValue),cstrParamUpper(a.cstrParamUpper),
		cstrParamLower(a.cstrParamLower),timeStamp(a.timeStamp)
	{
	}
	ParamRecipeBase(const CString & strCode,const CString & strUpper,const CString & strLower,const time_t & tTime ,const CString& strValue = ""):
	cstrParamCode(strCode),cstrParamValue(strValue),cstrParamUpper(strUpper),cstrParamLower(strLower),timeStamp(tTime)
	{
	}
	ParamRecipeBase(const CString & strCode,const CString& strValue = "",const CString& strUpper = "",const CString& strLower = ""):
	cstrParamCode(strCode),cstrParamValue(strValue),cstrParamUpper(strUpper),cstrParamLower(strLower)
	{
		time(&timeStamp);
	}
};

struct	ParamRecipe 
{
	CString	cstrProductCode;	
	CString	cstrRecipeCode;
	CString	cstrVersion;
	std::vector<ParamRecipeBase>vecParamList;

	ParamRecipe():cstrProductCode(""),cstrRecipeCode(""),cstrVersion("")
	{
		vecParamList.clear();
	}
	ParamRecipe(const ParamRecipe& pP):cstrProductCode(pP.cstrProductCode),cstrRecipeCode(pP.cstrRecipeCode),cstrVersion(pP.cstrVersion)
	{
		for (std::vector<ParamRecipeBase>::const_iterator itr = pP.vecParamList.begin();itr != pP.vecParamList.end();itr++)
		{
			vecParamList.push_back(*itr);
		}
	}
	ParamRecipe(const CString strProductCode,const CString strRecipeCode = "",const CString strVersion = ""):
		cstrProductCode(strProductCode),cstrRecipeCode(strRecipeCode),cstrVersion(strVersion)
	{
		vecParamList.clear();

	}
	ParamRecipe(std::vector<ParamRecipeBase> tParamList):cstrProductCode(""),cstrRecipeCode(""),cstrVersion("")
	{
		for (std::vector<ParamRecipeBase>::const_iterator itr = tParamList.begin();itr != tParamList.end();itr++)
		{
			vecParamList.push_back(*itr);
		}
	}
	ParamRecipe(const CString strProductCode,const CString strRecipeCode ,const CString strVersion ,std::vector<ParamRecipeBase> tParamList):
		cstrProductCode(strProductCode),cstrRecipeCode(strRecipeCode),cstrVersion(strVersion)
	{
		for (std::vector<ParamRecipeBase>::const_iterator itr = tParamList.begin();itr != tParamList.end();itr++)
		{
			vecParamList.push_back(*itr);
		}
	}

};

#endif