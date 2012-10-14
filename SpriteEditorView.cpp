// 2DSpriteEditorView.cpp : implementation of the CSpriteEditorView class
//

#include "stdafx.h"
#include "SpriteEditor.h"

#include "SpriteEditorDoc.h"
#include "SpriteEditorView.h"
#include "MainFrm.h"
#include <cmath>
#include <sstream>
#include "messages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpriteEditorView

IMPLEMENT_DYNCREATE(CSpriteEditorView, COpenGLWnd)

BEGIN_MESSAGE_MAP(CSpriteEditorView, COpenGLWnd)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &COpenGLWnd::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &COpenGLWnd::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &COpenGLWnd::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_MESSAGE(UWM_DRAW_NEXTSPRITEFRAME, OnDrawNextFrame)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CSpriteEditorView construction/destruction

CSpriteEditorView::CSpriteEditorView()
: COpenGLWnd( ), m_isPrinting(false)
{
	// TODO: add construction code here
	m_zoom = 1.2f;
	m_xpos = 0.0f;
	m_ypos = 0.0f;
	
}

CSpriteEditorView::~CSpriteEditorView()
{
}

BOOL CSpriteEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style = cs.style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return COpenGLWnd::PreCreateWindow(cs);
}

// CSpriteEditorView drawing

void CSpriteEditorView::OnDraw(CDC* pDC)
{
	CSpriteEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	glLoadIdentity( );


	//SetGLConfig();

	RenderScene( );
	
	if( !m_isPrinting )
		SwapGLBuffers( );
}


// CSpriteEditorView printing

BOOL CSpriteEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	//pInfo->SetMinPage( 1 );
	//pInfo->SetMaxPage( 1 );
	//pInfo->m_rectDraw.top = 400;
	return DoPreparePrinting(pInfo);
}

void CSpriteEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSpriteEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSpriteEditorView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
// TODO: Add your specialized code here and/or call the base class
	CSpriteEditorDoc* pDoc = reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) );
	CString name( pDoc->getName( ).c_str( ) );
	CRect rcClient;
	m_isPrinting = true;
	//CDC MemDC;
	//HDC hDC = ::GetDC( this->GetSafeHwnd() );
	//MemDC.Attach( hDC );
	//MemDC.CreateCompatibleDC(NULL);
	DWORD code;
	CBitmap bmp;
	UINT32 wpix = pDC->GetDeviceCaps(HORZRES);
	UINT32 hpix = pDC->GetDeviceCaps(VERTRES);

	wpix = (UINT32) wpix - 0.2 * wpix;
	hpix = (UINT32) hpix - 0.4 * hpix;

	BITMAPINFO i;
	ZeroMemory( &i.bmiHeader, sizeof(BITMAPINFOHEADER) );
	i.bmiHeader.biWidth = wpix;     // Set size you need
	i.bmiHeader.biHeight = hpix;    // Set size you need
	i.bmiHeader.biCompression = BI_RGB;
	i.bmiHeader.biPlanes = 1;
	i.bmiHeader.biBitCount = 32; 
	i.bmiHeader.biSize = wpix * hpix * (32 >> 3) * sizeof(BYTE);
	i.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	i.bmiHeader.biClrUsed = 0;
	i.bmiHeader.biClrImportant = 0;

	code = GetLastError();
	LPVOID data;
	HBITMAP hBmp = CreateDIBSection( pDC->GetSafeHdc(), &i, DIB_RGB_COLORS, &data, NULL, NULL );
	bmp.Attach( hBmp );
	CBitmap *pOldBmp = pDC->SelectObject( &bmp );

	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd,0,sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR) ;
	pfd.nVersion = 1 ;
	pfd.dwFlags = PFD_DRAW_TO_BITMAP  // replaces PFD_DRAW_TO_WINDOW
				  |PFD_SUPPORT_OPENGL |
				  PFD_SUPPORT_GDI ;
	pfd.iPixelType = PFD_TYPE_RGBA ; 
	pfd.cColorBits = 32 ;
	pfd.cDepthBits = 8 ;
	pfd.iLayerType = PFD_MAIN_PLANE ; 
	code = GetLastError();
	int pix_index = ChoosePixelFormat(pDC->GetSafeHdc(),&pfd);
	code = GetLastError();
	if(!SetPixelFormat(pDC->GetSafeHdc(),pix_index,&pfd))
	{
		code = GetLastError();
	}










	//VERIFY( SetDCPixelFormat( hDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI |  PFD_DRAW_TO_WINDOW ) );

	

	GetClientRect( &rcClient );
	int err = 0;
	




	
	

	//wglMakeCurrent( NULL, NULL );


	HGLRC hRC = wglCreateContext( pDC->GetSafeHdc( ) );
	ASSERT( hRC != 0 );
	//wglMakeCurrent( NULL, NULL );
	VERIFY( wglMakeCurrent( pDC->GetSafeHdc( ), hRC ) );
	
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}

	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClearDepth( 1.0f );
	glPopAttrib( );
	//glEnable( GL_TEXTURE_2D );

	//glPixelStorei( GL_PACK_ALIGNMENT, 4 );
	//glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

	glViewport( 0, 0, wpix, hpix );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho( (float)(wpix)/(float)(hpix)*-m_zoom-m_xpos, (float)(wpix)/(float)(hpix)*m_zoom-m_xpos,
		-m_zoom+m_ypos, m_zoom+m_ypos, -100.0f, 100.0f );
	glMatrixMode( GL_MODELVIEW );




	//Draw scene...
	OnDraw( pDC );


	

	BeginWaitCursor( );
	//pDC->BitBlt( 0, 0, wpix, hpix, &pDC, 0, 0, SRCCOPY );
	EndWaitCursor( );

	pDC->SelectObject( pOldBmp );
	//pDC->SelectObject( pPen );
	
	//Release hRC and return to the other context...
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	SetContext( );

	bmp.DeleteObject( );
	//ReleaseDC( &MemDC );
	m_isPrinting = false;
	//COpenGLWnd::OnPrint(pDC, pInfo);
}

