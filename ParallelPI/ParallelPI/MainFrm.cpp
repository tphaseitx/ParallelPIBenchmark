// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ParallelPI.h"
#include "ParallelPIDoc.h"
#include "ParallelPIView.h"

#include "MainFrm.h"

#include "ScreenImage.h"
#include "Base64Encoder.h"
#include "Gdiplusimaging.h"

#include "Shellapi.h"

#include <cmath>
#include <cctype>
#include <powrprof.h>
#include <ppl.h>
#include <thread>
#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" __declspec(dllexport) double __stdcall ParallelPI(int nDecimals, bool bMultiCore);

CString CMainFrame::strElapsedTime = L"";
CString CMainFrame::strResult = L"";
CString CMainFrame::strPI = L"";

CString CMainFrame::strPropertyName = L"Multi Core";
CString CMainFrame::strPropertyValue = L"YES";

bool CMainFrame::bPIDefault = FALSE;

int CMainFrame::nCounterPI = 0;

void DoEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_BUTTON_PI_BENCHMARK, &CMainFrame::OnButtonPiBenchmark)
	ON_COMMAND(ID_BUTTON_PI_DATAFILE, &CMainFrame::OnButtonPiDatafile)
	ON_COMMAND(ID_BUTTON_PI_HWBOT, &CMainFrame::OnButtonPiHwbot)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_AQUA);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowPos(NULL, 0, 0, 1010, 570, SWP_NOMOVE);

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Create a caption bar:
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties, AFX_IDW_DOCKBAR_LEFT);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	return CFrameWnd::PreCreateWindow(cs);
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 270, 0), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp;

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}


void CMainFrame::OnButtonPiBenchmark()
{
	BenchmarkPI(80000);
}


void CMainFrame::OnButtonPiDatafile()
{
	CreatePIXMLFile();
}


void CMainFrame::OnButtonPiHwbot()
{
	ShellExecuteW(NULL, L"open", L"http://hwbot.org/submit/benchmark/multi_threaded_pi", NULL, NULL, SW_SHOWNORMAL);
}


