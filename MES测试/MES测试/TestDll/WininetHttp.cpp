/*************************************************
File name  :  WininetHttp.cpp
Description:  通过URL访问HTTP请求方式获取JSON
Author     :  shike
Version    :  1.0
Date       :  2016/10/27
Copyright (C) 2016 -  All Rights Reserved
*************************************************/
#include "stdafx.h"
#include "WininetHttp.h"
#include <json/json.h>
//#pragma comment(lib, "jsoncpp.lib")
#include <fstream>
#pragma comment(lib, "Wininet.lib")
#include <tchar.h>
using namespace std;

CWininetHttp::CWininetHttp(void) :m_hSession(NULL), m_hConnect(NULL), m_hRequest(NULL)
{
}

CWininetHttp::~CWininetHttp(void)
{
	Release();
}

//  通过HTTP请求：Get或Post方式获取JSON信息 [3/14/2017/shike]
const std::string CWininetHttp::RequestJsonInfo(const std::string& lpUrl,
	HttpRequest type/* = Hr_Get*/,
	std::string strHeader/*=""*/,
	std::string strPostData/*=""*/)
{
	std::string strRet = "";
	try
	{
		if (lpUrl.empty())
		{
			throw Hir_ParamErr;
		}
		Release();
		m_hSession = InternetOpen(_T("Http-connect"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);    //局部

		if (NULL == m_hSession)
		{
			throw Hir_InitErr;
		}

		INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT;
		std::string strHostName = "";
		std::string strPageName = "";

		ParseURLWeb(lpUrl, strHostName, strPageName, port);
		printf("lpUrl:%s,\nstrHostName:%s,\nstrPageName:%s,\nport:%d\n", lpUrl.c_str(), strHostName.c_str(), strPageName.c_str(), (int)port);

		m_hConnect = InternetConnectA(m_hSession, strHostName.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);

		if (NULL == m_hConnect)
		{
			throw Hir_ConnectErr;
		}

		std::string strRequestType;
		if (Hr_Get == type)
		{
			strRequestType = "GET";
		}
		else
		{
			strRequestType = "POST";
		}

		m_hRequest = HttpOpenRequestA(m_hConnect, strRequestType.c_str(), strPageName.c_str(), "HTTP/1.1", NULL, NULL, INTERNET_FLAG_RELOAD, NULL);
		if (NULL == m_hRequest)
		{
			throw Hir_InitErr;
		}

		DWORD dwHeaderSize = (strHeader.empty()) ? 0 : strlen(strHeader.c_str());
		BOOL bRet = FALSE;
		if (Hr_Get == type)
		{
			bRet = HttpSendRequestA(m_hRequest, strHeader.c_str(), dwHeaderSize, NULL, 0);
		}
		else
		{
			DWORD dwSize = (strPostData.empty()) ? 0 : strlen(strPostData.c_str());
			bRet = HttpSendRequestA(m_hRequest, strHeader.c_str(), dwHeaderSize, (LPVOID)strPostData.c_str(), dwSize);
		}
		if (!bRet)
		{
			throw Hir_SendErr;
		}

		char szBuffer[READ_BUFFER_SIZE + 1] = { 0 };
		DWORD dwReadSize = READ_BUFFER_SIZE;
		if (!HttpQueryInfoA(m_hRequest, HTTP_QUERY_RAW_HEADERS, szBuffer, &dwReadSize, NULL))
		{
			throw Hir_QueryErr;
		}
		if (NULL != strstr(szBuffer, "404"))
		{
			throw Hir_404;
		}

		while (true)
		{
			bRet = InternetReadFile(m_hRequest, szBuffer, READ_BUFFER_SIZE, &dwReadSize);
			if (!bRet || (0 == dwReadSize))
			{
				break;
			}
			szBuffer[dwReadSize] = '\0';
			strRet.append(szBuffer);
		}
	}
	catch (HttpInterfaceError error)
	{
		m_error = error;
	}


	const  char* c_char = strRet.c_str();
	string strGBK = UtfToGbk(c_char);
	return std::move(strGBK);
}

// 解析Json数据 [11/8/2016/shike]
void CWininetHttp::ParseJsonInfo(const std::string &strJsonInfo)
{
	Json::Reader reader;                                    //解析json用Json::Reader
	Json::Value value;                                        //可以代表任意类型
	if (!reader.parse(strJsonInfo, value))
	{
		printf("error!");
	}
	if (!value["result"].isNull())
	{
		int nSize = value["result"].size();
		for (int nPos = 0; nPos < nSize; ++nPos)                //对数据数组进行遍历
		{
			//PGCARDDEVDATA stru ;
			//stru.strCardName        = value["result"][nPos]["tollgateName"].asString();
			//stru.strCardCode        = value["result"][nPos]["tollgateCode"].asString();
			//std::string strCDNum    = value["result"][nPos]["laneNumber"].asString();    //增加:车道总数
			//stru.nLaneNum            = atoi(strCDNum.c_str());
			//std::string strLaneDir    = value["result"][nPos]["laneDir"].asString();        //增加:车道方向,进行规则转换
			//stru.strLaneDir            = TransformLaneDir(strLaneDir);
			//stru.dWgs84_x            = value["result"][nPos]["wgs84_x"].asDouble();
			//stru.dWgs84_y            = value["result"][nPos]["wgs84_y"].asDouble();
			//stru.dMars_x            = value["result"][nPos]["mars_x"].asDouble();
			//stru.dMars_y            = value["result"][nPos]["mars_y"].asDouble();
			//lstCardDevData.emplace_back(stru);
		}
	}
}

// 解析URL地址 [3/14/2017/shike]
void CWininetHttp::ParseURLWeb(std::string strUrl, std::string& strHostName, std::string& strPageName, WORD& sPort)
{
	sPort = 80;
	string strTemp(strUrl);
	std::size_t nPos = strTemp.find("http://");
	if (nPos != std::string::npos)
	{
		strTemp = strTemp.substr(nPos + 7, strTemp.size() - nPos - 7);
	}

	nPos = strTemp.find('/');
	if (nPos == std::string::npos)    //没有找到
	{
		strHostName = strTemp;
	}
	else
	{
		strHostName = strTemp.substr(0, nPos);
	}

	std::size_t nPos1 = strHostName.find(':');
	if (nPos1 != std::string::npos)
	{
		std::string strPort = strTemp.substr(nPos1 + 1, strHostName.size() - nPos1 - 1);
		strHostName = strHostName.substr(0, nPos1);
		sPort = (WORD)atoi(strPort.c_str());
	}
	if (nPos == std::string::npos)
	{
		return;
	}
	strPageName = strTemp.substr(nPos, strTemp.size() - nPos);
}

// 关闭句柄 [3/14/2017/shike]
void CWininetHttp::Release()
{
	ReleaseHandle(m_hRequest);
	ReleaseHandle(m_hConnect);
	ReleaseHandle(m_hSession);
}

// 释放句柄 [3/14/2017/shike]
void CWininetHttp::ReleaseHandle(HINTERNET& hInternet)
{
	if (hInternet)
	{
		InternetCloseHandle(hInternet);
		hInternet = NULL;
	}
}

// UTF-8转为GBK2312 [3/14/2017/shike]
char* CWininetHttp::UtfToGbk(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}