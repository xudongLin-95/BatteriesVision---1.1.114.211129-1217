#include "StdAfx.h"
#include "Find.h"
#include "DlgParamSet.h"



CFindMark::CFindMark(void)
{
	InitializeCriticalSection(&m_csFindChange);
}


CFindMark::~CFindMark(void)
{
	ClearMark();
	DeleteCriticalSection(&m_csFindChange);
}
CFindMark& CFindMark::operator= (const CFindMark& in)
{
	
	m_tInitParam=in.m_tInitParam;
	m_tMarkList=in.m_tMarkList;
	return *this;
}
int CFindMark::ShowDialog(HObject ho_Image)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	try
	{
		CDlgParamSet dlg;
		//dlg.SetOwner(this);
		//dlg.m_hoCurrentDispImage.GenEmptyObj();
		//CopyObj(ho_Image,&dlg.m_hoCurrentDispImage,1,ho_Image.CountObj());
		//dlg.DoModal();


	    dlg.SetOwner(this);
		dlg.m_pOwner->DoSomething("COPYOBJECTS",this,NULL);
		dlg.m_hoCurrentDispImage = ho_Image;
		dlg.DoModal();



	}catch(HException)
	{

	}
	/*
	CDlgParamSet* pDlg = new CDlgParamSet;
	try
	{
		pDlg->m_hoCurrentDispImage.GenEmptyObj();
		CopyObj(ho_Image,&pDlg->m_hoCurrentDispImage,1,ho_Image.CountObj());

	}catch(HException)
	{

	}
	pDlg->SetOwner(this);
	pDlg->Create(IDD_DIALOG_PARAM);
	pDlg->ShowWindow(SW_NORMAL);
	pDlg->CenterWindow();
	pDlg->SetFocus();*/
	return TRUE;
}
int CFindMark::Init(NS_SCIFindMark::INITPARAM& tInitParam)//初始化
{
	m_tInitParam = tInitParam;
	m_strConfigPath.Format("%s\\Config\\Products\\%s\\SCIFindMark\\%d",GetCurrentAppPath(),tInitParam.strProductName,tInitParam.nCameraSN);
	CreateMultilevelPath(m_strConfigPath);
	m_strConfigFile.Format("%s\\Config\\Products\\%s\\SCIFindMark\\%d\\Config.ini",GetCurrentAppPath(),tInitParam.strProductName,tInitParam.nCameraSN);
	m_nType = GetPrivateProfileInt("Mark","Type",0,m_strConfigFile);
	ReadMark();
	
	
	return TRUE;
}


