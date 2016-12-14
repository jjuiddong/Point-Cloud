
// Delaunay.h : main header file for the Delaunay application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDelaunayApp:
// See Delaunay.cpp for the implementation of this class
//

class CDelaunayApp : public CWinAppEx
{
public:
	CDelaunayApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDelaunayApp theApp;
