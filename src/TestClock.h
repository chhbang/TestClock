// TestClock.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CTestClockApp:
// �� Ŭ������ ������ ���ؼ��� TestClock.cpp�� �����Ͻʽÿ�.
//

class CTestClockApp : public CWinApp
{
public:
	CTestClockApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CTestClockApp theApp;