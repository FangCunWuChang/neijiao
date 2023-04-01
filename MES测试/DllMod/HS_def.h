


#ifndef HS_DEF_H_
#define HS_DEF_H_
#pragma once
#include <Windows.h>

//enum E
//{
//	E_Init,
//	E_FreqCurve_Measure,
//	E_VelCurve_Measure,
//	E_BrightFit,
//	E_Jetting
//};

struct St_MotionCMD
{
	int axis;
	unsigned char Command;
	int nStep;
};

struct St_SocketCMD 
{
	unsigned char Head;
	unsigned char Command;
	unsigned char Add1;
	unsigned char Add2;
	unsigned char Add3;
	unsigned char Data1;
	unsigned char Data2;
	unsigned char End;
};

struct St_SocketRES
{
	unsigned char resData[10];
};


/////////////////////////////Ethernet Command code//////////////////////////////
#define CMD_HEAD			(char)0xaa			//命令头
#define CMD_TAIL			(char)0x1b			//命令结尾
#define CMD_START_WRITE_RAM	(char)0x05			//写RAM命令
#define CMD_START_PRINT		(char)0x06			//BYTE5 0x55 BYTE60x55开始打印
#define CMD_SET_VOL			(char)0x0c			//设置电源
#define CMD_STOP_PRINT		(char)0x08			//停止打印		
#define CMD_PAUSE_PRINT		(char)0x09			//暂停打印
#define CMD_GET_STAUS		(char)0x0a			//读取状态
#define CMD_ENCODER_RESET	(char)0x0b			//光栅复位	
#define CMD_9832_CTL		(char)0x0d			//9832
#define CMD_LED_CTL			(char)0x0e			//LED   SWITCH_ON
#define CMD_READ_RAM		(char)0x0f			//读RAM
#define CMD_GAS_ALLCTL		(char)0x10			//总气阀开关
#define CMD_GAS_VCMMCTL		(char)0x11			//平台真空开关
#define CMD_INK_CTL			(char)0x13			//墨盒泵
#define CMD_HEAD_CLEAR      (char)0x14          //喷头清洗开关
#define CMD_DOOR_ENABLE     (char)0x15          //前门开关使能
//#define CMD_HEAT_CTL		(char)0x15			//喷头加热开关
#define CMD_STOP_WRITE_RAM	(char)0x16			//结束写RAM
#define CMD_CHANGE_ROW		(char)0x17			//喷头选排
#define CMD_TEST_NOZZLE		(char)0x18			//喷头测试开关
#define CMD_NOZZLE_FREQ		(char)0x19			//喷头频率

#define	CMD_LED_LUM			(char)0x1a			//观测LED亮度调节
#define CMD_NOZZLE_DATA		(char)0x1b			//观测时喷头数据
#define CMD_LED_CURRENT		(char)0x1c			//观测LED电流
#define CMD_SLOW_MODE		(char)0x1d			//慢镜头
#define CMD_DELAY_SPEED		(char)0x1e			//慢镜头速度
#define CMD_REPEAT_CTL		(char)0x1f
#define CMD_REPEAT_TIMES	(char)0x20

#define SWITCH_ON			(char)0x00			//开
#define SWITCH_OFF			(char)0x01			//关


/////////////////////////////AXIS define//////////////////////////////////////
#define AXISX 0x01
#define AXISY 0x02
#define AXISZ 0x03


#define STEP_LEFTMOVE 0x05
#define STEP_RIFHTMOVE 0x06
#define CONTINUE_LEFTMOVE 0x07
#define CONTINUE_RIGHTMOVE 0x08

#define MOVEHOME 0x09
#define STOPMOVE 0x0a
#define GETPOS   0x0b
#define GETSTATUS 0x0c
#define SET_XSPEED 0x0d

#define STEP_UPMOVE 0x15
#define STEP_DOWNMOVE 0x16
#define CONTINUE_UPMOVE 0x17
#define CONTINUE_DOWNMOVE 0x18
#define ZMOVEHOME 0x19
#define ZSTOPMOVE 0x1a
#define SET_ZSPEED 0x1d

#define MAX_MOTOR_SPEED 500000
/////////////////////////////Ethernet return code//////////////////////////////
enum RES_COMM
{
	COMM_OK=0,
	COMM_ERR,
	COMM_FAILED	
};


//////////////////////Wave Form///////////////////////////////////////
#define WAVE_FLAG 0xFF00FF00


//////////////////////////Const define/////////////////////////////////////////

#define umPerInch	25400

///////////////////////For _beginthredex function//////////////////////////////////////
#include <process.h> 
typedef unsigned (__stdcall *PTHREAD_START) (void *);
#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
   pvParam, fdwCreate, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                      \
         (void *)        (psa),                     \
         (unsigned)      (cbStack),                 \
         (PTHREAD_START) (pfnStartAddr),            \
         (void *)        (pvParam),                 \
         (unsigned)      (fdwCreate),               \
         (unsigned *)    (pdwThreadId)))




///////////////////Message Define///////////////////////////////////////////
#define MESSAGE_LEFT_RUN			WM_USER+0x01				//
#define MESSAGE_LEFT_STOP			WM_USER+0x02
#define MESSAGE_RIGHT_RUN			WM_USER+0x03
#define MESSAGE_RIGHT_STOP			WM_USER+0x04

