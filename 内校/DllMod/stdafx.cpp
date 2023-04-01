
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Paste.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"
#include "Singleton.h"
#include "Log.h"
int g_nExt = 0;

void DoEvent()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void SetDisplayFont(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold, HTuple hv_Slant)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_OS, hv_Fonts, hv_Style, hv_Exception;
	HTuple  hv_AvailableFonts, hv_Fdx, hv_Indices;

	//This procedure sets the text font of the current window with
	//the specified attributes.
	//
	//Input parameters:
	//WindowHandle: The graphics window for which the font will be set
	//Size: The font size. If Size=-1, the default of 16 is used.
	//Bold: If set to 'true', a bold font is used
	//Slant: If set to 'true', a slanted font is used
	//
	GetSystem("operating_system", &hv_OS);
	// dev_get_preferences(...); only in hdevelop
	// dev_set_preferences(...); only in hdevelop
	if (0 != (HTuple(hv_Size == HTuple()).TupleOr(hv_Size == -1)))
	{
		hv_Size = 16;
	}
	if (0 != ((hv_OS.TupleSubstr(0, 2)) == HTuple("Win")))
	{
		//Restore previous behaviour
		hv_Size = (1.13677*hv_Size).TupleInt();
	}
	else
	{
		hv_Size = hv_Size.TupleInt();
	}
	if (0 != (hv_Font == HTuple("Courier")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Courier";
		hv_Fonts[1] = "Courier 10 Pitch";
		hv_Fonts[2] = "Courier New";
		hv_Fonts[3] = "CourierNew";
		hv_Fonts[4] = "Liberation Mono";
	}
	else if (0 != (hv_Font == HTuple("mono")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Consolas";
		hv_Fonts[1] = "Menlo";
		hv_Fonts[2] = "Courier";
		hv_Fonts[3] = "Courier 10 Pitch";
		hv_Fonts[4] = "FreeMono";
		hv_Fonts[5] = "Liberation Mono";
	}
	else if (0 != (hv_Font == HTuple("sans")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Luxi Sans";
		hv_Fonts[1] = "DejaVu Sans";
		hv_Fonts[2] = "FreeSans";
		hv_Fonts[3] = "Arial";
		hv_Fonts[4] = "Liberation Sans";
	}
	else if (0 != (hv_Font == HTuple("serif")))
	{
		hv_Fonts.Clear();
		hv_Fonts[0] = "Times New Roman";
		hv_Fonts[1] = "Luxi Serif";
		hv_Fonts[2] = "DejaVu Serif";
		hv_Fonts[3] = "FreeSerif";
		hv_Fonts[4] = "Utopia";
		hv_Fonts[5] = "Liberation Serif";
	}
	else
	{
		hv_Fonts = hv_Font;
	}
	hv_Style = "";
	if (0 != (hv_Bold == HTuple("true")))
	{
		hv_Style += HTuple("Bold");
	}
	else if (0 != (hv_Bold != HTuple("false")))
	{
		hv_Exception = "Wrong value of control parameter Bold";
		throw HException(hv_Exception);
	}
	if (0 != (hv_Slant == HTuple("true")))
	{
		hv_Style += HTuple("Italic");
	}
	else if (0 != (hv_Slant != HTuple("false")))
	{
		hv_Exception = "Wrong value of control parameter Slant";
		throw HException(hv_Exception);
	}
	if (0 != (hv_Style == HTuple("")))
	{
		hv_Style = "Normal";
	}
	QueryFont(hv_WindowHandle, &hv_AvailableFonts);
	hv_Font = "";
	{
		HTuple end_val50 = (hv_Fonts.TupleLength()) - 1;
		HTuple step_val50 = 1;
		for (hv_Fdx = 0; hv_Fdx.Continue(end_val50, step_val50); hv_Fdx += step_val50)
		{
			hv_Indices = hv_AvailableFonts.TupleFind(HTuple(hv_Fonts[hv_Fdx]));
			if (0 != ((hv_Indices.TupleLength()) > 0))
			{
				if (0 != (HTuple(hv_Indices[0]) >= 0))
				{
					hv_Font = HTuple(hv_Fonts[hv_Fdx]);
					break;
				}
			}
		}
	}
	if (0 != (hv_Font == HTuple("")))
	{
		throw HException("Wrong value of control parameter Font");
	}
	hv_Font = (((hv_Font + "-") + hv_Style) + "-") + hv_Size;
	SetFont(hv_WindowHandle, hv_Font);
	// dev_set_preferences(...); only in hdevelop
	return;
}


void DispMessage(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem, HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box)
{
	HTuple  hv_Red, hv_Green, hv_Blue, hv_Row1Part;
	HTuple  hv_Column1Part, hv_Row2Part, hv_Column2Part, hv_RowWin;
	HTuple  hv_ColumnWin, hv_WidthWin, hv_HeightWin, hv_MaxAscent;
	HTuple  hv_MaxDescent, hv_MaxWidth, hv_MaxHeight, hv_R1;
	HTuple  hv_C1, hv_FactorRow, hv_FactorColumn, hv_UseShadow;
	HTuple  hv_ShadowColor, hv_Exception, hv_Width, hv_Index;
	HTuple  hv_Ascent, hv_Descent, hv_W, hv_H, hv_FrameHeight;
	HTuple  hv_FrameWidth, hv_R2, hv_C2, hv_DrawMode, hv_CurrentColor;

	//This procedure displays text in a graphics window.
	//
	//Input parameters:
	//WindowHandle: The WindowHandle of the graphics window, where
	//   the message should be displayed
	//String: A tuple of strings containing the text message to be displayed
	//CoordSystem: If set to 'window', the text position is given
	//   with respect to the window coordinate system.
	//   If set to 'image', image coordinates are used.
	//   (This may be useful in zoomed images.)
	//Row: The row coordinate of the desired text position
	//   If set to -1, a default value of 12 is used.
	//Column: The column coordinate of the desired text position
	//   If set to -1, a default value of 12 is used.
	//Color: defines the color of the text as string.
	//   If set to [], '' or 'auto' the currently set color is used.
	//   If a tuple of strings is passed, the colors are used cyclically
	//   for each new textline.
	//Box: If Box[0] is set to 'true', the text is written within an orange box.
	//     If set to' false', no box is displayed.
	//     If set to a color string (e.g. 'white', '#FF00CC', etc.),
	//       the text is written in a box of that color.
	//     An optional second value for Box (Box[1]) controls if a shadow is displayed:
	//       'true' -> display a shadow in a default color
	//       'false' -> display no shadow (same as if no second value is given)
	//       otherwise -> use given string as color string for the shadow color
	//
	//Prepare window
	GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
	GetPart(hv_WindowHandle, &hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
	GetWindowExtents(hv_WindowHandle, &hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);
	SetPart(hv_WindowHandle, 0, 0, hv_HeightWin - 1, hv_WidthWin - 1);
	//
	//default settings
	if (0 != (hv_Row == -1))
	{
		hv_Row = 12;
	}
	if (0 != (hv_Column == -1))
	{
		hv_Column = 12;
	}
	if (0 != (hv_Color == HTuple()))
	{
		hv_Color = "";
	}
	//
	hv_String = (("" + hv_String) + "").TupleSplit("\n");
	//
	//Estimate extentions of text depending on font size.
	GetFontExtents(hv_WindowHandle, &hv_MaxAscent, &hv_MaxDescent, &hv_MaxWidth, &hv_MaxHeight);
	if (0 != (hv_CoordSystem == HTuple("window")))
	{
		hv_R1 = hv_Row;
		hv_C1 = hv_Column;
	}
	else
	{
		//Transform image to window coordinates
		hv_FactorRow = (1.*hv_HeightWin) / ((hv_Row2Part - hv_Row1Part) + 1);
		hv_FactorColumn = (1.*hv_WidthWin) / ((hv_Column2Part - hv_Column1Part) + 1);
		hv_R1 = ((hv_Row - hv_Row1Part) + 0.5)*hv_FactorRow;
		hv_C1 = ((hv_Column - hv_Column1Part) + 0.5)*hv_FactorColumn;
	}
	//
	//Display text box depending on text size
	hv_UseShadow = 1;
	hv_ShadowColor = "gray";
	if (0 != (HTuple(hv_Box[0]) == HTuple("true")))
	{
		hv_Box[0] = "#fce9d4";
		hv_ShadowColor = "#f28d26";
	}
	if (0 != ((hv_Box.TupleLength()) > 1))
	{
		if (0 != (HTuple(hv_Box[1]) == HTuple("true")))
		{
			//Use default ShadowColor set above
		}
		else if (0 != (HTuple(hv_Box[1]) == HTuple("false")))
		{
			hv_UseShadow = 0;
		}
		else
		{
			hv_ShadowColor = ((const HTuple&)hv_Box)[1];
			//Valid color?
			try
			{
				SetColor(hv_WindowHandle, HTuple(hv_Box[1]));
			}
			// catch (Exception) 
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_Exception);
				hv_Exception = "Wrong value of control parameter Box[1] (must be a 'true', 'false', or a valid color string)";
				throw HalconCpp::HException(hv_Exception);
			}
		}
	}
	if (0 != (HTuple(hv_Box[0]) != HTuple("false")))
	{
		//Valid color?
		try
		{
			SetColor(hv_WindowHandle, HTuple(hv_Box[0]));
		}
		// catch (Exception) 
		catch (HalconCpp::HException &HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&hv_Exception);
			hv_Exception = "Wrong value of control parameter Box[0] (must be a 'true', 'false', or a valid color string)";
			throw HalconCpp::HException(hv_Exception);
		}
		//Calculate box extents
		hv_String = (" " + hv_String) + " ";
		hv_Width = HTuple();
		{
			HTuple end_val93 = (hv_String.TupleLength()) - 1;
			HTuple step_val93 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val93, step_val93); hv_Index += step_val93)
			{
				GetStringExtents(hv_WindowHandle, HTuple(hv_String[hv_Index]), &hv_Ascent,
					&hv_Descent, &hv_W, &hv_H);
				hv_Width = hv_Width.TupleConcat(hv_W);
			}
		}
		hv_FrameHeight = hv_MaxHeight*(hv_String.TupleLength());
		hv_FrameWidth = (HTuple(0).TupleConcat(hv_Width)).TupleMax();
		hv_R2 = hv_R1 + hv_FrameHeight;
		hv_C2 = hv_C1 + hv_FrameWidth;
		//Display rectangles
		GetDraw(hv_WindowHandle, &hv_DrawMode);
		SetDraw(hv_WindowHandle, "fill");
		//Set shadow color
		SetColor(hv_WindowHandle, hv_ShadowColor);
		if (0 != hv_UseShadow)
		{
			DispRectangle1(hv_WindowHandle, hv_R1 + 1, hv_C1 + 1, hv_R2 + 1, hv_C2 + 1);
		}
		//Set box color
		SetColor(hv_WindowHandle, HTuple(hv_Box[0]));
		DispRectangle1(hv_WindowHandle, hv_R1, hv_C1, hv_R2, hv_C2);
		SetDraw(hv_WindowHandle, hv_DrawMode);
	}
	//Write text.
	{
		HTuple end_val115 = (hv_String.TupleLength()) - 1;
		HTuple step_val115 = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val115, step_val115); hv_Index += step_val115)
		{
			hv_CurrentColor = ((const HTuple&)hv_Color)[hv_Index % (hv_Color.TupleLength())];
			if (0 != (HTuple(hv_CurrentColor != HTuple("")).TupleAnd(hv_CurrentColor != HTuple("auto"))))
			{
				SetColor(hv_WindowHandle, hv_CurrentColor);
			}
			else
			{
				SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
			}
			hv_Row = hv_R1 + (hv_MaxHeight*hv_Index);
			SetTposition(hv_WindowHandle, hv_Row, hv_C1);
			WriteString(hv_WindowHandle, HTuple(hv_String[hv_Index]));
		}
	}
	//Reset changed window settings
	SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
	SetPart(hv_WindowHandle, hv_Row1Part, hv_Column1Part, hv_Row2Part, hv_Column2Part);
	return;
}

