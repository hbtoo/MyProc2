// MyProc2.h : main header file for the MYPROC2 application
//

#if !defined(AFX_MYPROC2_H__6B56550B_69C1_4767_9A1F_D1DC819A9E6F__INCLUDED_)
#define AFX_MYPROC2_H__6B56550B_69C1_4767_9A1F_D1DC819A9E6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyProc2App:
// See MyProc2.cpp for the implementation of this class
//

class CMyProc2App : public CWinApp
{
public:
	CMyProc2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyProc2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyProc2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPROC2_H__6B56550B_69C1_4767_9A1F_D1DC819A9E6F__INCLUDED_)