#define MESSAGE_FRONT_RUN			WM_USER+0x05
#define MESSAGE_FRONT_STOP			WM_USER+0x06
#define MESSAGE_BACK_RUN			WM_USER+0x07
#define MESSAGE_BACK_STOP			WM_USER+0x08

#define MESSAGE_UP_RUN				WM_USER+0x09
#define MESSAGE_UP_STOP				WM_USER+0x10
#define MESSAGE_DOWN_RUN			WM_USER+0x11
#define MESSAGE_DOWN_STOP			WM_USER+0x12

#define MESSAGE_PREVIEW_UPDATE		WM_USER+0x13
#define MESSAGE_UPDATE_POS			WM_USER+0x14

#define MESSAGE_SELECT_NEWIMAGE		WM_USER+0x15
#define MESSAGE_SELECT_NEWNOZZLE	WM_USER+0x16
#define MESSAGE_SELECT_NEWJET		WM_USER+0x17
#define MESSAGE_SELECT_NEWWFM		WM_USER+0x18	//新波形
#define MESSAGE_SELECT_NEWDPI		WM_USER+0x19
#define MESSAGE_SELECT_NEWOFFSET	WM_USER+0x20

#define MESSAGE_UPDATE_PLATEVCM		WM_USER+0x21
#define MESSAGE_UPDATE_INKPUMP		WM_USER+0x22
#define MESSAGE_UPDATE_HEADTEMP		WM_USER+0x23
#define MESSAGE_UPDATE_PLATETEMP	WM_USER+0x24
#define MESSAGE_UPDATE_GAS			WM_USER+0x1a


#define MESSAGE_SHOW_IMAGE			WM_USER+0x25

#define MESSAGE_GET_RECT			WM_USER+0x26
#define MESSAGE_DRAW_MEMDC			WM_USER+0x27

#define MESSAGE_CANCEL_QUERY		WM_USER+0x28
#define MESSAGE_RESUME_QUERY		WM_USER+0x29
#define MESSAGE_SELECT_NEWTHICK		WM_USER+0x30
#define MESSAGE_SELECT_NEWFIRSTJET	WM_USER+0x31
#define MESSAGE_CANCEL_HOVER		WM_USER+0x32
#define MESSAGE_RESUME_HOVER		WM_USER+0x33
#define MESSAGE_UPDATE_GLOBALSTATUS	WM_USER+0x34

#define MESSAGE_FINISH_TESTTHICK	WM_USER+0x35
#define MESSAGE_DATA_CHANGED		WM_USER+0x36
#define MESSAGE_NEW_UMPERPIX		WM_USER+0x37
#define MESSAGE_HOME_DONE			WM_USER+0x38
#define MESSAGE_PRINT_DONE			WM_USER+0x39
#define MESSAGE_SELECT_BASEMARK     WM_USER+0x40

//#define MESSAGE_CANCEL_STEP			WM_USER+0x41	//取消属性页消息
#define MESSAGE_SHOW_PRINTINFO			WM_USER+0x42	//显示打印信息消息

#define MESSAGE_HEAD_ROTATE         WM_USER+0x44
#define MESSAGE_BASE_TWOMARK_OFFSET          WM_USER+0x45
#define MESSAGE_FIRSTJET_ORIGIN          WM_USER+0x46
#define MESSAGE_FIRSTJET_NUM         WM_USER+0x47
#define MESSAGE_ACTIVE_PAGE2         WM_USER+0x48


//////////////////////////////DPI define///////////////////////////////////
#define MAX_X_DPI 2400
#define MAX_Y_DPI 2400

/////////////////////////////Position LIMIT//////////////////////////////
#define X_LIMIT_NEG					-40000*2	//测厚等动作X负极限	
#define Y_LIMIT_LOW					-30000*2	//测厚等动作Y负极限
#define X_TEST_POS					75000*2		//测厚等动作的X正极限
#define Y_TEST_POS					242000*2	//测厚等动作的Y正极限
#define X_SELECTJET_NEG				200000*2	//观测时的左极限
#define X_SELECTJET_PLUS			306000*2	//观测时的右极限				
#define A4WIDTH						210000*2	//A4尺寸定义	
#define A4HEIGHT					297000*2	
#define BASEHEIGHT                  3200*20     //基板位置
#define HOLEDIS1_2                   1016;       //单位微米
#define HOLEDIS1_3                   8000;       //单位微米
#define Z_LIMIT_NEG					-42*3200	

#define HOME_MOVE_OFFSETX			123950
#define HOME_MOVE_OFFSETY			68000	

					
//////////////////////////////Global status///////////////////////////////
#define STATUS_NONE				0x00	//0000 0000
#define STATUS_HOME				0x01	//0000 0001
#define STATUS_PRINTING			0x02	//0000 0010
#define STATUS_GETTHICK			0x04	//0000 0100
#define STATUS_FREQ_RESPONSE	0x08	//0000 1000
#define STATUS_DROP_WATCH		0x10    //0001 0000

BOOL HSCheckGlobalStatus(int status);

//#define 
//////////////////////////connect set/////////////////////////////////////
#define DEFAULT_IPADDRESS			0xc0a80372
#define DEFAULT_PORTNUM				1234


/////////////////////SEND DATA/////////////////////////////////////////////
#define MAX_SEND_SIZE	0x80000//512*1024
#endif