// CSpriteEditorView diagnostics

#ifdef _DEBUG
void CSpriteEditorView::AssertValid() const
{
	COpenGLWnd::AssertValid();
}

void CSpriteEditorView::Dump(CDumpContext& dc) const
{
	COpenGLWnd::Dump(dc);
}

CSpriteEditorDoc* CSpriteEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpriteEditorDoc)));
	return (CSpriteEditorDoc*)m_pDocument;
}
#endif //_DEBUG

void CSpriteEditorView::RenderScene( )
{
	if( m_RenderScene != NULL )
	{
		if( m_isPrinting )
		{
			glPushAttrib( GL_COLOR_BUFFER_BIT );
				glClearColor( 1.0f,0.0f,0.0f, 0.0f );
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glPopAttrib( );
		}
		else
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		Render( reinterpret_cast<CSpriteEditorDoc *>( GetDocument( ) ) );
		
	}
	else
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void CSpriteEditorView::Render( CSpriteEditorDoc *pDoc )
{
	static float mat[4] = {0.5f, 0.5f, 0.5f, 0.0f};
	glPushMatrix( );
		glColor3f( 0.2f, 0.2f, 0.2f );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat );
		glBindTexture( GL_TEXTURE_2D, pDoc->getCurrentFrame( ) );
		glBegin( GL_QUADS );
			glTexCoord2i( 0, 1 ); glVertex2i( -1, 1 );
			glTexCoord2i( 0, 0 ); glVertex2i( -1, -1 );
			glTexCoord2i( 1, 0 ); glVertex2i( 1, -1 );
			glTexCoord2i( 1, 1 ); glVertex2i( 1, 1 );
		glEnd( );
	glPopMatrix( );

	DrawSpriteBox( );
	//DrawCube( pDoc );
}

void CSpriteEditorView::OnSize(UINT nType, int cx, int cy)
{
	COpenGLWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if ( 0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED )
		return;

	// Change the orthographic viewing volume to
	// reflect the new dimensions of the window
	// and the zoom and position of the viewport.
	//SetContext();
	glViewport( 0, 0, cx, cy );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( (float)(cx)/(float)(cy)*-m_zoom-m_xpos, (float)(cx)/(float)(cy)*m_zoom-m_xpos,
		-m_zoom+m_ypos, m_zoom+m_ypos, -100.0f, 100.0f );
	glMatrixMode( GL_MODELVIEW );
}


void CSpriteEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar)
	{
		case VK_UP:
			m_ypos = m_ypos - 0.001f * m_zoom;
			break;
		case VK_DOWN:
			m_ypos = m_ypos + 0.001f * m_zoom;
			break;
		case VK_LEFT:
			m_xpos = m_xpos - 0.001f * m_zoom;
			break;
		case VK_RIGHT:
			m_xpos = m_xpos + 0.001f * m_zoom;
			break;
		default:                  
			break;
	} 
	OnDraw( NULL );
	COpenGLWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSpriteEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect cr;
	GetClientRect( &cr );

	if ( nFlags /*& MK_CONTROL*/ )
	{
		
		if ( nFlags & MK_RBUTTON )
		{
			// Middle mouse button is being
			// pressed. Zoom the camera.
			if ( m_lastMouseY != -1 )
			{
				if( m_zoom < 1 )
					m_zoom += (point.y - m_lastMouseY) * 0.025f * pow( 0.5f, 1 / m_zoom);
				else
					m_zoom += (point.y - m_lastMouseY) * 0.025f;
				
				if( m_zoom < 1.0f )
					m_zoom = 1.0f;
				else if( m_zoom > 20 )
					m_zoom = 20;
				
				CString szZoomStr;
				szZoomStr.Format( _T("Zoom: %.2f x"), m_zoom );

				CMainFrame *m = (CMainFrame *) AfxGetMainWnd( );
				m->setStatusBar( 1, szZoomStr );

				//m_zoomFactor = m_zoom;
				// Apply the position changes to
				// the viewport.
				OnSize( SIZE_MAXIMIZED, cr.Width(), cr.Height() );
				OnDraw( NULL );
			}
			m_lastMouseY = point.y;
		}
		//else if ( nFlags & MK_MBUTTON )
		//{
		//	// Right mouse button is being
		//	// pressed. Pan the camera.
		//	if ( m_lastMouseX != -1 )
		//	{
		//		
		//		m_xpos += (point.x - m_lastMouseX) * 0.001f * m_zoom;
		//		m_ypos += (point.y - m_lastMouseY) * 0.001f * m_zoom;
		//		
		//		// Apply the position changes to
		//		// the viewport.

		//		if( m_xpos < -60.0f )
		//			m_xpos = -60.0f;
		//		else if( m_xpos > 60.0f )
		//			m_xpos = 60.0f;

		//		if( m_ypos < -60.0f )
		//			m_ypos = -60.0f;
		//		else if( m_ypos > 60.0f )
		//			m_ypos = 60.0f;
		//		OnSize( SIZE_MAXIMIZED, cr.Width(), cr.Height() );
		//		OnDraw( NULL );
		//	}
		//	m_lastMouseX = point.x;
		//	m_lastMouseY = point.y;
		//}
		else
		{
			// No mouse button was pressed.
			// Mark the mouse flags to indicate
			// the camera did not move last
			// message.
			m_lastMouseX = -1;
			m_lastMouseY = -1;
		}
	}
	else
	{
		// Control was not pressed.
		// Mark the mouse flags to indicate
		// the camera did not move last
		// message.
		m_lastMouseX = -1;
		m_lastMouseY = -1;
	}
	COpenGLWnd::OnMouseMove(nFlags, point);
}

void CSpriteEditorView::DrawSpriteBox( )
{
	glPushAttrib( GL_COLOR );
	glPushMatrix( );
		glLineWidth( 2.0f );
		glColor4f( 1.01f, 1.01f, 1.01f, 1.0f );
		glBegin( GL_LINE_LOOP );
			glVertex2f( -1.01f, 1.01f );
			glVertex2f( -1.01f, -1.01f );
			glVertex2f( 1.01f, -1.01f );
			glVertex2f( 1.01f, 1.01f );
		glEnd( );
	glPopMatrix( );
	glPopAttrib( );
}

// CSpriteEditorView message handlers



int CSpriteEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COpenGLWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetContext( );
	return 0;
}


void CSpriteEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class
	this->RedrawWindow( 0, 0, RDW_INVALIDATE | RDW_NOERASE );
}

LRESULT CSpriteEditorView::OnDrawNextFrame( WPARAM wParam, LPARAM lParam )
{
	CSpriteEditorDoc* pDoc = CSpriteEditorDoc::getDoc( );
	unsigned int i = pDoc->getCurrentFrame( );
	if( i == pDoc->getNumberOfFrames( ) )
		i = 0;
	pDoc->setCurrentFrame( ++i );
	pDoc->SetModifiedFlag( );
	RedrawWindow( 0, 0, RDW_INVALIDATE | RDW_NOERASE  );
	UpdateData( FALSE );
	//pDoc->UpdateAllViews( NULL );
	return 0;
}
BOOL CSpriteEditorView::OnEraseBkgnd(CDC* pDC)
{
	/*
	 *   Avoid erasing the background as it will 
	 *	 cause flickering...
	 */
	return COpenGLWnd::OnEraseBkgnd(pDC);
}
