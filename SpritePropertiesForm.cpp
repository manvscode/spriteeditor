// SpritePropertiesForm.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "SpritePropertiesForm.h"
#include "SpriteEditorDoc.h"
#include "MainFrm.h"
#include "SpriteEditorView.h"
#include "KeyFrameEditorDlg.h"
#include "messages.h"
#include <sstream>
#include <climits>
#include <cmath>
#include <gl/gl.h>
#include <afxpriv.h>
#include "Sprite/SpriteFile.h"

// CSpritePropertiesForm

IMPLEMENT_DYNCREATE(CSpritePropertiesForm, CFormView)

CSpritePropertiesForm::CSpritePropertiesForm()
	: CFormView(CSpritePropertiesForm::IDD)
	, m_Name(_T("Untitled"))
	, m_ImageList( )
	, m_szSequenceName(_T(""))
	, m_uiStartFrame(1)
	, m_uiEndFrame(1)
	, m_uiPlaybackSpeed(100)
	, m_pSpritePlaybackThread( NULL )
	, m_bCreateTextureImage(FALSE)
	, m_bCreateSpriteFile(FALSE)
	, m_bInitButtons(FALSE)
	, m_bInitSpriteThread(FALSE)
	, m_isAnimatingSprite(FALSE)
	, m_Semaphore( 1, 1 )
	, m_bInitialized(FALSE)
	, m_pDragImage(NULL)
	, m_bDragging(FALSE)
	, m_nDragIndex(0)
	, m_nDropIndex(0)
	, m_pDropWnd(NULL)
	, m_ptDropPoint()
	, m_uiTextureWidth(1024)
	, m_uiTextureHeight(1024)
	, m_Tooltips()
{
	m_ImageList.Create( THUMBNAIL_SIZE, THUMBNAIL_SIZE, ILC_COLOR24 | ILC_MASK, 0, 0 );
	m_Tooltips.Create(this);

	m_SpriteAnimationParams.uiPlaybackSpeed = 100;
	m_SpriteAnimationParams.hWnd = NULL;
	m_SpriteAnimationParams.pView = NULL;


}

CSpritePropertiesForm::~CSpritePropertiesForm()
{
	// kill thread...
	m_isAnimatingSprite = FALSE;

	if( m_pSpritePlaybackThread != NULL )
	{
		m_isAnimatingSprite = FALSE;
		//WaitForSingleObject( m_pSpritePlaybackThread->m_hThread, INFINITE );
		//delete m_pSpritePlaybackThread;
	}
	m_ImageList.DeleteImageList( );

}

void CSpritePropertiesForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAMEEDIT, m_Name);
	DDV_MaxChars(pDX, m_Name, 128);

	DDX_Text(pDX, IDC_SEQUENCENAMEEDIT, m_szSequenceName);
	DDV_MaxChars(pDX, m_szSequenceName, 128);
	DDX_Check(pDX, IDC_CREATETEXTUREIMAGECHECK, m_bCreateTextureImage);
	DDX_Check(pDX, IDC_CREATESPRITEFILECHECK, m_bCreateSpriteFile);
	DDX_Text(pDX, IDC_STARTFRAMEEDIT, m_uiStartFrame);
	DDV_MinMaxUInt(pDX, m_uiStartFrame, 1, 1000);
	DDX_Text(pDX, IDC_ENDFRAMEEDIT, m_uiEndFrame);
	DDV_MinMaxUInt(pDX, m_uiEndFrame, 1, 1000);
	DDX_Text(pDX, IDC_PLAYBACKEDIT, m_uiPlaybackSpeed);
	DDV_MinMaxUInt(pDX, m_uiPlaybackSpeed, 10, 5000);
	DDX_Control(pDX, IDC_FRAMELIST, m_FrameListCtrl);
	DDX_Text(pDX, IDC_TEXTUREWIDTHEDIT, m_uiTextureWidth);
	DDV_MinMaxUInt(pDX, m_uiTextureWidth, 1, 4096);
	DDX_Text(pDX, IDC_TEXTUREHEIGHTEDIT, m_uiTextureHeight);
	DDV_MinMaxUInt(pDX, m_uiTextureHeight, 1, 4096);
	DDX_Control(pDX, IDC_RESAMPLINGCOMBO, m_ResamplingComboBox);
}

BEGIN_MESSAGE_MAP(CSpritePropertiesForm, CFormView)
	ON_BN_CLICKED(IDC_ADDFRAMEBUTTON, &CSpritePropertiesForm::OnBnClickedAddframebutton)
	ON_BN_CLICKED(IDC_REMOVEFRAMEBUTTON, &CSpritePropertiesForm::OnBnClickedRemoveframebutton)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_KEYDOWN, IDC_FRAMELIST, &CSpritePropertiesForm::OnLvnKeydownFramelist)
	ON_BN_CLICKED(IDC_PLAYBUTTON, &CSpritePropertiesForm::OnBnClickedPlaybutton)
	ON_BN_CLICKED(IDC_STOPBUTTON, &CSpritePropertiesForm::OnBnClickedStopbutton)
	ON_BN_CLICKED(IDC_STEPLEFTBUTTON, &CSpritePropertiesForm::OnBnClickedStepleftbutton)
	ON_BN_CLICKED(IDC_STEPRIGHTBUTTON, &CSpritePropertiesForm::OnBnClickedSteprightbutton)
	ON_BN_CLICKED(IDC_EXPORTBUTTON, &CSpritePropertiesForm::OnBnClickedExportbutton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FRAMELIST, &CSpritePropertiesForm::OnLvnItemchangedFramelist)
	ON_BN_CLICKED(IDC_SETKEYFRAMEBUTTON, &CSpritePropertiesForm::OnBnClickedSetkeyframebutton)
	ON_BN_CLICKED(IDC_EDITKEYFRAMEBUTTON, &CSpritePropertiesForm::OnBnClickedEditkeyframebutton)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_FRAMELIST, &CSpritePropertiesForm::OnLvnBegindragFramelist)
	ON_EN_CHANGE(IDC_NAMEEDIT, &CSpritePropertiesForm::OnEnChangeNameedit)
	ON_EN_CHANGE(IDC_PLAYBACKEDIT, &CSpritePropertiesForm::OnEnChangePlaybackedit)
	ON_EN_CHANGE(IDC_SEQUENCENAMEEDIT, &CSpritePropertiesForm::OnEnChangeSequencenameedit)
	ON_EN_CHANGE(IDC_STARTFRAMEEDIT, &CSpritePropertiesForm::OnEnChangeStartframeedit)
	ON_EN_CHANGE(IDC_ENDFRAMEEDIT, &CSpritePropertiesForm::OnEnChangeEndframeedit)
	ON_NOTIFY(LVN_ENDSCROLL, IDC_FRAMELIST, &CSpritePropertiesForm::OnLvnEndScrollFramelist)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE(IDC_TEXTUREWIDTHEDIT, &CSpritePropertiesForm::OnEnChangeTexturewidthedit)
	ON_EN_CHANGE(IDC_TEXTUREHEIGHTEDIT, &CSpritePropertiesForm::OnEnChangeTextureheightedit)
	//ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT,0,0xFFFF,OnToolTipNotify)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify) 
	ON_BN_CLICKED(IDC_CREATETEXTUREIMAGECHECK, &CSpritePropertiesForm::OnBnClickedCreatetextureimagecheck)
	ON_BN_CLICKED(IDC_CREATESPRITEFILECHECK, &CSpritePropertiesForm::OnBnClickedCreatespritefilecheck)
	ON_COMMAND(ID_ANIMATE_PLAY, &CSpritePropertiesForm::OnAnimatePlay)
	ON_COMMAND(ID_ANIMATE_STOP, &CSpritePropertiesForm::OnAnimateStop)
