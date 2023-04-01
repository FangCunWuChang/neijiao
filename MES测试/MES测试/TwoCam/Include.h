#if !defined(_INCLUDE_H_)
#define _INCLUDE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef PUBLIC_EXPORT
#define MONTION_CONTROL_EXPORT AFX_EXT_CLASS
#else
#define MONTION_CONTROL_EXPORT AFX_CLASS_IMPORT
#endif

enum MONTION_CONTROL_EXPORT SoftElemType
{
	//AM600
	ELEM_QX = 0,     //QX元件
	ELEM_MW = 1,     //MW元件
	ELEM_X = 2,		 //X元件(对应QX200~QX300)
	ELEM_Y = 3,		 //Y元件(对应QX300~QX400)

	//H3U
	REGI_H3U_Y    = 0x20,       //Y元件的定义	
	REGI_H3U_X    = 0x21,		//X元件的定义							
	REGI_H3U_S    = 0x22,		//S元件的定义				
	REGI_H3U_M    = 0x23,		//M元件的定义							
	REGI_H3U_TB   = 0x24,		//T位元件的定义				
	REGI_H3U_TW   = 0x25,		//T字元件的定义				
	REGI_H3U_CB   = 0x26,		//C位元件的定义				
	REGI_H3U_CW   = 0x27,		//C字元件的定义				
	REGI_H3U_DW   = 0x28,		//D字元件的定义				
	REGI_H3U_CW2  = 0x29,	    //C双字元件的定义
	REGI_H3U_SM   = 0x2a,		//SM
	REGI_H3U_SD   = 0x2b,		//
	REGI_H3U_R    = 0x2c		//
};


struct H3uAddrTypeInfo
{
	DWORD dwAddrStart;		//起始地址
	DWORD dwAddrPlc;		//起始地址对应的PLC地址
	DWORD dwAddrLen;		//地址总的个数
	DWORD dwAddrType;		//地址类型代表的字节数, 0位类型，2：WORD型，4：双字型
};

static H3uAddrTypeInfo g_dwH3uReadAddr[]={
	{0, 0xfc00, 255, 0},		//Y元件的定义
	{0, 0xf800, 255, 0},		//X元件的定义
	{0, 0xe000, 4095, 0},		//S元件的定义
	{0, 0x000, 8511, 0},		//M元件的定义
	{0, 0xf000, 511, 0},		//T位元件的定义
	{0, 0xf000, 511, 2},		//T字元件的定义
	{0, 0xf400, 255, 0},		//C位元件的定义	
	{0, 0xf400, 199, 2},		//C字元件的定义
	{0, 0x0000, 8511, 2},		//D字元件的定义	
	{200, 0xf700, 55, 4},		//C双字元件的定义
	{0, 0x2400, 1023, 0},		//SM	
	{0, 0x2400, 1023, 2},		//SD
	{0, 0x3000, 32767, 2},		//R元件的定义
	{0, 0, 0,	 0},
};

static H3uAddrTypeInfo g_dwAm600ReadAddr[]={
	{0, 0, 81917, 0},		//qx
	{0, 0, 65535, 2},		//mw
	{0, 1600, 99*8+7, 0},		//x
	{0, 2400, 99*8+7, 0},		//y
	{0, 0, 0,	 0},
};

static H3uAddrTypeInfo g_dwH3uBitWriteAddr[]={
	{0, 0xfc00, 255, 0},		//Y元件的定义
	{0, 0xf800, 255, 0},		//X元件的定义
	{0, 0xe000, 4095, 0},		//S元件的定义
	{0, 0x000, 8511, 0},		//M元件的定义
	{0, 0xf000, 511, 0},		//T位元件的定义
	{0, 0xf000, 511, 2},		//T字元件的定义
	{0, 0xf400, 255, 0},		//C位元件的定义	
	{0, 0xf400, 199, 2},		//C字元件的定义
	{0, 0x0000, 8511, 2},		//D字元件的定义	
	{200, 0xf700, 55, 4},		//C双字元件的定义
	{0, 0x2400, 1023, 0},		//SM	
	{0, 0x2400, 1023, 2},		//SD
	{0, 0x3000, 32767, 2},		//R元件的定义
	{0, 0, 0,	 0},
};

static H3uAddrTypeInfo g_dwWiteBitAddr_H3u[]={
	{5000, 10, 0},		
	{5100, 10, 0},		
	{5200, 10, 0},		
	{5300, 10, 0},		
	{5400, 10, 0},		
	{5500, 10, 0},		
	{5600, 10, 0},		
	{5700, 10, 0},		
	//{5800, 10, 0},		
	//{5900, 10, 0},		
	{6100, 10, 0},		
	{6200, 10, 0},		
	{6300, 10, 0},		
	{6400, 10, 0},		
	//{6500, 10, 0},	
	{0, 0,	 0},
};


#define AXIS_NUM_ADDR 1000

#define MODBUSTCP_RD_COIL_MAX                       1968                        //读线圈最大数量
#define MODBUSTCP_WR_COIL_MAX                       1936                        //写线圈最大数量
#define MODBUSTCP_RD_REG_MAX                        123                         //读寄存器最大数量
#define MODBUSTCP_WR_REG_MAX                        121                         //写寄存器最大数量


#endif	//	#if !defined(_INCLUDE_H_)