#pragma once


// CCoolEdit

class CCoolEdit : public CEdit
{
	DECLARE_DYNAMIC(CCoolEdit)

public:
	CCoolEdit();
	virtual ~CCoolEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};


