//////////////////////////////////////////////////////////////////////////
///FoolProofing.h �����ж��࣬���ڸ�������ķ����ж�
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"

class CFoolProofing : public CBaseTmpl
{
public:
	struct tagFoolProofingParam
	{
		BOOL	bUseFoolProofing;		//�Ƿ�ʹ�÷�������
		int		nMinImageGray;			//��Сͼ��Ҷȣ����ڴ˻Ҷ���Ϊ�����û�е�����
	};
public:
	CFoolProofing();
	~CFoolProofing();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//�����ж�
	BOOL FoolProofing(C_MergeWork& tOutput);
private:
	//�ж�ͼ���������ж�ͼ���ǲ��ǲ��ϸ�����쳣
	BOOL JudgeImageQuality(C_MergeWork& tOutput);

private:
	tagFoolProofingParam m_tParam;

};
