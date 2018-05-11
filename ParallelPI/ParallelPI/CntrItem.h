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

// CntrItem.h : interface of the CParallelPICntrItem class
//

#pragma once

class CParallelPIDoc;
class CParallelPIView;

class CParallelPICntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CParallelPICntrItem)

// Constructors
public:
	CParallelPICntrItem(REOBJECT* preo = NULL, CParallelPIDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CParallelPIDoc* GetDocument()
		{ return reinterpret_cast<CParallelPIDoc*>(CRichEditCntrItem::GetDocument()); }
	CParallelPIView* GetActiveView()
		{ return reinterpret_cast<CParallelPIView*>(CRichEditCntrItem::GetActiveView()); }

// Implementation
public:
	~CParallelPICntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