END_MESSAGE_MAP()


// CSpritePropertiesForm diagnostics

#ifdef _DEBUG
void CSpritePropertiesForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSpritePropertiesForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSpritePropertiesForm message handlers


void CSpritePropertiesForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	EnableToolTips( TRUE );

	m_FrameListCtrl.SetImageList( &m_ImageList, LVSIL_SMALL );
	ASSERT( m_FrameListCtrl.GetImageList( LVSIL_SMALL ) == &m_ImageList );
	m_FrameListCtrl.SetIconSpacing( 5, 5 );
	
	// Delete all items in listctrl and set up the controls...
	//m_FrameListCtrl.DeleteAllItems( );
	//m_Name = "Untitled";

	//while( m_ImageList.GetImageCount( ) > 0 )
	//	m_ImageList.Remove( 0 );


		// Set the CCoolButton look onto all of the buttons...
	if( !m_bInitButtons )
	{
		m_CoolButtons[ 0 ].SubclassDlgItem( IDC_ADDFRAMEBUTTON, this );
		m_CoolButtons[ 1 ].SubclassDlgItem( IDC_REMOVEFRAMEBUTTON, this );
		m_CoolButtons[ 2 ].SubclassDlgItem( IDC_PLAYBUTTON, this );
		m_CoolButtons[ 3 ].SubclassDlgItem( IDC_STOPBUTTON, this );
		m_CoolButtons[ 4 ].SubclassDlgItem( IDC_STEPLEFTBUTTON, this );
		m_CoolButtons[ 5 ].SubclassDlgItem( IDC_STEPRIGHTBUTTON, this );
		m_CoolButtons[ 6 ].SubclassDlgItem( IDC_EXPORTBUTTON, this );
		m_CoolButtons[ 7 ].SubclassDlgItem( IDC_SETKEYFRAMEBUTTON, this );
		m_CoolButtons[ 8 ].SubclassDlgItem( IDC_EDITKEYFRAMEBUTTON, this );
		
		m_CoolButtons[ 0 ].EnableToolTips( );
		m_CoolButtons[ 1 ].EnableToolTips( );
		m_CoolButtons[ 2 ].EnableToolTips( );
		m_CoolButtons[ 3 ].EnableToolTips( );
		m_CoolButtons[ 4 ].EnableToolTips( );
		m_CoolButtons[ 5 ].EnableToolTips( );
		m_CoolButtons[ 6 ].EnableToolTips( );
		m_CoolButtons[ 7 ].EnableToolTips( );
		m_CoolButtons[ 8 ].EnableToolTips( );

		m_bInitButtons = TRUE;
		m_Tooltips.Activate( TRUE );
		//add tooltips...
		m_Tooltips.AddTool( &m_CoolButtons[ 0 ], IDC_ADDFRAMEBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 1 ], IDC_REMOVEFRAMEBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 2 ], IDC_PLAYBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 3 ], IDC_STOPBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 4 ], IDC_STEPLEFTBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 5 ], IDC_STEPRIGHTBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 6 ], IDC_EXPORTBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 7 ], IDC_SETKEYFRAMEBUTTON );
		m_Tooltips.AddTool( &m_CoolButtons[ 8 ], IDC_EDITKEYFRAMEBUTTON );
		m_Tooltips.AddTool( &m_FrameListCtrl, IDC_FRAMELIST );

		CWnd *ctrl = GetDlgItem( IDC_NAMEEDIT );
		m_Tooltips.AddTool( ctrl, IDC_NAMEEDIT );

		ctrl = GetDlgItem( IDC_PLAYBACKEDIT );
		m_Tooltips.AddTool( ctrl, IDC_PLAYBACKEDIT );

		ctrl = GetDlgItem( IDC_SEQUENCENAMEEDIT );
		m_Tooltips.AddTool( ctrl, IDC_SEQUENCENAMEEDIT );

		ctrl = GetDlgItem( IDC_STARTFRAMEEDIT );
		m_Tooltips.AddTool( ctrl, IDC_STARTFRAMEEDIT );

		ctrl = GetDlgItem( IDC_ENDFRAMEEDIT );
		m_Tooltips.AddTool( ctrl, IDC_ENDFRAMEEDIT );
		
		ctrl = GetDlgItem( IDC_TEXTUREWIDTHEDIT );
		m_Tooltips.AddTool( ctrl, IDC_TEXTUREWIDTHEDIT );

		ctrl = GetDlgItem( IDC_TEXTUREHEIGHTEDIT );
		m_Tooltips.AddTool( ctrl, IDC_TEXTUREHEIGHTEDIT );

		ctrl = GetDlgItem( IDC_CREATETEXTUREIMAGECHECK );
		m_Tooltips.AddTool( ctrl, IDC_CREATETEXTUREIMAGECHECK );

		ctrl = GetDlgItem( IDC_CREATESPRITEFILECHECK );
		m_Tooltips.AddTool( ctrl, IDC_CREATESPRITEFILECHECK );

		ctrl = GetDlgItem( IDC_RESAMPLINGCOMBO );
		m_Tooltips.AddTool( ctrl, IDC_RESAMPLINGCOMBO );
	}
	
	
	
	m_CoolButtons[ 2 ].EnableWindow( );
	m_CoolButtons[ 3 ].EnableWindow( FALSE );

	if( !m_bInitialized )
	{
		m_FrameListCtrl.InsertColumn( 0, _T("Frame"), LVCFMT_LEFT /*| LVCFMT_BITMAP_ON_RIGHT*/, THUMBNAIL_SIZE + 30, -1 );
		m_FrameListCtrl.InsertColumn( 1, _T("Info"), LVCFMT_LEFT, 400, -1 );
		m_FrameListCtrl.InsertColumn( 2, _T("Dimensions"), LVCFMT_LEFT, 0, -1 );

		m_ResamplingComboBox.DeleteString( 0 );
		m_ResamplingComboBox.InsertString( 0, _T("nearest neighbor") );
		m_ResamplingComboBox.InsertString( 1, _T("bilinear") );
		m_ResamplingComboBox.InsertString( 2, _T("bilinear sharper") );
		m_ResamplingComboBox.InsertString( 3, _T("bicubic") );
		m_ResamplingComboBox.SelectString( -1, _T("bicubic") ); //set default

	}

	//UpdateData( FALSE );
	m_bInitialized = TRUE;
}

