// 2DSpriteEditorView.h : interface of the CSpriteEditorView class
//
#include "OpenGLWnd.h"

#pragma once


class CSpriteEditorView : public COpenGLWnd
{
protected: // create from serialization only
	CSpriteEditorView();
	DECLARE_DYNCREATE(CSpriteEditorView)

// Attributes
	float m_zoom,
		  m_xpos,
		  m_ypos,
		  m_xrot,
		  m_yrot;
	int	m_lastMouseX,
		m_lastMouseY;
	bool m_isPrinting;

public:
	CSpriteEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void RenderScene( );
	void Render( CSpriteEditorDoc *pDoc );
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void DrawSpriteBox( );
	afx_msg LRESULT OnDrawNextFrame( WPARAM wParam, LPARAM lParam );
// Implementation
public:
	virtual ~CSpriteEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in 2DSpriteEditorView.cpp
inline CSpriteEditorDoc* CSpriteEditorView::GetDocument() const
   { return reinterpret_cast<CSpriteEditorDoc*>(m_pDocument); }
#endif

