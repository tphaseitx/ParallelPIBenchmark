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

// ParallelPIView.cpp : implementation of the CParallelPIView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ParallelPI.h"
#endif

#include "ParallelPIDoc.h"
#include "ParallelPIView.h"

#include "CntrItem.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CParallelPIView

IMPLEMENT_DYNCREATE(CParallelPIView, CRichEditView)

BEGIN_MESSAGE_MAP(CParallelPIView, CRichEditView)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CParallelPIView construction/destruction

CParallelPIView::CParallelPIView()
{
	// TODO: add construction code here

}

CParallelPIView::~CParallelPIView()
{
}

BOOL CParallelPIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

void CParallelPIView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();


	// Set the printing margins (720 twips = 1/2 inch)
	SetMargins(CRect(720, 720, 720, 720));
}

void CParallelPIView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
	{
		pActiveItem->Deactivate();
		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}
	CRichEditView::OnDestroy();
}


void CParallelPIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CParallelPIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CParallelPIView diagnostics

#ifdef _DEBUG
void CParallelPIView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CParallelPIView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CParallelPIDoc* CParallelPIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CParallelPIDoc)));
	return (CParallelPIDoc*)m_pDocument;
}
#endif //_DEBUG


// CParallelPIView message handlers
