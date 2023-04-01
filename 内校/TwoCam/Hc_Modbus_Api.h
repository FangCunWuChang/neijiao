#if !defined(_HC_MODBUS_API_H_)
#define _HC_MODBUS_API_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Include.h"
#include <string>
using namespace std;
void strcount(string & s);



#ifdef PUBLIC_EXPORT
#define STANDARD_MODBUS_FUNC_EXPORT __declspec( dllexport )
#else
#define STANDARD_MODBUS_FUNC_EXPORT __declspec( dllimport )
#endif



/******************************************************************************
 1.功能描述 :创建网络连接	                  
 2.返 回 值 :TRUE 成功  FALSE 失败
 3.参    数 : IpAddr:以太网IP地址，
              nNetId:网络链接编号,用于标记是第几条网络链接,取值范围0~255,默认0 
			  IpPort:以太网端口号,默认502(modbusTcp协议默认端口号为502)
 4.注意事项 : 
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT BOOL Init_ETH(DWORD IpAddr,int nNetId = 0,int IpPort = 502);


/******************************************************************************
 1.功能描述 :创建网络连接	                  
 2.返 回 值 :TRUE 成功  FALSE 失败
 3.参    数 : sIpAddr:以太网IP地址，
              nNetId:网络链接编号,用于标记是第几条网络链接,取值范围0~255,默认0 
			  IpPort:以太网端口号,默认502(modbusTcp协议默认端口号为502)
 4.注意事项 : 
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT BOOL Init_ETH_String(char* pIpAddr,int nNetId = 0,int IpPort = 502);


/******************************************************************************
 1.功能描述 :关闭网络连接                  
 2.返 回 值 :TRUE 成功  FALSE 失败
 3.参    数 : nNetId:网络链接编号,与Init_ETH（）调用的ID一样
 4.注意事项 : 
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT BOOL Exit_ETH(int nNetId = 0 );


/******************************************************************************
 1.功能描述 :设置超时时间，            
 2.返 回 值 :TRUE 成功  FALSE 失败
 3.参    数 : nNetId:网络链接编号,与Init_ETH（）调用的ID一样
 4.注意事项 : 在建立连接前调用，如果建立连接前没有调用，默认超时时间为500ms
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT BOOL Set_ETH_TimeOut(int nTime,int nNetId = 0);


/******************************************************************************
 1.功能描述 :设置延时时间           
 2.返 回 值 :TRUE 成功  FALSE 失败
 3.参    数 : nNetId:网络链接编号,与Init_ETH（）调用的ID一样
 4.注意事项 : 用于需要延时回复的场合，没有需要可以不调用
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT BOOL Set_ETH_Delay(int nTime,int nNetId = 0);


/******************************************************************************
 1.功能描述 : 写H3u软元件
 2.返 回 值 :1 成功  0 失败
 3.参    数 : nNetId:网络链接编号
			  eType：软元件类型  
				  REGI_H3U_Y    = 0x20,     //Y元件的定义	
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
				  REGI_H3U_SD   = 0x2b,		//SD
				  REGI_H3U_R    = 0x2c		//SD
			  nStartAddr:软元件起始地址
			  nCount：软元件个数
			  pValue：数据缓存区
 4.注意事项 : 1.x和y元件地址需为8进制; 2. 当元件位C元件双字寄存器时，每个寄存器需占4个字节的数据
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT int H3u_Write_Soft_Elem(SoftElemType eType,int nStartAddr,int nCount,BYTE* pValue,int nNetId = 0);


/******************************************************************************
 1.功能描述 : 读H3u软元件
 2.返 回 值 :1 成功  0 失败
 3.参    数 : nNetId:网络链接编号
			  eType：软元件类型  
				  REGI_H3U_Y    = 0x20,     //Y元件的定义	
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
				  REGI_H3U_SD   = 0x2b,		//SD
				  REGI_H3U_R    = 0x2c		//SD
			  nStartAddr:软元件起始地址
			  nCount：软元件个数
			  pValue：返回数据缓存区
 4.注意事项 : 1.x和y元件地址需为8进制; 2. 当元件位C元件双字寄存器时，每个寄存器需占4个字节的数据
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT int H3u_Read_Soft_Elem(SoftElemType eType,int nStartAddr,int nCount,BYTE* pValue,int nNetId = 0);


/******************************************************************************
 1.功能描述 : 写Am600软元件
 2.返 回 值 :1 成功  0 失败
 3.参    数 : nNetId:网络链接编号
			  eType：软元件类型    ELEM_QX = 0//QX元件  ELEM_MW = 1 //MW元件
			  nStartAddr:软元件起始地址（QX元件由于带小数点，地址需要乘以10去掉小数点，如QX10.1，请输入101，MW元件直接就是它的元件地址不用处理）
			  nCount：软元件个数
			  pValue：数据缓存区
 4.注意事项 : 
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT int Am600_Write_Soft_Elem(SoftElemType eType,int nStartAddr,int nCount,BYTE* pValue,int nNetId = 0);


/******************************************************************************
 1.功能描述 : 读Am600软元件
 2.返 回 值 :1 成功  0 失败
 3.参    数 : nNetId:网络链接编号
			  eType：软元件类型   ELEM_QX = 0//QX元件  ELEM_MW = 1 //MW元件
			  nStartAddr:软元件起始地址（QX元件由于带小数点，地址需要乘以10去掉小数点，如QX10.1，请输入101，其它元件不用处理）
			  nCount：软元件个数
			  pValue：返回数据缓存区
 4.注意事项 : 
******************************************************************************/
STANDARD_MODBUS_FUNC_EXPORT int Am600_Read_Soft_Elem(SoftElemType eType,int nStartAddr,int nCount,BYTE* pValue,int nNetId = 0);































#endif	//	#if !defined(_INCLUDE_H_)