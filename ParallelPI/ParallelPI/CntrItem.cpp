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

// CntrItem.cpp : implementation of the CParallelPICntrItem class
//

#include "stdafx.h"
#include "ParallelPI.h"

#include "ParallelPIDoc.h"
#include "ParallelPIView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CParallelPICntrItem implementation

IMPLEMENT_SERIAL(CParallelPICntrItem, CRichEditCntrItem, 0)

CParallelPICntrItem::CParallelPICntrItem(REOBJECT* preo, CParallelPIDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
}

CParallelPICntrItem::~CParallelPICntrItem()
{
	// TODO: add cleanup code here
}


// CParallelPICntrItem diagnostics

#ifdef _DEBUG
void CParallelPICntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CParallelPICntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

