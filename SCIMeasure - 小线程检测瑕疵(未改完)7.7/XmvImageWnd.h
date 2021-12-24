#pragma once
#include "stdafx.h"
#include <vector>
// CXmvImageWnd
#define IDR_ZOOM_P      100//ָʾ
#define IDR_ZOOM_IN     101//�Ŵ�
#define IDR_ZOOM_OUT    102//��С
#define IDR_ZOOM_MOVE   103//�϶�
#define IDR_ZOOM_CENTER 104//����
#define IDR_ZOOM_REVERT 105//��ԭ
enum eXmvDispLocate
{
	XDL_LeftTop = 0,
	XDL_RightTop,
	XDL_RightButtom,
	XDL_LeftButtom,
	XDL_Center,
};
enum eDispCoordSystem
{
	DCS_Window = 0,
	DCS_Image,
};
class CXmvImageWnd : public CStatic
{
	DECLARE_DYNAMIC(CXmvImageWnd)

public:
	CXmvImageWnd();
	virtual ~CXmvImageWnd();
public:
	//��ʾͼ��
	void DispImage(const HObject& hoImage,BOOL bAdaptive = TRUE/*�����True�Ļ� ֱ������Ӧ��ʾ*/);
	//��ʾ����
	void DispRegion(const HObject& hoRegion,const CString& strColor,const CString& strMode,const int nLineWidth = 1,BOOL bClear = TRUE);
	//��ʾ��Ϣ
	void DispMessage(const CString& strMsg, const int& nRow, const int& nColumn, const CString& strColor = "blue");
	void DispMessage(const CString& strMsg, const eXmvDispLocate& eLocate = XDL_LeftTop, const CString& strColor = "blue");
	void DispMessage(const CString& strMsg, const int& nRow, const int& nColumn, const CString& strColor = "blue", const eDispCoordSystem& eCoord = DCS_Window, BOOL bClear = TRUE);
	//��ȡ��ʾ���
	HTuple GetDispWnd();
	//��ȡ��ʾͼ��
	HObject GetDispImage();
	//���û���ģʽ
	void SetDrawing(BOOL bDrawing = FALSE);
	//�Ƿ��ǻ���ģʽ
	BOOL GetDrawing();
	//��ʾ��ǰ����ͼ��
	void DispImage(BOOL bAdaptive = TRUE);
	//��ʾ��ǰ��������
	void DispRegion();
	//���ͼ��
	void ClearImage();
	//�������
	void ClearRegion();
	//�����Ϣ
	void ClearMessage();

	//��ȡ���λ��
	void GetMPosition(HTuple* hvRow, HTuple* hvCol);

private:
	
	void SetPart(const HTuple& Row1, const HTuple& Column1, const HTuple& Row2, const HTuple& Column2);
	HTuple m_hDispWnd,m_hvRegionColor,m_hvRegionMode,m_hvLineWidth;
	double m_dDispImagePartRow0;
	double m_dDispImagePartCol0;
	double m_dDispImagePartRow1;
	double m_dDispImagePartCol1;
	HObject m_hoCurrentDispImage;
	std::vector<HObject> m_vec_hoCurrentDispRegion;
	int    m_nCurrentMode;//��ǰģʽ���������ù����״
	BOOL   m_bLeftButtonDown;
	CPoint m_ptBtnDown;;
	BOOL   m_bDrawing;

	std::vector<CString> m_vec_strMsg;
	std::vector<int> m_vec_nX;
	std::vector<int> m_vec_nY;
	std::vector<CString> m_vec_strMsgColor;
	std::vector<CString> m_vec_strCoord;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
private:
	CMenu m_Menu;
	afx_msg void OnMenuZoomIn();
	afx_msg void OnMenuZoomOut();
	afx_msg void OnMenuZoomP();
	afx_msg void OnMenuZoomMove();
	afx_msg void OnMenuZoomCenter();
	afx_msg void OnMenuZoomRevert();
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


