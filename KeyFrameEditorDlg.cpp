// KeyFrameEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteEditor.h"
#include "KeyFrameEditorDlg.h"
#include "SpriteEditorDoc.h"
#include <vector>
#include <sstream>
#include <cmath>

#define		DIALOG_DIVIDER_LINE_Y	35
#define		DIALOG_EDGE_SPACING		10
#define		BITMAP_WIDTH			200
#define		BITMAP_HEIGHT			200

// CKeyFrameEditorDlg dialog

IMPLEMENT_DYNAMIC(CKeyFrameEditorDlg, CDialog)

CKeyFrameEditorDlg::CKeyFrameEditorDlg( CWnd* pParent, CSpriteEditorDoc *pDoc /*= NULL*/ )
	: CDialog(CKeyFrameEditorDlg::IDD, pParent),
	  m_bInitButtons(FALSE),
	  m_pDoc( pDoc ),
	  m_ImageBitmaps()

{}

CKeyFrameEditorDlg::CKeyFrameEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyFrameEditorDlg::IDD, pParent),
	  m_bInitButtons(FALSE),
	  m_pDoc(NULL),
	  m_ImageBitmaps()
{
}

CKeyFrameEditorDlg::~CKeyFrameEditorDlg()
{
}

void CKeyFrameEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEYFRAMELIST, m_KeyFrameList);
}


BEGIN_MESSAGE_MAP(CKeyFrameEditorDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDREMOVEKEYFRAME, &CKeyFrameEditorDlg::OnBnClickedRemovekeyframe)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_KEYFRAMELIST, &CKeyFrameEditorDlg::OnLvnEndlabeleditKeyframelist)
END_MESSAGE_MAP()


// CKeyFrameEditorDlg message handlers

void CKeyFrameEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( m_bInitButtons )
	{
		// move the buttons...
		CRect rcOKRect, rcCANCELRect;
		m_Buttons[ IDOK_BTN ].GetClientRect( &rcOKRect );
		m_Buttons[ IDCANCEL_BTN ].GetClientRect( &rcCANCELRect );


		m_Buttons[ IDOK_BTN ].MoveWindow( cx - 2 * rcOKRect.Width( ) - 12, cy - rcOKRect.Height( ) - DIALOG_EDGE_SPACING, rcOKRect.Width( ), rcOKRect.Height( ), TRUE );
		m_Buttons[ IDCANCEL_BTN ].MoveWindow( cx - rcCANCELRect.Width( ) - 8, cy - rcCANCELRect.Height( ) - DIALOG_EDGE_SPACING, rcCANCELRect.Width( ), rcCANCELRect.Height( ), TRUE ); 
		
		CRect rcDividerLine( 0, cy - DIALOG_DIVIDER_LINE_Y - 1, cx, cy - DIALOG_DIVIDER_LINE_Y + 2 );

		CRect rcKeyFrameList;

		m_KeyFrameList.GetClientRect( &rcKeyFrameList );

		m_KeyFrameList.MoveWindow( DIALOG_EDGE_SPACING, DIALOG_EDGE_SPACING, cx - 2 * DIALOG_EDGE_SPACING, cy - 2 * DIALOG_EDGE_SPACING - DIALOG_DIVIDER_LINE_Y  , TRUE );
		

		

		//InvalidateRect( &rcOKRect );
		//InvalidateRect( &rcCANCELRect );
		//InvalidateRect( &rcDividerLine );
		//InvalidateRect( &rcKeyFrameList );
	}
}

