// MyProc2Dlg.h : header file
//

#include "MyProc2.h"
#include "ComPort.h"

#if !defined(AFX_MYPROC2DLG_H__2DEB900E_E836_4E01_92B1_02F8556E324B__INCLUDED_)
#define AFX_MYPROC2DLG_H__2DEB900E_E836_4E01_92B1_02F8556E324B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyProc2Dlg dialog

class CMyProc2Dlg : public CDialog
{
// Construction
public:
	CMyProc2Dlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CMyProc2Dlg();

	bool runCommandDev1(const char *command, int n);
	bool runCommandDev2(const char *command, int n);
	bool runCommandDev3(const char *command, int n);
	void runAutoThread(int mode);

// Dialog Data
	//{{AFX_DATA(CMyProc2Dlg)
	enum { IDD = IDD_MYPROC2_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyProc2Dlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
//protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyProc2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnRun0();
	afx_msg void OnRun1();
	afx_msg void OnRun2();
	afx_msg void OnSelchangeCombo3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPROC2DLG_H__2DEB900E_E836_4E01_92B1_02F8556E324B__INCLUDED_)