void CSpritePropertiesForm::OnBnClickedAddframebutton()
{
	//CCoolButton buttonOK;
	CFileDialog  ofd( TRUE, _T("tga"), _T(""), OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, _T("Targa Files (*.tga)|*.tga|Windows Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), this );
	WCHAR files[ 2048 ];

	ofd.m_ofn.lpstrTitle = _T("Open an image file...");
	ofd.m_ofn.nMaxFile = 2048;
	ofd.m_ofn.lpstrFile = files;
	memset( files, 0, sizeof(WCHAR) * 2048 );

	if( ofd.DoModal( ) == IDOK )
	{
		POSITION pos = ofd.GetStartPosition( );
		BeginWaitCursor( );

		while( pos != NULL )
		{
			
			CString count;
			CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
			CString pathName = ofd.GetNextPathName( pos );/*ofd.GetPathName( );*/
			CSpriteEditorDoc::ImageBitmap image;
			char pathname[ _MAX_PATH ];
	

			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, pathName.GetString( ), -1, pathname, _MAX_PATH, NULL, NULL );

			std::string ext( pathname );
			size_t index = 0;
			index = ext.find( '.', 0 );
			
			if( index == std::string::npos )
				ext = "tga";
			else if( ext.length() - index > 3 )
				ext.erase( 0, index + 1 );

			CString extension( ext.c_str( ) );
			
			ImageType type;

			if( getImageType( extension, &type ) )
			{
				if( !loadImage( type, image, pathname ) )
				{
					//MessageBox( _T("For some reason the image file failed to load."), _T("Failed to load image..."), MB_OK | MB_ICONERROR );
					return;
				}
			}
			else {
				MessageBox( _T("A file with an unsupported image extension was tried to be opened. Unfortunately we do not support this image type at this time."), _T("Unsupported image..."), MB_OK | MB_ICONERROR );
				return;
			}



			unsigned int total = pDoc->getNumberOfFrames( );
			pDoc->addFrame( image );

			count.Format( _T("%.3d"), total + 1 );
			
			CBitmap screenshot;
			makeScreenshotBitmap( image, screenshot );
			
			//ImageIO::detectEdges( image.width, image.height, image.bitsPerPixel, image.bitmap, image.bitmap, 50 );
			//ImageIO::RGB color; color.r = 0; color.g = 0; color.b = 255;
			//ImageIO::extractColor( image.width, image.height, image.bitsPerPixel, image.bitmap, image.bitmap, &color, 50 );
			//ImageIO::convertToGrayscale( image.width, image.height, image.bitsPerPixel, image.bitmap, image.bitmap );
			//ImageIO::convertToColorscale( image.width, image.height, image.bitsPerPixel, image.bitmap, image.bitmap, &color );
			//ImageIO::modifyContrast( image.width, image.height, image.bitsPerPixel, image.bitmap, image.bitmap, 100 );
			//ImageIO::modifyBrightness( image.width, image.height, image.bitsPerPixel, image.bitmap, image.bitmap, -80 );
			
			//CSpriteEditorDoc::ImageBitmap image2;
			//image2.bitsPerPixel = image.bitsPerPixel;
			//image2.width = image.width;
			//image2.height = image.height;
			//image2.bitmap = new BYTE[ image2.width * image2.height * (image2.bitsPerPixel >> 3) ];

			//memset( image2.bitmap,0xAA, image2.width * image2.height * (image2.bitsPerPixel >> 3) * sizeof(BYTE) );

			//ImageIO::resizeImage( image.width, image.height, image.bitmap,
			//					  image2.width, image2.height, image2.bitmap, image2.bitsPerPixel,
			//					  ImageIO::ALG_NEARESTNEIGHBOR );
			//ImageIO::flipXImage( image2.width, image2.height, image.bitmap, image2.bitmap, image2.bitsPerPixel >> 3 );

			//image.bitsPerPixel = image2.bitsPerPixel;
			//image.height = image2.height;
			//image.width = image2.width;
			//image.bitmap = image2.bitmap;

			m_ImageList.Add( &screenshot, (CBitmap *) NULL );

			// Release bitmap...
			screenshot.DeleteObject( );

			m_FrameListCtrl.InsertItem( total, count.GetString( ), total );
			CString bpp;
			bpp.Format( _T("%d bpp, %s"), image.bitsPerPixel, pathName );
			m_FrameListCtrl.SetItemText( total, 1, bpp );
			CString dimensions;
			dimensions.Format( _T("%d x %d"), image.width, image.height );
			m_FrameListCtrl.SetItemText( total, 2, dimensions );

			// add the texture data to OpenGL
			glBindTexture( GL_TEXTURE_2D, total + 1 );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, /*GL_NEAREST*/ GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
			
			int err = 0;
			if( (err = glGetError( )) != GL_NO_ERROR )
			{
				CString error;
				DWORD dwError = GetLastError( );
				error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
				AfxMessageBox( error );
			}

			glTexImage2D( GL_TEXTURE_2D, 0, (image.bitsPerPixel / 8 == 4 ? GL_RGBA : GL_RGB), image.width, image.height, 0, /*GL_RGB*/(image.bitsPerPixel / 8 == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image.bitmap );
		}

		EndWaitCursor( );
	}
}

void CSpritePropertiesForm::OnBnClickedRemoveframebutton()
{
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	POSITION pos = m_FrameListCtrl.GetFirstSelectedItemPosition( );

	while (pos)
	{
		int selected = m_FrameListCtrl.GetNextSelectedItem( pos );

		selected++;
		std::vector<CSpriteEditorDoc::ImageBitmap> frames = pDoc->getFrames( );



		// delete all textures from the deleted frame to max
		GLuint *textures = NULL;
		if( frames.size( ) - selected == 0 )
			textures = new GLuint[ 1 ];
		else
			textures = new GLuint[ frames.size( ) - selected + 1 ];

		for( unsigned int i = 0; i < frames.size( ) - selected + 1; i++ )
			textures[ i ] = selected + i;
		
		glDeleteTextures( (GLuint) frames.size( ), textures );
		delete [] textures;

		// remove the frame...
		pDoc->removeFrame( selected );
		m_FrameListCtrl.DeleteItem( selected - 1 );
		m_ImageList.Remove( selected - 1 );

		// add the remaing textures...
		for( unsigned int i = selected; i < frames.size( ); i++ )
		{
			CSpriteEditorDoc::ImageBitmap image = frames[ i ];
			// add the texture data to OpenGL
			glBindTexture( GL_TEXTURE_2D, i );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, /*GL_NEAREST*/ GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
			glTexImage2D( GL_TEXTURE_2D, 0, (image.bitsPerPixel / 8 == 4 ? GL_RGBA : GL_RGB), image.width, image.height, 0, /*GL_RGB*/(image.bitsPerPixel / 8 == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image.bitmap );

			CString strFrameNumber;
			strFrameNumber.Format( _T("%.3d"), i );
			m_FrameListCtrl.SetItemText( i - 1, 0, strFrameNumber );
		}

		//m_FrameListCtrl.SetSelectionMark( selected );
		m_FrameListCtrl.SetItemState( selected - 1, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);


		pDoc->SetModifiedFlag( );

		// Refresh the view...
		pDoc->UpdateAllViews( this );
			
	}
}


int CSpritePropertiesForm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO:  Add your specialized creation code here
	//m_FrameListCtrl.Create( WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect( 10, 121, 110, 407 ), this, IDC_FRAMELIST );


	return 0;
}

void CSpritePropertiesForm::OnLvnKeydownFramelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	if( pLVKeyDow->wVKey == VK_UP || pLVKeyDow->wVKey == VK_DOWN )
	{
		int selected = m_FrameListCtrl.GetSelectionMark( );
		int lastItem =  m_FrameListCtrl.GetItemCount( ) - 1;
		CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );


		if( selected < 0 )
			selected = 0;
		else if( selected > lastItem)
			selected = lastItem;

		m_FrameListCtrl.SetSelectionMark( selected );
		m_FrameListCtrl.SetItemState( selected, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);

		pDoc->setCurrentFrame( selected + 1 );
		pDoc->SetModifiedFlag( );
		
		// Refresh the view...
		pDoc->UpdateAllViews( this );
	}
	*pResult = 0;
}

