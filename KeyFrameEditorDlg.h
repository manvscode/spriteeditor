#pragma once

#include "CoolButton.h"
#include "CoolListCtrl.h"

#include "afxcmn.h"
#include "SpriteEditorDoc.h"

class CSpriteEditorDoc;
// CKeyFrameEditorDlg dialog

class CKeyFrameEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CKeyFrameEditorDlg)

public:
	CKeyFrameEditorDlg(CWnd* pParent , CSpriteEditorDoc *pDoc = NULL);
	CKeyFrameEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKeyFrameEditorDlg();

// Dialog Data
	enum { IDD = IDD_KEYFRAMEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support




	enum { IDOK_BTN = 0, IDCANCEL_BTN = 1, IDREMOVEKEYFRAME_BTN = 2 };
	CCoolButton m_Buttons[ 3 ];
	BOOL m_bInitButtons;

	CImageList m_ImageBitmaps;
	/*CCoolListCtrl*/CListCtrl m_KeyFrameList;
	CSpriteEditorDoc *m_pDoc;

	void makeKeyFrameBitmap( CBitmap *bmp, const CSpriteEditorDoc::KeyFrames &kf );


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedRemovekeyframe();
public:
//	afx_msg void OnLvnItemchangedKeyframelist(NMHDR *pNMHDR, LRESULT *pResult);
public:
//	afx_msg void OnLvnSetdispinfoKeyframelist(NMHDR *pNMHDR, LRESULT *pResult);
public:
//	afx_msg void OnLvnItemchangingKeyframelist(NMHDR *pNMHDR, LRESULT *pResult);
public:
//	afx_msg void OnLvnSetdispinfoKeyframelist(NMHDR *pNMHDR, LRESULT *pResult);
public:
//	afx_msg void OnLvnBeginlabeleditKeyframelist(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnLvnEndlabeleditKeyframelist(NMHDR *pNMHDR, LRESULT *pResult);
};
