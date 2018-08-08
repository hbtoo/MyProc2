// MyProc2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyProc2.h"
#include "MyProc2Dlg.h"
#include "Data.h"
#include "Dev1.h"
#include "Dev2.h"
#include <fstream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



int maxNoResponseTime1, maxNoResponseTime2, maxNoResponseTime3;

LsComm::CComPort m_ComPort1, m_ComPort2, m_ComPort3;
int nPort1, nPort2, nPort3;
int defaultPort1, defaultPort2, defaultPort3;
bool connected1, connected2, connected3;

bool sendWaiting1, hasResponse1;
bool sendWaiting2, hasResponse2;
bool sendWaiting3, hasResponse3;

Data data;
Dev1 dev1;
Dev2 dev2;


// Auto thread related
HANDLE thread;

/* 0: not running
 * 1: running
 * 2: paused */
int executingState;

// 1, Run1, wash
// 2, Run2, eject one batch
int autoThreadMode;

bool generatePause = false;


CString showCodeMessage(byte *a, int n) {
	CString c1, c2;
	char b[100];
	for(int i = 0; i < n; i++)
	{
	   sprintf(b, "%d", a[i]);
	   c1 += b;
	   sprintf(b, "%c", a[i]);
	   c2 += b;
	   if (i + 1 == n) {
		   c1 += "!";
		   c2 += "!";
	   } else {
		   c1 += " ";
		   c2 += " ";
	   }
	}
	return c1 + "\n" + c2;
}

bool showExecutionMessage() {
	if (executingState == 1) {
		AfxMessageBox("自动执行中，如要手动执行命令，请先暂停或终止自动执行");
		return true;
	}
	return false;
}

void OnComBreak(LPVOID pSender,DWORD dwMask,COMSTAT stat) {
}

void OnReceiveData1(LPVOID pSender, void* pBuf, DWORD InBufferCount) {
	byte a[100];
	memcpy(a, pBuf, InBufferCount);
	
	int n = (int)a[2];
	if (n == 103) {
		AfxMessageBox("错误，注射器设备还未用ZR指令初始化！");
	} else if (n == 64) {	// in execution
		
	} else if (n == 96)	{	// execution finished, release send lock
		sendWaiting1 = false;
	} else {
		CString c1 = "错误！不能识别的返回信息";
		AfxMessageBox(c1 + "\n" + showCodeMessage(a, (int)InBufferCount));
	}
	hasResponse1 = true;
}

void OnReceiveData2(LPVOID pSender, void* pBuf, DWORD InBufferCount) {
	byte a[100];
	memcpy(a, pBuf, InBufferCount);
	
	int n = (int)a[0];
	if (n == 100) {		// 'd' in "done"
		sendWaiting2 = false;
	} else {
		sendWaiting2 = false;
		CString c1 = "错误！不能识别的返回信息";
		AfxMessageBox(c1 + "\n" + showCodeMessage(a, (int)InBufferCount));
	}
	//AfxMessageBox("收到返回信息\n" + showCodeMessage(a, (int)InBufferCount));
}

void OnReceiveData3(LPVOID pSender, void* pBuf, DWORD InBufferCount) {
	byte a[100];
	memcpy(a, pBuf, InBufferCount);
	
	int n = (int)a[0];
	if (n == 100) {		// 'd' in "done"
		sendWaiting3 = false;
	} else {
		sendWaiting3 = false;
		CString c1 = "错误！不能识别的返回信息";
		AfxMessageBox(c1 + "\n" + showCodeMessage(a, (int)InBufferCount));
	}
	//AfxMessageBox("收到返回信息\n" + showCodeMessage(a, (int)InBufferCount));
}



/////////////////////////////////////////////////////////////////////////////
// CMyProc2Dlg dialog

