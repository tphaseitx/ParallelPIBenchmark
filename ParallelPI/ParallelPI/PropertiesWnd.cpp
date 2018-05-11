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

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ParallelPI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;

	CMainFrame::strPropertyName = pProp->GetName();
	CMainFrame::strPropertyValue = pProp->GetValue().bstrVal;

	return 0;
}

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), rectClient.Height() - m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroupPI = new CMFCPropertyGridProperty(_T("PI"));

	CMFCPropertyGridProperty *pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Multi Threaded PI"));
	pProp->Enable(FALSE);
	pGroupPI->AddSubItem(pProp);

	/*CMFCPropertyGridProperty* pDecimals = new CMFCPropertyGridProperty(_T("Decimals"), _T("80000"), _T("Specifies the number of decimals for PI. Default setting for HWBOT submission is 80000."));
	pDecimals->AddOption(_T("10000"));
	pDecimals->AddOption(_T("20000"));
	pDecimals->AddOption(_T("30000"));
	pDecimals->AddOption(_T("40000"));
	pDecimals->AddOption(_T("50000"));
	pDecimals->AddOption(_T("60000"));
	pDecimals->AddOption(_T("70000"));
	pDecimals->AddOption(_T("80000"));
	pDecimals->AllowEdit(FALSE);
	pGroupPI->AddSubItem(pDecimals);*/

	pProp = new CMFCPropertyGridProperty(_T("Decimals"), _T("80000"));
	pProp->Enable(FALSE);
	pGroupPI->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Library"), _T("PPL (Parallel Patterns Library)"));
	pProp->Enable(FALSE);
	pGroupPI->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Multi Core"), _T("YES"), _T("Enable parallelization"));
	/*pProp->AddOption(_T("YES"));
	pProp->AddOption(_T("NO"));
	pProp->AllowEdit(FALSE);*/
	pProp->Enable(FALSE);
	pGroupPI->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroupPI);

	/*CMFCPropertyGridProperty* pGroupLINPACK = new CMFCPropertyGridProperty(_T("LINPACK"));

	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Multi Threaded LINPACK"));
	pProp->Enable(FALSE);
	pGroupLINPACK->AddSubItem(pProp);

	//CMFCPropertyGridProperty* pMatrixSize = new CMFCPropertyGridProperty(_T("Matrix size"), _T("4000"), _T("Specifies the size of the Matrix. Default setting for HWBOT submission is 4000."));
	//pMatrixSize->AddOption(_T("1000"));
	//pMatrixSize->AddOption(_T("2000"));
	//pMatrixSize->AddOption(_T("3000"));
	//pMatrixSize->AddOption(_T("4000"));
	//pMatrixSize->AllowEdit(FALSE);
	//pGroupLINPACK->AddSubItem(pMatrixSize);

	pProp = new CMFCPropertyGridProperty(_T("Matrix size"), _T("4000"));
	pProp->Enable(FALSE);
	pGroupLINPACK->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Library"), _T("PPL (Parallel Patterns Library)"));
	pProp->Enable(FALSE);
	pGroupLINPACK->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("Multi Core"), _T("YES"), _T("Enable parallelization"));
	//pProp->AddOption(_T("YES"));
	//pProp->AddOption(_T("NO"));
	//pProp->AllowEdit(FALSE);
	pProp->Enable(FALSE);
	pGroupLINPACK->AddSubItem(pProp);

	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Matrix size"), (_variant_t)_T("1000"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroupLINPACK);*/

	CMFCPropertyGridProperty* pGroupSYSINFO = new CMFCPropertyGridProperty(_T("SYSTEM INFO"));

	try
	{
		pProp = new CMFCPropertyGridProperty(_T("CPU"), CMainFrame::GetCPU());
		pProp->Enable(FALSE);
		pGroupSYSINFO->AddSubItem(pProp);

		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

		int nCPUCores = sysinfo.dwNumberOfProcessors;

		CString strCPUCores = L"";
		strCPUCores.Format(_T(" %d"), nCPUCores);

		pProp = new CMFCPropertyGridProperty(_T("Cores"), strCPUCores);
		pProp->Enable(FALSE);
		pGroupSYSINFO->AddSubItem(pProp);

		CString strFrequency = L"";
		strFrequency.Format(_T(" %.0f"), CMainFrame::GetCPUFrequency());
		strFrequency += " Mhz";

		pProp = new CMFCPropertyGridProperty(_T("Frequency"), strFrequency);
		pProp->Enable(FALSE);
		pGroupSYSINFO->AddSubItem(pProp);
	}
	catch (...) {}

	m_wndPropList.AddProperty(pGroupSYSINFO);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}
