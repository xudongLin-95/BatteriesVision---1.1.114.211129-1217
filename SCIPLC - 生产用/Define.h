#pragma once
#ifndef P_DEFINE_P
#define P_DEFINE_P
#include "stdafx.h"

enum ErrorType//产品缺陷类型
{
	OK_PRODUCT,              //产品缺陷名称 OK合格，ERROR有缺陷
	ERROR_WIDTH_COAT,
	ERROR_HEIGHT_COAT,
	ERROR_MHEIGHT_COAT,
	ERROR_DISTANCE,
	ERROR_ANGLE_ERROR,
	ERROR_WIDTH_FOIL,
	ERROR_HEIGHT_FOIL,
	ERROR_MISMATCH,
};

struct s_ProductParam//产品检测相关参数
{
	double    dCoatWidthMax;
	double    dCoatWidthMin;
	double    dCoatWidthOffset;//补偿

	double    dCoatHeightMin;
	double    dCoatHeightMax;
	double    dCoatHeightOffset;//补偿

	double    dCoatMiddleHeightMin;//中间料高
	double    dCoatMiddleHeightMax;//中间料高上限
	double    dCoatMiddleHeightOffset;//补偿

	double    dFoilWidthMax;
	double    dFoilWidthMin;
	double    dFoilWidthOffset;//补偿

	double    dFoilHeightMin;
	double    dFoilHeightMax;
	double    dFoilHeightOffset;//补偿

	double    dAngleMax;
	double    dAngleMin;
	double    dAngleOffset;//补偿
	double    dDistaceMax;
	double    dDistanceMin;
	double    dDistanceOffset;//补偿

	double    dMisMatchMin;//定位不齐最小值
	double    dMisMatchMax;//定位不齐最大值
	double    dMisMatchOffset;//补偿
};
typedef struct  tagTRESULTCONTAINER
{
	int nErrorCode;//错误码
	DETECTOUTPUT1 tDetectOut;//处理信息
	HObject hoImageRectify;//矫正后的图像
	HObject hoGrabImageFirst;//相机0图像
	HObject hoGrabImageSecond;//相机1图像
	HObject hoGrabImageThird;//相机2图像
	UINT   nImageSN;
	HTuple hvTime;
	tagTRESULTCONTAINER::tagTRESULTCONTAINER()//构造函数
	{
		try
		{
			nErrorCode = OK_PRODUCT;
			GenEmptyObj(&hoGrabImageFirst);
			GenEmptyObj(&hoGrabImageSecond);
			GenEmptyObj(&hoGrabImageThird);
			nImageSN = 0;
			hvTime = 0;
			GenEmptyObj(&tDetectOut.hoDispRegion);
			tDetectOut.hvAngle.Append(0);
			TupleGenConst(2, 0.0, &tDetectOut.hvDistance);
			TupleGenConst(3, 0.0, &tDetectOut.hvHeight);
			TupleGenConst(2, 0.0, &tDetectOut.hvWidth);
			GenEmptyObj(&hoImageRectify);
		}
		catch (HException& e)
		{
			CString strMessageBox;
			strMessageBox.Format("运行许可已到期，请联系供应商%s", e.ErrorMessage().Text());
			AfxMessageBox(strMessageBox);
			return;
		}

	}
	void copy(const tagTRESULTCONTAINER& tRes)
	{
		try {
			HTuple Number;
			CountObj(tRes.tDetectOut.hoDispRegion, &Number);
			nErrorCode = tRes.nErrorCode;
			CopyObj(tRes.tDetectOut.hoDispRegion, &tDetectOut.hoDispRegion, 1, Number);
			tDetectOut.hvAngle = tRes.tDetectOut.hvAngle;
			tDetectOut.hvDistance = tRes.tDetectOut.hvDistance;
			tDetectOut.hvHeight = tRes.tDetectOut.hvHeight;
			tDetectOut.hvWidth = tRes.tDetectOut.hvWidth;
			CopyImage(tRes.hoGrabImageFirst, &hoGrabImageFirst);
			CopyImage(tRes.hoGrabImageSecond, &hoGrabImageSecond);
			CopyImage(tRes.hoGrabImageThird, &hoGrabImageThird);

			//copy_image(tRes.hoImageRectify,&hoImageRectify);
		}
		catch (HException&e)
		{
			AfxMessageBox(e.ErrorMessage().Text());
		}
	}
}RESULTCONTAINER, *PRESULTCONTAINER;

struct s_WriteDisk
{
	int nSaveType;//存图类型,0 不存图,1 保存全部，2 只存错误图
	BOOL bImage;//是否存图
	BOOL bImageAll;//写入全部图片
	BOOL bErrorImage;//写入错误图片
};

struct s_History
{
	int nCount;//保留的历史图片的个数
	int nType;//0 不存记录,1 保存全部记录，2 只存错误记录
};