void CSpritePropertiesForm::OnBnClickedPlaybutton()
{
	// initialize thread with parameters...
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	if( m_isAnimatingSprite ) return; // another playing...
	UINT speed = 0;
	//char speedString[ 6 ]; // speed has a max of 6 chars
	//WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, m_strPlaybackSpeed.GetString( ), -1, speedString, _MAX_PATH, NULL, NULL );

	//
	//std::istringstream iss( speedString );
	//iss >> m_SpriteAnimationParams.uiPlaybackSpeed;
	m_SpriteAnimationParams.uiPlaybackSpeed = m_uiPlaybackSpeed;
	
	CMainFrame *mainWnd = reinterpret_cast<CMainFrame *>( AfxGetMainWnd( ) );
	mainWnd->setView( CMainFrame::MAIN_VIEW );
	CSpriteEditorView *pView = reinterpret_cast<CSpriteEditorView *>( mainWnd->GetActiveView( ) );
	ASSERT( pView->IsKindOf( RUNTIME_CLASS(CSpriteEditorView) ) );
	m_SpriteAnimationParams.hWnd = pView->GetSafeHwnd( );
	m_SpriteAnimationParams.pView = this;
	m_isAnimatingSprite = TRUE;
	
	
	m_CoolButtons[ 2 ].EnableWindow( FALSE );
	m_CoolButtons[ 3 ].EnableWindow( );
	m_pSpritePlaybackThread = AfxBeginThread( animateSprite, &m_SpriteAnimationParams, THREAD_PRIORITY_NORMAL );
	singleLock.Unlock( );
}

void CSpritePropertiesForm::OnBnClickedStopbutton( )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	m_isAnimatingSprite = FALSE;
	m_CoolButtons[ 2 ].EnableWindow( );
	m_CoolButtons[ 3 ].EnableWindow( FALSE );
	singleLock.Unlock( );
}

UINT CSpritePropertiesForm::animateSprite( LPVOID p )
{
	SpriteAnimateParams *pParams = static_cast<SpriteAnimateParams *>( p );

	pParams->pView ->animateSprite( pParams->hWnd, pParams->uiPlaybackSpeed );
	AfxEndThread( 0 );
	return 0;
}

void CSpritePropertiesForm::animateSprite( const HWND hWnd, const UINT speed  )
{
	CWnd *wnd = CWnd::FromHandle( hWnd );
	
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	bool isAnimatingSprite = m_isAnimatingSprite;
	singleLock.Unlock( );
	

	while( isAnimatingSprite )
	{
		::PostMessage( hWnd, UWM_DRAW_NEXTSPRITEFRAME, 0, 0 );
		//wnd->RedrawWindow( 0, 0, RDW_NOERASE | RDW_INVALIDATE );
		
		singleLock.Lock( );
		isAnimatingSprite = m_isAnimatingSprite;
		singleLock.Unlock( );

		Sleep( static_cast<DWORD>( speed ) );
	}

}

void CSpritePropertiesForm::OnBnClickedStepleftbutton()
{
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );

	m_FrameListCtrl.SetSelectionMark( 0 );
	m_FrameListCtrl.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );

	pDoc->setCurrentFrame( 1 );
	
	//m_FrameListCtrl.Invalidate( );
	pDoc->SetModifiedFlag( );
	pDoc->UpdateAllViews( this );
}

void CSpritePropertiesForm::OnBnClickedSteprightbutton()
{
	int lastItem =  m_FrameListCtrl.GetItemCount( ) - 1;
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );

	m_FrameListCtrl.SetItemState( lastItem, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);
	m_FrameListCtrl.SetSelectionMark( lastItem );
	pDoc->setCurrentFrame( lastItem + 1 );

	// Refresh the view...
	pDoc->SetModifiedFlag( );
	pDoc->UpdateAllViews( this );
}

void CSpritePropertiesForm::OnBnClickedExportbutton()
{
	CString resamplingAlgorithm;
	GetDlgItemText( IDC_RESAMPLINGCOMBO, resamplingAlgorithm );
	CSpriteEditorDoc::ImageBitmap image;
	SpriteExportInfo exportInfo;
	char pathname[ _MAX_PATH ];

	if( !m_bCreateTextureImage && !m_bCreateSpriteFile )
	{
		MessageBox( _T("Atleast one export option must be selected."), _T("Select an export option."), MB_OK | MB_ICONINFORMATION );
		return;
	}

	// save a texture image...
	if( m_bCreateTextureImage )
	{
		CFileDialog  sfd( FALSE, _T("tga"), _T(""), OFN_PATHMUSTEXIST, _T("Targa Files (*.tga)|*.tga|Windows Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), this );
	
		if( sfd.DoModal( ) == IDOK )
		{
			CString filename = sfd.GetPathName( );
			CString fileExtension = sfd.GetFileExt( );
			
			if( resamplingAlgorithm == "nearest neighbor" )
				postProcessFrames( image,  RES_NEARESTNEIGHBOR, exportInfo );
			else if ( resamplingAlgorithm == "bilinear" )
				postProcessFrames( image, RES_BILINEAR, exportInfo );
				else if ( resamplingAlgorithm == "bilinear sharper" )
					postProcessFrames( image, RES_BILINEAR_SHARPER, exportInfo );
			else if( resamplingAlgorithm == "bicubic" )
				postProcessFrames( image, RES_BICUBIC, exportInfo );
			else // will assume bilinear
				postProcessFrames( image, RES_BILINEAR, exportInfo );

			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, filename.GetString( ), -1, pathname, _MAX_PATH, NULL, NULL );

			// saving...
			if( fileExtension == "tga" )
				saveImage( IMG_TARGA, image, pathname );
			else if( fileExtension == "bmp" )
				saveImage( IMG_BITMAP, image, pathname );
			else {
				MessageBox( _T("An attempt was made to save to an unsupported image type. Please try another file type."), _T("Unsupported file type error."), MB_OK | MB_ICONERROR );
				return;
			}


			if( !m_bCreateSpriteFile )
				delete [] image.bitmap;
		}
	}

	// save a sprite object...
	if( m_bCreateSpriteFile )
	{
		CFileDialog  sfd( FALSE, _T("sprite"), _T(""), OFN_PATHMUSTEXIST, _T("Sprite Object Files (*.sprite)|*.sprite|All Files (*.*)|*.*||"), this );

		if( sfd.DoModal( ) == IDOK )
		{
			CString filename = sfd.GetFileName( );
			CString fileExtension = sfd.GetFileExt( );
		
			if( fileExtension != "sprite" && 
				(MessageBox( _T("Are you sure you want to save to an unsupported file extension type?"), _T("Unsupported file extension"), MB_YESNO | MB_ICONQUESTION ) == IDNO)
				)
			{
				return;
			}

			if( !m_bCreateTextureImage ) // avoid doing it twice
			{
				if( resamplingAlgorithm == "nearest neighbor" )
					postProcessFrames( image,  RES_NEARESTNEIGHBOR, exportInfo );
				else if ( resamplingAlgorithm == "bilinear" )
					postProcessFrames( image, RES_BILINEAR, exportInfo );
				else if ( resamplingAlgorithm == "bilinear sharper" )
					postProcessFrames( image, RES_BILINEAR_SHARPER, exportInfo );
				else if( resamplingAlgorithm == "bicubic" )
					postProcessFrames( image, RES_BICUBIC, exportInfo );
				else // will assume bilinear
					postProcessFrames( image, RES_BILINEAR, exportInfo );
			}
			
			WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, filename.GetString( ), -1, pathname, _MAX_PATH, NULL, NULL );


			// build sprite object
			SpriteIO::SpriteFileHeader spriteHeader;
			memset( &spriteHeader, 0, sizeof(SpriteIO::SpriteFileHeader) );
			spriteHeader.uiBitsPerPixel = image.bitsPerPixel;
			spriteHeader.usByteCount = image.bitsPerPixel >> 3;
			spriteHeader.uiWidth = image.width;
			spriteHeader.uiHeight = image.height;
			spriteHeader.uiFrameWidth = exportInfo.frameWidth;
			spriteHeader.uiFrameHeight = exportInfo.frameHeight;
			spriteHeader.uiNumberOfFrames = exportInfo.numberOfFrames;
			spriteHeader.uiNumberOfStates = exportInfo.numberOfStates;
			spriteHeader.uiSize = image.width * image.height * spriteHeader.usByteCount;
			strncpy( spriteHeader.description, CSpriteEditorDoc::getDoc( )->getName( ).c_str(), 80 ); // max is 80
			spriteHeader.uiOffsetToTexels = sizeof( SpriteIO::SpriteFileHeader );
			spriteHeader.bTexels = image.bitmap;


			if( !SpriteIO::writeSpriteFile( pathname, spriteHeader ) )
				MessageBox( _T("The export process failed!"), _T("Error."), MB_OK | MB_ICONERROR );

			delete [] image.bitmap;
		}
	}
}