void rake(HObject ho_Image, HObject *ho_Regions, HTuple hv_Elements, HTuple hv_DetectHeight,
	HTuple hv_DetectWidth, HTuple hv_Sigma, HTuple hv_Threshold, HTuple hv_Transition,
	HTuple hv_Select, HTuple hv_Row1, HTuple hv_Column1, HTuple hv_Row2, HTuple hv_Column2,
	HTuple *hv_ResultRow, HTuple *hv_ResultColumn)
{

	// Local iconic variables
	HObject  ho_RegionLines, ho_Rectangle, ho_Arrow1;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_ATan, hv_i, hv_RowC;
	HTuple  hv_ColC, hv_Distance, hv_RowL2, hv_RowL1, hv_ColL2;
	HTuple  hv_ColL1, hv_MsrHandle_Measure, hv_RowEdge, hv_ColEdge;
	HTuple  hv_Amplitude, hv_tRow, hv_tCol, hv_t, hv_Number;
	HTuple  hv_j;

	//��ȡͼ��ߴ�
	GetImageSize(ho_Image, &hv_Width, &hv_Height);
	//����һ������ʾ����������ʾ
	GenEmptyObj(&(*ho_Regions));
	//��ʼ����Ե��������
	(*hv_ResultRow) = HTuple();
	(*hv_ResultColumn) = HTuple();
	//����ֱ��xld
	GenContourPolygonXld(&ho_RegionLines, hv_Row1.TupleConcat(hv_Row2), hv_Column1.TupleConcat(hv_Column2));
	//�洢����ʾ����
	ConcatObj((*ho_Regions), ho_RegionLines, &(*ho_Regions));
	//����ֱ����x��ļнǣ���ʱ�뷽��Ϊ����
	AngleLx(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_ATan);

	//��Ե��ⷽ��ֱ�ڼ��ֱ�ߣ�ֱ�߷���������ת90��Ϊ��Ե��ⷽ��
	hv_ATan += HTuple(90).TupleRad();

	//���ݼ��ֱ�߰�˳���������������Σ����洢����ʾ����
	{
		HTuple end_val18 = hv_Elements;
		HTuple step_val18 = 1;
		for (hv_i = 1; hv_i.Continue(end_val18, step_val18); hv_i += step_val18)
		{
			//RowC := Row1+(((Row2-Row1)*i)/(Elements+1))
			//ColC := Column1+(Column2-Column1)*i/(Elements+1)
			//if (RowC>Height-1 or RowC<0 or ColC>Width-1 or ColC<0)
			//continue
			//endif
			//���ֻ��һ���������Σ���Ϊ���߹��ߣ����Ϊ���ֱ�ߵĳ���
			if (0 != (hv_Elements == 1))
			{
				hv_RowC = (hv_Row1 + hv_Row2)*0.5;
				hv_ColC = (hv_Column1 + hv_Column2)*0.5;
				//�ж��Ƿ񳬳�ͼ��,����������Ե
				if (0 != (HTuple(HTuple(HTuple(hv_RowC > (hv_Height - 1)).TupleOr(hv_RowC<0)).TupleOr(hv_ColC>(hv_Width - 1))).TupleOr(hv_ColC<0)))
				{
					continue;
				}
				DistancePp(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_Distance);
				hv_DetectWidth = hv_Distance;
				GenRectangle2ContourXld(&ho_Rectangle, hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
					hv_Distance / 2);
			}
			else
			{
				//����ж���������Σ������ò�������xld
				hv_RowC = hv_Row1 + (((hv_Row2 - hv_Row1)*(hv_i - 1)) / (hv_Elements - 1));
				hv_ColC = hv_Column1 + (((hv_Column2 - hv_Column1)*(hv_i - 1)) / (hv_Elements - 1));
				//�ж��Ƿ񳬳�ͼ��,����������Ե
				if (0 != (HTuple(HTuple(HTuple(hv_RowC>(hv_Height - 1)).TupleOr(hv_RowC<0)).TupleOr(hv_ColC>(hv_Width - 1))).TupleOr(hv_ColC < 0)))
				{
					continue;
				}
				GenRectangle2ContourXld(&ho_Rectangle, hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
					hv_DetectWidth / 2);
			}

			//�Ѳ�������xld�洢����ʾ����
			ConcatObj((*ho_Regions), ho_Rectangle, &(*ho_Regions));
			if (0 != (hv_i == 1))
			{
				//�ڵ�һ���������λ���һ����ͷxld������ֻ�Ǳ�Ե��ⷽ��
				hv_RowL2 = hv_RowC + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_RowL1 = hv_RowC - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_ColL2 = hv_ColC + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				hv_ColL1 = hv_ColC - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				//gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,25);
				//��xld�洢����ʾ����
				ConcatObj((*ho_Regions), ho_Arrow1, &(*ho_Regions));
			}
			//��������������
			GenMeasureRectangle2(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2, hv_DetectWidth / 2,
				hv_Width, hv_Height, "nearest_neighbor", &hv_MsrHandle_Measure);

			//���ü���
			if (0 != (hv_Transition == HTuple("negative")))
			{
				hv_Transition = "negative";
			}
			else
			{
				if (0 != (hv_Transition == HTuple("positive")))
				{

					hv_Transition = "positive";
				}
				else
				{
					hv_Transition = "all";
				}
			}
			//���ñ�Եλ�á���ǿ���Ǵ����б�Ե��ѡ����Ⱦ���ֵ���㣬��Ҫ����Ϊ'all'
			if (0 != (hv_Select == HTuple("first")))
			{
				hv_Select = "first";
			}
			else
			{
				if (0 != (hv_Select == HTuple("last")))
				{

					hv_Select = "last";
				}
				else
				{
					hv_Select = "all";
				}
			}
			//����Ե
			MeasurePos(ho_Image, hv_MsrHandle_Measure, hv_Sigma, hv_Threshold, hv_Transition,
				hv_Select, &hv_RowEdge, &hv_ColEdge, &hv_Amplitude, &hv_Distance);
			//�������������
			CloseMeasure(hv_MsrHandle_Measure);

			//��ʱ������ʼ��
			//tRow��tCol�����ҵ�ָ����Ե������
			hv_tRow = 0;
			hv_tCol = 0;
			//t�����Ե�ķ��Ⱦ���ֵ
			hv_t = 0;
			//�ҵ��ı�Ե��������Ϊ1��
			TupleLength(hv_RowEdge, &hv_Number);
			if (0 != (hv_Number < 1))
			{
				continue;
			}
			//�ж����Եʱ��ѡ����Ⱦ���ֵ���ı�Ե
	{
		HTuple end_val100 = hv_Number - 1;
		HTuple step_val100 = 1;
		for (hv_j = 0; hv_j.Continue(end_val100, step_val100); hv_j += step_val100)
		{
			if (0 != ((HTuple(hv_Amplitude[hv_j]).TupleAbs()) > hv_t))
			{

				hv_tRow = HTuple(hv_RowEdge[hv_j]);
				hv_tCol = HTuple(hv_ColEdge[hv_j]);
				hv_t = HTuple(hv_Amplitude[hv_j]).TupleAbs();
			}
		}
	}
			//���ҵ��ı�Ե�������������
			if (0 != (hv_t > 0))
			{
				(*hv_ResultRow) = (*hv_ResultRow).TupleConcat(hv_tRow);
				(*hv_ResultColumn) = (*hv_ResultColumn).TupleConcat(hv_tCol);
			}
		}
	}

	return;
}

