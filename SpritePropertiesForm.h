#pragma once
#include "afxmt.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "CoolEdit.h"
#include "CoolButton.h"
#include "CoolListCtrl.h"
#include "SpritePlaybackThread.h"
#include <afxcmn.h>

#define THUMBNAIL_SIZE		40


// CSpritePropertiesForm form view

class CSpritePropertiesForm : public CFormView
{
	DECLARE_DYNCREATE(CSpritePropertiesForm)

protected:
	CSpritePropertiesForm();           // protected constructor used by dynamic creation
	virtual ~CSpritePropertiesForm();

public:
	enum { IDD = IDD_SPRITEPROPERTIESFORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	typedef struct tagSpriteAnimateParams {
		HWND hWnd;
		UINT uiPlaybackSpeed;
		CSpritePropertiesForm *pView;
	} SpriteAnimateParams;

	SpriteAnimateParams m_SpriteAnimationParams;

protected:
	CString m_Name;
	CImageList m_ImageList;
	CToolTipCtrl m_Tooltips;


	CCoolListCtrl m_FrameListCtrl;

	CString m_szSequenceName;
	UINT m_uiStartFrame;
	UINT m_uiEndFrame;


	UINT m_uiPlaybackSpeed;
	CWinThread *m_pSpritePlaybackThread;
	static UINT animateSprite( LPVOID p );
	void animateSprite(  const HWND hWnd, const UINT speed );
	volatile BOOL m_isAnimatingSprite;
	
	BOOL m_bInitSpriteThread;

	CComboBox m_ResamplingComboBox;
	BOOL m_bCreateTextureImage;
	BOOL m_bCreateSpriteFile;
	UINT m_uiTextureWidth;
	UINT m_uiTextureHeight;	

	CCoolButton m_CoolButtons[ 9 ];
	BOOL m_bInitButtons;
	BOOL m_bInitialized;

	CSemaphore m_Semaphore;

	// members for dragging frames on the list control...
	CImageList* m_pDragImage;
	BOOL m_bDragging;
	int m_nDragIndex, m_nDropIndex;
	CWnd* m_pDropWnd;
	CPoint m_ptDropPoint;
	void DropFrameItemOnList( );

	typedef enum tagImageType {
		IMG_TARGA,
		IMG_BITMAP
	} ImageType;

	typedef enum tagResamplingMethod {
		RES_NEARESTNEIGHBOR,
		RES_BILINEAR,
		RES_BILINEAR_SHARPER,
		RES_BICUBIC
	} ResamplingMethod;

	typedef struct tagSpriteExportInfo {
		unsigned int frameWidth;
		unsigned int frameHeight;
		unsigned int numberOfFrames;
		unsigned int numberOfStates;
	} SpriteExportInfo;

	bool getImageType( CString &extension, ImageType *type );
	void makeScreenshotBitmap( const CSpriteEditorDoc::ImageBitmap &img, CBitmap &screenshot );

	bool loadImage( ImageType type, CSpriteEditorDoc::ImageBitmap &img, const char *filename );
	bool loadTargaImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename );
	bool loadBitmapImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename );
	bool saveImage( ImageType type, CSpriteEditorDoc::ImageBitmap &img, const char *filename );
	bool saveTargaImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename );
	bool saveBitmapImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename );
	void postProcessFrames( CSpriteEditorDoc::ImageBitmap &img, const ResamplingMethod &method, SpriteExportInfo &info );

	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedAddframebutton();
	afx_msg void OnBnClickedRemoveframebutton();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClickFramelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownFramelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPlaybutton();
	afx_msg void OnBnClickedStopbutton();
	afx_msg void OnBnClickedStepleftbutton();
	afx_msg void OnBnClickedSteprightbutton();
	afx_msg void OnBnClickedExportbutton();
	afx_msg void OnLvnItemchangedFramelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSetkeyframebutton();
	afx_msg void OnBnClickedEditkeyframebutton();
	afx_msg void OnLvnBegindragFramelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeNameedit();
	afx_msg void OnEnChangePlaybackedit();
	afx_msg void OnEnChangeTexturewidthedit();
	afx_msg void OnEnChangeTextureheightedit();
	afx_msg void OnEnChangeSequencenameedit();
	afx_msg void OnEnChangeStartframeedit();
	afx_msg void OnEnChangeEndframeedit();
	afx_msg void OnLvnEndScrollFramelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCreatetextureimagecheck();
	afx_msg void OnBnClickedCreatespritefilecheck();

public:
	afx_msg void OnAnimatePlay();
public:
	afx_msg void OnAnimateStop();
};


