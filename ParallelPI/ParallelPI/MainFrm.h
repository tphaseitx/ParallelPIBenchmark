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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "PropertiesWnd.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();
public:
	afx_msg void OnButtonPiBenchmark();

public:
	CStatic m_staticElapsedTime;
	CStatic m_staticCPUCores;
	CStatic m_staticCPU;

	CString strTitle;
	CString strEndStatus;

	static CString strElapsedTime;
	static CString strResult;
	static CString strPI;

	static bool bPIDefault;

	CString strMilliseconds;
	CString strSeconds;
	CString strMinutes;

	CString strCPU;
	CString strGPU;
	CString strFrequency;
	CString strVersion;
	CString strEXE;

	CString strImageBase64;

	double dMilliseconds;
	int nSeconds;
	int nMinutes;
	int nMilliseconds;

	static int nCounterPI;

	void BenchmarkPI(int nDecimals);
	void CreatePIXMLFile();

	static CString GetCPU();
	static float GetCPUFrequency();

	static CString strPropertyName;
	static CString strPropertyValue;

	afx_msg void OnButtonPiDatafile();
	afx_msg void OnButtonPiHwbot();
};


