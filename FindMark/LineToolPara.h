#pragma once
class CLineToolPara
{
public:
	double	m_dSigma;			//��Ӧmeasure_pos�е�Sigma
	int		m_nThr;				//��Ӧmeasure_pos�е�Threshold
	int		m_nDirect;			//��Ӧmeasure_pos�е�Transition:0-'all', 1-'positive', 2-'negative' 
	int		m_nSelect;			//��Ӧmeasure_pos�е�Select:0-'all', 1-'first', 2-'last' 

	double	m_dWidth;			//��������Ŀ��
	double	m_dHeight;			//��������ĸ߶�
	int		m_nMeasureNum;		//����ĸ���
	double	m_dStartRow;		//��ʼ���Row����
	double	m_dStartCol;		//��ʼ���Col����
	double	m_dEndRow;			//��ֹ���Row����
	double	m_dEndCol;			//��ֹ���Col����

	int     m_nChoice;          //�ߵ�λ���Ƿ�ʹ��������ϵķ�ʽ

	int ReadParam(CString strName);			//��ָ���ļ��м��ز���
	int WriteParam(CString strName);		//��������ָ���ļ���

	CLineToolPara AffineTrans(const HTuple& HomMat2d);
public:
	CLineToolPara(void);
	~CLineToolPara(void);
};