void peak(HObject ho_Image, HTuple hv_Row, HTuple hv_Coloumn, HTuple hv_Phi, HTuple hv_Length1,
	HTuple hv_Length2, HTuple hv_DetectWidth, HTuple hv_Sigma, HTuple hv_Threshold,
	HTuple hv_Transition, HTuple hv_Select, HTuple *hv_EdgeRows, HTuple *hv_EdgeColumns,
	HTuple *hv_ResultRow, HTuple *hv_ResultColumn)
{

	// Local iconic variables
	HObject  ho_Regions1;

	// Local control variables
	HTuple  hv_ResultCol, hv_ROILineRow1, hv_ROILineCol1;
	HTuple  hv_ROILineRow2, hv_ROILineCol2, hv_StdLineRow1;
	HTuple  hv_StdLineCol1, hv_StdLineRow2, hv_StdLineCol2;
	HTuple  hv_Cos, hv_Sin, hv_Col1, hv_Row1, hv_Col2, hv_Row2;
	HTuple  hv_Col3, hv_Row3, hv_Col4, hv_Row4, hv_ResultRows;
	HTuple  hv_ResultColumns, hv_Max, hv_i, hv_Distance1;

	//��ʼ��
	(*hv_ResultRow) = -9999;
	hv_ResultCol = -9999;
	(*hv_EdgeColumns) = HTuple();
	(*hv_EdgeRows) = HTuple();

	//�������Length2����ֱ����Ϊrake���ߵ�ROI
	hv_ROILineRow1 = 0;
	hv_ROILineCol1 = 0;
	hv_ROILineRow2 = 0;
	hv_ROILineCol2 = 0;

	//������η�����ֱ�ߵı�����׼��
	hv_StdLineRow1 = 0;
	hv_StdLineCol1 = 0;
	hv_StdLineRow2 = 0;
	hv_StdLineCol2 = 0;
	//�жϷ�������Ƿ���Ч
	if (0 != (HTuple(hv_Length1 <= 0).TupleOr(hv_Length2 <= 0)))
	{
		return;
	}

	//���������νǶȵ�����ֵ������ֵ
	TupleCos(hv_Phi, &hv_Cos);
	TupleSin(hv_Phi, &hv_Sin);

	//���ε�һ���˵�����
	hv_Col1 = 1.0*((hv_Coloumn - (hv_Length1*hv_Cos)) - (hv_Length2*hv_Sin));
	hv_Row1 = 1.0*(hv_Row - (((-hv_Length1)*hv_Sin) + (hv_Length2*hv_Cos)));

	//���εڶ����˵�����
	hv_Col2 = 1.0*((hv_Coloumn + (hv_Length1*hv_Cos)) - (hv_Length2*hv_Sin));
	hv_Row2 = 1.0*(hv_Row - ((hv_Length1*hv_Sin) + (hv_Length2*hv_Cos)));

	//���ε������˵�����
	hv_Col3 = 1.0*((hv_Coloumn + (hv_Length1*hv_Cos)) + (hv_Length2*hv_Sin));
	hv_Row3 = 1.0*(hv_Row - ((hv_Length1*hv_Sin) - (hv_Length2*hv_Cos)));

	//���ε��ĸ��˵�����
	hv_Col4 = 1.0*((hv_Coloumn - (hv_Length1*hv_Cos)) + (hv_Length2*hv_Sin));
	hv_Row4 = 1.0*(hv_Row - (((-hv_Length1)*hv_Sin) - (hv_Length2*hv_Cos)));

	//������η�����ֱ�ߵı�����׼��
	hv_StdLineRow1 = hv_Row2;
	hv_StdLineCol1 = hv_Col2;
	hv_StdLineRow2 = hv_Row3;
	hv_StdLineCol2 = hv_Col3;

	//�������Length2����ֱ����Ϊrake���ߵ�ROI
	hv_ROILineRow1 = (hv_Row1 + hv_Row2)*0.5;
	hv_ROILineCol1 = (hv_Col1 + hv_Col2)*0.5;
	hv_ROILineRow2 = (hv_Row3 + hv_Row4)*0.5;
	hv_ROILineCol2 = (hv_Col3 + hv_Col4)*0.5;
	//gen_rectangle2_contour_xld (Rectangle, Row, Coloumn, Phi, Length1, Length2)
	rake(ho_Image, &ho_Regions1, (1.0*hv_Length2) * 2, hv_Length1 * 2, hv_DetectWidth,
		hv_Sigma, hv_Threshold, hv_Transition, hv_Select, hv_ROILineRow1, hv_ROILineCol1,
		hv_ROILineRow2, hv_ROILineCol2, &hv_ResultRows, &hv_ResultColumns);

	//�����б�Ե�㵽��׼�ߵľ��룬���������뼰���Ӧ�ı�Ե�����꣬��Ϊ����
	hv_Max = 0;
	if (0 != ((hv_ResultColumns.TupleLength()) > 0))
	{
		(*hv_EdgeRows) = hv_ResultRows;
		(*hv_EdgeColumns) = hv_ResultColumns;
		{
			HTuple end_val61 = (hv_ResultColumns.TupleLength()) - 1;
			HTuple step_val61 = 1;
			for (hv_i = 0; hv_i.Continue(end_val61, step_val61); hv_i += step_val61)
			{
				DistancePl(HTuple(hv_ResultRows[hv_i]), HTuple(hv_ResultColumns[hv_i]), hv_StdLineRow1,
					hv_StdLineCol1, hv_StdLineRow2, hv_StdLineCol2, &hv_Distance1);
				if (0 != (hv_Max < hv_Distance1))
				{
					hv_Max = hv_Distance1;
					(*hv_ResultRow) = HTuple(hv_ResultRows[hv_i]);
					(*hv_ResultColumn) = HTuple(hv_ResultColumns[hv_i]);
				}

			}
		}
	}
	return;
}