BOOL CKeyFrameEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ImageBitmaps.Create( BITMAP_WIDTH, BITMAP_HEIGHT, ILC_COLOR24 | ILC_MASK, 0, 0 );

	m_Buttons[ IDOK_BTN ].SubclassDlgItem( IDOK, this );
	m_Buttons[ IDCANCEL_BTN ].SubclassDlgItem( IDCANCEL, this );
	m_Buttons[ IDREMOVEKEYFRAME_BTN ].SubclassDlgItem( IDREMOVEKEYFRAME, this );

	m_KeyFrameList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_BORDERSELECT
					  | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES /*| LVS_EX_SINGLEROW */ );
	m_KeyFrameList.SetImageList( &m_ImageBitmaps, LVSIL_SMALL );

	m_KeyFrameList.InsertColumn( 0, _T("Key Frame Sequence"), LVCFMT_LEFT | LVCFMT_BITMAP_ON_RIGHT, 300, -1 );
	m_KeyFrameList.InsertColumn( 1, _T("Start"), LVCFMT_LEFT, 50, -1 );
	m_KeyFrameList.InsertColumn( 2, _T("End"), LVCFMT_LEFT, 50, -1 );
	
	
	std::vector<CSpriteEditorDoc::KeyFrames>::const_iterator itr;
	std::vector<CSpriteEditorDoc::KeyFrames> list = m_pDoc->getKeyFrames( );

	std::ostringstream oss;
	WCHAR *wName = new WCHAR[ 10 ];
	int nLen = 10, nNewLen = 0;
	unsigned int nNumberOfItems = 0;

	// this is bad but an attempt was made not to allocate memory if not needed...
	for( itr = list.begin( ); itr != list.end( ); ++itr )
	{

		nNewLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, itr->sequenceName.c_str( ), -1, NULL, NULL );
		if( nLen < nNewLen )
		{
			delete [] wName;
			wName = new WCHAR[ nNewLen ];
			nLen = nNewLen;
		}
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, itr->sequenceName.c_str( ), -1, wName, nLen );
		m_KeyFrameList.InsertItem( nNumberOfItems, wName, nNumberOfItems );


		oss << itr->startFrame;
		nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, oss.str( ).c_str( ), -1, NULL, NULL );
		if( nLen < nNewLen )
		{
			delete [] wName;
			wName = new WCHAR[ nNewLen ];
			nLen = nNewLen;
		}
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, oss.str( ).c_str( ), -1, wName, nLen );
		m_KeyFrameList.SetItemText( nNumberOfItems, 1, wName );
		oss.str("");

		oss << itr->endFrame;
		nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, oss.str( ).c_str( ), -1, NULL, NULL );
		if( nLen < nNewLen )
		{
			delete [] wName;
			wName = new WCHAR[ nNewLen ];
			nLen = nNewLen;
		}
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, oss.str( ).c_str( ), -1, wName, nLen );
		m_KeyFrameList.SetItemText( nNumberOfItems++, 2, wName );
		oss.str("");

		CBitmap bmp;

		makeKeyFrameBitmap( &bmp, *itr );
		
		m_ImageBitmaps.Add( &bmp, RGB(0,0,0) );
		bmp.DeleteObject( );
	}
	delete [] wName;

	m_bInitButtons = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyFrameEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect rcClient;
	GetClientRect( &rcClient );

	CPen blackPen( PS_SOLID | PS_ENDCAP_SQUARE, 1, RGB(20,20,20) );
	CPen grayPen( PS_SOLID | PS_ENDCAP_SQUARE, 1, RGB(250,250,250) );
	CPen *pOldPen = dc.SelectObject( &grayPen );

	//draw divider line...
	dc.MoveTo( 10, rcClient.Height( ) - DIALOG_DIVIDER_LINE_Y );
	dc.LineTo( rcClient.Width( )-10, rcClient.Height( ) - DIALOG_DIVIDER_LINE_Y );

	dc.SelectObject( &blackPen );
	dc.MoveTo( 10, rcClient.Height( ) - DIALOG_DIVIDER_LINE_Y - 1 );
	dc.LineTo( rcClient.Width( )-10, rcClient.Height( ) - DIALOG_DIVIDER_LINE_Y - 1 );

	//dc.FillSolidRect( 0, rcClient.Height() - DIALOG_DIVIDER_LINE_Y - 1, rcClient.Width(), rcClient.Height() - DIALOG_DIVIDER_LINE_Y + 2, RGB(255,0,0) );

	dc.SelectObject( pOldPen );
}

void CKeyFrameEditorDlg::OnBnClickedRemovekeyframe( )
{
	POSITION pos = m_KeyFrameList.GetFirstSelectedItemPosition( );

	while (pos)
	{
		int nItem = m_KeyFrameList.GetNextSelectedItem( pos );

		m_pDoc->removeKeyFrame( static_cast<unsigned int>( nItem ) );

		// Remove item and image
		m_KeyFrameList.DeleteItem( nItem );
		m_ImageBitmaps.Remove( nItem );
		//if( m_KeyFrameList.GetItemCount( ) > 0 )
		//{
		//	CString newKeyFrameSequenceName;

		//	if( nItem <= 0 )
		//		newKeyFrameSequenceName = m_KeyFrameList.GetItemText( 0, 0 );
		//	else
		//		newKeyFrameSequenceName = m_KeyFrameList.GetItemText( nItem - 1, 0 );
		//	m_KeyFrameList.SetItemText( nItem - 1, 0, newKeyFrameSequenceName );
		//}
		
	
	}
}

