#include "stdafx.h"
#include "MyHalconEx.h"
#include <direct.h>

// Short Description: 黑点检测 
void BlackSpot (HObject ho_Image, HObject ho_Region, HObject *ho_ErrorRegion, HTuple hv_Param)
{

	// Local iconic variables
	HObject  ho_ImageReduced, ho_Region1, ho_ConnectedRegions;
	HObject  ho_SelectedRegions;

	// Local control variables
	HTuple  hv_dReslution, hv_nGray, hv_dWidth, hv_dHeight;
	HTuple  hv_dArea;

	//Param 0是否检测，1灰度值，2宽度，3高度，4面积, 5分辨率
	if (0 != (HTuple(hv_Param[0])==0))
	{
		return;
	}
	hv_dReslution = ((const HTuple&)hv_Param)[5];
	hv_nGray = ((const HTuple&)hv_Param)[1];
	hv_dWidth = HTuple(hv_Param[2])/hv_dReslution;
	hv_dHeight = HTuple(hv_Param[3])/hv_dReslution;
	hv_dArea = (HTuple(hv_Param[4])/hv_dReslution)/hv_dReslution;

	GenEmptyObj(&(*ho_ErrorRegion));

	ReduceDomain(ho_Image, ho_Region, &ho_ImageReduced);
	FastThreshold(ho_ImageReduced, &ho_Region1, 0, hv_nGray, 5);
	Connection(ho_Region1, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, ((HTuple("width").Append("height")).Append("area")), 
		"and", (hv_dWidth.TupleConcat(hv_dHeight)).TupleConcat(hv_dArea), ((HTuple(99999999).Append(99999999)).Append(99999999)));
	ConcatObj((*ho_ErrorRegion), ho_SelectedRegions, &(*ho_ErrorRegion));

	return;
}

// Short Description: 涂布漏箔检测 
void FoilLeak (HObject ho_Image, HObject ho_Region, HObject *ho_ErrorRegion, HTuple hv_Param)
{

	// Local iconic variables
	HObject  ho_ImageReduced, ho_Region1, ho_ConnectedRegions;
	HObject  ho_SelectedRegions;

	// Local control variables
	HTuple  hv_dReslution, hv_nGray, hv_dWidth, hv_dHeight;
	HTuple  hv_dArea;

	//Param 0是否检测，1灰度值，2宽度，3高度，4面积, 5分辨率
	if (0 != (HTuple(hv_Param[0])==0))
	{
		return;
	}
	hv_dReslution = ((const HTuple&)hv_Param)[5];
	hv_nGray = ((const HTuple&)hv_Param)[1];
	hv_dWidth = HTuple(hv_Param[2])/hv_dReslution;
	hv_dHeight = HTuple(hv_Param[3])/hv_dReslution;
	hv_dArea = (HTuple(hv_Param[4])/hv_dReslution)/hv_dReslution;

	GenEmptyObj(&(*ho_ErrorRegion));

	ReduceDomain(ho_Image, ho_Region, &ho_ImageReduced);
	FastThreshold(ho_ImageReduced, &ho_Region1, hv_nGray, 255, 5);
	Connection(ho_Region1, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, ((HTuple("width").Append("height")).Append("area")), 
		"and", (hv_dWidth.TupleConcat(hv_dHeight)).TupleConcat(hv_dArea), ((HTuple(99999999).Append(99999999)).Append(99999999)));
	ConcatObj((*ho_ErrorRegion), ho_SelectedRegions, &(*ho_ErrorRegion));

	return;
}

// Short Description: 白点检测 
void WhiteSpot (HObject ho_Image, HObject ho_Region, HObject *ho_ErrorRegion, HTuple hv_Param)
{

	// Local iconic variables
	HObject  ho_ImageReduced, ho_Region1, ho_ConnectedRegions;
	HObject  ho_SelectedRegions;

	// Local control variables
	HTuple  hv_dReslution, hv_nGray, hv_dWidth, hv_dHeight;
	HTuple  hv_dArea;

	//Param 0是否检测，1灰度值，2宽度，3高度，4面积, 5分辨率
	if (0 != (HTuple(hv_Param[0])==0))
	{
		return;
	}

	hv_dReslution = ((const HTuple&)hv_Param)[5];
	hv_nGray = ((const HTuple&)hv_Param)[1];
	hv_dWidth = HTuple(hv_Param[2])/hv_dReslution;
	hv_dHeight = HTuple(hv_Param[3])/hv_dReslution;
	hv_dArea = (HTuple(hv_Param[4])/hv_dReslution)/hv_dReslution;

	GenEmptyObj(&(*ho_ErrorRegion));
	ReduceDomain(ho_Image, ho_Region, &ho_ImageReduced);
	FastThreshold(ho_ImageReduced, &ho_Region1, hv_nGray, 255, 5);
	Connection(ho_Region1, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, ((HTuple("width").Append("height")).Append("area")), 
		"and", (hv_dWidth.TupleConcat(hv_dHeight)).TupleConcat(hv_dArea), ((HTuple(99999999).Append(99999999)).Append(99999999)));
	ConcatObj((*ho_ErrorRegion), ho_SelectedRegions, &(*ho_ErrorRegion));

	return;
}

