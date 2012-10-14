#pragma once


// CCoolButton

class CCoolButton : public CButton
{
	DECLARE_DYNAMIC(CCoolButton)

public:
	CCoolButton();
	virtual ~CCoolButton();
	
protected:
	DECLARE_MESSAGE_MAP()

	BOOL m_bOverControl;
	UINT m_nTimerID;
	CBitmap m_NormalBitmap, m_OverBitmap, m_SelectedBitmap;
	UINT m_BitmapWidth, m_BitmapHeight;
	CDC m_MemoryDC;
	CRect m_rcBounds;

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
protected:
	virtual void PreSubclassWindow();
public:
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