void CSpritePropertiesForm::postProcessFrames( CSpriteEditorDoc::ImageBitmap &img, const ResamplingMethod &method, SpriteExportInfo &info )
{
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	unsigned int smallestFrameWidth = UINT_MAX;
	unsigned int smallestFrameHeight = UINT_MAX;
	unsigned int maxNumberOfFrames = pDoc->getNumberOfFrames( );
	unsigned int framesAlongX = std::floor( (double) std::sqrt( (double) maxNumberOfFrames ) );
	unsigned int framesAlongY = std::ceil( (double) maxNumberOfFrames / framesAlongX );
	
	BeginWaitCursor( );
	std::vector<CSpriteEditorDoc::ImageBitmap> &list = pDoc->getCopyOfFrames( ); 

	// find smallest overall dimensions...
	for( unsigned int f = 0; f < list.size( ); f++ )
	{
		if( list[ f ].width < smallestFrameWidth )
			smallestFrameWidth = list[ f ].width;
		if( list[ f ].height < smallestFrameHeight )
			smallestFrameHeight = list[ f ].height;
	}
	


	// go through all frames and resize to ensure that they are all the same size;
	for( unsigned int f = 0; f < list.size( ); f++ )
	{
		CSpriteEditorDoc::ImageBitmap &img = list[ f ];

		switch( method )
		{
			//case RES_NEARESTNEIGHBOR:
			//	ImageIO::resizeImage( img.width, img.height, img.bitmap,
			//		                  smallestFrameWidth, smallestFrameHeight, img.bitmap,
			//						  img.bitsPerPixel, ImageIO::ALG_NEARESTNEIGHBOR );
			//	break;
			//case RES_BILINEAR:
			//	ImageIO::resizeImage( img.width, img.height, img.bitmap,
			//		                  smallestFrameWidth, smallestFrameHeight, img.bitmap,
			//						  img.bitsPerPixel, ImageIO::ALG_BILINEAR_NICER );
			//	break;
			//case RES_BICUBIC:
			//	ImageIO::resizeImage( img.width, img.height, img.bitmap,
			//		                  smallestFrameWidth, smallestFrameHeight, img.bitmap,
			//						  img.bitsPerPixel, ImageIO::ALG_BICUBIC );
			//	break;
			default:
				ImageIO::resizeImage( img.width, img.height, img.bitmap,
				                      smallestFrameWidth, smallestFrameHeight, img.bitmap,
									  img.bitsPerPixel, ImageIO::ALG_NEARESTNEIGHBOR );
				break;
		}
		
		// adjust the dimensions...
		img.width = smallestFrameWidth;
		img.height = smallestFrameHeight;
	}

	unsigned int frameByteCount = list[ 0 ].bitsPerPixel >> 3;
	unsigned int intermediateWidth = framesAlongX * smallestFrameWidth;
	unsigned int intermediateHeight = framesAlongY * smallestFrameHeight;
	unsigned int textureByteCount = frameByteCount;
	
	BYTE *bitmap = new BYTE[ intermediateWidth * intermediateHeight * textureByteCount ]; 

	unsigned int position = 0, uv_position = 0;
	unsigned int frameNumber = 0;

	ASSERT( textureByteCount == frameByteCount );

	memset( bitmap, 0, intermediateWidth * intermediateHeight * textureByteCount ); // set to black background


	// for every frame...
	for( unsigned int j = 0; j < intermediateHeight; j += smallestFrameHeight )
	{
		for( unsigned i = 0; i < intermediateWidth && frameNumber < maxNumberOfFrames; i += smallestFrameWidth )
		{
			// for every pixel in this frame...
			for( unsigned int y = j, v = 0; y < j + smallestFrameHeight; y++, v++ )
				for( unsigned int x = i, u = 0; x < i + smallestFrameWidth; x++, u++ )
				{
					position = y * intermediateWidth * textureByteCount + x * textureByteCount;
					uv_position = v * smallestFrameWidth * frameByteCount + u * frameByteCount;

					bitmap[ position ] = list[ frameNumber ].bitmap[ uv_position ];
					bitmap[ position + 1 ] = list[ frameNumber ].bitmap[ uv_position + 1 ];
					bitmap[ position + 2 ] = list[ frameNumber ].bitmap[ uv_position + 2 ];
					if( textureByteCount == 4 )
						bitmap[ position + 3 ] = list[ frameNumber ].bitmap[ uv_position + 3 ];
				}
			
			frameNumber++;
		}
	}


	// Complete construction of the texture...
	BYTE *textureBitmap = new BYTE[ m_uiTextureWidth * m_uiTextureHeight * textureByteCount ];
	memset( textureBitmap, 0, m_uiTextureWidth * m_uiTextureHeight * textureByteCount );
	img.bitmap = textureBitmap;
	img.bitsPerPixel = textureByteCount << 3;


	switch( method )
	{
		case RES_NEARESTNEIGHBOR:
			ImageIO::resizeImage( intermediateWidth, intermediateHeight, bitmap,
				                  m_uiTextureWidth, m_uiTextureHeight, img.bitmap,
								  img.bitsPerPixel, ImageIO::ALG_NEARESTNEIGHBOR );
			break;
		case RES_BILINEAR:
			ImageIO::resizeImage( intermediateWidth, intermediateHeight, bitmap,
								  m_uiTextureWidth, m_uiTextureHeight, img.bitmap,
								  img.bitsPerPixel, ImageIO::ALG_BILINEAR ); 
			break;
		case RES_BILINEAR_SHARPER:
			ImageIO::resizeImage( intermediateWidth, intermediateHeight, bitmap,
								  m_uiTextureWidth, m_uiTextureHeight, img.bitmap,
								  img.bitsPerPixel, ImageIO::ALG_BILINEAR_SHARPER ); 
			break;
		case RES_BICUBIC:
			ImageIO::resizeImage( intermediateWidth, intermediateHeight, bitmap,
				                  m_uiTextureWidth, m_uiTextureHeight, img.bitmap,
								  img.bitsPerPixel, ImageIO::ALG_BICUBIC );
			break;
		default:
			ASSERT( false ); // should not have gotten here...
			break;
	}

	delete [] bitmap;

	img.width = m_uiTextureWidth;
	img.height = m_uiTextureHeight;

	//destroy copy made at the start...
	std::vector<CSpriteEditorDoc::ImageBitmap>::iterator itr;

	for( itr = list.begin( ); itr != list.end( ); ++itr )
		delete [] itr->bitmap;


	// fill in the info
	info.frameWidth = m_uiTextureWidth / framesAlongX;
	info.frameHeight = m_uiTextureHeight / framesAlongY;
	info.numberOfFrames = maxNumberOfFrames;
	info.numberOfStates = 1; //change this later  <=============================================


	EndWaitCursor( );
}

