// CoolEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "CoolEdit.h"


// CCoolEdit

IMPLEMENT_DYNAMIC(CCoolEdit, CEdit)

CCoolEdit::CCoolEdit()
{

}

CCoolEdit::~CCoolEdit()
{
}


BEGIN_MESSAGE_MAP(CCoolEdit, CEdit)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// CCoolEdit message handlers



void CCoolEdit::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	CEdit::OnDrawItem(nIDCtl, lpDrawItemStruct);

}

void CCoolEdit::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CEdit::PreSubclassWindow();

	//ModifyStyle( 0, BS_OWNERDRAW );
}

HBRUSH CCoolEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	
	DeleteObject( hbr );

	hbr = CreateSolidBrush( RGB(255,20,20) );

	return hbr;
}
