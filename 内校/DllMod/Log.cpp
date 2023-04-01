#include <stdio.h>
#include "stdafx.h"
#include "Log.h"



Log::Log()
    :m_bEnabled(true)
{
}

Log::~Log()
{
}

bool Log::Open(string sFileName)
{
    m_tOLogFile.open(sFileName.c_str(), ios_base::out | ios_base::app);

    if( !m_tOLogFile )
    {
        return false ;
    }

    return true;
}

void Log::Close()
{
    if(m_tOLogFile.is_open())
    {
        m_tOLogFile.close();
    }
}

//生成文本文档
bool Log::CommonLogInit()
{
	struct tm tLocalTime;                           //tm结构指针
	time_t tNowTime;               //声明time_t类型变量
	time(&tNowTime);              //获取系统日期和时间
	localtime_s(&tLocalTime, &tNowTime);     //获取当地日期和时间

    //time_t tNowTime;
    //time(&tNowTime);
    //tm* tLocalTime = localtime_s(&tNowTime);

    //得到日期的字符串
    string sDateStr = ValueToStr(tLocalTime.tm_year + 1900) + "-" +
                      ValueToStr(tLocalTime.tm_mon + 1) + "-" +
                      ValueToStr(tLocalTime.tm_mday);

	return Open(sDateStr+".log");
}

bool Log::OffsetLogInit()
{
	string sDateStr = "D:\\INI\\REC.log";
	return Open(sDateStr);
}

void Log::Enable()
{
    m_bEnabled = true;
}

void Log::Disable()
{
    m_bEnabled = false;
}

//得到当前时间的字符串
string Log::GetTimeStr()
{
	struct tm tLocalTime;                           //tm结构指针
	time_t tNowTime;               //声明time_t类型变量
	time(&tNowTime);              //获取系统日期和时间
	localtime_s(&tLocalTime, &tNowTime);     //获取当地日期和时间


    //得到日期的字符串
    string strDateTime = "[" +ValueToStr(tLocalTime.tm_year+1900) + "-" +
                               ValueToStr(tLocalTime.tm_mon+1)     + "-" +
                               ValueToStr(tLocalTime.tm_mday)      + " " +
                               ValueToStr(tLocalTime.tm_hour)      + ":" +
                               ValueToStr(tLocalTime.tm_min)       + ":" +
                               ValueToStr(tLocalTime.tm_sec)       + "]" + " ";
    return strDateTime;
}