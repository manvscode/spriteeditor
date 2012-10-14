#pragma once


// CCoolListCtrl

class CCoolListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CCoolListCtrl)

public:
	CCoolListCtrl();
	virtual ~CCoolListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL m_bOverControl;
	UINT m_nTimerID;
	BOOL m_bColor;

	BOOL m_bInFocus;


	CImageList* m_pDragImage;
	BOOL m_bDragging;
	int m_nDragIndex, m_nDropIndex;
	CWnd* m_pDropWnd;
	CPoint m_ptDropPoint;
	void DropFrameItemOnList( );


	void DrawTheLines( int Index );
	void InsertDraggedItem( );
	void DoTheScrolling( CPoint Point,CRect ClientRect );

	enum {
		TID_SCROLLDOWN = 100,
		TID_SCROLLUP = 101
	};

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

//	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg void OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdstatechanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
public:

public:
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
public:
//	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
};