// this one turns out to be faster than using an extra DC and the call to StretchBlt. owned...
void CKeyFrameEditorDlg::makeKeyFrameBitmap( CBitmap *bmp, const CSpriteEditorDoc::KeyFrames &kf )
{
	CDC memDC;
	CDC bmpDC;
	CBitmap frame;
	std::vector<CSpriteEditorDoc::ImageBitmap> list = m_pDoc->getFrames( );
	unsigned int frameDistance = kf.endFrame - kf.startFrame + 1;
	unsigned int framesAlongY = std::floor( (double) std::sqrt( (double) frameDistance ) );
	unsigned int framesAlongX = std::ceil( (double) frameDistance / framesAlongY );
	unsigned int frameWidth = BITMAP_WIDTH / framesAlongX;
	unsigned int frameHeight = BITMAP_HEIGHT / framesAlongY;
	
	if( kf.startFrame > list.size( ) || kf.endFrame > list.size( ) ) return;
	
	BeginWaitCursor( );

	BITMAPINFO i;
	ZeroMemory( &i.bmiHeader, sizeof(BITMAPINFOHEADER) );
	i.bmiHeader.biWidth = BITMAP_WIDTH;     // Set size you need
	i.bmiHeader.biHeight = BITMAP_HEIGHT;    // Set size you need
	i.bmiHeader.biCompression = BI_RGB;
	i.bmiHeader.biPlanes = 1;
	i.bmiHeader.biBitCount = 16; // Can be 8, 16, 32 bpp or
								   // other number
	i.bmiHeader.biSize = BITMAP_WIDTH * BITMAP_HEIGHT * (i.bmiHeader.biBitCount >> 3) * sizeof(BYTE);
	i.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	i.bmiHeader.biClrUsed = 0;
	i.bmiHeader.biClrImportant = 0;

	BYTE *data;
	HBITMAP hBmp = CreateDIBSection( bmpDC.GetSafeHdc(), &i, DIB_RGB_COLORS, (void **) &data, NULL, 0 );
	bmp->Attach( hBmp );

	memDC.CreateCompatibleDC( NULL );
	bmpDC.CreateCompatibleDC( &memDC );

	CBitmap *pOldBmp = memDC.SelectObject( bmp );


	// make all bitmaps same width and height...
	unsigned int frameNumber = kf.startFrame - 1;
	
	for( int y = 0; y < BITMAP_HEIGHT; y += frameHeight )
	{
		for( int x = 0; x < BITMAP_WIDTH && frameNumber < kf.endFrame; x += frameWidth, frameNumber++ )	
		{	
			unsigned int colorMode = list[ frameNumber ].bitsPerPixel >> 3;
			unsigned int size = frameWidth * frameHeight * colorMode * sizeof(BYTE);
			CBitmap frameBmp;
			unsigned char *bitmap = new unsigned char[ size ];

			
			// resize the bitmap using nearest neighbor...
			ImageIO::resizeImage( list[ frameNumber ].width, list[ frameNumber ].height, list[ frameNumber ].bitmap,
								  frameWidth, frameHeight, bitmap, list[ frameNumber ].bitsPerPixel,
								  ImageIO::ALG_NEARESTNEIGHBOR );							
			ImageIO::convertRGBtoBGR( frameWidth, frameHeight, colorMode * sizeof(BYTE), bitmap );

			// flip the image
			ImageIO::flipYImage( frameWidth, frameHeight, bitmap, bitmap, colorMode * sizeof(BYTE) );

			frameBmp.CreateBitmap( frameWidth, frameHeight, 1, list[ frameNumber ].bitsPerPixel, bitmap );


			CBitmap *pOld = bmpDC.SelectObject( &frameBmp );
					
			memDC.BitBlt( x, y, frameWidth, frameHeight, &bmpDC, 0, 0, SRCCOPY | NOMIRRORBITMAP );
			bmpDC.SelectObject( pOld );

			frameBmp.DeleteObject( );
			delete [] bitmap;
		}
	}

	memDC.SelectObject( pOldBmp );

	EndWaitCursor( );

	ReleaseDC( &bmpDC );
	ReleaseDC( &memDC );
}

void CKeyFrameEditorDlg::OnLvnEndlabeleditKeyframelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int item = pDispInfo->item.iItem;
	CString newSequenceName("");
	newSequenceName = pDispInfo->item.pszText;

	if( newSequenceName.IsEmpty( ) )
	{
		*pResult = 0;
		return;
	}

	int nLen = WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, newSequenceName.GetString( ), -1, NULL, 0, NULL, NULL );
	LPSTR wNewName = new CHAR[ nLen ];
	WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, newSequenceName.GetString( ), -1, wNewName, nLen, NULL, NULL );

	std::vector<CSpriteEditorDoc::KeyFrames> &list = m_pDoc->getKeyFrames( );

	if( item < list.size( ) )
		list[ item ].sequenceName.assign( wNewName );


	delete [] wNewName;

	*pResult = 1;
}
