//////////////////////////////////////////////////////////////////////////
///FoolProofing.h 防呆判断类，用于各种情况的防呆判断
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CFoolProofing : public CBaseTmpl
{
public:
	struct tagFoolProofingParam
	{
		BOOL	bUseFoolProofing;		//是否使用防呆功能
		int		nMinImageGray;			//最小图像灰度，低于此灰度认为是相机没有调整好
	};
public:
	CFoolProofing();
	~CFoolProofing();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//防呆判断
	BOOL FoolProofing(C_MergeWork& tOutput);
private:
	//判断图像质量，判断图像是不是不合格，提出异常
	BOOL JudgeImageQuality(C_MergeWork& tOutput);

private:
	tagFoolProofingParam m_tParam;

};