struct s_ContinuousErrorAlarms
{
	BOOL bEffective;//是否生效
	int  nMAXContinuousError;//最大错误个数
	int  nCurrentContinuousError;//当前连续错误个数
	int nImageErrorType;//图像报错提醒类型，0不显示，1显示
	int nDistanceErrorType;//距离报错提醒类型，0不显示，1显示
};
struct s_Count
{
	int nTotal;//全部片数
	int nErrorTotal;//错误总数
	int nTypeCount;//错误种数
	std::vector<int>vecErrorCount;//每种错误数量的统计
};

//定义数据字符0~9  
#define   VK_0  0x30   
#define   VK_1  0x31   
#define   VK_2  0x32   
#define   VK_3  0x33   
#define   VK_4  0x34   
#define   VK_5  0x35   
#define   VK_6  0x36   
#define   VK_7  0x37   
#define   VK_8  0x38   
#define   VK_9  0x39  

//定义数据字符A~Z  
#define   VK_A  0x41   
#define   VK_B  0x42   
#define   VK_C  0x43   
#define   VK_D  0x44   
#define   VK_E  0x45   
#define   VK_F  0x46   
#define   VK_G  0x47   
#define   VK_H  0x48   
#define   VK_I  0x49   
#define   VK_J  0x4A   
#define   VK_K  0x4B   
#define   VK_L  0x4C   
#define   VK_M  0x4D   
#define   VK_N  0x4E   
#define   VK_O  0x4F   
#define   VK_P  0x50   
#define   VK_Q  0x51   
#define   VK_R  0x52   
#define   VK_S  0x53   
#define   VK_T  0x54   
#define   VK_U  0x55   
#define   VK_V  0x56   
#define   VK_W  0x57   
#define   VK_X  0x58   
#define   VK_Y  0x59   
#define   VK_Z  0x5A   

//定义数据字符a~z  
#define   VK_a  0x61   
#define   VK_b  0x62   
#define   VK_c  0x63   
#define   VK_d  0x64   
#define   VK_e  0x65   
#define   VK_f  0x66   
#define   VK_g  0x67   
#define   VK_h  0x68   
#define   VK_i  0x69   
#define   VK_j  0x6A   
#define   VK_k  0x6B   
#define   VK_l  0x6C   
#define   VK_m  0x6D   
#define   VK_n  0x6E   
#define   VK_o  0x6F   
#define   VK_p  0x70   
#define   VK_q  0x71   
#define   VK_r  0x72   
#define   VK_s  0x73   
#define   VK_t  0x74   
#define   VK_u  0x75   
#define   VK_v  0x76   
#define   VK_w  0x77   
#define   VK_x  0x78   
#define   VK_y  0x79   
#define   VK_z  0x7A

#endif //P_DEFINE_P






typedef struct tagTA_GRABWORKSHOPOUTPUT//采集车间产出
{
	HObject hoGrabImage;//采集到的图像
	HTuple  hvGrabberSN;//相机序号
	HTuple  hvImageSN;//图像号
	HTuple hvTimeStamp;//时间戳
#pragma region "读写消息"
	inline int ReadMessage(const HTuple& hvMessage, const int& nGrabberSN)//读取带序号的消息
	{
		CString strKey;
		try
		{

			strKey.Format("A%d_IMAGESN", nGrabberSN);
			GetMessageTuple(hvMessage, HTuple(strKey), &hvImageSN);//
		}
		catch (HException& e)
		{
			AfxMessageBox("消息队列出错");

		}

		strKey.Format("A%d_GRABBERSN", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &hvGrabberSN);//
		strKey.Format("A%d_TIMESTAMP", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &hvTimeStamp);//
		strKey.Format("A%d_IMAGE", nGrabberSN);
		GetMessageObj(&hoGrabImage, hvMessage, HTuple(strKey));//
		return TRUE;

	}

	inline int WriteMessage(const HTuple& hvMessage, const int& nGrabberSN)
	{
		CString strKey;
		strKey.Format("A%d_IMAGESN", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), hvImageSN);//
		strKey.Format("A%d_GRABBERSN", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), hvGrabberSN);//
		strKey.Format("A%d_TIMESTAMP", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), hvTimeStamp);//
		strKey.Format("A%d_IMAGE", nGrabberSN);
		SetMessageObj(hoGrabImage, hvMessage, HTuple(strKey));//
		return TRUE;
	}

#pragma endregion
}A_GRABWORKSHOPOUTPUT, *PA_GRABWORKSHOPOUTPUT;

