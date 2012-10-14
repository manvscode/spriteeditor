// 2DSpriteEditor.h : main header file for the 2DSpriteEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSpriteEditorApp:
// See 2DSpriteEditor.cpp for the implementation of this class
//

class CSpriteEditorApp : public CWinApp
{
public:
	CSpriteEditorApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
};

extern CSpriteEditorApp theApp;