//������С����
//int iOne = atoi(XMLImg[nID][nStep].psLeaf[0].strMN);
//int iTwo = atoi(XMLImg[nID][nStep].psLeaf[1].strMN);
//HTuple hvNum, hvNO;
//HObject hoTwo, hoOne;
//CountObj(hoImage[nStep], &hvNum);
//int nNum = hvNum.I();
//if (nNum < 2 || iOne < 0 || iTwo < 0)
//	return false;
//if (iTwo >= nNum)
//{
//	iTwo = nNum - 1;
//}
//if (iOne >= nNum)
//{
//	iOne = nNum - 1;
//}
//if (iOne == iTwo)
//{
//	LogFile("ͬһ�������޷��������룡");
//	return false;
//}
//SelectObj(hoImage[nStep], &hoOne, iOne + 1);
//SelectObj(hoImage[nStep], &hoTwo, iTwo + 1);
//HTuple hvMinDist, hvRow1, hvCol1, hvRow2, hvCol2;
//DistanceRrMin(hoOne, hoTwo, &hvMinDist, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
//DispLine(HW.WndID, hvRow1, hvCol1, hvRow2, hvCol2);
//hvRes[nStep] = HTuple();
//hvRes[nStep][0] = hvMinDist;
//hvRes[nStep][1] = hvCol1;
//hvRes[nStep][2] = hvRow1;
//hvRes[nStep][3] = hvCol2;
//hvRes[nStep][4] = hvRow2;

//������ȡ
//CString strMethod = XMLImg[nID][nStep].psLeaf[0].strMN;                           //�õ����򷽷�
//CString strIndex = XMLImg[nID][nStep].psLeaf[1].strMN;
//int nInx = atoi(strIndex);
//if (nInx >= 0)
//{
//	HObject hoXLD;
//	GenContourRegionXld(hoImage[nStep], &hoXLD, strMethod.GetBuffer());
//	strMethod.ReleaseBuffer();
//	SelectObj(hoXLD, &hoImage[nStep + 1], nInx + 1);
//}
//else
//{
//	GenContourRegionXld(hoImage[nStep], &hoImage[nStep + 1], strMethod.GetBuffer());
//	strMethod.ReleaseBuffer();
//}

//��������
//CString strConsSel = XMLImg[nID][nStep].psLeaf[0].strMN;             //�õ����򷽷�
//CString strOne = XMLImg[nID][nStep].psLeaf[1].strMN;
//CString strTwo = XMLImg[nID][nStep].psLeaf[2].strMN;
//CString strMethod = XMLImg[nID][nStep].psLeaf[3].strMN;
//CString strDistType = XMLImg[nID][nStep].psLeaf[4].strMN;           //�������ͣ���С���룬���������룬Ĭ����С����
//int nDistType = 0;
//if (strDistType == "������")
//	nDistType = 1;
//CString strSel = XMLImg[nID][nStep].psLeaf[5].strMN;                    //ѡ����Сֵ���������ֵ
//HTuple hvNum;
//CountObj(hoImage[nStep], &hvNum);
//int nCons = hvNum.I();
//if (nCons < 2)                                                                                         //��С������0
//{
//	hvRes[nStep][0] = 0;
//	return false;
//}
//HObject hoOne, hoTwo;
//HTuple hvDistType[2];
//if (strConsSel == "��������")
//{
//	int m = 0;                                                //���ֵ������
//	double fDist = 10000;
//	for (int i = 0; i < nCons - 1; i++)
//	{
//		SelectObj(hoImage[nStep], &hoOne, i + 1);
//		for (int j = 0; j < nCons; j++)
//		{
//			if (i == j)
//				continue;
//			SelectObj(hoImage[nStep], &hoTwo, j + 1);
//			DistanceCc(hoOne, hoTwo, strMethod.GetBuffer(), &hvDistType[0], &hvDistType[1]);
//			strMethod.ReleaseBuffer();
//			if (strSel == "���ֵ")
//			{
//				if (fDist < hvDistType[nDistType])
//				{
//					fDist = hvDistType[nDistType].D();
//				}
//			}
//			else
//			{
//				if (fDist > hvDistType[nDistType])
//				{
//					fDist = hvDistType[nDistType].D();
//				}
//			}
//		}
//	}
//	hvRes[nStep][m] = fDist;
//	m++;
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//}
//else
//{
//	SelectObj(hoImage[nStep], &hoOne, atoi(strOne) + 1);
//	if (strTwo == "���ֵ")
//	{
//		HTuple hvNO;
//		CountObj(hoImage[nStep], &hvNO);
//		SelectObj(hoImage[nStep], &hoTwo, hvNO);
//	}
//	else
//		SelectObj(hoImage[nStep], &hoTwo, atoi(strTwo));
//	DistanceCc(hoOne, hoTwo, strMethod.GetBuffer(), &hvDistType[0], &hvDistType[1]);
//	strMethod.ReleaseBuffer();
//	double fDist = hvDistType[nDistType].D();   //��������
//	hvRes[nStep][0] = fDist;
//}