void CSpritePropertiesForm::OnLvnItemchangedFramelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	pDoc->setCurrentFrame( pNMLV->iItem + 1 );

	// Refresh the view...
	pDoc->UpdateAllViews( this );

	*pResult = 0;
}

void CSpritePropertiesForm::OnBnClickedSetkeyframebutton()
{
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	CSpriteEditorDoc::KeyFrames keyframe;
	char sequenceName[ 128 ]; // 128 is the max the control could hold. If this changes make changes below.
	WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, m_szSequenceName.GetString( ), -1, sequenceName, 128, NULL, NULL );

	
	keyframe.sequenceName.assign( sequenceName, 128 );
	keyframe.startFrame = m_uiStartFrame;
	keyframe.endFrame = m_uiEndFrame;

	// validate the data; this list is expected to be < 100 items
	std::vector<CSpriteEditorDoc::KeyFrames>::const_iterator itr;
	std::vector<CSpriteEditorDoc::KeyFrames> list = pDoc->getKeyFrames( );
	bool doOverwrite = false;
	unsigned int count = 0;

	for( itr = list.begin( ); itr != list.end( ); ++itr )
		if( itr->startFrame == keyframe.startFrame || itr->endFrame == keyframe.endFrame )
		{
			CString err;
			err.Format( _T("There is another key frame sequence set between %d to %d.\nDo you want to overwrite existing one?"), keyframe.startFrame, keyframe.endFrame );
			
			if( MessageBox( err , _T("Existing key frame found!"), MB_YESNO | MB_ICONQUESTION ) == IDYES )
			{
				doOverwrite = true;
				break;
			}
			else
				return;
			
			count++;
		}
	

	// if your overwriting data...
	if( doOverwrite )
		pDoc->setKeyFrame( count, keyframe );
	else
		pDoc->addKeyFrame( keyframe );
}


void CSpritePropertiesForm::OnBnClickedEditkeyframebutton()
{
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	CKeyFrameEditorDlg dlg( this->GetParent( ), pDoc );

	if( dlg.DoModal( ) == IDOK )
	{
		// DDX
		AfxMessageBox(_T("test"));
	}
}

bool CSpritePropertiesForm::getImageType( CString &extension, ImageType *type )
{
	if( extension == "tga" )
		*type = IMG_TARGA;
	else if( extension == "bmp" )
		*type = IMG_BITMAP;
	else
		return false;

	return true;
}

void CSpritePropertiesForm::makeScreenshotBitmap( const CSpriteEditorDoc::ImageBitmap &img, CBitmap &screenshot )
{
	CBitmap bmp;
	CDC memDC, final;
	
	memDC.CreateCompatibleDC( NULL );
	memDC.SetLayout( LAYOUT_BITMAPORIENTATIONPRESERVED );
	final.CreateCompatibleDC( &memDC );
	final.SetLayout( LAYOUT_BITMAPORIENTATIONPRESERVED );

	const unsigned int bytesPerPixel = (img.bitsPerPixel / 8);
	const unsigned int size = bytesPerPixel * img.width * img.height;

	BITMAPINFO i, j;
	ZeroMemory( &i.bmiHeader, sizeof(BITMAPINFOHEADER) );
	i.bmiHeader.biWidth = img.width;     // Set size you need
	i.bmiHeader.biHeight = img.height;    // Set size you need
	i.bmiHeader.biCompression = BI_RGB;
	i.bmiHeader.biPlanes = 1;
	i.bmiHeader.biBitCount = img.bitsPerPixel; // Can be 8, 16, 32 bpp or
								   // other number
	i.bmiHeader.biSize = size;
	i.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	i.bmiHeader.biClrUsed = 0;
	i.bmiHeader.biClrImportant = 0;

	ZeroMemory( &j.bmiHeader, sizeof(BITMAPINFOHEADER) );
	j.bmiHeader.biWidth = THUMBNAIL_SIZE;     // Set size you need
	j.bmiHeader.biHeight = THUMBNAIL_SIZE;    // Set size you need
	j.bmiHeader.biCompression = BI_RGB;
	j.bmiHeader.biPlanes = 1;
	j.bmiHeader.biBitCount = img.bitsPerPixel; // Can be 8, 16, 32 bpp or
								   // other number
	j.bmiHeader.biSize = bytesPerPixel * THUMBNAIL_SIZE * THUMBNAIL_SIZE;
	j.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	j.bmiHeader.biClrUsed = 0;
	j.bmiHeader.biClrImportant = 0;

	BYTE *data, *finalData;
	HBITMAP hbmp = CreateDIBSection( memDC.GetSafeHdc(), &i, DIB_RGB_COLORS, (void **) &data, NULL, 0 );
	HBITMAP hbmpFinal = CreateDIBSection( final.GetSafeHdc(), &j, DIB_RGB_COLORS, (void **) &finalData, NULL, 0 );

	// copy over data and convert RGB to BGR...
	unsigned int pos = 0;
	for( unsigned int y = 0; y < img.height; y++ )
		for( unsigned int x = 0; x < img.width * bytesPerPixel; x += bytesPerPixel )
		{
			pos = y * bytesPerPixel * img.width + x;
			data[ pos ] = img.bitmap[ pos + 2 ];
			data[ pos + 1 ] = img.bitmap[ pos + 1];
			data[ pos + 2 ] = img.bitmap[ pos ];
		}
		
	VERIFY( screenshot.Attach( hbmpFinal ) );
	VERIFY( bmp.Attach( hbmp ) );

		
	CBitmap *pOldBmp = memDC.SelectObject( &bmp );
	CBitmap *pfinalOldBmp = final.SelectObject( &screenshot );

	if( pOldBmp != NULL && pfinalOldBmp != NULL )
	{
		SetStretchBltMode( final.GetSafeHdc( ), STRETCH_HALFTONE );
		final.StretchBlt( 0, 0, THUMBNAIL_SIZE, THUMBNAIL_SIZE, &memDC, 0, 0, img.width, img.height, SRCCOPY );
		memDC.SelectObject( pOldBmp );
		final.SelectObject( pfinalOldBmp );
	}

	// Release some DC's
	DeleteDC( memDC );
	DeleteDC( final );
	
	//release bitmap...
	bmp.DeleteObject( );
}

bool CSpritePropertiesForm::loadImage( ImageType type, CSpriteEditorDoc::ImageBitmap &img, const char *filename )
{
	switch( type )
	{
		case IMG_TARGA:
			return loadTargaImage( img, filename );
			break;
		case IMG_BITMAP:
			return loadBitmapImage( img, filename );
			break;
		default:
			ASSERT( false ); // somehow we got some unsupported image type...
			return false;
			break;
	}
}