bool CMyProc2Dlg::runCommandDev1(const char *command, int n) {
	char a[100];
	memcpy(a, command, n);
	a[n++] = '\xD';

	//AfxMessageBox(showCodeMessage((byte *)a, n));
	if (!connected1) {
		AfxMessageBox("设备1还未连接");
		return false;
	}
	
	m_ComPort1.Output(a, n);
	sendWaiting1 = true;
	hasResponse1 = false;

	int j = 200;
	int i = maxNoResponseTime1 / j;
	while (sendWaiting1) {
		Sleep(j);
		m_ComPort1.Output("/1QR\xD", 5);
		if (!hasResponse1 && i-- <= 0) {
			AfxMessageBox("设备1没有响应");
			return false;
		}
	}
	Sleep(500);	// wait for all responses for earlier requests

	//AfxMessageBox(showCodeMessage((byte *)a, n));
	return true;
}

bool CMyProc2Dlg::runCommandDev2(const char *command, int n) {
	if (!connected2) {
		AfxMessageBox("设备2还未连接");
		return false;
	}
	char a[100];
	memcpy(a, command, n);

	/*a[n] = '!';
	a[n+1] = '\0';
	AfxMessageBox(a);*/

	a[n++] = 'z';
	m_ComPort2.Output(a, n);

	sendWaiting2 = true;
	hasResponse2 = true;

	int j = 100;
	int i = maxNoResponseTime2 / j;
	while (sendWaiting2) {
		Sleep(j);
		if (i-- <= 0) {
			AfxMessageBox("设备2没有响应");
			return false;
		}
	}
	return true;
}


bool CMyProc2Dlg::runCommandDev3(const char *command, int n) {
	if (!connected3) {
		AfxMessageBox("设备3还未连接");
		return false;
	}
	char a[100];
	memcpy(a, command, n);

	/*a[n] = '!';
	a[n+1] = '\0';
	AfxMessageBox(a);*/

	a[n++] = 'z';
	m_ComPort3.Output(a, n);

	sendWaiting3 = true;
	hasResponse3 = true;

	int j = 100;
	int i = maxNoResponseTime3 / j;
	while (sendWaiting3) {
		Sleep(j);
		if (i-- <= 0) {
			AfxMessageBox("设备3没有响应");
			return false;
		}
	}
	return true;
}


CMyProc2Dlg::CMyProc2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyProc2Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CMyProc2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMyProc2Dlg::~CMyProc2Dlg() {
	// TODO: Add your control notification handler code here
	AfxMessageBox("I am never executed?!");
	if (m_ComPort1.IsOpen()) {
		m_ComPort1.Close();
	}
}