//NCC��λ
//GetImageSize(HW.hoImage, &hvW, &hvH);
//ZoomImageSize(HW.hoImage, &hoZoom, fZoom*hvW, fZoom*hvH, "constant"); //
//FindNccModel(hoZoom, hvRes[nStep - 1], fAngST, fAngRange, fScore, iFind, 0.5, "true", 0, &hvR, &hvC, &hvPhi, &hvScore);
//ClearNccModel(hvRes[nStep - 1]);
//int nResNum = hvScore.TupleLength().I();
//if (nResNum < 1)
//{
//	TRACE("\n û�м�⵽NCCģ�壡");
//	return false;
//}
//hvRes[nStep] = HTuple();                         //����ѹ�������飬ÿ��4�����
//for (int j = 0; j < nResNum; j++)
//{
//	hvRes[nStep][j * 5] = hvC[j] / fZoom;
//	hvRes[nStep][j * 5 + 1] = hvR[j] / fZoom;
//
//	hvRes[nStep][j * 5 + 2] = hvPhi[j];
//	hvRes[nStep][j * 5 + 3] = hvScore[j];
//	if (j < nResNum - 1)
//		hvRes[nStep][j * 5 + 4] = ",";
//	if (j == iSel || iSel < 0)  //ѡ���ģ�����߽�������ģ���
//		DispCross(HW.WndID, hvR[j] / fZoom, hvC[j] / fZoom, 96, 0.25*PI);
//}

//////////////////////////////////////////////////////////////////////////
//hvRes[nStep] = HTuple();
//double fLen[2] = { 0 };
//fLen[0] = atof(XMLImg[nID][nStep].psLeaf[3].strMN);
//fLen[1] = atof(XMLImg[nID][nStep].psLeaf[4].strMN);
//if (fLen[0] < 1 || fLen[1] < 1)
//	return false;
//HTuple hvR, hvC, hvPhi, hvLen1, hvLen2;
//double fRCVal[7];                                            //X,Y,Phi,L1,L2,Sigma,Thres			
//CStringArray ss;
//CString strMethod, strArrayDir;
//int nArraySum = 1, nArrayCircle = 1, iLnSel; //ѡ�������ı�
//int nMsrAngNum = 0;
//double fArrayStep[2];                                     //���в�����������������о���DX��DY�������Բ�����о�ֻ�в���1 �ǽǶȲ���
//for (int j = 0; j < nSumLeaf; j++)
//{
//	if (j < 7)
//	{
//		fRCVal[j] = atof(XMLImg[nID][nStep].psLeaf[j].strMN);
//		if (j < 2)
//			fRCVal[j] *= fZoom;
//	}
//	else if (j == 7)
//		strMethod.Format("%s", XMLImg[nID][nStep].psLeaf[j].strMN);
//	else if (j == 8)
//	{
//		iLnSel = atoi(XMLImg[nID][nStep].psLeaf[j].strMN);
//	}
//	else if (j == 9)
//	{
//		nArraySum = atoi(XMLImg[nID][nStep].psLeaf[j].strMN);
//	}
//	else if (j == 10)
//	{
//		nArrayCircle = atoi(XMLImg[nID][nStep].psLeaf[j].strMN);
//	}
//	else if (j == 11 || j == 12)
//	{
//		fArrayStep[j - 11] = atof(XMLImg[nID][nStep].psLeaf[j].strMN);
//	}
//	else if (j == 13)                                   //��Ӧ�Ƕ��Ż�������������
//		nMsrAngNum = atoi(XMLImg[nID][nStep].psLeaf[j].strMN);
//	else if (j == 14)
//		strArrayDir.Format("%s", XMLImg[nID][nStep].psLeaf[j].strMN);
//}
////////////////////////////��������ʼ��ʽ��ȡ��Ҫ��������/////////////////////////			
//HTuple hvMat2D, hvTransY, hvTransX, hvW, hvH;
//double fCenRC[2];
//GetAffineRect(fRCVal, atoi(XMLImg[nID][nStep].psLeaf[nSumLeaf - 1].strMN), hvRes[nStep - 1], hvTransX, hvTransY);
//fCenRC[0] = hvTransX.D();
//fCenRC[1] = hvTransY.D();
//GetImageSize(HW.hoImage, &hvW, &hvH);
//double fImageSize[2];
//fImageSize[0] = hvW.D();
//fImageSize[1] = hvH.D();
//int iRes = 0, iLnNum = 0;
//double fMsrPhi = hvRes[nStep - 1][2].D() + fRCVal[2];         //ģ����������ƫ��Ƕ� + ����Ƕ� + ���в����Ƕ�
//for (int j = 0; j < nArraySum; j++)
//{
//	HTuple hvMsr, hvY, hvX, hvAmp, hvDist, hvMaxAmp, Ys, Xs;
//	if (nArraySum > 1 && j > 0)				                                  //Ҫ�ı�ÿһ�������λ��
//	{
//		if (nArrayCircle)                              //Բ����
//		{
//			fMsrPhi += fArrayStep[0];
//			HTuple HomMat2D, HomMat2DRot, hvRow, hvCol;
//			HomMat2dIdentity(&HomMat2D);
//			HomMat2dRotate(HomMat2D, fArrayStep[0] * j, hvRes[nStep - 1][1], hvRes[nStep - 1][0], &HomMat2DRot); //��ת�˶���һ���ĽǶ�
//			AffineTransPoint2d(HomMat2DRot, hvTransY, hvTransX, &hvRow, &hvCol);
//			fCenRC[0] = hvCol.D();
//			fCenRC[1] = hvRow.D();
//		}
//		else                                                   //ֱ������
//		{
//			fCenRC[0] += fArrayStep[0];
//			fCenRC[1] += fArrayStep[1];
//		}
//	}
//	bool bGet = false;
//	double fDelta[2], fMaxAmpCen[2], fAbsAmp = fRCVal[6], fFitPhi, fRange[2] = { -PI, PI };
//	fDelta[0] = sin(fMsrPhi);
//	fDelta[1] = cos(fMsrPhi);
//	Xs = HTuple();
//	Ys = HTuple();
//	int n = 0;
//	fMaxAmpCen[0] = fCenRC[0];
//	fMaxAmpCen[1] = fCenRC[1];
//	for (int k = 0; k < nMsrAngNum && nArrayCircle; k++)
//	{
//		HTuple hvTmpX, hvTmpY;
//		if (strArrayDir == "˫��")
//		{
//			fDelta[0] = sin(fMsrPhi)*pow(-1, k);
//			fDelta[1] = cos(fMsrPhi)*pow(-1, k);
//		}
//		else if (strArrayDir == "X+" && fDelta[0] < 0 ||
//			strArrayDir == "X-" && fDelta[0] > 0 ||
//			strArrayDir == "Y+" && fDelta[1] < 0 ||
//			strArrayDir == "Y-" && fDelta[1] > 0)
//		{
//			fDelta[0] *= -1;
//			fDelta[1] *= -1;
//		}
//		double fMsrCen[2];
//		fMsrCen[0] = fCenRC[0] + fArrayStep[1] * fDelta[0];
//		fMsrCen[1] = fCenRC[1] + fArrayStep[1] * fDelta[1];
//		if (fMsrCen[1] < 0 || fMsrCen[1] > fImageSize[1] - 1 || fMsrCen[0] < 0 || fMsrCen[0] > fImageSize[0] - 1)
//			continue;
//		GenMeasureRectangle2(fMsrCen[1], fMsrCen[0], fMsrPhi, fRCVal[3], fRCVal[4], hvW, hvH, "nearest_neighbor", &hvMsr);
//		MeasurePos(HW.hoImage, hvMsr, fRCVal[5], fRCVal[6], strMethod.GetBuffer(0), "all", &hvTmpY, &hvTmpX, &hvAmp, &hvDist);
//		strMethod.ReleaseBuffer();
//		CloseMeasure(hvMsr);
//		if (hvTmpX.TupleLength().I() == 0)
//			continue;
//		if (ABS(hvAmp.D()) > fAbsAmp)
//		{
//			fMaxAmpCen[0] = hvTmpX[0].D();
//			fMaxAmpCen[1] = hvTmpY[0].D();
//			fAbsAmp = ABS(hvAmp.D());
//		}
//		Xs[n] = hvTmpX[0];
//		Ys[n] = hvTmpY[0];
//		n++;
//	}
//	if (nMsrAngNum > 1 && nArrayCircle)      //Բ���з���Ƕ��Ż�
//	{
//		bool bFitOK = pSng->GetFitLineAng(HW.WndID, Ys, Xs, fFitPhi, 6, false);
//		if (bFitOK == false)
//		{
//			LogFile("û���㹻��ϸ�Ŀ��Ż�������Ƕȵĵ㣡");
//			return false;
//		}
//		double fAngErr = 0.5;
//		for (int k = 0; k < 2; k++)
//		{
//			double fAng = pSng->GetVertAng(fFitPhi, fRange, k == 0);
//			if (ABS(fAng - fMsrPhi) < fAngErr || ABS(fAng - fMsrPhi) > 2.0* PI - fAngErr)
//			{
//				fMsrPhi = fAng;
//				break;
//			}
//		}
//	}
//	//////////////////////////������ȷ��Ե����////////////////////////////////////////
//	GenMeasureRectangle2(fMaxAmpCen[1], fMaxAmpCen[0], fMsrPhi, fRCVal[3], fRCVal[4], hvW, hvH, "nearest_neighbor", &hvMsr);
//	MeasurePos(HW.hoImage, hvMsr, fRCVal[5], max(fRCVal[6], 0.8*fAbsAmp), strMethod.GetBuffer(), "all", &hvY, &hvX, &hvAmp, &hvDist);
//	strMethod.ReleaseBuffer();
//	CloseMeasure(hvMsr);
//	n = hvY.TupleLength().I();
//	if (n == 0)
//	{
//		hvRes[nStep][iRes++] = 0;
//		hvRes[nStep][iRes++] = 0;
//		hvRes[nStep][iRes++] = 0;
//		hvRes[nStep][iRes++] = ";";
//		continue;
//	}
//	//DispRectangle2(m_HW.WndID, fMaxAmpCen[1], fMaxAmpCen[0], fMsrPhi, fRCVal[3], fRCVal[4]);  //ֻ���������˲���ʾ����
//	DispCross(HW.WndID, hvY, hvX, 36, 0);
//	iLnNum++;
//	if (iLnSel > n)            //Ҫʹ�õıߴ��� ȫ���������ı�
//		iLnSel = n - 1;
//	if (iLnSel >= 0)
//	{
//		hvRes[nStep][iRes++] = hvX[iLnSel].D();
//		hvRes[nStep][iRes++] = hvY[iLnSel].D();
//		hvRes[nStep][iRes++] = hvAmp[iLnSel].D();
//		hvRes[nStep][iRes++] = ";";
//		continue;
//	}
//	//iLnSel < 0 ��˵�����еı߶�������Ҫ��
//	for (int k = 0; k < n; k++)
//	{
//		hvRes[nStep][iRes++] = hvX[k].D();
//		hvRes[nStep][iRes++] = hvY[k].D();
//		hvRes[nStep][iRes++] = hvAmp[k].D();
//		if (k < n - 1)
//			hvRes[nStep][iRes++] = ",";
//	}
//	hvRes[nStep][iRes++] = ";";
//}