typedef struct tagTB_DETECTWORKSHOPOUTPUT//检测车间产出
{
	A_GRABWORKSHOPOUTPUT A;
	DETECTOUTPUT1  tDetectOuput;
	HTuple  hvTimeStamp;//时间戳
#pragma region "读写消息"
	inline int ReadMessage(const HTuple& hvMessage, const int& nGrabberSN)
	{
		A.ReadMessage(hvMessage, nGrabberSN);
		CString strKey;
		strKey.Format("B%d_WIDTH", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &tDetectOuput.hvWidth);
		strKey.Format("B%d_HEIGHT", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &tDetectOuput.hvHeight);
		strKey.Format("B%d_ANGLE", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &tDetectOuput.hvAngle);
		strKey.Format("B%d_DISTANCE", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &tDetectOuput.hvDistance);
		strKey.Format("B%d_DISPREGION", nGrabberSN);
		GetMessageObj(&tDetectOuput.hoDispRegion, hvMessage, HTuple(strKey));
		strKey.Format("B%d_TIMESTAMP", nGrabberSN);
		GetMessageTuple(hvMessage, HTuple(strKey), &hvTimeStamp);
		return TRUE;
	}
	inline int WriteMessage(const HTuple& hvMessage, const int& nGrabberSN)
	{
		A.WriteMessage(hvMessage, nGrabberSN);
		CString strKey;
		strKey.Format("B%d_WIDTH", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), tDetectOuput.hvWidth);
		strKey.Format("B%d_HEIGHT", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), tDetectOuput.hvHeight);
		strKey.Format("B%d_ANGLE", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), tDetectOuput.hvAngle);
		strKey.Format("B%d_DISTANCE", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), tDetectOuput.hvDistance);
		strKey.Format("B%d_DISPREGION", nGrabberSN);
		SetMessageObj(tDetectOuput.hoDispRegion, hvMessage, HTuple(strKey));
		strKey.Format("B%d_TIMESTAMP", nGrabberSN);
		SetMessageTuple(hvMessage, HTuple(strKey), hvTimeStamp);
		return TRUE;
	}
#pragma endregion
}B_DETECTWORKSHOPOUTPUT, *PB_DETECTWORKSHOPOUTPUT;
typedef struct tagTC_ALLOCATORWORKSHOPOUTPUT
{
	B_DETECTWORKSHOPOUTPUT B[3];//3个检测车间产出
	HTuple hvPackageSN;//第几个数据包
	HTuple hvPackageType;//包类型,包含两个图像的信息还是单个图像的信息 0为0相机,1为1相机,2为2相机,3为3个相机的综合
	HTuple hvTimeStamp;//时间戳
#pragma region "读写消息"
	inline int ReadMessage(const HTuple& hvMessage)
	{
		CString strKey;
		strKey.Format("PACKAGESN");
		GetMessageTuple(hvMessage, HTuple(strKey), &hvPackageSN);
		strKey.Format("PACKAGETYPE");
		GetMessageTuple(hvMessage, HTuple(strKey), &hvPackageType);
		strKey.Format("C_TIMESTAMP");
		GetMessageTuple(hvMessage, HTuple(strKey), &hvTimeStamp);

#pragma region MyRegion
		//CString strKey;
		//strKey.Format("PACKAGESN");
		//GetMessageTuple(hvMessage, HTuple(strKey), &hvPackageSN);
		//strKey.Format("PACKAGETYPE");
		//GetMessageTuple(hvMessage, HTuple(strKey), &hvPackageType);
		//strKey.Format("C_TIMESTAMP");
		//GetMessageTuple(hvMessage, HTuple(strKey), &hvTimeStamp);
#pragma endregion

		if (0 == hvPackageType || 3 == hvPackageType)
		{
			B[0].ReadMessage(hvMessage, 0);
		}
		if (1 == hvPackageType || 3 == hvPackageType)
		{
			B[1].ReadMessage(hvMessage, 1);
		}
		if (2 == hvPackageType || 3 == hvPackageType)
		{
			B[2].ReadMessage(hvMessage, 2);
		}
		return TRUE;
	}
	inline int WriteMessage(const HTuple& hvMessage)
	{
		CString strKey;
		strKey.Format("PACKAGESN");
		SetMessageTuple(hvMessage, HTuple(strKey), hvPackageSN);
		strKey.Format("PACKAGETYPE");
		SetMessageTuple(hvMessage, HTuple(strKey), hvPackageType);
		strKey.Format("C_TIMESTAMP");
		SetMessageTuple(hvMessage, HTuple(strKey), hvTimeStamp);
		if (0 == hvPackageType || 3 == hvPackageType)
		{
			B[0].WriteMessage(hvMessage, 0);
		}
		if (1 == hvPackageType || 3 == hvPackageType)
		{
			B[1].WriteMessage(hvMessage, 1);
		}
		if (2 == hvPackageType || 3 == hvPackageType)
		{
			B[2].WriteMessage(hvMessage, 2);
		}
		return TRUE;
	}
#pragma endregion
}C_ALLOCATORWORKSHOPOUTPUT, *PC_ALLOCATORWORKSHOPOUTPUT;