void CMainFrame::BenchmarkPI(int nDecimals)
{
	/*bool bMultiCore;
	if ((CMainFrame::strPropertyName == "Multi Core") && (CMainFrame::strPropertyValue == "YES"))
		bMultiCore = true;
	else
		bMultiCore = false;*/

	bool bMultiCore = true;

	CRichEditCtrl &m_wndOutput = ((CParallelPIView*)GetActiveView())->GetRichEditCtrl();

	CHARFORMAT charformat;
	charformat.cbSize = sizeof(CHARFORMAT);
	charformat.yHeight = 10 * 20;
	charformat.dwMask = CFM_FACE | CFM_CHARSET | CFM_BOLD | CFM_SIZE;
	charformat.bPitchAndFamily = DEFAULT_PITCH;
	charformat.dwEffects = 0;
	lstrcpy(charformat.szFaceName, _T("Consolas"));
	
	m_wndOutput.SetDefaultCharFormat(charformat);

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(CMainFrame::strPropertyName);

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(L": ");

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(CMainFrame::strPropertyValue);

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(L"\n\n");

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(L"===========================  PI Benchmark  ===========================\n");

	CMainFrame::nCounterPI++;

	CString strCounterPI = L"";
	CString strString = L"";
	CString strOutput = L"";

	strCounterPI.Format(_T("%d"), CMainFrame::nCounterPI);

	CString strStringLeft = L">----------------  Calculate PI with  ";
	CString strStringDecimals = L"";
	CString strStringRight = L" decimals ---------------";
	CString strSeparator = L"";

	strStringDecimals.Format(_T("%d"), nDecimals);

	TCHAR chSeparator[4];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, chSeparator, 4);
	strSeparator.Append(chSeparator);

	for (int nIdx = strStringDecimals.GetLength() - 3; nIdx > 0; nIdx -= 3)
	{
		strStringDecimals.Insert(nIdx, strSeparator);
	}

	strString.Append(strStringLeft);
	strString.Append(strStringDecimals);
	strString.Append(strStringRight);

	strOutput.Append(strCounterPI);
	strOutput.Append(strString);
	strOutput.Append(L"\n");

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(strOutput);

	strOutput = L"";

	if (bMultiCore == true)
	{
		strString = L">  Running multithreaded benchmark...";
	}
	else
	{
		strString = L">  Running singlethreaded benchmark...";
	}

	strOutput.Append(strCounterPI);
	strOutput.Append(strString);
	strOutput.Append(L"\n");

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(strOutput);

	m_wndCaptionBar.SetText(L"Running PI benchmark, please wait...", CMFCCaptionBar::ALIGN_LEFT);

	DoEvents();

	strMilliseconds = L"";
	strSeconds = L"";
	strMinutes = L"";

	dMilliseconds = ParallelPI(nDecimals, bMultiCore);
	nSeconds = (int)dMilliseconds / 1000;
	nMinutes = (int)nSeconds / 60;
	nMilliseconds = (int)dMilliseconds - nSeconds * 1000;

	strMilliseconds.Format(_T("%d"), nMilliseconds);

	CMainFrame::strResult = L"";
	CMainFrame::strResult.Format(_T("%d"), nSeconds);
	CMainFrame::strResult.Append(L".");

	if (nMilliseconds < 10)
	{
		CMainFrame::strResult.Append(_T("00"));
	}

	if ((nMilliseconds >= 10) && (nMilliseconds < 100))
	{
		CMainFrame::strResult.Append(_T("0"));
	}

	CMainFrame::strResult.Append(strMilliseconds);

	nSeconds = nSeconds - nMinutes * 60;

	strSeconds.Format(_T("%d"), nSeconds);
	strMinutes.Format(_T("%d"), nMinutes);

	CMainFrame::strElapsedTime = " ";
	CMainFrame::strElapsedTime += strMinutes;
	CMainFrame::strElapsedTime += " min ";
	CMainFrame::strElapsedTime += strSeconds;
	CMainFrame::strElapsedTime += " sec ";
	CMainFrame::strElapsedTime += strMilliseconds;
	CMainFrame::strElapsedTime += " ms";

	strOutput = L"";
	strString = L">  Done";
	strOutput.Append(strCounterPI);
	strOutput.Append(strString);
	strOutput.Append(L"\n");

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(strOutput);

	strOutput = L"";
	strString = L">  Ellapsed time: ";
	strOutput.Append(strCounterPI);
	strOutput.Append(strString);
	strOutput.Append(CMainFrame::strElapsedTime);
	strOutput.Append(L"\n");

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(strOutput);

	m_wndOutput.SetSel(-1, -1);
	m_wndOutput.ReplaceSel(L"========================  Benchmark succeeded ========================\n\n");

	m_wndCaptionBar.SetText(L"PI benchmark completed", CMFCCaptionBar::ALIGN_LEFT);

	if (nDecimals == 80000)
	{
		CMainFrame::bPIDefault = TRUE;
	}
	else
	{
		CMainFrame::bPIDefault = FALSE;
	}

	m_wndOutput.SetModify(FALSE);
}