int CFindMark::Find(const HObject& ho_Image,NS_SCIFindMark::PMARKRESULT ptResult)//匹配
{
	ptResult->hv_Angle.Clear();
	ptResult->hv_Column.Clear();
	ptResult->hv_Angle.Clear();
	ptResult->hv_Score.Clear();
	ptResult->hv_AllScore.Clear();
	ptResult->ho_DispObj.GenEmptyObj();
	try
	{
		HTuple hv_Width,hv_Height;
		GetImageSize(ho_Image,&hv_Width,&hv_Height);
		std::vector<SINGLEMARKINFO>::iterator it;
		HTuple hv_RowAll, hv_ColumnAll, hv_AngleAll, hv_ScoreAll;
		HObject ho_Search,ho_ImageReduced;
		HTuple hv_Row, hv_Column, hv_Angle, hv_Score;
		for(it=m_tMarkList.begin();it!=m_tMarkList.end();it++)
		{
			//先计算出来搜索区域
			HTuple hv_Row1,hv_Column1,hv_Row2,hv_Column2;
			SmallestRectangle1(it->ho_ModleInfo.SelectObj(3),&hv_Row1,&hv_Column1,&hv_Row2,&hv_Column2);
			GenRectangle1(&ho_Search,0,hv_Column1,hv_Height-1,hv_Column2);
			ReduceDomain(ho_Image,ho_Search,&ho_ImageReduced);
			if ("形状匹配" == it->hv_ModelParam[1])
			{
				FindShapeModel(ho_ImageReduced,it->hv_ModelID,HTuple(-10).TupleRad(), HTuple(10).TupleRad(), 
					0.5, 1, 0.1, "none", 0, 0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			}else if ("灰度匹配" == it->hv_ModelParam[1])
			{
			    double dMinScore = 0.8;
				if (it->hv_ModelParam.Length()>=4)
				{
				   dMinScore = it->hv_ModelParam[3];
				}
				FindNccModel (ho_ImageReduced, it->hv_ModelID,HTuple(-10).TupleRad(), HTuple(10).TupleRad(), 
					dMinScore, 1, 0.5, "false", 0, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			}
			if (0 == hv_Score.Length() )
			{
				hv_Row    = 0;
				hv_Column = 0;
				hv_Angle  = 0;
				hv_Score  = 0;
			}
			hv_RowAll.Append(hv_Row);
			hv_ColumnAll.Append(hv_Column);
			hv_AngleAll.Append(hv_Angle);
			hv_ScoreAll.Append(hv_Score);
			ptResult->hv_AllScore.Append(hv_Score);
		}
		HTuple hv_Indices,hv_Index(0);
		TupleSortIndex(hv_ScoreAll,&hv_Indices);
		hv_Index = hv_Indices[hv_Indices.Length()-1];//匹配率最高的
		if (0 == hv_ScoreAll[hv_Index])//如果匹配率最高的是0，
		{
			ptResult->hv_Angle = 0;
			ptResult->hv_Column = 0;
			ptResult->hv_Row =0;
			ptResult->hv_Score = 0;
			return TRUE;
		}else
		{
			HObject hoSearch = m_tMarkList[hv_Index.I()].ho_ModleInfo.SelectObj(3);
			HObject hoModel  = m_tMarkList[hv_Index.I()].ho_ModleInfo.SelectObj(2);
			HObject hoLine   = m_tMarkList[hv_Index.I()].ho_Line;
			HTuple hv_HomMat2D;
			HTuple hv_ModelCenterRow,hv_ModelCenterColumn;
			AreaCenter(m_tMarkList[hv_Index.I()].ho_ModleInfo.SelectObj(2),NULL,&hv_ModelCenterRow,&hv_ModelCenterColumn);
			VectorAngleToRigid(hv_ModelCenterRow,hv_ModelCenterColumn,0,hv_RowAll[hv_Index],hv_ColumnAll[hv_Index],hv_AngleAll[hv_Index],&hv_HomMat2D);
			HObject ho_RegionAffineTrans,ho_RegionAffineTrans1;
			AffineTransRegion(hoLine,&ho_RegionAffineTrans,hv_HomMat2D,"constant");
			AffineTransRegion(hoModel,&ho_RegionAffineTrans1,hv_HomMat2D,"constant");
			AreaCenter(ho_RegionAffineTrans,NULL,&ptResult->hv_Row,&ptResult->hv_Column);
			ptResult->hv_Angle = 0;
			ptResult->hv_Score = hv_ScoreAll[hv_Index];
			ConcatObj(ho_RegionAffineTrans1,ho_RegionAffineTrans,&ptResult->ho_DispObj);
			
		}

	}catch(HException e)
	{
		return FALSE;
	}
	return TRUE;
}

int CFindMark::FindMult(const HObject& ho_Image, NS_SCIFindMark::PMARKRESULT ptResult)//匹配
{
	ptResult->hv_Angle.Clear();
	ptResult->hv_Column.Clear();
	ptResult->hv_Angle.Clear();
	ptResult->hv_Score.Clear();
	ptResult->hv_AllScore.Clear();
	ptResult->ho_DispObj.GenEmptyObj();
	try
	{
		HTuple hv_Width, hv_Height;
		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		std::vector<SINGLEMARKINFO>::iterator it;
		HTuple hv_RowAll, hv_ColumnAll, hv_AngleAll, hv_ScoreAll;
		HObject ho_Search, ho_ImageReduced;
		HTuple hv_Row, hv_Column, hv_Angle, hv_Score;
		BOOL   bAutoFindLocation=TRUE;
		int k=0;
		for (it = m_tMarkList.begin(); it != m_tMarkList.end(); it++)
		{
			//先计算出来搜索区域
			HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;
			SmallestRectangle1(it->ho_ModleInfo.SelectObj(3), &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
			GenRectangle1(&ho_Search, 0, hv_Column1, hv_Height - 1, hv_Column2);
			ReduceDomain(ho_Image, ho_Search, &ho_ImageReduced);
			HTuple hv_Row0,hv_Column0;
			HObject ho_CropPart;
			CropDomain(ho_ImageReduced,&ho_CropPart);
			SmallestRectangle1(ho_ImageReduced,&hv_Row0,&hv_Column0,NULL,NULL);

			if ("形状匹配" == it->hv_ModelParam[1])
			{
				FindShapeModel(ho_CropPart, it->hv_ModelID, HTuple(-15).TupleRad(), HTuple(15).TupleRad(),
					0.5, 0/*寻找所有匹配点*/, 0.1, "none", 0, 0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			}
			else if ("灰度匹配" == it->hv_ModelParam[1])
			{
				double dMinScore = 0.8;
				if (it->hv_ModelParam.Length() >= 4)
				{
					dMinScore = it->hv_ModelParam[3];
				}
				FindNccModel(ho_CropPart, it->hv_ModelID, HTuple(-15).TupleRad(), HTuple(15).TupleRad(),dMinScore, 0/*寻找所有匹配点*/, 0.5, "true", 0, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			}
			/*if (0 == hv_Score.Length())
			{
				hv_Row = 0;
				hv_Column = 0;
				hv_Angle = 0;
				hv_Score = 0;
			}*/

			if (hv_Score.Length()>0)
			{
				HTuple hv_ActualRow,hv_ActualColumn;
				hv_ActualRow=hv_Row+hv_Row0;
				hv_ActualColumn=hv_Column+hv_Column0;
				hv_RowAll.Append(hv_ActualRow);
				hv_ColumnAll.Append(hv_ActualColumn);
				hv_AngleAll.Append(hv_Angle);
				hv_ScoreAll.Append(hv_Score);
				ptResult->hv_AllScore.Append(hv_Score);
			}
			
		}


		HTuple hv_Indices, hv_Index(0);
		TupleSortIndex(hv_ScoreAll, &hv_Indices);
		hv_Index = hv_Indices[hv_Indices.Length() - 1];//匹配率最高的
		if (0 == hv_ScoreAll[hv_Index])//如果匹配率最高的是0，
		{
			ptResult->hv_Angle = 0;
			ptResult->hv_Column = 0;
			ptResult->hv_Row = 0;
			ptResult->hv_Score = 0;
			ptResult->hv_AllScore= 0;
			return TRUE;
		}
		else
		{
			HObject hoSearch = m_tMarkList[0].ho_ModleInfo.SelectObj(3);
			HObject hoModel = m_tMarkList[0].ho_ModleInfo.SelectObj(2);
			HObject hoLine = m_tMarkList[0].ho_Line;
			HTuple hv_HomMat2D;
			HTuple hv_ModelCenterRow, hv_ModelCenterColumn;
			AreaCenter(hoModel, NULL, &hv_ModelCenterRow, &hv_ModelCenterColumn);

			////进行Row排序
		   TupleSortIndex(hv_RowAll, &hv_Indices);
		   TupleSelect(hv_RowAll, hv_Indices, &hv_RowAll);
			HTuple hv_TempRow, hv_TempIndex;
			hv_TempRow.Clear();
			hv_TempIndex.Clear();
			for (int k = 0; k < hv_RowAll.Length(); k++)
			{
				if (0 == k)
				{
					hv_TempRow.Append(hv_RowAll[k]);
					hv_TempIndex.Append(hv_Indices[k]);
				}
				else
				{
					if (hv_RowAll[k] - hv_TempRow[hv_TempRow.Length() - 1] >= 700)//0515
					{
						hv_TempRow.Append(hv_RowAll[k]);
						hv_TempIndex.Append(hv_Indices[k]);
					}
				}
			}
			hv_RowAll = hv_TempRow;
			TupleSelect(hv_AngleAll, hv_TempIndex, &hv_AngleAll);
			TupleSelect(hv_ColumnAll, hv_TempIndex, &hv_ColumnAll);
			TupleSelect(hv_ScoreAll, hv_TempIndex, &hv_ScoreAll);
			ptResult->hv_Row.Clear();
			ptResult->hv_Column.Clear();
			ptResult->hv_Angle.Clear();
			ptResult->hv_Score.Clear();
			ptResult->hv_AllScore.Clear();

		    for (int i=0;i<hv_RowAll.Length();i++)
		    {
				VectorAngleToRigid(hv_ModelCenterRow, hv_ModelCenterColumn, 0, hv_RowAll[i], hv_ColumnAll[i], hv_AngleAll[i], &hv_HomMat2D);
				HObject ho_RegionAffineTrans, ho_RegionAffineTrans1;
				AffineTransRegion(hoLine, &ho_RegionAffineTrans, hv_HomMat2D, "constant");
				AffineTransRegion(hoModel, &ho_RegionAffineTrans1, hv_HomMat2D, "constant");
				HTuple hv_Row, hv_Column;
				AreaCenter(ho_RegionAffineTrans, NULL, &hv_Row, &hv_Column);

			
	        	ptResult->hv_Row.Append(hv_RowAll[i]);
				ptResult->hv_Column.Append(hv_ColumnAll[i]);
				ptResult->hv_Angle.Append(hv_AngleAll[i]);
				ptResult->hv_Score.Append(hv_ScoreAll[i]);
				ptResult->hv_AllScore.Append(hv_ScoreAll[i]);

				HObject ho_Union2;
				Union2(ho_RegionAffineTrans1, ho_RegionAffineTrans, &ho_Union2);
				ConcatObj(ptResult->ho_DispObj, ho_Union2, &ptResult->ho_DispObj);
		    }
			

		}

	}
	catch (HException e)
	{
		return FALSE;
	}
	return TRUE;
}

int CFindMark::ThresholdFind(HObject ho_Image,NS_SCIFindMark::PMARKRESULT ptResult)//采用阈值的方式
{
	ptResult->hv_Angle.Clear();
	ptResult->hv_Column.Clear();
	ptResult->hv_Angle.Clear();
	ptResult->hv_Score.Clear();
	ptResult->hv_AllScore.Clear();
	ptResult->ho_DispObj.GenEmptyObj();

	HTuple hv_Width,hv_Height;
	GetImageSize(ho_Image,&hv_Width,&hv_Height);
	HObject ho_Serach,ho_ImageReduced,ho_Region,ho_ConnectedRegions,ho_SelectedRegions;
	GenRectangle1(&ho_Serach,0,hv_Width*0.5,hv_Height-1,hv_Width-1);
	ReduceDomain(ho_Image,ho_Serach,&ho_ImageReduced);
	Threshold(ho_ImageReduced, &ho_Region, 180,255 );
	Connection(ho_Region,&ho_ConnectedRegions);

    HTuple hv_JierWidth(400),hv_Number;
	SelectShape(ho_ConnectedRegions,&ho_SelectedRegions,HTuple("width").Append("height").Append("area"),"and",HTuple(hv_JierWidth*0.6).Append(hv_JierWidth*0.6).Append(100000),
		HTuple(INT_MAX).Append(INT_MAX).Append(INT_MAX));
	CountObj(ho_SelectedRegions,&hv_Number);
	if (hv_Number > 0)
	{
		HObject ho_RegionClosing,ho_RegionOpening,ho_RegionDifference,ho_ConnectedRegions1,ho_SelectedRegions1,ho_SelectedRegions2,ho_Jier;
		ClosingCircle (ho_SelectedRegions, &ho_RegionClosing, hv_JierWidth/2);
		OpeningRectangle1 (ho_RegionClosing, &ho_RegionOpening, 5, hv_Height/2);
		Difference (ho_RegionClosing, ho_RegionOpening, &ho_RegionDifference);
		Connection (ho_RegionDifference, &ho_ConnectedRegions1);
		SelectShapeStd (ho_ConnectedRegions1, &ho_SelectedRegions1, "max_area", 70);
		SelectShape (ho_SelectedRegions1, &ho_SelectedRegions2, "width", "and", hv_JierWidth/2, 99999); 
		OpeningRectangle1(ho_SelectedRegions2,&ho_Jier,30,30);
		CountObj (ho_Jier, &hv_Number);
		if (hv_Number > 0)
		{
			SmallestRectangle1(ho_Jier,&ptResult->hv_Row,NULL,NULL,NULL);
			ptResult->ho_DispObj = ho_Jier;
			ptResult->hv_Angle = 0;
			ptResult->hv_Column = 0;
			ptResult->hv_Score = 1;
		}else
		{
			ptResult->hv_Angle = 0;
			ptResult->hv_Column = 0;
			ptResult->hv_Row =0;
			ptResult->hv_Score = 0;
		}
	}else
	{
		ptResult->hv_Angle = 0;
		ptResult->hv_Column = 0;
		ptResult->hv_Row =0;
		ptResult->hv_Score = 0;
	}




	return TRUE;
	
}

BOOL CFindMark::SaveJigsawImage(const int& nGrabberSN,const HObject& ho_ImageSave)
{
	try
	{
		CString strObjectFilePATH,strFileName;
		SYSTEMTIME CurSysTime;
		GetLocalTime(&CurSysTime);
		strObjectFilePATH.Format("%s\\Map\\匹配模板\\%d\\%d年%02d月%02d日"
			,GetCurrentAppPath(),nGrabberSN,CurSysTime.wYear,CurSysTime.wMonth,CurSysTime.wDay);
		CreateMultilevelPath(strObjectFilePATH);
		HTuple  hv_ImageFiles;
		ListFiles(HTuple(strObjectFilePATH),(HTuple("files").Append("follow_links")), &hv_ImageFiles);
		TupleRegexpSelect(hv_ImageFiles, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$").Append("ignore_case")),
			&hv_ImageFiles);
		strFileName.Format("%s\\Image%04d", strObjectFilePATH,hv_ImageFiles.Length());
		WriteImage(ho_ImageSave,"bmp",0,HTuple(strFileName));
	}catch(HException e)
	{
		return FALSE;
	}
	return TRUE;

}



void CFindMark::ClearMark()
{
	m_tMarkList.clear();
}
int CFindMark::DoSomething(void* message,void* wParam,void* lParam)//函数入口
{
	CString strMessage;
	strMessage.Format("%s",message);
	if ("INITIALIZE" == strMessage)
	{
		return Init(*(NS_SCIFindMark::PINITPARAM)wParam);
	}else if ("SHOW" == strMessage)
	{
		return ShowDialog(*(HObject*)wParam);
	}else if("FIND" == strMessage)
	{
		CriticalSectionHelper lock(&m_csFindChange);
		return FindMult(*(HObject*)wParam, (NS_SCIFindMark::PMARKRESULT)lParam);
		
	}else if ("COPYOBJECTS" == strMessage)
	{
		CriticalSectionHelper lock(&m_csFindChange);
		*this = *(CFindMark*)wParam;

		return TRUE;

	}
	return 0;
}


void CFindMark::WriteMark()
{
	CriticalSectionHelper lock (&m_csFindChange);
	DeleteFolder(m_strConfigPath);
	CreateMultilevelPath(m_strConfigPath);
	int nMarkCount = (int)m_tMarkList.size();

	for (int i=0;i<nMarkCount;i++)
	{
		try
		{
			SINGLEMARKINFO& tSingleMark = m_tMarkList[i];
			CString strModleParam,strModel,strModelInfo,strLine;
			strModleParam.Format("%s\\%d.tup",m_strConfigPath,i);
			strModelInfo.Format("%s\\%d.hobj",m_strConfigPath,i);
			strLine.Format("%s\\%d.line",m_strConfigPath,i);
			WriteTuple(tSingleMark.hv_ModelParam,HTuple(strModleParam));
			WriteObject(tSingleMark.ho_ModleInfo,HTuple(strModelInfo));
			WriteObject(tSingleMark.ho_Line,HTuple(strLine));
			if ("形状匹配" == tSingleMark.hv_ModelParam[1])
			{
				strModel.Format("%s\\%d.shm",m_strConfigPath,i);
				WriteShapeModel(tSingleMark.hv_ModelID,HTuple(strModel));
			}else
			{
				strModel.Format("%s\\%d.ncm",m_strConfigPath,i);
				WriteNccModel(tSingleMark.hv_ModelID,HTuple(strModel));
			}
		}catch(HException e)
		{
			CString strMessageBox;
			strMessageBox.Format("写入WriteMark%d出错:%s",i,e.ErrorMessage().Text());
			AfxMessageBox(strMessageBox);
		}	
	}
	WritePrivateProfileInt("Mark","Count",nMarkCount,m_strConfigFile);
	WritePrivateProfileInt("Mark","Type",m_nType,m_strConfigFile);


	//for (int i=0;i<LINE_COUNT;i++)
	//{
	//	CString strFileName;
	//	strFileName.Format("%s\\%d.ini",m_strConfigPath,i);
	//	m_sLineTool[i].ReadParam(strFileName);
	//}
}

int CFindMark::FitLine (const HObject& Image,const CLineToolPara& FitLinePara, HObject* Line, 
	double* dStartY,double* dStartX,double* dEndY,double* dEndX,int nLineDisplay)
{
	try
	{
#pragma region "先对拟合方式进行判断，如果不是正常拟合直接就返回了"

		if (0!=FitLinePara.m_nChoice)
		{
			HObject Domain;
			HTuple Row1,Column1,Row2,Column2;
			GetDomain(Image,&Domain);
			SmallestRectangle1(Domain,&Row1,&Column1,&Row2,&Column2);
			if (1 == FitLinePara.m_nChoice)//固定值
			{
				*dStartX = FitLinePara.m_dStartCol;
				*dStartY = FitLinePara.m_dStartRow;
				*dEndX   = FitLinePara.m_dEndCol;
				*dEndY   = FitLinePara.m_dEndRow;
				GenRegionLine(Line,*dStartY,*dStartX,*dEndY,*dEndX);
			}else if (2 == FitLinePara.m_nChoice)//固定顶端
			{
				*dStartX = FitLinePara.m_dStartCol;
				*dStartY = Row1.I();
				*dEndX   = FitLinePara.m_dEndCol;
				*dEndY   = Row1.I();
				GenRegionLine(Line,*dStartY,*dStartX,*dEndY,*dEndX);
			}else if (3 == FitLinePara.m_nChoice)//固定底端
			{
				*dStartX = FitLinePara.m_dStartCol;
				*dStartY = Row2.I();
				*dEndX   = FitLinePara.m_dEndCol;
				*dEndY   = Row2.I();
				GenRegionLine(Line,*dStartY,*dStartX,*dEndY,*dEndX);
			}else if (4 == FitLinePara.m_nChoice)//固定左侧
			{
				*dStartX = Column1.I();
				*dStartY = FitLinePara.m_dStartRow;
				*dEndX   = Column1.I();
				*dEndY   = FitLinePara.m_dEndRow;
				GenRegionLine(Line,*dStartY,*dStartX,*dEndY,*dEndX);
			}else if (5 == FitLinePara.m_nChoice)//固定右侧
			{
				*dStartX = Column2.I();
				*dStartY = FitLinePara.m_dStartRow;
				*dEndX   = Column2.I();
				*dEndY   = FitLinePara.m_dEndRow;
				GenRegionLine(Line,*dStartY,*dStartX,*dEndY,*dEndX);
			}
			return TRUE;
		}

#pragma endregion 

		// Local iconic variables 
		HObject  Rectangle, Contour;
		// Local control variables 
		HTuple  Phi, ResultRow, ResultCol, i, RowC, ColC;
		HTuple  MsrHandle_Measure, transition;
		HTuple  select, RowEdge, ColEdge, Amplitude, Distance, tRow;
		HTuple  tCol, t, RowBegin, ColBegin, RowEnd, ColEnd, Nr;
		HTuple  Nc, Dist;
		HTuple Width, Height;
		GetImageSize(Image,&Width,&Height);
		Line->GenEmptyObj();
		Phi = ((HTuple(FitLinePara.m_dStartRow)-HTuple(FitLinePara.m_dEndRow)).TupleAtan2(HTuple(FitLinePara.m_dEndCol)-HTuple(FitLinePara.m_dStartCol)))\
			+(HTuple(90).TupleRad());
		HObject hMeasureRect,hCross;
		ResultRow = HTuple();
		ResultCol = HTuple();
		if (0 != (HTuple(FitLinePara.m_nDirect)==0))
		{
			transition = "all";
		}
		else if (0 != (HTuple(FitLinePara.m_nDirect)==1))
		{
			transition = "positive";
		}
		else
		{
			transition = "negative";
		}
		if (0 != (HTuple(FitLinePara.m_nSelect)==0))
		{
			select = "all";
		}
		else if (0 != (HTuple(FitLinePara.m_nSelect)==1))
		{
			select = "first";
		}
		else
		{
			select = "last";
		}

		for (i=0; i<HTuple(FitLinePara.m_nMeasureNum); i+=1)
		{
			RowC = HTuple(FitLinePara.m_dStartRow)+(((HTuple(FitLinePara.m_dEndRow)-HTuple(FitLinePara.m_dStartRow))*i)/(HTuple(FitLinePara.m_nMeasureNum)-1));
			ColC = HTuple(FitLinePara.m_dStartCol)+(((HTuple(FitLinePara.m_dEndCol)-HTuple(FitLinePara.m_dStartCol))*i)/(HTuple(FitLinePara.m_nMeasureNum)-1));
			//Convert coordinates to rectangle2 type
			GenRectangle2(&hMeasureRect,RowC, ColC, Phi, HTuple(FitLinePara.m_dHeight), HTuple(FitLinePara.m_dWidth));
			GenRectangle2(&Rectangle,RowC, ColC, Phi, HTuple(FitLinePara.m_dHeight), 1);
			if (0 == nLineDisplay)
			{
				ConcatObj(*Line,Rectangle,Line);
			}else
			{
				ConcatObj(*Line,hMeasureRect,Line);
			}

			GenMeasureRectangle2(RowC, ColC, Phi, HTuple(FitLinePara.m_dHeight), HTuple(FitLinePara.m_dWidth), 
				Width, Height, "nearest_neighbor", &MsrHandle_Measure);
			try
			{
				MeasurePos(Image, MsrHandle_Measure, HTuple(FitLinePara.m_dSigma), HTuple(FitLinePara.m_nThr), 
					transition, select, &RowEdge, &ColEdge, &Amplitude, &Distance);
				CloseMeasure(MsrHandle_Measure);
			}
			catch(HException& e)
			{
				CString str;
				str.Format("%s",e.ErrorMessage().Text());
			}


			tRow = 0;
			tCol = 0;
			t = 0;
			for (int j=0; j<=(RowEdge.Length())-1; j+=1)
			{
				if (0 != (t<HTuple(Amplitude[j]).TupleAbs()))
				{
					tRow = RowEdge[j];
					tCol = ColEdge[j];
					t = HTuple(Amplitude[j]).TupleAbs();
				}
			}
			if (0 != (t>0))
			{
				ResultRow.Append(tRow);
				ResultCol.Append(tCol);
				GenCrossContourXld(&hCross,tRow[0].D(),tCol[0].D(),18,0.78);
				ConcatObj(*Line,hCross,Line);
			}
		}
		//stop ()
		HObject hLine;
		if (ResultRow.Length()>FitLinePara.m_nMeasureNum/2)
		{
			GenContourPolygonXld(&Contour, ResultRow, ResultCol);//tukey
			FitLineContourXld(Contour, "tukey", -1, 0, 5, 2, &RowBegin, 
				&ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);
			GenContourPolygonXld(&hLine, RowBegin.TupleConcat(RowEnd), ColBegin.TupleConcat(ColEnd));
			ConcatObj(*Line,hLine,Line);
			*dStartY = RowBegin[0].D();
			*dStartX = ColBegin[0].D();
			*dEndY = RowEnd[0].D();
			*dEndX = ColEnd[0].D();
		}
		else//如果没找到线，就是用原来的位置
		{
			GenContourPolygonXld(&hLine, HTuple(FitLinePara.m_dStartRow).TupleConcat(FitLinePara.m_dEndRow), 
				HTuple(FitLinePara.m_dStartCol).TupleConcat(FitLinePara.m_dEndCol));
			ConcatObj(*Line,hLine,Line);
			*dStartY = FitLinePara.m_dStartRow;
			*dStartX = FitLinePara.m_dStartCol;
			*dEndY = FitLinePara.m_dEndRow;
			*dEndX = FitLinePara.m_dEndCol;
			return TRUE;
		}
		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}


}

void CFindMark::ReadMark()
{
	m_tMarkList.clear();
	int nMarkCount = GetPrivateProfileInt("Mark","Count",0,m_strConfigFile);
	for (int i=0;i<nMarkCount;i++)
	{
		SINGLEMARKINFO tSingleMark;
		CString strModleParam,strModel,strModelInfo,strLine;
		strModleParam.Format("%s\\%d.tup",m_strConfigPath,i);
		strModelInfo.Format("%s\\%d.hobj",m_strConfigPath,i);
		strLine.Format("%s\\%d.line",m_strConfigPath,i);
		ReadTuple(HTuple(strModleParam),&tSingleMark.hv_ModelParam);
		ReadObject(&tSingleMark.ho_ModleInfo,HTuple(strModelInfo));
		ReadObject(&tSingleMark.ho_Line,HTuple(strLine));
		if ("形状匹配" == tSingleMark.hv_ModelParam[1])
		{
			strModel.Format("%s\\%d.shm",m_strConfigPath,i);
			ReadShapeModel(HTuple(strModel),&tSingleMark.hv_ModelID);
		}else
		{
			strModel.Format("%s\\%d.ncm",m_strConfigPath,i);
			ReadNccModel(HTuple(strModel),&tSingleMark.hv_ModelID);
		}

		m_tMarkList.push_back(tSingleMark);
	}

	/*for (int i=0;i<LINE_COUNT;i++)
	{
		CString strFileName;
		strFileName.Format("%s\\%d.ini",m_strConfigPath,i);
		m_sLineTool[i].ReadParam(strFileName);
	}*/
}