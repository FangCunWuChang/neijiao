// DllMod.h : DllMod DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDllModApp
// �йش���ʵ�ֵ���Ϣ������� DllMod.cpp
//

class CDllModApp : public CWinApp
{
public:
	CDllModApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
