
// TwoCam.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTwoCamApp: 
// �йش����ʵ�֣������ TwoCam.cpp
//

class CTwoCamApp : public CWinApp
{
public:
	CTwoCamApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTwoCamApp theApp;