void CMainFrame::CreatePIXMLFile()
{
	if (CMainFrame::bPIDefault == FALSE)
	{
		MessageBoxEx(GetSafeHwnd(), L"You have to run the benchmark with 80000 decimals for HWBOT submissions", L"Multi-Core PI", MB_OK | MB_ICONEXCLAMATION, 0);

		return;
	}

	if (FindWindow(NULL, L"CPU-Z ") == 0)
	{
		MessageBoxEx(GetSafeHwnd(), L"CPU-Z is not running! \n\nPlease run CPU-Z (this operation is required for datafile creation)", L"Multi-Core PI", MB_OK | MB_ICONEXCLAMATION, 0);

		return;
	}

	CScreenImage m_image;

	try
	{
		m_image.CaptureScreen();
		m_image.Save(L"MultiCorePIScreenShot.jpeg", Gdiplus::ImageFormatJPEG);
	}
	catch (...)
	{
		MessageBoxEx(GetSafeHwnd(), L"There was an error taking the screenshot (this operation is required for datafile creation). \n\nThe datafile could not be created.", L"Multi-Core PI", MB_OK | MB_ICONSTOP, 0);

		return;
	}

	try
	{
		CString strChar = L"";
		CString strFile = L"";
		strImageBase64 = L"";

		FILE * pFile;
		size_t result;
		errno_t err;

		if ((err = fopen_s(&pFile, "MultiCorePIScreenShot.jpeg", "rb")) != 0) return;

		fseek(pFile, 0, SEEK_END);
		long lSize = ftell(pFile);
		rewind(pFile);

		unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char)*lSize);
		if (data != NULL)
		{
			result = fread(data, 1, lSize, pFile);
			if (result == lSize)
			{
				for (int i = 0; i <= lSize; i++)
				{
					strChar = L"";
					strChar.Format(_T("%c"), data[i]);
					strFile += strChar;
				}
			}
		}

		fclose(pFile);

		aoBase64Encoder encoder;
		encoder.Encode(data, lSize);

		int encodedCopySize = encoder.GetEncodedSize();
		char* encodedCopy = new char[encodedCopySize];
		memcpy(encodedCopy, encoder.GetEncoded(), sizeof(unsigned char) * encodedCopySize);

		for (int i = 0; i <= encodedCopySize; i++)
		{
			strChar = L"";
			strChar.Format(_T("%c"), encodedCopy[i]);
			strImageBase64 += strChar;
		}

		delete[] data;
	}
	catch (...)
	{
		MessageBoxEx(GetSafeHwnd(), L"There was an error converting the screenshot from binary to Base64 (this operation is required for datafile creation). \n\nThe datafile could not be created.", L"Multi-Core PI", MB_OK | MB_ICONSTOP, 0);

		return;
	}

	try
	{
		FILE *f;
		errno_t err;
		CString strTime = CTime::GetCurrentTime().Format("%H:%M:%S");

		if ((err = fopen_s(&f, "Shell32CorePIAPI.xml", "w+")) == 0)
		{
			_fputts(L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n", f);
			_fputts(L"<submission xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"  xmlns=\"http://hwbot.org/submit/api\">\n", f);
			_fputts(L"<application>\n", f);
			_fputts(L"<name>", f);
			_fputts(L"MULTI CORE PI", f);
			_fputts(L"</name>\n", f);
			_fputts(L"<version>", f);
			_fputts(strVersion, f);
			_fputts(L"</version>\n", f);
			_fputts(L"</application>\n", f);
			_fputts(L"<score>\n", f);
			_fputts(L"<points>", f);
			_fputts(strResult, f);
			_fputts(L"</points>\n", f);
			_fputts(L"<subscores>\n", f);
			_fputts(L"<subscore name=\"\">\n", f);
			_fputts(L"</subscore>\n", f);
			_fputts(L"</subscores>\n", f);
			_fputts(L"</score>\n", f);
			_fputts(L"<timestamp>", f);
			_fputts(strTime, f);
			_fputts(L"</timestamp>\n", f);
			_fputts(L"<screenshot contentType=\"image/jpg\">\n", f);
			_fputts(strImageBase64, f);
			_fputts(L"\n</screenshot>\n", f);
			_fputts(L"<verificationUrl></verificationUrl>\n", f);
			_fputts(L"<applicationChecksum></applicationChecksum>\n", f);
			_fputts(L"<hardware>\n", f);
			_fputts(L"<processor>\n", f);
			_fputts(L"<name>", f);
			//_fputts( strCPU, f );
			_fputts(L"</name>\n", f);
			_fputts(L"<amount></amount>\n", f);
			_fputts(L"<idleTemp></idleTemp>\n", f);
			_fputts(L"<loadTemp></loadTemp>\n", f);
			_fputts(L"<coreClock>", f);
			//_fputts( strFrequency, f );
			_fputts(L"</coreClock>\n", f);
			_fputts(L"</processor>\n", f);
			_fputts(L"<videocard>\n", f);
			_fputts(L"<name>", f);
			//_fputts( strGPU, f );
			_fputts(L"</name>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<amount></amount>\n", f);
			_fputts(L"</videocard>\n", f);
			_fputts(L"<motherboard>\n", f);
			_fputts(L"<name></name>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<chipset></chipset>\n", f);
			_fputts(L"</motherboard>\n", f);
			_fputts(L"<memory>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<type></type>\n", f);
			_fputts(L"<rating></rating>\n", f);
			_fputts(L"<totalSize></totalSize>\n", f);
			_fputts(L"</memory>\n", f);
			_fputts(L"<disk>\n", f);
			_fputts(L"<vendor></vendor>\n", f);
			_fputts(L"<amount></amount>\n", f);
			_fputts(L"<diskCapInGB></diskCapInGB>\n", f);
			_fputts(L"<series></series>\n", f);
			_fputts(L"<configuration></configuration>\n", f);
			_fputts(L"</disk>\n", f);
			_fputts(L"</hardware>\n", f);
			_fputts(L"<software>\n", f);
			_fputts(L"<os>\n", f);
			_fputts(L"<family>Windows</family>\n", f);
			_fputts(L"<fullName></fullName>\n", f);
			_fputts(L"</os>\n", f);
			_fputts(L"</software>\n", f);
			_fputts(L"<metadata name=\"\"> </metadata> \n", f);
			_fputts(L"</submission>\n", f);

			fclose(f);
		}
	}
	catch (...)
	{
		MessageBoxEx(GetSafeHwnd(), L"There was an error creating the .XML datafile", L"Multi-Core PI", MB_OK | MB_ICONSTOP, 0);

		return;
	}

	try
	{
		TCHAR szDirectory[MAX_PATH] = L"";
		::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory);

		CString strPath = L"";
		strPath.Append(szDirectory);
		strPath.Append(L"\\Shell32CorePIAPI.xml");

		m_wndCaptionBar.SetText(L"The datafile <MultiCorePISubmission.xml> was created successfully", CMFCCaptionBar::ALIGN_LEFT);

		ShellExecuteW(NULL, L"open", L"openssl.exe", L"aes-128-cbc -K 4228768326CC48357B8734BD061C52DF -iv C7A5EA40CA9465F9175FE490902C43CD -in Shell32CorePIAPI.xml -out MultiCorePISubmission.xml", NULL, SW_HIDE);

		MessageBoxEx(GetSafeHwnd(), L"The datafile <MultiCorePISubmission.xml> was created successfully.\n\nYou can use this datafile to submit your results to HWBOT", L"Multi-Core PI", MB_OK | MB_ICONINFORMATION, 0);

		ShellExecuteW(NULL, L"open", szDirectory, NULL, NULL, SW_SHOWNORMAL);

	}
	catch (...) {}
}