/////////////////////////�Ƕ�������///////////////////////////////
//hvRes[nStep] = HTuple();
//double fRCVal[5];  //X,Y,A,L1,L2
//for (int j = 0; j < 5; j++)
//{
//	fRCVal[j] = atof(XMLImg[nID][nStep].psLeaf[j].strMN);
//	if (j < 2)
//		fRCVal[j] *= fZoom;
//}
//HObject hoRC;
//HTuple hvTransX, hvTransY;
//GetAffineRect(fRCVal, atoi(XMLImg[nID][nStep].psLeaf[nSumLeaf - 1].strMN), hvRes[nStep - 1], hvTransX, hvTransY);
//GenRectangle2(&hoRC, hvTransY, hvTransX, fRCVal[2], fRCVal[3], fRCVal[4]);
//ReduceDomain(HW.hoImage, hoRC, &hoImage[nStep + 1]);
//DispObj(hoRC, HW.WndID);


//////////Բ�����򡢡���������������������������
//hvRes[nStep] = HTuple();
//double fRadius[2] = { 0 };
//fRadius[0] = atof(XMLImg[nID][nStep].psLeaf[2].strMN);
//fRadius[1] = atof(XMLImg[nID][nStep].psLeaf[3].strMN);
//double fRadiusVal[4];
//for (int j = 0; j < 4; j++)
//{
//	fRadiusVal[j] = atof(XMLImg[nID][nStep].psLeaf[j].strMN);
//	if (j < 2)
//		fRadiusVal[j] *= fZoom;
//}
//HObject hoOCir, hoICir, hoRing;
//HTuple hvTransX, hvTransY;
//GetAffineRect(fRadiusVal, atoi(XMLImg[nID][nStep].psLeaf[nSumLeaf - 1].strMN), hvRes[nStep - 1], hvTransX, hvTransY);
//GenCircle(&hoOCir, hvTransY, hvTransX, fRadiusVal[2]);
//GenCircle(&hoICir, hvTransY, hvTransX, fRadiusVal[3]);
//Difference(hoOCir, hoICir, &hoRing);
//ReduceDomain(HW.hoImage, hoRing, &hoImage[nStep + 1]);
//DispObj(hoRing, HW.WndID);