void plot_tuple(HTuple hv_WindowHandle, HTuple hv_XValues, HTuple hv_YValues, HTuple hv_XLabel,
	HTuple hv_YLabel, HTuple hv_Color, HTuple hv_GenParamName, HTuple hv_GenParamValue)
{

	// Local iconic variables
	HObject  ho_ContourXGrid, ho_ContourYGrid, ho_XArrow;
	HObject  ho_YArrow, ho_ContourXTick, ho_ContourYTick, ho_Contour;
	HObject  ho_Cross, ho_Filled, ho_Stair, ho_StairTmp;

	// Local control variables
	HTuple  hv_PreviousWindowHandle, hv_ClipRegion;
	HTuple  hv_Row, hv_Column, hv_Width, hv_Height, hv_PartRow1;
	HTuple  hv_PartColumn1, hv_PartRow2, hv_PartColumn2, hv_Red;
	HTuple  hv_Green, hv_Blue, hv_DrawMode, hv_OriginStyle;
	HTuple  hv_XAxisEndValue, hv_YAxisEndValue, hv_XAxisStartValue;
	HTuple  hv_YAxisStartValue, hv_XValuesAreStrings, hv_XTickValues;
	HTuple  hv_XTicks, hv_YAxisPosition, hv_XAxisPosition, hv_LeftBorder;
	HTuple  hv_RightBorder, hv_UpperBorder, hv_LowerBorder;
	HTuple  hv_AxesColor, hv_Style, hv_Clip, hv_YTicks, hv_XGrid;
	HTuple  hv_YGrid, hv_GridColor, hv_YPosition, hv_FormatX;
	HTuple  hv_FormatY, hv_NumGenParamNames, hv_NumGenParamValues;
	HTuple  hv_GenParamIndex, hv_XGridTicks, hv_YTickDirection;
	HTuple  hv_XTickDirection, hv_XAxisWidthPx, hv_XAxisWidth;
	HTuple  hv_XScaleFactor, hv_YAxisHeightPx, hv_YAxisHeight;
	HTuple  hv_YScaleFactor, hv_YAxisOffsetPx, hv_XAxisOffsetPx;
	HTuple  hv_DotStyle, hv_XGridValues, hv_XGridStart, hv_XCoord;
	HTuple  hv_IndexGrid, hv_YGridValues, hv_YGridStart, hv_YCoord;
	HTuple  hv_Ascent, hv_Descent, hv_TextWidthXLabel, hv_TextHeightXLabel;
	HTuple  hv_TextWidthYLabel, hv_TextHeightYLabel, hv_XTickStart;
	HTuple  hv_Indices, hv_TypeTicks, hv_IndexTicks, hv_Ascent1;
	HTuple  hv_Descent1, hv_TextWidthXTicks, hv_TextHeightXTicks;
	HTuple  hv_YTickValues, hv_YTickStart, hv_TextWidthYTicks;
	HTuple  hv_TextHeightYTicks, hv_Num, hv_I, hv_YSelected;
	HTuple  hv_Y1Selected, hv_X1Selected, hv_Index, hv_Row1;
	HTuple  hv_Row2, hv_Col1, hv_Col2;

	//This procedure plots tuples representing functions
	//or curves in a coordinate system.
	//
	//Input parameters:
	//
	//XValues: X values of the function to be plotted
	//         If XValues is set to [], it is internally set to 0,1,2,...,|YValues|-1.
	//         If XValues is a tuple of strings, the values are taken as categories.
	//
	//YValues: Y values of the function(s) to be plotted
	//         If YValues is set to [], it is internally set to 0,1,2,...,|XValues|-1.
	//         The number of y values must be equal to the number of x values
	//         or an integral multiple. In the latter case,
	//         multiple functions are plotted, that share the same x values.
	//
	//XLabel: X-axis label
	//
	//XLabel: Y-axis label
	//
	//Color: Color of the plotted function
	//       If [] is given, the currently set display color is used.
	//       If 'none is given, the function is not plotted, but only
	//       the coordinate axes as specified.
	//       If more than one color is given, multiple functions
	//       can be displayed in different colors.
	//
	//GenParamName:  Generic parameters to control the presentation
	//               Possible Values:
	//   'axes_color': coordinate system color
	//                 Default: 'white'
	//                 If 'none' is given, no coordinate system is shown.
	//   'style': Graph style
	//            Possible values: 'line' (default), 'cross', 'step', 'filled'
	//   'clip': Clip graph to coordinate system area
	//           Possible values: 'yes', 'no' (default)
	//   'ticks': Control display of ticks on the axes
	//            If 'min_max_origin' is given (default), ticks are shown
	//            at the minimum and maximum values of the axes and at the
	//            intercept point of x- and y-axis.
	//            If 'none' is given, no ticks are shown.
	//            If any number != 0 is given, it is interpreted as distance
	//            between the ticks.
	//   'ticks_x': Control display of ticks on x-axis only
	//   'ticks_y': Control display of ticks on y-axis only
	//   'format_x': Format of the values next to the ticks of the x-axis
	//               (see tuple_string for more details).
	//   'format_y': Format of the values next to the ticks of the y-axis
	//               (see tuple_string for more details).
	//   'grid': Control display of grid lines within the coordinate system
	//           If 'min_max_origin' is given (default), grid lines are shown
	//           at the minimum and maximum values of the axes.
	//           If 'none' is given, no grid lines are shown.
	//           If any number != 0 is given, it is interpreted as distance
	//           between the grid lines.
	//   'grid_x': Control display of grid lines for the x-axis only
	//   'grid_y': Control display of grid lines for the y-axis only
	//   'grid_color': Color of the grid (default: 'dim gray')
	//   'margin': The distance in pixels of the coordinate system area
	//             to all four window borders.
	//   'margin_left': The distance in pixels of the coordinate system area
	//                  to the left window border.
	//   'margin_right': The distance in pixels of the coordinate system area
	//                   to the right window border.
	//   'margin_top': The distance in pixels of the coordinate system area
	//                 to the upper window border.
	//   'margin_bottom': The distance in pixels of the coordinate system area
	//                    to the lower window border.
	//   'start_x': Lowest x value of the x-axis
	//              Default: min(XValues)
	//   'end_x': Highest x value of the x-axis
	//            Default: max(XValues)
	//   'start_y': Lowest y value of the y-axis
	//              Default: min(YValues)
	//   'end_y': Highest y value of the y-axis
	//            Default: max(YValues)
	//   'axis_location_x': Either 'bottom', 'origin', or 'top'
	//               to position the x-axis conveniently,
	//               or the Y coordinate of the intercept point of x- and y-axis.
	//               Default: 'bottom'
	//               (Used to be called 'origin_y')
	//   'axis_location_y': Either 'left', 'origin', or 'right'
	//               to position the y-axis conveniently,
	//               or the X coordinate of the intercept point of x- and y-axis.
	//               Default: 'left'
	//               (Used to be called 'origin_x')
	//
	//GenParamValue: Values of the generic parameters of GenericParamName
	//
	//
	//Store current display settings
	//if (HDevWindowStack::IsOpen())
	//hv_PreviousWindowHandle = HDevWindowStack::GetActive();
	//HDevWindowStack::SetActive(hv_WindowHandle);
	GetSystem("clip_region", &hv_ClipRegion);
	GetWindowExtents(hv_WindowHandle, &hv_Row, &hv_Column, &hv_Width, &hv_Height);
	GetPart(hv_WindowHandle, &hv_PartRow1, &hv_PartColumn1, &hv_PartRow2, &hv_PartColumn2);
	GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
	GetDraw(hv_WindowHandle, &hv_DrawMode);
	GetLineStyle(hv_WindowHandle, &hv_OriginStyle);
	//
	//Set display parameters
	SetLineStyle(hv_WindowHandle, HTuple());
	SetSystem("clip_region", "false");
	//if (HDevWindowStack::IsOpen())
	SetPart(hv_WindowHandle, 0, 0, hv_Height - 1, hv_Width - 1);
	//
	//Check input coordinates
	//
	if (0 != (HTuple(int(hv_XValues == HTuple())).TupleAnd(int(hv_YValues == HTuple()))))
	{
		//Neither XValues nor YValues are given:
		//Set axes to interval [0,1]
		hv_XAxisEndValue = 1;
		hv_YAxisEndValue = 1;
		hv_XAxisStartValue = 0;
		hv_YAxisStartValue = 0;
		hv_XValuesAreStrings = 0;
	}
	else
	{
		if (0 != (int(hv_XValues == HTuple())))
		{
			//XValues are omitted:
			//Set equidistant XValues
			hv_XValues = HTuple::TupleGenSequence(0, (hv_YValues.TupleLength()) - 1, 1);
			hv_XValuesAreStrings = 0;
		}
		else if (0 != (int(hv_YValues == HTuple())))
		{
			//YValues are omitted:
			//Set equidistant YValues
			hv_YValues = HTuple::TupleGenSequence(0, (hv_XValues.TupleLength()) - 1, 1);
		}
		if (0 != (int(((hv_YValues.TupleLength()) % (hv_XValues.TupleLength())) != 0)))
		{
			//Number of YValues does not match number of XValues
			throw HException("Number of YValues is no multiple of the number of XValues!");
			return;
		}
		hv_XValuesAreStrings = hv_XValues.TupleIsStringElem();
		hv_XValuesAreStrings = int((hv_XValuesAreStrings.TupleSum()) == (hv_XValuesAreStrings.TupleLength()));
		if (0 != hv_XValuesAreStrings)
		{
			//XValues are given as strings:
			//Show XValues as ticks
			hv_XTickValues = hv_XValues;
			hv_XTicks = 1;
			//Set x-axis dimensions
			hv_XValues = HTuple::TupleGenSequence(1, hv_XValues.TupleLength(), 1);
		}
		//Set default x-axis dimensions
		if (0 != (int((hv_XValues.TupleLength()) > 1)))
		{
			hv_XAxisStartValue = hv_XValues.TupleMin();
			hv_XAxisEndValue = hv_XValues.TupleMax();
		}
		else
		{
			hv_XAxisEndValue = HTuple(hv_XValues[0]) + 0.5;
			hv_XAxisStartValue = HTuple(hv_XValues[0]) - 0.5;
		}
	}
	//Set default y-axis dimensions
	if (0 != (int((hv_YValues.TupleLength()) > 1)))
	{
		hv_YAxisStartValue = hv_YValues.TupleMin();
		hv_YAxisEndValue = hv_YValues.TupleMax();
	}
	else if (0 != (int((hv_YValues.TupleLength()) == 1)))
	{
		hv_YAxisStartValue = HTuple(hv_YValues[0]) - 0.5;
		hv_YAxisEndValue = HTuple(hv_YValues[0]) + 0.5;
	}
	else
	{
		hv_YAxisStartValue = 0;
		hv_YAxisEndValue = 1;
	}
	//Set default interception point of x- and y- axis
	hv_YAxisPosition = "default";
	hv_XAxisPosition = "default";
	//
	//Set more defaults
	hv_LeftBorder = hv_Width*0.1;
	hv_RightBorder = hv_Width*0.1;
	hv_UpperBorder = hv_Height*0.1;
	hv_LowerBorder = hv_Height*0.1;
	hv_AxesColor = "white";
	hv_Style = "line";
	hv_Clip = "no";
	hv_XTicks = "min_max_origin";
	hv_YTicks = "min_max_origin";
	hv_XGrid = "none";
	hv_YGrid = "none";
	hv_GridColor = "dim gray";
	hv_YPosition = "left";
	hv_FormatX = "default";
	hv_FormatY = "default";
	//
	//Parse generic parameters
	//
	hv_NumGenParamNames = hv_GenParamName.TupleLength();
	hv_NumGenParamValues = hv_GenParamValue.TupleLength();
	if (0 != (int(hv_NumGenParamNames != hv_NumGenParamValues)))
	{
		throw HException("Number of generic parameter names does not match generic parameter values!");
		return;
	}
	//
	{
		HTuple end_val190 = (hv_GenParamName.TupleLength()) - 1;
		HTuple step_val190 = 1;
		for (hv_GenParamIndex = 0; hv_GenParamIndex.Continue(end_val190, step_val190); hv_GenParamIndex += step_val190)
		{
			//
			//Set 'axes_color'
			if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("axes_color"))))
			{
				hv_AxesColor = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'style'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("style"))))
			{
				hv_Style = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'clip'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("clip"))))
			{
				hv_Clip = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				if (0 != (HTuple(int(hv_Clip != HTuple("yes"))).TupleAnd(int(hv_Clip != HTuple("no")))))
				{
					throw HException(("Unsupported clipping option: '" + hv_Clip) + "'");
				}
				//
				//Set 'ticks'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("ticks"))))
			{
				hv_XTicks = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				hv_YTicks = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'ticks_x'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("ticks_x"))))
			{
				hv_XTicks = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'ticks_y'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("ticks_y"))))
			{
				hv_YTicks = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'grid'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("grid"))))
			{
				hv_XGrid = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				hv_YGrid = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				hv_XGridTicks = hv_XTicks;
				//
				//Set 'grid_x'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("grid_x"))))
			{
				hv_XGrid = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'grid_y'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("grid_y"))))
			{
				hv_YGrid = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'grid_color'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("grid_color"))))
			{
				hv_GridColor = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'start_x'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("start_x"))))
			{
				hv_XAxisStartValue = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'end_x'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("end_x"))))
			{
				hv_XAxisEndValue = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'start_y'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("start_y"))))
			{
				hv_YAxisStartValue = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'end_y'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("end_y"))))
			{
				hv_YAxisEndValue = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'axis_location_y' (old name 'origin_x')
			}
			else if (0 != (HTuple(int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("axis_location_y"))).TupleOr(int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("origin_x")))))
			{
				hv_YAxisPosition = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'axis_location_x' (old name: 'origin_y')
			}
			else if (0 != (HTuple(int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("axis_location_x"))).TupleOr(int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("origin_y")))))
			{
				hv_XAxisPosition = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'margin'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("margin"))))
			{
				hv_LeftBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				hv_RightBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				hv_UpperBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				hv_LowerBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'margin_left'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("margin_left"))))
			{
				hv_LeftBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'margin_right'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("margin_right"))))
			{
				hv_RightBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'margin_top'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("margin_top"))))
			{
				hv_UpperBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
				//
				//Set 'margin_bottom'
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("margin_bottom"))))
			{
				hv_LowerBorder = HTuple(hv_GenParamValue[hv_GenParamIndex]);
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("format_x"))))
			{
				hv_FormatX = HTuple(hv_GenParamValue[hv_GenParamIndex]);
			}
			else if (0 != (int(HTuple(hv_GenParamName[hv_GenParamIndex]) == HTuple("format_y"))))
			{
				hv_FormatY = HTuple(hv_GenParamValue[hv_GenParamIndex]);
			}
			else
			{
				throw HException(("Unknown generic parameter: '" + HTuple(hv_GenParamName[hv_GenParamIndex])) + "'");
			}
		}
	}
	//
	//Check consistency of start and end values
	//of the axes.
	if (0 != (int(hv_XAxisStartValue > hv_XAxisEndValue)))
	{
		throw HException("Value for 'start_x' is greater than value for 'end_x'");
	}
	if (0 != (int(hv_YAxisStartValue > hv_YAxisEndValue)))
	{
		throw HException("Value for 'start_y' is greater than value for 'end_y'");
	}
	//
	//Set the position of the y-axis.
	if (0 != (int(hv_YAxisPosition == HTuple("default"))))
	{
		hv_YAxisPosition = hv_XAxisStartValue;
	}
	if (0 != (int((hv_YAxisPosition.TupleIsString()) == 1)))
	{
		if (0 != (int(hv_YAxisPosition == HTuple("left"))))
		{
			hv_YAxisPosition = hv_XAxisStartValue;
		}
		else if (0 != (int(hv_YAxisPosition == HTuple("right"))))
		{
			hv_YAxisPosition = hv_XAxisEndValue;
		}
		else if (0 != (int(hv_YAxisPosition == HTuple("origin"))))
		{
			hv_YAxisPosition = 0;
		}
		else
		{
			throw HException(("Unsupported axis_location_y: '" + hv_YAxisPosition) + "'");
		}
	}
	//Set the position of the ticks on the y-axis
	//depending of the location of the y-axis.
	if (0 != (int(((hv_XAxisStartValue.TupleConcat(hv_XAxisEndValue)).TupleMean()) > hv_YAxisPosition)))
	{
		hv_YTickDirection = "right";
	}
	else
	{
		hv_YTickDirection = "left";
	}
	//
	//Set the position of the x-axis.
	if (0 != (int(hv_XAxisPosition == HTuple("default"))))
	{
		hv_XAxisPosition = hv_YAxisStartValue;
	}
	if (0 != (int((hv_XAxisPosition.TupleIsString()) == 1)))
	{
		if (0 != (int(hv_XAxisPosition == HTuple("bottom"))))
		{
			hv_XAxisPosition = hv_YAxisStartValue;
		}
		else if (0 != (int(hv_XAxisPosition == HTuple("top"))))
		{
			hv_XAxisPosition = hv_YAxisEndValue;
		}
		else if (0 != (int(hv_XAxisPosition == HTuple("origin"))))
		{
			hv_XAxisPosition = 0;
		}
		else
		{
			throw HException(("Unsupported axis_location_x: '" + hv_XAxisPosition) + "'");
		}
	}
	//Set the position of the ticks on the y-axis
	//depending of the location of the y-axis.
	if (0 != (int(((hv_YAxisStartValue.TupleConcat(hv_YAxisEndValue)).TupleMean()) > hv_XAxisPosition)))
	{
		hv_XTickDirection = "up";
	}
	else
	{
		hv_XTickDirection = "down";
	}
	//
	//Calculate basic pixel coordinates and scale factors
	//
	hv_XAxisWidthPx = (hv_Width - hv_LeftBorder) - hv_RightBorder;
	hv_XAxisWidth = hv_XAxisEndValue - hv_XAxisStartValue;
	if (0 != (int(hv_XAxisWidth == 0)))
	{
		hv_XAxisStartValue = hv_XAxisStartValue - 0.5;
		hv_XAxisEndValue += 0.5;
		hv_XAxisWidth = 1;
	}
	hv_XScaleFactor = hv_XAxisWidthPx / (hv_XAxisWidth.TupleReal());
	hv_YAxisHeightPx = (hv_Height - hv_LowerBorder) - hv_UpperBorder;
	hv_YAxisHeight = hv_YAxisEndValue - hv_YAxisStartValue;
	if (0 != (int(hv_YAxisHeight == 0)))
	{
		hv_YAxisStartValue = hv_YAxisStartValue - 0.5;
		hv_YAxisEndValue += 0.5;
		hv_YAxisHeight = 1;
	}
	hv_YScaleFactor = hv_YAxisHeightPx / (hv_YAxisHeight.TupleReal());
	hv_YAxisOffsetPx = (hv_YAxisPosition - hv_XAxisStartValue)*hv_XScaleFactor;
	hv_XAxisOffsetPx = (hv_XAxisPosition - hv_YAxisStartValue)*hv_YScaleFactor;
	//
	//Display grid lines
	//
	if (0 != (int(hv_GridColor != HTuple("none"))))
	{
		hv_DotStyle.Clear();
		hv_DotStyle[0] = 5;
		hv_DotStyle[1] = 7;
		SetLineStyle(hv_WindowHandle, hv_DotStyle);
		//if (HDevWindowStack::IsOpen())
		SetColor(hv_WindowHandle, hv_GridColor);
		//
		//Display x grid lines
		if (0 != (int(hv_XGrid != HTuple("none"))))
		{
			if (0 != (int(hv_XGrid == HTuple("min_max_origin"))))
			{
				//Calculate 'min_max_origin' grid line coordinates
				if (0 != (int(hv_YAxisPosition == hv_XAxisStartValue)))
				{
					hv_XGridValues.Clear();
					hv_XGridValues.Append(hv_XAxisStartValue);
					hv_XGridValues.Append(hv_XAxisEndValue);
				}
				else
				{
					hv_XGridValues.Clear();
					hv_XGridValues.Append(hv_XAxisStartValue);
					hv_XGridValues.Append(hv_YAxisPosition);
					hv_XGridValues.Append(hv_XAxisEndValue);
				}
			}
			else
			{
				//Calculate equidistant grid line coordinates
				hv_XGridStart = ((hv_XAxisStartValue / hv_XGrid).TupleCeil())*hv_XGrid;
				hv_XGridValues = HTuple::TupleGenSequence(hv_XGridStart, hv_XAxisEndValue, hv_XGrid);
			}
			hv_XCoord = (hv_XGridValues - hv_XAxisStartValue)*hv_XScaleFactor;
			//Generate and display grid lines
			{
				HTuple end_val392 = (hv_XGridValues.TupleLength()) - 1;
				HTuple step_val392 = 1;
				for (hv_IndexGrid = 0; hv_IndexGrid.Continue(end_val392, step_val392); hv_IndexGrid += step_val392)
				{
					GenContourPolygonXld(&ho_ContourXGrid, (hv_Height - hv_LowerBorder).TupleConcat(hv_UpperBorder),
						(hv_LeftBorder + HTuple(hv_XCoord[hv_IndexGrid])).TupleConcat(hv_LeftBorder + HTuple(hv_XCoord[hv_IndexGrid])));
					//if (HDevWindowStack::IsOpen())
					DispObj(ho_ContourXGrid, hv_WindowHandle);
				}
			}
		}
		//
		//Display y grid lines
		if (0 != (int(hv_YGrid != HTuple("none"))))
		{
			if (0 != (int(hv_YGrid == HTuple("min_max_origin"))))
			{
				//Calculate 'min_max_origin' grid line coordinates
				if (0 != (int(hv_XAxisPosition == hv_YAxisStartValue)))
				{
					hv_YGridValues.Clear();
					hv_YGridValues.Append(hv_YAxisStartValue);
					hv_YGridValues.Append(hv_YAxisEndValue);
				}
				else
				{
					hv_YGridValues.Clear();
					hv_YGridValues.Append(hv_YAxisStartValue);
					hv_YGridValues.Append(hv_XAxisPosition);
					hv_YGridValues.Append(hv_YAxisEndValue);
				}
			}
			else
			{
				//Calculate equidistant grid line coordinates
				hv_YGridStart = ((hv_YAxisStartValue / hv_YGrid).TupleCeil())*hv_YGrid;
				hv_YGridValues = HTuple::TupleGenSequence(hv_YGridStart, hv_YAxisEndValue, hv_YGrid);
			}
			hv_YCoord = (hv_YGridValues - hv_YAxisStartValue)*hv_YScaleFactor;
			//Generate and display grid lines
			{
				HTuple end_val414 = (hv_YGridValues.TupleLength()) - 1;
				HTuple step_val414 = 1;
				for (hv_IndexGrid = 0; hv_IndexGrid.Continue(end_val414, step_val414); hv_IndexGrid += step_val414)
				{
					GenContourPolygonXld(&ho_ContourYGrid, ((hv_Height - hv_LowerBorder) - HTuple(hv_YCoord[hv_IndexGrid])).TupleConcat((hv_Height - hv_LowerBorder) - HTuple(hv_YCoord[hv_IndexGrid])),
						hv_LeftBorder.TupleConcat(hv_Width - hv_RightBorder));
					//if (HDevWindowStack::IsOpen())
					DispObj(ho_ContourYGrid, hv_WindowHandle);
				}
			}
		}
	}
	SetLineStyle(hv_WindowHandle, HTuple());
	//
	//
	//Display the coordinate system axes
	if (0 != (int(hv_AxesColor != HTuple("none"))))
	{
		//Display axes
		//if (HDevWindowStack::IsOpen())
		SetColor(hv_WindowHandle, hv_AxesColor);
		gen_arrow_contour_xld(&ho_XArrow, (hv_Height - hv_LowerBorder) - hv_XAxisOffsetPx,
			hv_LeftBorder, (hv_Height - hv_LowerBorder) - hv_XAxisOffsetPx, hv_Width - hv_RightBorder,
			0, 0);
		//if (HDevWindowStack::IsOpen())
		DispObj(ho_XArrow, hv_WindowHandle);
		gen_arrow_contour_xld(&ho_YArrow, hv_Height - hv_LowerBorder, hv_LeftBorder + hv_YAxisOffsetPx,
			hv_UpperBorder, hv_LeftBorder + hv_YAxisOffsetPx, 0, 0);
		//if (HDevWindowStack::IsOpen())
		DispObj(ho_YArrow, hv_WindowHandle);
		//Display labels
		GetStringExtents(hv_WindowHandle, hv_XLabel, &hv_Ascent, &hv_Descent, &hv_TextWidthXLabel,
			&hv_TextHeightXLabel);
		GetStringExtents(hv_WindowHandle, hv_YLabel, &hv_Ascent, &hv_Descent, &hv_TextWidthYLabel,
			&hv_TextHeightYLabel);
		if (0 != (int(hv_YTickDirection == HTuple("right"))))
		{
			if (0 != (int(hv_XTickDirection == HTuple("up"))))
			{
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, hv_XLabel, "window", ((hv_Height - hv_LowerBorder) - hv_TextHeightXLabel) - 3,
				((hv_Width - hv_RightBorder) - hv_TextWidthXLabel) - 3, hv_AxesColor, "box",
				"false");*/
				disp_message(hv_WindowHandle, hv_XLabel, "window", ((hv_Height - hv_LowerBorder) - hv_TextHeightXLabel) - 3,
					((hv_Width - hv_RightBorder) - hv_TextWidthXLabel) - 3, hv_AxesColor, "false");
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, " " + hv_YLabel, "image", hv_UpperBorder,
				(hv_LeftBorder + 3) + hv_YAxisOffsetPx, hv_AxesColor, "box", "false");*/
				disp_message(hv_WindowHandle, " " + hv_YLabel, "image", hv_UpperBorder,
					(hv_LeftBorder + 3) + hv_YAxisOffsetPx, hv_AxesColor, "false");
			}
			else
			{
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, hv_XLabel, "image", ((hv_Height - hv_LowerBorder) + 3) - hv_XAxisOffsetPx,
				((hv_Width - hv_RightBorder) - hv_TextWidthXLabel) - 3, hv_AxesColor, "box",
				"false");*/
				disp_message(hv_WindowHandle, hv_XLabel, "image", ((hv_Height - hv_LowerBorder) + 3) - hv_XAxisOffsetPx,
					((hv_Width - hv_RightBorder) - hv_TextWidthXLabel) - 3, hv_AxesColor, "false");
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, " " + hv_YLabel, "image", ((hv_Height - hv_LowerBorder) - hv_TextHeightXLabel) - 3,
				(hv_LeftBorder + 3) + hv_YAxisOffsetPx, hv_AxesColor, "box", "false");*/
				disp_message(hv_WindowHandle, " " + hv_YLabel, "image", ((hv_Height - hv_LowerBorder) - hv_TextHeightXLabel) - 3,
					(hv_LeftBorder + 3) + hv_YAxisOffsetPx, hv_AxesColor, "false");
			}
		}
		else
		{
			if (0 != (int(hv_XTickDirection == HTuple("up"))))
			{
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, hv_XLabel, "image", ((hv_Height - hv_LowerBorder) - (2 * hv_TextHeightXLabel)) + 3,
				hv_LeftBorder - 3, hv_AxesColor, "box", "false");*/
				disp_message(hv_WindowHandle, hv_XLabel, "image", ((hv_Height - hv_LowerBorder) - (2 * hv_TextHeightXLabel)) + 3,
					hv_LeftBorder - 3, hv_AxesColor, "false");
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, " " + hv_YLabel, "image", hv_UpperBorder,
				((hv_Width - hv_RightBorder) - hv_TextWidthYLabel) - 13, hv_AxesColor, "box",
				"false");*/
				disp_message(hv_WindowHandle, " " + hv_YLabel, "image", hv_UpperBorder,
					((hv_Width - hv_RightBorder) - hv_TextWidthYLabel) - 13, hv_AxesColor, "false");
			}
			else
			{
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, hv_XLabel, "image", ((hv_Height - hv_LowerBorder) + 3) - hv_XAxisOffsetPx,
				hv_LeftBorder - 3, hv_AxesColor, "box", "false");*/
				disp_message(hv_WindowHandle, hv_XLabel, "image", ((hv_Height - hv_LowerBorder) + 3) - hv_XAxisOffsetPx,
					hv_LeftBorder - 3, hv_AxesColor, "false");
				//if (HDevWindowStack::IsOpen())
				/*DispText(hv_WindowHandle, " " + hv_YLabel, "image", ((hv_Height - hv_LowerBorder) - hv_TextHeightXLabel) - 3,
				((hv_Width - hv_RightBorder) - (2 * hv_TextWidthYLabel)) - 3, hv_AxesColor,
				"box", "false");*/
				disp_message(hv_WindowHandle, " " + hv_YLabel, "image", ((hv_Height - hv_LowerBorder) - hv_TextHeightXLabel) - 3,
					((hv_Width - hv_RightBorder) - (2 * hv_TextWidthYLabel)) - 3, hv_AxesColor, "false");
			}
		}
	}
	//
	//Display ticks
	//
	if (0 != (int(hv_AxesColor != HTuple("none"))))
	{
		//if (HDevWindowStack::IsOpen())
		SetColor(hv_WindowHandle, hv_AxesColor);
		if (0 != (int(hv_XTicks != HTuple("none"))))
		{
			//
			//Display x ticks
			if (0 != hv_XValuesAreStrings)
			{
				//Display string XValues as categories
				hv_XTicks = (hv_XValues.TupleLength()) / (hv_XTickValues.TupleLength());
				hv_XCoord = (hv_XValues - hv_XAxisStartValue)*hv_XScaleFactor;
			}
			else
			{
				//Display tick values
				if (0 != (int(hv_XTicks == HTuple("min_max_origin"))))
				{
					//Calculate 'min_max_origin' tick coordinates
					if (0 != (int(hv_YAxisPosition == hv_XAxisStartValue)))
					{
						hv_XTickValues.Clear();
						hv_XTickValues.Append(hv_XAxisStartValue);
						hv_XTickValues.Append(hv_XAxisEndValue);
					}
					else
					{
						hv_XTickValues.Clear();
						hv_XTickValues.Append(hv_XAxisStartValue);
						hv_XTickValues.Append(hv_YAxisPosition);
						hv_XTickValues.Append(hv_XAxisEndValue);
					}
				}
				else
				{
					//Calculate equidistant tick coordinates
					hv_XTickStart = ((hv_XAxisStartValue / hv_XTicks).TupleCeil())*hv_XTicks;
					hv_XTickValues = HTuple::TupleGenSequence(hv_XTickStart, hv_XAxisEndValue, hv_XTicks);
				}
				//Remove ticks that are smaller than the x-axis start.
				hv_Indices = (hv_XTickValues.TupleLessElem(hv_XAxisStartValue)).TupleFind(1);
				hv_XCoord = (hv_XTickValues - hv_XAxisStartValue)*hv_XScaleFactor;
				hv_XCoord = hv_XCoord.TupleRemove(hv_Indices);
				hv_XTickValues = hv_XTickValues.TupleRemove(hv_Indices);
				//
				if (0 != (int(hv_FormatX == HTuple("default"))))
				{
					hv_TypeTicks = hv_XTicks.TupleType();
					if (0 != (int(hv_TypeTicks == 4)))
					{
						//String ('min_max_origin')
						//Format depends on actual values
						hv_TypeTicks = hv_XTickValues.TupleType();
					}
					if (0 != (int(hv_TypeTicks == 1)))
					{
						//Round to integer
						hv_XTickValues = hv_XTickValues.TupleInt();
					}
					else
					{
						//Use floating point numbers
						hv_XTickValues = hv_XTickValues.TupleString(".2f");
					}
				}
				else
				{
					hv_XTickValues = hv_XTickValues.TupleString(hv_FormatX);
				}
			}
			//Generate and display ticks
			{
				HTuple end_val503 = (hv_XTickValues.TupleLength()) - 1;
				HTuple step_val503 = 1;
				for (hv_IndexTicks = 0; hv_IndexTicks.Continue(end_val503, step_val503); hv_IndexTicks += step_val503)
				{
					GetStringExtents(hv_WindowHandle, HTuple(hv_XTickValues[hv_IndexTicks]),
						&hv_Ascent1, &hv_Descent1, &hv_TextWidthXTicks, &hv_TextHeightXTicks);
					if (0 != (int(hv_XTickDirection == HTuple("up"))))
					{
						GenContourPolygonXld(&ho_ContourXTick, ((hv_Height - hv_LowerBorder) - hv_XAxisOffsetPx).TupleConcat(((hv_Height - hv_LowerBorder) - hv_XAxisOffsetPx) - 5),
							(hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks])).TupleConcat(hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks])));
						//if (HDevWindowStack::IsOpen())
						/*DispText(hv_WindowHandle, HTuple(hv_XTickValues[hv_IndexTicks]),
						"image", ((hv_Height - hv_LowerBorder) + 2) - hv_XAxisOffsetPx, hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks]),
						hv_AxesColor, "box", "false");*/
						disp_message(hv_WindowHandle, HTuple(hv_XTickValues[hv_IndexTicks]),
							"image", ((hv_Height - hv_LowerBorder) + 2) - hv_XAxisOffsetPx, hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks]),
							hv_AxesColor, "false");
					}
					else
					{
						GenContourPolygonXld(&ho_ContourXTick, (((hv_Height - hv_LowerBorder) - hv_XAxisOffsetPx) + 5).TupleConcat((hv_Height - hv_LowerBorder) - hv_XAxisOffsetPx),
							(hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks])).TupleConcat(hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks])));
						//if (HDevWindowStack::IsOpen())
						/*DispText(hv_WindowHandle, HTuple(hv_XTickValues[hv_IndexTicks]),
						"image", ((hv_Height - hv_LowerBorder) - (2 * hv_TextHeightXTicks)) - hv_XAxisOffsetPx,
						hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks]), hv_AxesColor, "box",
						"false");*/
						disp_message(hv_WindowHandle, HTuple(hv_XTickValues[hv_IndexTicks]),
							"image", ((hv_Height - hv_LowerBorder) - (2 * hv_TextHeightXTicks)) - hv_XAxisOffsetPx,
							hv_LeftBorder + HTuple(hv_XCoord[hv_IndexTicks]), hv_AxesColor, "false");
					}
					//if (HDevWindowStack::IsOpen())
					DispObj(ho_ContourXTick, hv_WindowHandle);
				}
			}
		}
		//
		if (0 != (int(hv_YTicks != HTuple("none"))))
		{
			//
			//Display y ticks
			if (0 != (int(hv_YTicks == HTuple("min_max_origin"))))
			{
				//Calculate 'min_max_origin' tick coordinates
				if (0 != (int(hv_XAxisPosition == hv_YAxisStartValue)))
				{
					hv_YTickValues.Clear();
					hv_YTickValues.Append(hv_YAxisStartValue);
					hv_YTickValues.Append(hv_YAxisEndValue);
				}
				else
				{
					hv_YTickValues.Clear();
					hv_YTickValues.Append(hv_YAxisStartValue);
					hv_YTickValues.Append(hv_XAxisPosition);
					hv_YTickValues.Append(hv_YAxisEndValue);
				}
			}
			else
			{
				//Calculate equidistant tick coordinates
				hv_YTickStart = ((hv_YAxisStartValue / hv_YTicks).TupleCeil())*hv_YTicks;
				hv_YTickValues = HTuple::TupleGenSequence(hv_YTickStart, hv_YAxisEndValue, hv_YTicks);
			}
			//Remove ticks that are smaller than the y-axis start.
			hv_Indices = (hv_YTickValues.TupleLessElem(hv_YAxisStartValue)).TupleFind(1);
			hv_YCoord = (hv_YTickValues - hv_YAxisStartValue)*hv_YScaleFactor;
			hv_YCoord = hv_YCoord.TupleRemove(hv_Indices);
			hv_YTickValues = hv_YTickValues.TupleRemove(hv_Indices);
			//
			if (0 != (int(hv_FormatY == HTuple("default"))))
			{
				hv_TypeTicks = hv_YTicks.TupleType();
				if (0 != (int(hv_TypeTicks == 4)))
				{
					//String ('min_max_origin')
					//Format depends on actual values
					hv_TypeTicks = hv_YTickValues.TupleType();
				}
				if (0 != (int(hv_TypeTicks == 1)))
				{
					//Round to integer
					hv_YTickValues = hv_YTickValues.TupleInt();
				}
				else
				{
					//Use floating point numbers
					hv_YTickValues = hv_YTickValues.TupleString(".2f");
				}
			}
			else
			{
				hv_YTickValues = hv_YTickValues.TupleString(hv_FormatY);
			}
			//Generate and display ticks
			{
				HTuple end_val555 = (hv_YTickValues.TupleLength()) - 1;
				HTuple step_val555 = 1;
				for (hv_IndexTicks = 0; hv_IndexTicks.Continue(end_val555, step_val555); hv_IndexTicks += step_val555)
				{
					GetStringExtents(hv_WindowHandle, HTuple(hv_YTickValues[hv_IndexTicks]),
						&hv_Ascent1, &hv_Descent1, &hv_TextWidthYTicks, &hv_TextHeightYTicks);
					if (0 != (int(hv_YTickDirection == HTuple("right"))))
					{
						GenContourPolygonXld(&ho_ContourYTick, ((hv_Height - hv_LowerBorder) - HTuple(hv_YCoord[hv_IndexTicks])).TupleConcat((hv_Height - hv_LowerBorder) - HTuple(hv_YCoord[hv_IndexTicks])),
							(hv_LeftBorder + hv_YAxisOffsetPx).TupleConcat((hv_LeftBorder + hv_YAxisOffsetPx) + 5));
						//if (HDevWindowStack::IsOpen())
						/*DispText(hv_WindowHandle, HTuple(hv_YTickValues[hv_IndexTicks]),
						"image", (((hv_Height - hv_LowerBorder) - hv_TextHeightYTicks) + 3) - HTuple(hv_YCoord[hv_IndexTicks]),
						((hv_LeftBorder - hv_TextWidthYTicks) - 2) + hv_YAxisOffsetPx, hv_AxesColor,
						"box", "false");*/
						disp_message(hv_WindowHandle, HTuple(hv_YTickValues[hv_IndexTicks]),
							"image", (((hv_Height - hv_LowerBorder) - hv_TextHeightYTicks) + 3) - HTuple(hv_YCoord[hv_IndexTicks]),
							((hv_LeftBorder - hv_TextWidthYTicks) - 2) + hv_YAxisOffsetPx, hv_AxesColor, "false");
					}
					else
					{
						GenContourPolygonXld(&ho_ContourYTick, ((hv_Height - hv_LowerBorder) - HTuple(hv_YCoord[hv_IndexTicks])).TupleConcat((hv_Height - hv_LowerBorder) - HTuple(hv_YCoord[hv_IndexTicks])),
							((hv_LeftBorder + hv_YAxisOffsetPx) - 5).TupleConcat(hv_LeftBorder + hv_YAxisOffsetPx));
						//if (HDevWindowStack::IsOpen())
						/*DispText(hv_WindowHandle, HTuple(hv_YTickValues[hv_IndexTicks]),
						"image", (((hv_Height - hv_LowerBorder) - hv_TextHeightYTicks) + 3) - HTuple(hv_YCoord[hv_IndexTicks]),
						(hv_LeftBorder + 2) + hv_YAxisOffsetPx, hv_AxesColor, "box", "false");*/
						disp_message(hv_WindowHandle, HTuple(hv_YTickValues[hv_IndexTicks]),
							"image", (((hv_Height - hv_LowerBorder) - hv_TextHeightYTicks) + 3) - HTuple(hv_YCoord[hv_IndexTicks]),
							(hv_LeftBorder + 2) + hv_YAxisOffsetPx, hv_AxesColor, "false");
					}
					//if (HDevWindowStack::IsOpen())
					DispObj(ho_ContourYTick, hv_WindowHandle);
				}
			}
		}
	}
	//
	//Display function plot
	//
	if (0 != (int(hv_Color != HTuple("none"))))
	{
		if (0 != (HTuple(int(hv_XValues != HTuple())).TupleAnd(int(hv_YValues != HTuple()))))
		{
			hv_Num = (hv_YValues.TupleLength()) / (hv_XValues.TupleLength());
			//
			//Iterate over all functions to be displayed
			{
				HTuple end_val576 = hv_Num - 1;
				HTuple step_val576 = 1;
				for (hv_I = 0; hv_I.Continue(end_val576, step_val576); hv_I += step_val576)
				{
					//Select y values for current function
					hv_YSelected = hv_YValues.TupleSelectRange(hv_I*(hv_XValues.TupleLength()), ((hv_I + 1)*(hv_XValues.TupleLength())) - 1);
					//Set color
					if (0 != (int(hv_Color == HTuple())))
					{
						SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
					}
					else
					{
						//if (HDevWindowStack::IsOpen())
						SetColor(hv_WindowHandle, HTuple(hv_Color[hv_I % (hv_Color.TupleLength())]));
					}
					//
					//Display in different styles
					//
					if (0 != (HTuple(int(hv_Style == HTuple("line"))).TupleOr(int(hv_Style == HTuple()))))
					{
						//Line
						GenContourPolygonXld(&ho_Contour, ((hv_Height - hv_LowerBorder) - (hv_YSelected*hv_YScaleFactor)) + (hv_YAxisStartValue*hv_YScaleFactor),
							((hv_XValues*hv_XScaleFactor) + hv_LeftBorder) - (hv_XAxisStartValue*hv_XScaleFactor));
						//Clip, if necessary
						if (0 != (int(hv_Clip == HTuple("yes"))))
						{
							ClipContoursXld(ho_Contour, &ho_Contour, hv_UpperBorder, hv_LeftBorder,
								hv_Height - hv_LowerBorder, hv_Width - hv_RightBorder);
						}
						//if (HDevWindowStack::IsOpen())
						DispObj(ho_Contour, hv_WindowHandle);
					}
					else if (0 != (int(hv_Style == HTuple("cross"))))
					{
						//Cross
						GenCrossContourXld(&ho_Cross, ((hv_Height - hv_LowerBorder) - (hv_YSelected*hv_YScaleFactor)) + (hv_YAxisStartValue*hv_YScaleFactor),
							((hv_XValues*hv_XScaleFactor) + hv_LeftBorder) - (hv_XAxisStartValue*hv_XScaleFactor),
							6, 0.785398);
						//Clip, if necessary
						if (0 != (int(hv_Clip == HTuple("yes"))))
						{
							ClipContoursXld(ho_Cross, &ho_Cross, hv_UpperBorder, hv_LeftBorder, hv_Height - hv_LowerBorder,
								hv_Width - hv_RightBorder);
						}
						//if (HDevWindowStack::IsOpen())
						DispObj(ho_Cross, hv_WindowHandle);
					}
					else if (0 != (int(hv_Style == HTuple("filled"))))
					{
						//Filled
						hv_Y1Selected.Clear();
						hv_Y1Selected.Append(0 + hv_XAxisPosition);
						hv_Y1Selected.Append(hv_YSelected);
						hv_Y1Selected.Append(0 + hv_XAxisPosition);
						hv_X1Selected.Clear();
						hv_X1Selected.Append(hv_XValues.TupleMin());
						hv_X1Selected.Append(hv_XValues);
						hv_X1Selected.Append(hv_XValues.TupleMax());
						//if (HDevWindowStack::IsOpen())
						SetDraw(hv_WindowHandle, "fill");
						GenRegionPolygonFilled(&ho_Filled, ((hv_Height - hv_LowerBorder) - (hv_Y1Selected*hv_YScaleFactor)) + (hv_YAxisStartValue*hv_YScaleFactor),
							((hv_X1Selected*hv_XScaleFactor) + hv_LeftBorder) - (hv_XAxisStartValue*hv_XScaleFactor));
						//Clip, if necessary
						if (0 != (int(hv_Clip == HTuple("yes"))))
						{
							ClipRegion(ho_Filled, &ho_Filled, hv_UpperBorder, hv_LeftBorder, hv_Height - hv_LowerBorder,
								hv_Width - hv_RightBorder);
						}
						//if (HDevWindowStack::IsOpen())
						DispObj(ho_Filled, hv_WindowHandle);
					}
					else if (0 != (int(hv_Style == HTuple("step"))))
					{
						GenEmptyObj(&ho_Stair);
						{
							HTuple end_val617 = (hv_XValues.TupleLength()) - 2;
							HTuple step_val617 = 1;
							for (hv_Index = 0; hv_Index.Continue(end_val617, step_val617); hv_Index += step_val617)
							{
								hv_Row1 = ((hv_Height - hv_LowerBorder) - (HTuple(hv_YSelected[hv_Index])*hv_YScaleFactor)) + (hv_YAxisStartValue*hv_YScaleFactor);
								hv_Row2 = ((hv_Height - hv_LowerBorder) - (HTuple(hv_YSelected[hv_Index + 1])*hv_YScaleFactor)) + (hv_YAxisStartValue*hv_YScaleFactor);
								hv_Col1 = ((HTuple(hv_XValues[hv_Index])*hv_XScaleFactor) + hv_LeftBorder) - (hv_XAxisStartValue*hv_XScaleFactor);
								hv_Col2 = ((HTuple(hv_XValues[hv_Index + 1])*hv_XScaleFactor) + hv_LeftBorder) - (hv_XAxisStartValue*hv_XScaleFactor);
								GenContourPolygonXld(&ho_StairTmp, (hv_Row1.TupleConcat(hv_Row1)).TupleConcat(hv_Row2),
									(hv_Col1.TupleConcat(hv_Col2)).TupleConcat(hv_Col2));
								ConcatObj(ho_Stair, ho_StairTmp, &ho_Stair);
							}
						}
						UnionAdjacentContoursXld(ho_Stair, &ho_Stair, 0.1, 0.1, "attr_keep");
						if (0 != (int(hv_Clip == HTuple("yes"))))
						{
							ClipRegion(ho_Stair, &ho_Stair, hv_UpperBorder, hv_LeftBorder, hv_Height - hv_LowerBorder,
								hv_Width - hv_RightBorder);
						}
						//if (HDevWindowStack::IsOpen())
						DispObj(ho_Stair, hv_WindowHandle);
					}
					else
					{
						throw HException("Unsupported style: " + hv_Style);
					}
				}
			}
		}
	}
	//
	//
	//Reset original display settings
	SetPart(hv_WindowHandle, hv_PartRow1, hv_PartColumn1, hv_PartRow2, hv_PartColumn2);
	/*if (HDevWindowStack::IsOpen())
	SetPart(HDevWindowStack::GetActive(), hv_PartRow1, hv_PartColumn1, hv_PartRow2,
	hv_PartColumn2);
	if (HDevWindowStack::IsOpen())
	HDevWindowStack::SetActive(hv_PreviousWindowHandle);*/
	SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
	/*if (HDevWindowStack::IsOpen())
	SetDraw(HDevWindowStack::GetActive(), hv_DrawMode);*/
	SetLineStyle(hv_WindowHandle, hv_OriginStyle);
	SetSystem("clip_region", hv_ClipRegion);
	return;
}