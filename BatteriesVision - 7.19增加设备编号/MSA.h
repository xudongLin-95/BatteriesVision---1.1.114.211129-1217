#pragma once
class CMSA
{
public:
	CMSA();
	~CMSA();
public:
	void   MSAFrame(C_MergeWork& tOutput);
	void   Init();
	//void   Set(BOOL bValue = TRUE);
	HTuple m_R_hvFHeight[2][2];
	HTuple m_R_hvFWidth[2][2];
	HTuple m_R_hvLG[2][2];
	HTuple m_R_hvDistance[2][2];
	HTuple m_R_hvEALength[2][2];

	HTuple m_hvFHeight[2][2];
	HTuple m_hvFWidth[2][2];
	HTuple m_hvLG[2][2];
	HTuple m_hvDistance[2][2];
	HTuple m_hvEALength[2][2];

	double dScore;
	int    m_nFNum;
};