void CMyProc2Dlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyProc2Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyProc2Dlg, CDialog)
	//{{AFX_MSG_MAP(CMyProc2Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_BN_CLICKED(IDC_RUN0, OnRun0)
	ON_BN_CLICKED(IDC_RUN, OnRun1)
	ON_BN_CLICKED(IDC_RUN2, OnRun2)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnSelchangeCombo3)
	ON_BN_CLICKED(IDC_RUN1, OnRun1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyProc2Dlg message handlers

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyProc2Dlg::OnPaint() {
	if (IsIconic())	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyProc2Dlg::OnQueryDragIcon() {
	return (HCURSOR) m_hIcon;
}



BOOL CMyProc2Dlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	GetDlgItem(IDC_RUN0)->SetWindowText("暂停执行");
	GetDlgItem(IDC_RUN0)->EnableWindow(false);
	GetDlgItem(IDC_RUN1)->SetWindowText("自动清洗");
	GetDlgItem(IDC_RUN2)->SetWindowText("自动注射");
	//GetDlgItem(IDC_RUN3)->SetWindowText("全套自动");

	GetDlgItem(IDC_STATIC0)->SetWindowText("端口选择");
	GetDlgItem(IDC_STATIC1)->SetWindowText("手动发送命令");
	GetDlgItem(IDC_STATIC2)->SetWindowText("设备1（注射器）");
	GetDlgItem(IDC_STATIC3)->SetWindowText("设备2（源液轴）");
	GetDlgItem(IDC_STATIC14)->SetWindowText("设备3（配液轴）");

	GetDlgItem(IDC_COMMAND1)->SetWindowText("/1ZR");
	GetDlgItem(IDC_COMMAND2)->SetWindowText("2000 2000 2000");
	GetDlgItem(IDC_COMMAND14)->SetWindowText("2000 2000 2000");

	GetDlgItem(IDC_STATIC4)->SetWindowText("X");
	GetDlgItem(IDC_STATIC5)->SetWindowText("Y");
	GetDlgItem(IDC_STATIC6)->SetWindowText("试管数");
	GetDlgItem(IDC_STATIC7)->SetWindowText("最远位置");
	GetDlgItem(IDC_COMMAND3)->SetWindowText("8");
	GetDlgItem(IDC_COMMAND4)->SetWindowText("8");
	GetDlgItem(IDC_COMMAND5)->SetWindowText("22000");
	GetDlgItem(IDC_COMMAND6)->SetWindowText("21000");
	GetDlgItem(IDC_STATIC8)->SetWindowText("Z");
	GetDlgItem(IDC_STATIC9)->SetWindowText("移动高度");
	GetDlgItem(IDC_STATIC10)->SetWindowText("注射高度");
	GetDlgItem(IDC_COMMAND7)->SetWindowText("10");
	GetDlgItem(IDC_COMMAND8)->SetWindowText("2000");
	GetDlgItem(IDC_STATIC13)->SetWindowText("吸液高度");
	GetDlgItem(IDC_COMMAND13)->SetWindowText("10000");

	GetDlgItem(IDC_STATIC11)->SetWindowText("水1位置");
	GetDlgItem(IDC_COMMAND9)->SetWindowText("1000");
	GetDlgItem(IDC_COMMAND10)->SetWindowText("27000");
	GetDlgItem(IDC_STATIC22)->SetWindowText("水2位置");
	GetDlgItem(IDC_COMMAND21)->SetWindowText("10000");
	GetDlgItem(IDC_COMMAND22)->SetWindowText("27000");
	GetDlgItem(IDC_STATIC28)->SetWindowText("水3位置");
	GetDlgItem(IDC_COMMAND26)->SetWindowText("20000");
	GetDlgItem(IDC_COMMAND27)->SetWindowText("27000");
	GetDlgItem(IDC_STATIC26)->SetWindowText("吸液速度");
	GetDlgItem(IDC_COMMAND24)->SetWindowText("500");

	GetDlgItem(IDC_STATIC24)->SetWindowText("源液部分");


	GetDlgItem(IDC_STATIC15)->SetWindowText("X");
	GetDlgItem(IDC_STATIC16)->SetWindowText("Y");
	GetDlgItem(IDC_STATIC17)->SetWindowText("试管数");
	GetDlgItem(IDC_STATIC18)->SetWindowText("最远位置");
	GetDlgItem(IDC_COMMAND15)->SetWindowText("9");
	GetDlgItem(IDC_COMMAND16)->SetWindowText("8");
	GetDlgItem(IDC_COMMAND17)->SetWindowText("24000");
	GetDlgItem(IDC_COMMAND18)->SetWindowText("21000");
	GetDlgItem(IDC_STATIC19)->SetWindowText("Z");
	GetDlgItem(IDC_STATIC20)->SetWindowText("移动高度");
	GetDlgItem(IDC_STATIC21)->SetWindowText("注射高度");
	GetDlgItem(IDC_COMMAND19)->SetWindowText("10");
	GetDlgItem(IDC_COMMAND20)->SetWindowText("2000");
	GetDlgItem(IDC_STATIC23)->SetWindowText("吸液高度");
	GetDlgItem(IDC_COMMAND23)->SetWindowText("10000");
	
	GetDlgItem(IDC_STATIC12)->SetWindowText("废液位置");
	GetDlgItem(IDC_COMMAND11)->SetWindowText("11500");
	GetDlgItem(IDC_COMMAND12)->SetWindowText("25000");
	GetDlgItem(IDC_STATIC27)->SetWindowText("打液速度");
	GetDlgItem(IDC_COMMAND25)->SetWindowText("1000");

	GetDlgItem(IDC_STATIC25)->SetWindowText("配液部分");
	


	maxNoResponseTime1 = 1000;
	maxNoResponseTime2 = 20000;
	maxNoResponseTime3 = 20000;
	nPort1 = nPort2 = nPort3 = -1;
	defaultPort1 = defaultPort2 = defaultPort3 = -1;
	connected1 = connected2 = connected3 = false;

	executingState = 0;

	defaultPort1 = 4;
	defaultPort2 = 9;
	defaultPort3 = 10;

	CComboBox* pComb = (CComboBox*) GetDlgItem(IDC_COMBO1);
	pComb->AddString("Com01");
	pComb->AddString("Com02");
	pComb->AddString("Com03");
	pComb->AddString("Com04");
	pComb->AddString("Com05");
	pComb->AddString("Com06");
	pComb->AddString("Com07");
	pComb->AddString("Com08");
	pComb->AddString("Com09");
	pComb->AddString("Com10");
	pComb->AddString("Com11");
	pComb->AddString("Com12");
	pComb->AddString("Com13");
	pComb->AddString("Com14");
	pComb->AddString("Com15");
	if (defaultPort1 != -1) {
		pComb->SetCurSel(defaultPort1 - 1);
		OnSelchangeCombo1();
	}

	pComb = (CComboBox*) GetDlgItem(IDC_COMBO2);
	pComb->AddString("Com01");
	pComb->AddString("Com02");
	pComb->AddString("Com03");
	pComb->AddString("Com04");
	pComb->AddString("Com05");
	pComb->AddString("Com06");
	pComb->AddString("Com07");
	pComb->AddString("Com08");
	pComb->AddString("Com09");
	pComb->AddString("Com10");
	pComb->AddString("Com11");
	pComb->AddString("Com12");
	pComb->AddString("Com13");
	pComb->AddString("Com14");
	pComb->AddString("Com15");
	if (defaultPort2 != -1) {
		pComb->SetCurSel(defaultPort2 - 1);
		OnSelchangeCombo2();
	}

	pComb = (CComboBox*) GetDlgItem(IDC_COMBO3);
	pComb->AddString("Com01");
	pComb->AddString("Com02");
	pComb->AddString("Com03");
	pComb->AddString("Com04");
	pComb->AddString("Com05");
	pComb->AddString("Com06");
	pComb->AddString("Com07");
	pComb->AddString("Com08");
	pComb->AddString("Com09");
	pComb->AddString("Com10");
	pComb->AddString("Com11");
	pComb->AddString("Com12");
	pComb->AddString("Com13");
	pComb->AddString("Com14");
	pComb->AddString("Com15");
	if (defaultPort3 != -1) {
		pComb->SetCurSel(defaultPort3 - 1);
		OnSelchangeCombo3();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyProc2Dlg::OnSelchangeCombo1() {
	CComboBox* pComb = (CComboBox*) GetDlgItem(IDC_COMBO1);
	if (showExecutionMessage()) {
		if (nPort1 != -1)		// Undo the change by user
			pComb->SetCurSel(nPort1 - 1);
		return;
	}
	int newPortNum = pComb->GetCurSel() + 1;
	if (nPort1 == newPortNum)
		return;
	if (m_ComPort1.IsOpen())	// close the old port
		m_ComPort1.Close();		// turn out useless, connection stays open afterwards !!!!!!!
	
	nPort1 = newPortNum;
	LsComm::CComPort::ReceiveMode mode = LsComm::CComPort::AutoReceiveBySignal;
	DWORD dwBaud = 9600;
	LsComm::CComPort::Parity parity = LsComm::CComPort::NoParity;
	BYTE DataBits = 8;
	LsComm::CComPort::StopBits stopbits = LsComm::CComPort::OneStopBit;
	LsComm::CComPort::FlowControl fc = LsComm::CComPort::NoFlowControl;

	connected1 = false;
	if (!m_ComPort1.Open(nPort1, mode, dwBaud, parity, DataBits, stopbits, fc)) {
		CString c;
		c.Format("提示:不能打开串口%d!", nPort1);
		AfxMessageBox(c, MB_ICONINFORMATION | MB_OK);
		return;
	}
	if (!m_ComPort1.IsOpen()) {
		AfxMessageBox("what");
		return;
	}
	connected1 = true;

	m_ComPort1.SetReceiveFunc(OnReceiveData1, this); 
	m_ComPort1.SetBreakHandleFunc(OnComBreak);
}


void CMyProc2Dlg::OnSelchangeCombo2() {
	CComboBox* pComb = (CComboBox*) GetDlgItem(IDC_COMBO2); 
	if (showExecutionMessage()) {
		if (nPort2 != -1)		// Undo the change by user
			pComb->SetCurSel(nPort2 - 1);
		return;
	}
	int newPortNum = pComb->GetCurSel() + 1;
	if (nPort2 == newPortNum)
		return;
	if (m_ComPort2.IsOpen())	// close the old port
		m_ComPort2.Close();		// turn out useless, connection stays open afterwards !!!!!!!
	
	nPort2 = newPortNum;
	LsComm::CComPort::ReceiveMode mode = LsComm::CComPort::AutoReceiveBySignal;
										//LsComm::CComPort::ManualReceiveByQuery
	DWORD dwBaud = 9600;
	LsComm::CComPort::Parity parity = LsComm::CComPort::NoParity;
	BYTE DataBits = 8;
	LsComm::CComPort::StopBits stopbits = LsComm::CComPort::OneStopBit;
	LsComm::CComPort::FlowControl fc = LsComm::CComPort::NoFlowControl;

	connected2 = false;
	if (!m_ComPort2.Open(nPort2, mode, dwBaud, parity, DataBits, stopbits, fc)) {
		CString c;
		c.Format("提示:不能打开串口%d!", nPort2);
		AfxMessageBox(c, MB_ICONINFORMATION | MB_OK);
		return;
	}
	if (!m_ComPort2.IsOpen()) {
		AfxMessageBox("what");
		return;
	}
	connected2 = true;
	m_ComPort2.SetReceiveFunc(OnReceiveData2, this); 
	m_ComPort2.SetBreakHandleFunc(OnComBreak);

	if (!dev2.init(this)) {
		connected2 = false;
		return;
	}
}


void CMyProc2Dlg::OnSelchangeCombo3() {
	CComboBox* pComb = (CComboBox*) GetDlgItem(IDC_COMBO3); 
	if (showExecutionMessage()) {
		if (nPort3 != -1)		// Undo the change by user
			pComb->SetCurSel(nPort3 - 1);
		return;
	}
	int newPortNum = pComb->GetCurSel() + 1;
	if (nPort3 == newPortNum)
		return;
	if (m_ComPort3.IsOpen())	// close the old port
		m_ComPort3.Close();		// turn out useless, connection stays open afterwards !!!!!!!
	
	nPort3 = newPortNum;
	LsComm::CComPort::ReceiveMode mode = LsComm::CComPort::AutoReceiveBySignal;
										//LsComm::CComPort::ManualReceiveByQuery
	DWORD dwBaud = 9600;
	LsComm::CComPort::Parity parity = LsComm::CComPort::NoParity;
	BYTE DataBits = 8;
	LsComm::CComPort::StopBits stopbits = LsComm::CComPort::OneStopBit;
	LsComm::CComPort::FlowControl fc = LsComm::CComPort::NoFlowControl;

	connected3 = false;
	if (!m_ComPort3.Open(nPort3, mode, dwBaud, parity, DataBits, stopbits, fc)) {
		CString c;
		c.Format("提示:不能打开串口%d!", nPort3);
		AfxMessageBox(c, MB_ICONINFORMATION | MB_OK);
		return;
	}
	if (!m_ComPort3.IsOpen()) {
		AfxMessageBox("what");
		return;
	}
	connected3 = true;
	m_ComPort3.SetReceiveFunc(OnReceiveData3, this); 
	m_ComPort3.SetBreakHandleFunc(OnComBreak);

	if (!dev2.init(this)) {
		connected3 = false;
		return;
	}
}


BOOL CMyProc2Dlg::PreTranslateMessage(MSG* pMsg) {
	char a[100];
	int n;
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {	// handle manual commands
		CWnd* window = GetFocus();
		if (window == GetDlgItem(IDC_COMMAND1)) {
			n = GetDlgItem(IDC_COMMAND1)->GetWindowText(a, 100);
			if (!showExecutionMessage())
				runCommandDev1(a, n);
		} else if (window == GetDlgItem(IDC_COMMAND2)) {
			n = GetDlgItem(IDC_COMMAND2)->GetWindowText(a, 100);
			//runCommandDev2(a, n);
			if (!showExecutionMessage())
				dev2.moveToPosition(0, a, n);
		} else if (window == GetDlgItem(IDC_COMMAND14)) {
			n = GetDlgItem(IDC_COMMAND14)->GetWindowText(a, 100);
			//runCommandDev2(a, n);
			if (!showExecutionMessage())
				dev2.moveToPosition(1, a, n);
		} else if (window == GetDlgItem(IDC_RUN0)) {
			OnRun0();
		}
		return true;	// intersept all Enter pressed
	}
	return CDialog::PreTranslateMessage(pMsg);
}




void waitWhenPaused(CMyProc2Dlg *mainDlg, bool genPause) {
	if (genPause)
		mainDlg->OnRun0();
	while (executingState != 1)
		Sleep(200);
	return;
}

bool empty_hose(CMyProc2Dlg *mainDlg) {
	if (!dev2.moveToAir())
		return false;
	waitWhenPaused(mainDlg, generatePause);

	if (!dev2.moveToEjectWaste())
		return false;
	waitWhenPaused(mainDlg, generatePause);

	if (!dev1.goLeft())
		return false;
	dev1.goToPos(Air_Step + dev1.getPosition());
	waitWhenPaused(mainDlg, generatePause);
	dev1.goRight();
	dev1.goToPos(0);
	waitWhenPaused(mainDlg, generatePause);

	dev1.goLeft();
	dev1.goToPos(Air_Step);
	waitWhenPaused(mainDlg, generatePause);
	dev1.goRight();
	dev1.goToPos(0);
	waitWhenPaused(mainDlg, generatePause);
	return true;
}

bool process_wash(CMyProc2Dlg *mainDlg, bool hostStartEmpty, bool leaveHostEmpty) {
	if (!hostStartEmpty)
		empty_hose(mainDlg);

	dev2.moveToInhaleWater();
	waitWhenPaused(mainDlg, generatePause);
	dev2.moveToEjectWaste();
	waitWhenPaused(mainDlg, generatePause);

	for (int i = 0; i < 4; i++) {
		dev1.goLeft();
		dev1.goToPos(Max_Step);
		waitWhenPaused(mainDlg, generatePause);

		dev1.goRight();
		dev1.goToPos(0);
		waitWhenPaused(mainDlg, generatePause);
	}
	if (leaveHostEmpty)
		empty_hose(mainDlg);
	
	return true;
}

// Always assume hose are empty before start and after end
// return value:
//	0: succeed
//	1: failed, no need to do anything
//  2: failed, need to move both arms
//	3: failed, need to empty hose and move both arms
//
int process_full(CMyProc2Dlg *mainDlg) {
	int i, j, k, ejected, totalLeft;

	if (!data.readFromFile("D:/encripted.txt"))
		return 1;
	if (!dev2.init(mainDlg))
		return 1;

	if (!dev2.moveToInhaleWater())
		return 1;
	waitWhenPaused(mainDlg, generatePause);

	if (!dev2.moveToEjectWaste()) {
		dev2.moveToPosition(0, 0, 0, 0);
		return 1;
	}
	waitWhenPaused(mainDlg, generatePause);

	if (!dev1.init(mainDlg, 1))
		return 2;
	waitWhenPaused(mainDlg, generatePause);

	for (i = 0; i < data.nInput; i++) {
		totalLeft = Min_Step;
		for (j = 0; j < data.nOutput; j++)
			totalLeft += data.outArray[i][j];
		if (totalLeft == Min_Step)
			continue;

		process_wash(mainDlg, i == 0, true);

		dev2.moveToInhaleInput(i);
		waitWhenPaused(mainDlg, generatePause);
		// output is still in waste

		// get input solution into hoses
		for (j = 0; j < 2; j++) {
			dev1.goLeft();
			dev1.goToPos(Air_Step);
			waitWhenPaused(mainDlg, generatePause);
			dev1.goRight();
			dev1.goToPos(0);
			waitWhenPaused(mainDlg, generatePause);
		}

		for (j = 0; j < data.nOutput; j++) {
			k = data.outArray[i][j];
			if (k == 0)
				continue;

			dev2.moveToEjectOutput(j);
			waitWhenPaused(mainDlg, generatePause);

			bool first = true;
			while (k > 0) {
				if (dev1.canEject(k, true) || first == false) {
					// should eject when not first because of the case k is larger than max
					dev1.goRight();
					ejected = dev1.eject(k, true);
					waitWhenPaused(mainDlg, generatePause);

					k -= ejected;
					totalLeft -= ejected;
					if (k <= 0)
						break;
				}
				first = false;

				int target;
				if (totalLeft > Max_Step)
					target = Max_Step;
				else if (totalLeft > 0)
					target = totalLeft;
				else {
					CString c;
					c.Format("Bug at i=%d j=%d k=%d totalLeft=%d", i, j, k, totalLeft);
					AfxMessageBox(c);
					return 3;
				}
				if (target <= dev1.getPosition()) {
					CString c;
					c.Format("Bug! i=%d j=%d k=%d totalLeft=%d target=%d curPosition=%d", 
						i, j, k, totalLeft, target, dev1.getPosition());
					AfxMessageBox(c);
					return 3;
				}
				dev1.goLeft();
				dev1.goToPos(target);
				waitWhenPaused(mainDlg, generatePause);
			}
		}
	}
	process_wash(mainDlg, false, true);
	dev2.moveToPosition(0, 0, 0, 0);
	dev2.moveToPosition(1, 0, 0, 0);
	return 0;
}

void threadFinished(CMyProc2Dlg *mainDlg, bool succeed) {
	CWnd *button1 = mainDlg->GetDlgItem(IDC_RUN1);
	CWnd *button2 = mainDlg->GetDlgItem(IDC_RUN2);
	if (autoThreadMode == 1) {
		button1->SetWindowText("自动清洗");
		button2->EnableWindow(true);
	} else if (autoThreadMode == 2) {
		button1->EnableWindow(true);
		button2->SetWindowText("自动注射");
	}
	button1 = mainDlg->GetDlgItem(IDC_RUN0);
	button1->SetWindowText("暂停执行");
	button1->EnableWindow(false);
	
	if (succeed)
		AfxMessageBox("自动执行完毕");
	else
		AfxMessageBox("自动执行已终止");

	executingState = 0;
	thread = NULL;
}

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
	bool succeed = false;
    CMyProc2Dlg *mainDlg = (CMyProc2Dlg *)lpParam;
	
	if (autoThreadMode == 1) {			// Run1, wash
		if (dev2.init(mainDlg) && dev2.moveToAir()) {
			waitWhenPaused(mainDlg, generatePause);

			if (dev2.moveToEjectWaste()) {
				waitWhenPaused(mainDlg, generatePause);

				if (dev1.init(mainDlg, 1)) {
					succeed = process_wash(mainDlg, true, true);
					dev2.moveToPosition(0, 0, 0, 0);
					dev2.moveToPosition(1, 0, 0, 0);
				}
			}
		}
	} else if (autoThreadMode == 2) {	// Run2, full automation
		int retCode = process_full(mainDlg);
			// return value:
			//	0: succeed
			//	1: failed, no need to do anything
			//  2: failed, need to move both arms
			//	3: failed, need to empty hose and move both arms
		if (retCode >= 2) {
			if (dev2.moveToPosition(0, 0, 0, 0))
				if (retCode == 3)
					empty_hose(mainDlg);
			dev2.moveToPosition(1, 0, 0, 0);
		}
		succeed = (retCode == 0);
	} else {							// Run0, test
		int n = 5;
		while (n-- > 0) {
			CString c;
			c.Format("testing %d", n);
			mainDlg->GetDlgItem(IDC_COMMAND1)->SetWindowText(c);
			waitWhenPaused(mainDlg, false);
			Sleep(1000);
		}
		mainDlg->GetDlgItem(IDC_COMMAND1)->SetWindowText("");
		
		threadFinished(mainDlg, false);
		return 0;
	}
	threadFinished(mainDlg, succeed);
	return 0;
}

DWORD WINAPI AbortThreadFunc(LPVOID lpParam) {
	CMyProc2Dlg *mainDlg = (CMyProc2Dlg *)lpParam;
	dev2.moveToPosition(0, 0, 0, 0);
	empty_hose(mainDlg);
	dev2.moveToPosition(1, 0, 0, 0);
	threadFinished(mainDlg, false);
	return 0;
}

/* executingState
 * 0: not running
 * 1: running
 * 2: paused 
 * 3: aborting
 *
 * mode
 * 0: test
 * 1: wash
 * 2: full auto

*/
void CMyProc2Dlg::runAutoThread(int mode) {
	CWnd *button1, *button2;
	button1 = GetDlgItem(IDC_RUN1);
	button2 = GetDlgItem(IDC_RUN2);

	if (executingState == 0) {
		executingState = 1;
		autoThreadMode = mode;
		GetDlgItem(IDC_RUN0)->EnableWindow(true);

		DWORD dwThreadId;
		thread = CreateThread( 
			NULL,					// no security attributes 
			0,						// use default stack size  
			ThreadFunc,			// thread function 
			this,					// argument to thread function 
			CREATE_SUSPENDED,		// use default creation flags 
			&dwThreadId);			// returns the thread identifier 
		SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST); 
		ResumeThread(thread);

		if (autoThreadMode == 1) {
			button1->SetWindowText("终止清洗");
			button2->EnableWindow(false);
		} else if (autoThreadMode == 2) {
			button1->EnableWindow(false);
			button2->SetWindowText("终止注射");
		}
	} else if (executingState != 3) {
		if (thread != NULL && autoThreadMode == mode) {
			// Abort
			TerminateThread(thread, 0);

			if (autoThreadMode == 1) {
				button1->SetWindowText("停止中");
			} else if (autoThreadMode == 2) {
				button2->SetWindowText("停止中");
			}

			DWORD dwThreadId;
			thread = CreateThread( 
				NULL,					// no security attributes 
				0,						// use default stack size  
				AbortThreadFunc,			// thread function 
				this,					// argument to thread function 
				CREATE_SUSPENDED,		// use default creation flags 
				&dwThreadId);			// returns the thread identifier 
			SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST); 
			ResumeThread(thread);
		}
	}
}

void CMyProc2Dlg::OnRun0() {
	CWnd *button = GetDlgItem(IDC_RUN0);
	if (executingState == 1) {	// pause execution
		executingState = 2;
		button->SetWindowText("继续执行");
	} else if (executingState == 2) {	// resume execution
		executingState = 1;
		button->SetWindowText("暂停执行");
	}
}

void CMyProc2Dlg::OnRun1() {
	runAutoThread(1);
}

void CMyProc2Dlg::OnRun2() {
	runAutoThread(2);
}

/*
void CMyProc2Dlg::OnRun3() {
	runAutoThread(3);
}

void CMyProc2Dlg::OnRun4() {
	runAutoThread(4);
}*/
