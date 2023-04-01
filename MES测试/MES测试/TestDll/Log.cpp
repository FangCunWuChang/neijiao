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

//�����ı��ĵ�
bool Log::CommonLogInit()
{
	struct tm tLocalTime;                           //tm�ṹָ��
	time_t tNowTime;               //����time_t���ͱ���
	time(&tNowTime);              //��ȡϵͳ���ں�ʱ��
	localtime_s(&tLocalTime, &tNowTime);     //��ȡ�������ں�ʱ��

    //time_t tNowTime;
    //time(&tNowTime);
    //tm* tLocalTime = localtime_s(&tNowTime);

    //�õ����ڵ��ַ���
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

//�õ���ǰʱ����ַ���
string Log::GetTimeStr()
{
	struct tm tLocalTime;                           //tm�ṹָ��
	time_t tNowTime;               //����time_t���ͱ���
	time(&tNowTime);              //��ȡϵͳ���ں�ʱ��
	localtime_s(&tLocalTime, &tNowTime);     //��ȡ�������ں�ʱ��


    //�õ����ڵ��ַ���
    string strDateTime = "[" +ValueToStr(tLocalTime.tm_year+1900) + "-" +
                               ValueToStr(tLocalTime.tm_mon+1)     + "-" +
                               ValueToStr(tLocalTime.tm_mday)      + " " +
                               ValueToStr(tLocalTime.tm_hour)      + ":" +
                               ValueToStr(tLocalTime.tm_min)       + ":" +
                               ValueToStr(tLocalTime.tm_sec)       + "]" + " ";
    return strDateTime;
}