bool CSpritePropertiesForm::loadTargaImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename )
{
	ImageIO::TGAFILE tgaFile;

	if( ImageIO::loadTGAFile( filename, &tgaFile ) == ImageIO::FAILURE )
	{
		MessageBox( _T("There was an error when attempting to load this targa file."), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		return false;
	}

	// check if 8 bpp
	if( (tgaFile.bitCount >> 3) <= 1 )
	{
		MessageBox( _T("The targa image is an index color mode image. This is not supported at this time. Try loading an image atleast with atleast 16 bits per pixel."), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		ImageIO::destroyTGAImageData( &tgaFile );
		return false;
	}

	// Ensure dimensions are powers of two.
	if( !((tgaFile.imageWidth & tgaFile.imageWidth - 1) == 0 && tgaFile.imageWidth != 0) ||
		!((tgaFile.imageHeight & tgaFile.imageHeight - 1) == 0 && tgaFile.imageHeight != 0) )
	{
		MessageBox( _T("The dimensions of the image file are not powers of two!"), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		ImageIO::destroyTGAImageData( &tgaFile );
		return false;
	}

	// Initialize ImageBitmap struct...
	img.bitsPerPixel = tgaFile.bitCount;
	img.width = tgaFile.imageWidth;
	img.height = tgaFile.imageHeight;
	img.bitmap = new unsigned char[ img.width * img.height * (img.bitsPerPixel >> 3) * sizeof(BYTE) ];
	memcpy( img.bitmap, tgaFile.imageData, img.width * img.height * (img.bitsPerPixel  >> 3) * sizeof(BYTE) );
	
	// Release the memory allocated inside the c library...
	ImageIO::destroyTGAImageData( &tgaFile );

	return true;
}

bool CSpritePropertiesForm::loadBitmapImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename )
{
	BITMAPINFOHEADER bmpHeader;
	ImageIO::byte *imageData = NULL;

	if( ImageIO::loadBitmapFile( filename, &bmpHeader, &imageData ) == ImageIO::FAILURE )
	{
		MessageBox( _T("There was an error when attempting to load this windows bitmap file."), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		return false;
	}

	// Ensure not an 8 bpp image
	if( (bmpHeader.biBitCount >> 3) <= 1 )
	{
		MessageBox( _T("The bitmap is an indexed color mode picture. This is not supported at this time. Try loading an image atleast with atleast 16 bits per pixel."), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		ImageIO::destroyBMPImageData( imageData );
		return false;
	}

	// Ensure dimensions are powers of two.
	if( !((bmpHeader.biWidth & bmpHeader.biWidth  - 1) == 0 && bmpHeader.biWidth  != 0) ||
		!((bmpHeader.biHeight & bmpHeader.biHeight - 1) == 0 && bmpHeader.biHeight != 0) )
	{
		MessageBox( _T("The dimensions of the image file are not powers of two!"), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		ImageIO::destroyBMPImageData( imageData );
		return false;
	}

	// Initialize ImageBitmap struct...
	img.bitsPerPixel = bmpHeader.biBitCount;
	img.width = bmpHeader.biWidth;
	img.height = bmpHeader.biHeight;
	img.bitmap = new unsigned char[ bmpHeader.biSizeImage ];
	memcpy( img.bitmap, imageData, bmpHeader.biSizeImage );

	// Release the memory allocated inside the c library...
	ImageIO::destroyBMPImageData( imageData );

	return true;
}

bool CSpritePropertiesForm::saveImage( ImageType type, CSpriteEditorDoc::ImageBitmap &img, const char *filename )
{
	switch( type )
	{
		case IMG_TARGA:
			return saveTargaImage( img, filename );
			break;
		case IMG_BITMAP:
			return saveBitmapImage( img, filename );
			break;
		default:
			ASSERT( false ); // somehow we got some unsupported image type...
			return false;
			break;
	}
}

bool CSpritePropertiesForm::saveTargaImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename )
{
	ImageIO::TGAFILE tgaFile;

	// construct TGA header...
	tgaFile.imageTypeCode = 0x02; // must be 2 or 3
	tgaFile.bitCount = static_cast<unsigned int>( img.bitsPerPixel );
	tgaFile.imageHeight = img.height;
	tgaFile.imageWidth = img.width;
	tgaFile.imageData = img.bitmap;

	// Ensure dimensions are powers of two.
	if( !((tgaFile.imageWidth & tgaFile.imageWidth - 1) == 0 && tgaFile.imageWidth != 0) ||
		!((tgaFile.imageHeight & tgaFile.imageHeight - 1) == 0 && tgaFile.imageHeight != 0) )
	{
		MessageBox( _T("The dimensions of the image file are not powers of two!"), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		return false;
	}

	if( !ImageIO::writeTGAFile( filename, &tgaFile ) )
	{
		MessageBox( _T("There was an error when attempting to save this targa file."), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		return false;
	}

	return true;
}

bool CSpritePropertiesForm::saveBitmapImage( CSpriteEditorDoc::ImageBitmap &img, const char *filename )
{
	// Ensure dimensions are powers of two.
	if( !((img.width & img.width - 1) == 0 && img.width != 0) ||
		!((img.height & img.height - 1) == 0 && img.height != 0) )
	{
		MessageBox( _T("The dimensions of the image file are not powers of two!"), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		return false;
	}

	if( !ImageIO::writeBitmapFile( filename, img.width, img.height, img.bitsPerPixel, img.bitmap ) )
	{
		MessageBox( _T("There was an error when attempting to save this windows bitmap file."), _T("Error with the image file."), MB_OK | MB_ICONERROR );
		return false;
	}

	return true;
}

void CSpritePropertiesForm::OnLvnBegindragFramelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//RECORD THE INDEX OF THE ITEM BEIGN DRAGGED AS m_nDragIndex
	m_nDragIndex = pNMLV->iItem; //((NM_LISTVIEW *)pnmhdr)->iItem;

	//CREATE A DRAG IMAGE FROM THE CENTER POINT OF THE ITEM IMAGE
	POINT pt;
	pt.x = 8;
	pt.y = 8;
	
	//disable owner draw
	m_FrameListCtrl.ModifyStyle( LVS_OWNERDRAWFIXED, 0 );
	m_pDragImage = m_FrameListCtrl.CreateDragImage( m_nDragIndex, &pt );
	m_FrameListCtrl.ModifyStyle( 0, LVS_OWNERDRAWFIXED );

	m_pDragImage->BeginDrag( 0, CPoint( 8, 8 ) );
	m_pDragImage->DragEnter( GetDesktopWindow( ), pNMLV->ptAction );

	//SET THE FLAGS INDICATING A DRAG IN PROGRESS
	m_bDragging = TRUE;
	
	//m_hDropItem = NULL;
	m_nDropIndex = -1;
	m_pDropWnd = &m_FrameListCtrl;

	//CAPTURE ALL MOUSE MESSAGES IN CASE THE USER DRAGS OUTSIDE OF THE VIEW
	SetCapture( );

}

void CSpritePropertiesForm::DropFrameItemOnList( )
{
	//GET THE DROP INDEX
	m_ptDropPoint.y += 10;
	m_nDropIndex = m_FrameListCtrl.HitTest( m_ptDropPoint );

	//GET INFORMATION ON THE DRAGGED ITEM BY SETTING AN LV_ITEM STRUCTURE
	//AND THEN CALLING GetItem TO FILL IT IN
	WCHAR szLabel[256];
	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.stateMask = LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED;
	lvi.pszText = szLabel;
	lvi.iItem = m_nDragIndex;
	lvi.cchTextMax = 255;
	m_FrameListCtrl.GetItem( &lvi );

	//INSERT THE DROPPED ITEM
	if(m_nDropIndex < 0) m_nDropIndex = m_FrameListCtrl.GetItemCount();
	lvi.iItem = m_nDropIndex;
	m_FrameListCtrl.InsertItem(&lvi);

	//FILL IN ALL OF THE COLUMNS
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_FrameListCtrl.GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	lvi.mask = LVIF_TEXT;
	lvi.iItem = m_nDropIndex;
	//INDEX OF DRAGGED ITEM WILL CHANGE IF ITEM IS DROPPED ABOVE ITSELF
	if(m_nDragIndex > m_nDropIndex) m_nDragIndex++;
	for(int col=1; col < nColumnCount; col++)
	{
		wcscpy(lvi.pszText, (LPCTSTR)(m_FrameListCtrl.GetItemText(m_nDragIndex,
			col)));
		lvi.iSubItem = col;
		m_FrameListCtrl.SetItem(&lvi);
	}

	//DELETE THE ITEM THAT WAS DRAGGED FROM ITS ORIGINAL LOCATION
	m_FrameListCtrl.DeleteItem(m_nDragIndex);
}

void CSpritePropertiesForm::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		m_ptDropPoint = point;
		ClientToScreen( &m_ptDropPoint );

		//m_FrameListCtrl.ModifyStyle( LVS_OWNERDRAWFIXED, 0 );
		//MOVE THE DRAG IMAGE
		m_pDragImage->DragMove( m_ptDropPoint );

		//TEMPORARILY UNLOCK WINDOW UPDATES
		m_pDragImage->DragShowNolock( FALSE );
		//m_FrameListCtrl.ModifyStyle( 0, LVS_OWNERDRAWFIXED );

		//CONVERT THE DROP POINT TO CLIENT CO-ORDIANTES
		m_pDropWnd = WindowFromPoint( m_ptDropPoint );
		m_pDropWnd->ScreenToClient( &m_ptDropPoint );

		//LOCK WINDOW UPDATES
		m_pDragImage->DragShowNolock( TRUE );
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CSpritePropertiesForm::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		

		//RELEASE THE MOUSE CAPTURE AND END THE DRAGGING
		::ReleaseCapture( );
		m_bDragging = FALSE;
		m_pDragImage->DragLeave( GetDesktopWindow( ) );
		m_pDragImage->EndDrag( );

		//GET THE WINDOW UNDER THE DROP POINT
		CPoint pt( point );
		ClientToScreen( &pt );
		m_pDropWnd = WindowFromPoint( pt );

		//DROP THE ITEM ON THE LIST
		if( m_pDropWnd->IsKindOf( RUNTIME_CLASS(CListCtrl) ) )
			DropFrameItemOnList( );
	}
	CFormView::OnLButtonUp(nFlags, point);
}

void CSpritePropertiesForm::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pDoc->getName( ).c_str(), -1, NULL, NULL );
	LPWSTR wName = new WCHAR[ nLen ];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pDoc->getName( ).c_str(), -1, wName, nLen );

	m_Name.SetString( wName );
	delete [] wName;
	m_uiTextureWidth = pDoc->getTextureWidth( );
	m_uiTextureHeight = pDoc->getTextureHeight( );


	std::vector<CSpriteEditorDoc::ImageBitmap>::const_iterator itr;
	std::vector<CSpriteEditorDoc::ImageBitmap> frames = pDoc->getFrames( );

	if( m_bInitialized )
	{
		while( m_ImageList.GetImageCount( ) > 0 ) m_ImageList.Remove( 0 );
		m_FrameListCtrl.DeleteAllItems( );
	

		unsigned int total = 0;
		for( itr = frames.begin(); itr != frames.end(); ++itr )
		{
			CString count;
			CSpriteEditorDoc::ImageBitmap image = *itr;
			count.Format( _T("%.3d"), total + 1 );
				
			CBitmap screenshot;
			makeScreenshotBitmap( *itr, screenshot );


			m_ImageList.Add( &screenshot, (CBitmap *) NULL );

			// Release bitmap...
			screenshot.DeleteObject( );

			m_FrameListCtrl.InsertItem( total, count.GetString( ), total );
			CString bpp;
			bpp.Format( _T("%d bpp"), image.bitsPerPixel );
			m_FrameListCtrl.SetItemText( total, 1, bpp );
			CString dimensions;
			dimensions.Format( _T("%d x %d"), image.width, image.height );
			m_FrameListCtrl.SetItemText( total, 2, dimensions );

			// add the texture data to OpenGL
			glBindTexture( GL_TEXTURE_2D, total + 1 );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, /*GL_NEAREST*/ GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
				
			int err = 0;
			if( (err = glGetError( )) != GL_NO_ERROR )
			{
				CString error;
				error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
				AfxMessageBox( error );
			}

			glTexImage2D( GL_TEXTURE_2D, 0, (image.bitsPerPixel / 8 == 4 ? GL_RGBA : GL_RGB), image.width, image.height, 0, /*GL_RGB*/(image.bitsPerPixel / 8 == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image.bitmap );
			total++;
		}
	}


	UpdateData( FALSE );

}

void CSpritePropertiesForm::OnEnChangeNameedit()
{
	UpdateData( );
	CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	std::string name;
	char cname[ _MAX_PATH ];
	WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, m_Name.GetString( ), -1, cname, _MAX_PATH, NULL, NULL );
	name.assign( cname );
	pDoc->setName( name );
}

void CSpritePropertiesForm::OnEnChangePlaybackedit()
{
	if( m_bInitialized )
		UpdateData( );
}

void CSpritePropertiesForm::OnEnChangeSequencenameedit()
{
	if( m_bInitialized )
		UpdateData( );
}

void CSpritePropertiesForm::OnEnChangeStartframeedit()
{
	if( m_bInitialized )
		UpdateData( );
}

void CSpritePropertiesForm::OnEnChangeEndframeedit()
{
	if( m_bInitialized )
		UpdateData( );
}

void CSpritePropertiesForm::OnLvnEndScrollFramelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Internet Explorer 5.5 or greater.
	// The symbol _WIN32_IE must be >= 0x0560.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_FrameListCtrl.Invalidate( );
	*pResult = 0;
}