///���Բ//////////////////
//int iSel = 0;
//iSel = atoi(XMLImg[nID][nStep].psLeaf[0].strMN);
//int iRes = hvRes[nStep - 1].TupleLength().I();
//if (iRes < 3)
//{
//	TRACE("û���㹻�ĵ����Բ��");
//	return false;
//}
//CString strData = GetHTupleString(hvRes[nStep - 1]);
//CStringArray ss;
//pSng->SplitString(strData, ";", ss);		                    //ÿһ��������Ľ������
//HObject hoCons;
//HTuple hvRs, hvCs;
//int iAMT = 0;
//for (int j = 0; j < ss.GetCount(); j++)
//{
//	CStringArray Ds, Xs;
//	pSng->SplitString(ss[j], ",", Ds);
//	if (iSel >= Ds.GetCount() && Ds.GetCount() > 0)
//	{
//		iSel = Ds.GetUpperBound();                       //���һ��Ԫ��
//	}
//	pSng->SplitString(Ds[iSel], "*", Xs);
//	double fX, fY;
//	fX = atof(Xs[0]);
//	fY = atof(Xs[1]);
//	if (fX < ESP && fY < ESP)
//		continue;
//	hvRs[iAMT] = fY;
//	hvCs[iAMT] = fX;
//	iAMT++;
//}
//if (iAMT < 3)
//{
//	TRACE("û���㹻�ϸ��Բ���������Բ��");
//	return false;
//}
///////////////////////////////////////////���������Բ��,ǰ�ڵĵ��Ѿ��Ż����ˣ����Բ���Ҫ��һ������//////////////////////////////////////////////////////////////////
//HTuple hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_Row, hv_Col, hvAng;
//GenContourPolygonXld(&hoCons, hvRs, hvCs);
//FitCircleContourXld(hoCons, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Col, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
//hvRes[nStep] = HTuple();
//hvRes[nStep][0] = hv_Radius;
//hvRes[nStep][1] = hv_Col;
//hvRes[nStep][2] = hv_Row;
//double fCenR[3];
//fCenR[0] = hv_Col.D();
//fCenR[1] = hv_Row.D();
//fCenR[2] = hv_Radius.D();
//hvAng = HTuple();                      //�������������н�
//for (int j = 0; j < iAMT; j++)
//{
//	HTuple hvA;
//	AngleLx(hv_Row, hv_Col, hvRs[j], hvCs[j], &hvA);
//	hvAng[j] = hvA;
//}
//double fMaxAng = 0;
//double fMaxDir;
//for (int j = 0; j < iAMT; j++)
//{
//	double fDelta = hvAng[(j + 1) % iAMT].D() - hvAng[j].D();
//	if (fDelta < -1.0* PI)
//		fDelta += 2.0*PI;
//	else if (fDelta > PI)
//		fDelta -= 2.0*PI;
//	if (abs(fDelta) > fMaxAng)
//	{
//		fMaxAng = abs(fDelta);
//		fMaxDir = 0.5*(hvAng[(j + 1) % iAMT].D() + hvAng[j].D());  //360��
//	}
//}
//DispCircle(HW.WndID, hv_Row, hv_Col, hv_Radius);
//double fDX, fDY;
//fDX = cos(fMaxDir);
//fDY = sin(fMaxDir);
//DispLine(HW.WndID, hv_Row - fDY*hv_Radius, hv_Col - fDX*hv_Radius, hv_Row + fDY*hv_Radius, hv_Col + fDX*hv_Radius);


////���ֱ��/////
//int iSel = atoi(XMLImg[nID][nStep].psLeaf[0].strMN);
//int iRes = hvRes[nStep - 1].TupleLength().I();
//if (iRes < 5)
//{
//	TRACE("û���㹻�ĵ����ֱ�ߣ�");
//	return false;
//}
//CString strData = GetHTupleString(hvRes[nStep - 1]);
//CStringArray ss;
//pSng->SplitString(strData, ";", ss);		              //ÿһ��������Ľ������
//HObject hoCons;
//HTuple hvRs, hvCs;
//int iAMT = 0;
//for (int j = 0; j < ss.GetCount(); j++)
//{
//	CStringArray Ds, Xs;
//	pSng->SplitString(ss[j], ",", Ds);
//	if (iSel >= Ds.GetCount())
//		iSel = Ds.GetUpperBound();                       //���һ��Ԫ��
//	pSng->SplitString(Ds[iSel], "*", Xs);
//	double fX, fY;
//	fX = atof(Xs[0]);
//	fY = atof(Xs[1]);
//	if (fX < ESP && fY < ESP)
//		continue;
//	hvRs[iAMT] = fY;
//	hvCs[iAMT] = fX;
//	iAMT++;
//}
//if (iAMT < 3)
//{
//	TRACE("û���㹻�ϸ�ĵ����Բ��");
//	return false;
//}
//HTuple hvRowA, hvColA, hvRowB, hvColB;
//bool bRet = pSng->GetFitLine(HW.WndID, hvRs, hvCs, hvRowA, hvColA, hvRowB, hvColB);
//if (bRet == false)
//{
//	TRACE("ֱ�����ʧ�ܣ�");
//	return false;
//}
//TRACE("\n(%.1f,%.1f)---(%.1f,%.1f)", hvColA.D(), hvRowA.D(), hvColB.D(), hvRowB.D()),
//hvRes[nStep] = HTuple();
//hvRes[nStep][0] = hvColA;
//hvRes[nStep][1] = hvRowA;
//hvRes[nStep][2] = hvColB;
//hvRes[nStep][3] = hvRowB;


//////�㴦��
//CStringArray stsCof, stsInx;
//pSng->SplitString(XMLImg[nID][nStep].psLeaf[0].strMN, ",", stsInx);
//pSng->SplitString(XMLImg[nID][nStep].psLeaf[1].strMN, ",", stsCof);
//int nGrps = stsCof.GetCount();                                                                  //ÿ���ȡ����
//CString strData = GetHTupleString(hvRes[nStep - 1]);                            //�㴦��ֻ���ǽ����Ų���������
/////////���ϴ�������˵����X*Y*Thres,X*Y*Thres; ���ָ�ʽ/////////////////////////////
//CStringArray ss;
//pSng->SplitString(strData, ";", ss);		                                                      //ÿһ��������Ľ������
//HTuple hvRs, hvCs;
//hvRs = HTuple();
//hvCs = HTuple();
//int iNO = 0;
//for (int j = 0; j < ss.GetCount(); j++)
//{
//	CStringArray Ds, XYs;
//	pSng->SplitString(ss[j], ",", Ds);
//	if (Ds.GetCount() < nGrps)
//		continue;
//	hvRs[iNO] = 0;
//	hvCs[iNO] = 0;
//	for (int k = 0; k < Ds.GetCount(); k++)
//	{
//		int iSel = atoi(stsInx[k]);
//		if (iSel >= Ds.GetCount() && Ds.GetCount() > 0)
//		{
//			iSel = Ds.GetUpperBound();                       //���һ��Ԫ��
//		}
//		pSng->SplitString(Ds[iSel], "*", XYs);
//		double fX, fY;
//		fX = atof(XYs[0]);
//		fY = atof(XYs[1]);
//		if (fX < ESP && fY < ESP)
//			continue;
//		double fCoef = atof(stsCof[k]);
//		hvRs[iNO] += fY*fCoef;
//		hvCs[iNO] += fX*fCoef;
//	}
//	iNO++;
//}
//if (iNO == 0)
//return false;
////////////////////////��XYƽ���������,���浽���������///////////////////////////////////
//DispCross(HW.WndID, hvRs, hvCs, 36, 0);
//hvRes[nStep] = HTuple();
//int iRes = 0;
//for (int j = 0; j < iNO; j++)
//{
//	hvRes[nStep][iRes++] = hvCs[j];
//	hvRes[nStep][iRes++] = hvRs[j];
//	hvRes[nStep][iRes++] = ";";
//}