CString CMainFrame::GetCPU()
{
	try
	{
		int CPUInfo[4] = { -1 };
		__cpuid(CPUInfo, 0x80000000);
		unsigned int nExIds = CPUInfo[0];

		char CPUBrandString[0x40] = { 0 };
		for (unsigned int i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);

			if (i == 0x80000002)
			{
				memcpy(CPUBrandString,
					CPUInfo,
					sizeof(CPUInfo));
			}
			else if (i == 0x80000003)
			{
				memcpy(CPUBrandString + 16,
					CPUInfo,
					sizeof(CPUInfo));
			}
			else if (i == 0x80000004)
			{
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
			}
		}

		std::string strCPUFormatat = CPUBrandString;
		strCPUFormatat.erase(strCPUFormatat.begin(), std::find_if(strCPUFormatat.begin(), strCPUFormatat.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));

		CString strCPU = L"";
		strCPU += strCPUFormatat.c_str();

		return strCPU;
	}
	catch (...) {}
}

float CMainFrame::GetCPUFrequency()
{
	try
	{
		typedef struct _PROCESSOR_POWER_INFORMATION
		{
			ULONG  Number;
			ULONG  MaxMhz;
			ULONG  CurrentMhz;
			ULONG  MhzLimit;
			ULONG  MaxIdleState;
			ULONG  CurrentIdleState;
		}
		PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

		#pragma comment(lib, "Powrprof.lib")

		// get the number or processors 
		SYSTEM_INFO si = { 0 };
		::GetSystemInfo(&si);

		// allocate buffer to get info for each processor
		const int size = si.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
		LPBYTE pBuffer = new BYTE[size];

		NTSTATUS status = ::CallNtPowerInformation(ProcessorInformation, NULL, 0, pBuffer, size);
		try
		{
			// list each processor frequency 
			PPROCESSOR_POWER_INFORMATION ppi = (PPROCESSOR_POWER_INFORMATION)pBuffer;
			for (DWORD nIndex = 0; nIndex < si.dwNumberOfProcessors; nIndex++)
			{
				return ppi->CurrentMhz;

				ppi++;
			}
		}
		catch (...)
		{
			return 0;
		}

		delete[]pBuffer;
	}
	catch (...)
	{
		return 0;
	}
}