void CSpritePropertiesForm::OnEnChangeTexturewidthedit()
{
	if( m_bInitialized )
	{
		UpdateData( );
		CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
		pDoc->setTextureWidth( m_uiTextureWidth );
	}
}

void CSpritePropertiesForm::OnEnChangeTextureheightedit()
{
	if( m_bInitialized )
	{
		UpdateData( );
		CSpriteEditorDoc *pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
		pDoc->setTextureHeight( m_uiTextureHeight );
	}
}

BOOL CSpritePropertiesForm::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
    UINT nID = pTTTStruct->idFrom;

    if( pTTT->uFlags & TTF_IDISHWND )
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID( (HWND) nID );
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
			wcsncpy( pTTT->szText, _T("test"), 4 );
            pTTT->hinst = AfxGetResourceHandle();
            return TRUE;
        }
    }


	return FALSE;
}





BOOL CSpritePropertiesForm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_Tooltips.RelayEvent( pMsg );
	return CFormView::PreTranslateMessage(pMsg);
}

void CSpritePropertiesForm::OnBnClickedCreatetextureimagecheck()
{
	UpdateData( );
}

void CSpritePropertiesForm::OnBnClickedCreatespritefilecheck()
{
	UpdateData( );
}

void CSpritePropertiesForm::OnAnimatePlay()
{
	OnBnClickedPlaybutton( );
}

void CSpritePropertiesForm::OnAnimateStop()
{
	OnBnClickedStopbutton( );
}
