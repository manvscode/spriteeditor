// MainFrm.h : interface of the CMainFrame class
//


#pragma once
class CSpriteEditorView;
class CSpritePropertiesForm;

#define TOOLBARICON_SIZE	25
#define SPRITEFORM_WIDTH	245


class CMainFrame : public CFrameWnd
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
	void setStatusBar( UINT statusPane, const CString &str );
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	typedef enum tagViewType {
		MAIN_VIEW,
		FORM_VIEW
	} ViewType;
	
protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	ViewType m_CurrentView;
	CSpriteEditorView *m_pMainView;
	CSpritePropertiesForm *m_pFormView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL m_initSplitters;
	CSplitterWnd m_wndSplitter;
	CImageList m_ilToolBar;
	

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void setView(ViewType v);
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
public:
	afx_msg void OnHelpMakeadonation();
public:
	afx_msg void OnHelpOthergreatprograms();
public:
	afx_msg void OnAnimatePlay();
public:
	afx_msg void OnAnimateStop();
};


