#pragma once
class CLineToolPara
{
public:
	double	m_dSigma;			//对应measure_pos中的Sigma
	int		m_nThr;				//对应measure_pos中的Threshold
	int		m_nDirect;			//对应measure_pos中的Transition:0-'all', 1-'positive', 2-'negative' 
	int		m_nSelect;			//对应measure_pos中的Select:0-'all', 1-'first', 2-'last' 

	double	m_dWidth;			//单个检测框的宽度
	double	m_dHeight;			//单个检测框的高度
	int		m_nMeasureNum;		//检测框的个数
	double	m_dStartRow;		//起始点的Row坐标
	double	m_dStartCol;		//起始点的Col坐标
	double	m_dEndRow;			//终止点的Row坐标
	double	m_dEndCol;			//终止点的Col坐标

	int     m_nChoice;          //线的位置是否使用正常拟合的方式

	int ReadParam(CString strName);			//从指定文件中加载参数
	int WriteParam(CString strName);		//参数存入指定文件中

	CLineToolPara AffineTrans(const HTuple& HomMat2d);
public:
	CLineToolPara(void);
	~CLineToolPara(void);
};