////////////////////////�ҶȺͷ���////////////////////////////
//HTuple hvMean, hvDev;
//Intensity(hoImage[nStep], HW.hoImage, &hvMean, &hvDev);
//hvRes[nStep] = HTuple();
//if (XMLImg[nID][nStep].psLeaf[0].strMN == "�Ҷ�")
//{
//	hvRes[nStep][0] = hvMean;
//}
//else if (XMLImg[nID][nStep].psLeaf[1].strMN == "����")
//{
//	hvRes[nStep][0] = hvDev;
//}
//else
//{
//	hvRes[nStep][0] = hvMean;
//	hvRes[nStep][1] = hvDev;
//}

////////////////////////////////ֱ�ߵ��������롢��������������������������
//int iLNO = atoi(XMLImg[nID][nStep].psLeaf[0].strMN);
//int iCNO = atoi(XMLImg[nID][nStep].psLeaf[1].strMN);
//CString strType = XMLImg[nID][nStep].psLeaf[2].strMN;
//if (nStep + iCNO < 0 || nStep + iLNO < 0)
//	return false;
//HTuple hvDist[2];
//double fPtVal[4];
//for (int i = 0; i < 4; i++)
//	fPtVal[i] = hvRes[nStep + iLNO][i].D();
//DistanceLc(hoImage[nStep + iCNO], hvRes[nStep + iLNO][1], hvRes[nStep + iLNO][0],
//	hvRes[nStep + iLNO][3], hvRes[nStep + iLNO][2],
//	&hvDist[0], &hvDist[1]);
//double fDistVal;
//int nType = 0;
//if (strType == "���ֵ")
//{
//	fDistVal = hvDist[1].D();
//	nType = 1;
//}
//else
//{
//	fDistVal = hvDist[0].D();
//	nType = 0;
//}
//HObject hoSplit, hoObj;
//SegmentContoursXld(hoImage[nStep + iCNO], &hoSplit, "lines_circles", 7, 5, 2);
//HTuple hvNum, hvPrjY, hvPrjX, hvYs, hvXs, hvTmpDist;
//CountObj(hoSplit, &hvNum);
//int nCons = hvNum.I();
//hvRes[nStep] = HTuple();
//for (int i = 0; i < nCons; i++)
//{
//	SelectObj(hoSplit, &hoObj, i + 1);
//	DistanceLc(hoObj, hvRes[nStep + iLNO][1], hvRes[nStep + iLNO][0],
//		hvRes[nStep + iLNO][3], hvRes[nStep + iLNO][2],
//		&hvDist[0], &hvDist[1]);
//	if (abs(fDistVal - hvDist[nType].D()) > 2)
//		continue;
//	GetContourXld(hoObj, &hvYs, &hvXs);
//	int nPts = hvXs.TupleLength().I();
//	int nAdd = nPts / 20;
//	if (nAdd == 0)
//		nAdd = 1;
//	CPos psClose;
//	psClose.fVec = 10000;
//	for (int j = 0; j < nPts; j += nAdd)
//	{
//		double fPt[2] = { 0 };
//		int II = 0;
//		for (int k = 0; k < nAdd; k++)
//		{
//			if (j + k == nPts)
//				break;
//			fPt[0] += hvXs[j + k].D();
//			fPt[1] += hvYs[j + k].D();
//			II++;
//		}
//		fPt[0] /= II;
//		fPt[1] /= II;
//		DistancePl(fPt[1], fPt[0], hvRes[nStep + iLNO][1], hvRes[nStep + iLNO][0],
//			hvRes[nStep + iLNO][3], hvRes[nStep + iLNO][2], &hvTmpDist);
//		double fCloseVal = hvTmpDist.D() - fDistVal;
//		if (fCloseVal < 3)
//		{
//			psClose.x = fPt[0];
//			psClose.y = fPt[1];
//			break;
//		}
//		if (fCloseVal < psClose.fVec)
//		{
//			psClose.x = fPt[0];
//			psClose.y = fPt[1];
//			psClose.fVec = fCloseVal;
//		}
//	}
//	ProjectionPl(psClose.y, psClose.x, hvRes[nStep + iLNO][1], hvRes[nStep + iLNO][0],
//		hvRes[nStep + iLNO][3], hvRes[nStep + iLNO][2], &hvPrjY, &hvPrjX);
//	DispLine(HW.WndID, psClose.y, psClose.x, hvPrjY, hvPrjX);
//	hvRes[nStep][0] = hvDist[nType];
//	hvRes[nStep][1] = psClose.x;
//	hvRes[nStep][2] = psClose.y;
//	hvRes[nStep][3] = hvPrjX.D();
//	hvRes[nStep][4] = hvPrjY.D();
//	break;
//}

///////�������
//int iLNO = nStep + atoi(XMLImg[nID][nStep].psLeaf[0].strMN);
//double fCen[2][2], fLen[2];
//fCen[0][0] = 0.5*(hvRes[iLNO][0].D() + hvRes[iLNO][2].D());
//fCen[0][1] = 0.5*(hvRes[iLNO][1].D() + hvRes[iLNO][3].D());
//fLen[0] = atof(XMLImg[nID][nStep].psLeaf[2].strMN);
//fLen[1] = atof(XMLImg[nID][nStep].psLeaf[3].strMN);
//HTuple hvPhi, hvH, hvW;
//GetImageSize(HW.hoImage, &hvW, &hvH);
//AngleLx(hvRes[iLNO][1], hvRes[iLNO][0], hvRes[iLNO][3], hvRes[iLNO][2], &hvPhi);
//double fPhi = hvPhi.D();
//if (fPhi > 0.5*PI)
//fPhi -= PI;
//else if (fPhi < -0.5*PI)
//	fPhi += PI;
//////////////////////////////////////////////////////
//CStringArray stsOffset;
//pSng->SplitString(XMLImg[nID][nStep].psLeaf[1].strMN, ",", stsOffset);
//if (stsOffset.GetCount() != 3)
//{
//	LogFile("��������������");
//	return false;
//}
//double fOffset[3];
//fOffset[0] = atof(stsOffset[0]);
//fOffset[1] = atof(stsOffset[1]);
//fOffset[2] = atof(stsOffset[2]);
//fCen[1][0] = fCen[0][0] + fOffset[0];
//fCen[1][1] = fCen[0][1] + fOffset[1];
//if (fCen[1][0] > hvW.D() - 10 || fCen[1][0] < 10 || fCen[1][1] > hvH.D() - 10 || fCen[1][1] < 10)
//{
//	return false;
//}
//HObject hoRC;
//GenRectangle2(&hoRC, fCen[1][1], fCen[1][0], fPhi + fOffset[2], fLen[0], fLen[1]);
//ReduceDomain(HW.hoImage, hoRC, &hoImage[nStep + 1]);
//if (atoi(XMLImg[nID][nStep].psLeaf[nSumLeaf - 1].strMN) == 1)
//{
//	CopyImage(hoImage[nStep + 1], &HW.hoImage);    //���µ�ǰ��ͼ��
//}
//DispObj(hoRC, HW.WndID);
