// OpenGLWnd.cpp : implementation file
//

#include "stdafx.h"
#include "OpenGLWnd.h"
#include "Vertex.h"
#include ".\openglwnd.h"

// COpenGLWnd

IMPLEMENT_DYNCREATE(COpenGLWnd, CView)

COpenGLWnd::COpenGLWnd()
	: m_SelectionBufferSize( 16 )
{
	m_RenderScene = DrawCube; //change later
	m_SelectionBuffer = new GLuint[ m_SelectionBufferSize ];
	memset( m_SelectionBuffer, 0, sizeof(GLuint) * m_SelectionBufferSize );
	m_dNearPlane   = 1.0; 
	m_dFarPlane    = 15.0; 

}

COpenGLWnd::~COpenGLWnd()
{
	delete [] m_SelectionBuffer;
}

BEGIN_MESSAGE_MAP(COpenGLWnd, CView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// COpenGLWnd drawing
bool COpenGLWnd::SetDCPixelFormat(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = dwFlags;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	int nPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
	if (nPixelIndex == 0) // Choose default
	{
		nPixelIndex = 1;
		if (::DescribePixelFormat(hDC, nPixelIndex, 
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
			return false;
	}

	if (!::SetPixelFormat(hDC, nPixelIndex, &pixelDesc))
		return false;

	return true;
}

bool COpenGLWnd::InitOpenGL()
{
	//Get a DC for the Client Area
	m_hDC = new CClientDC(this);
	
	//Failure to Get DC
	if( m_hDC == NULL )
		return false;


	if( !SetDCPixelFormat(m_hDC->GetSafeHdc(), PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_BITMAP) )
		return false;


	//Create Rendering Context
	m_hRC = ::wglCreateContext(m_hDC->GetSafeHdc() );

	//Failure to Create Rendering Context
	if( m_hRC == 0 )
		return false;

	//Make the RC Current
	if( ::wglMakeCurrent( m_hDC->GetSafeHdc(), m_hRC ) == FALSE )
		return false;

	int err = 0;
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}



	

	
	SetGLConfig( );
	
	SetGLConfig( );
	glInitNames( );
	SetGLConfig( );
	glSelectBuffer( m_SelectionBufferSize, m_SelectionBuffer ); 
	glRenderMode( GL_SELECT  );
	SetGLConfig();
	glRenderMode( GL_RENDER );
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}

	return true;
}
void COpenGLWnd::SetGLConfig( )
{
	int err = 0;
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}
	//glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClearColor( 0.15f, 0.15f, 0.15f, 1.0f );
	glClearDepth(1.0f);
	//glEnable( GL_DEPTH_TEST );
	//glEnable( GL_TEXTURE_2D );		
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv( GL_LIGHT0, GL_POSITION, light_vector );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambient_light );

	glEnable(GL_COLOR_MATERIAL);
	glFrontFace( GL_CCW );
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);

	glEnable( GL_TEXTURE_2D );

	glPixelStorei( GL_PACK_ALIGNMENT, 4 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}
	glShadeModel( GL_SMOOTH ); //GL_FLAT	
	//glLineWidth( 1.0f );
	//glPointSize( 1.0f );
	glEnable( GL_POINT_SMOOTH );
	glEnable( GL_LINE_SMOOTH );
	//glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );  // if slow change to GL_FASTEST
	glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );  // GL_NICEST
	//glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST ); 
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

	
	if( (err = glGetError( )) != GL_NO_ERROR )
	{
		CString error;
		error.Format( _T("There was an OpenGL error. Error code %d. Will attempt to continue anyway..."), err );
		AfxMessageBox( error );
	}
}
void COpenGLWnd::DrawLine(VERTEX *v1, VERTEX *v2)
{
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
		glVertex3f(v1->v[0], v1->v[1], v1->v[2]);
		glVertex3f(v2->v[0], v2->v[1], v2->v[2]);
	glEnd();
	glPopMatrix();

}
void COpenGLWnd::DrawGrid(const float grid_size )  
{
	VERTEX v1, v2;
	//Grid Size


	//Grid Color
	//glColor4f(0.5f, 0.5f, 0.5f, 0.2f); //slightly transparent

	// Do the col lines
	//glPushMatrix( );
	//glTranslatef( 0.0f, 0.0f, 0.0f );

	for(int col = 0; col <= grid_size ; col++){

		if( col == ((int) grid_size / 2))
		{
			glColor4f(0.5f, 0.0f, 0.0f, 0.85f);
			for(int tick = (int) -grid_size / 2; tick <= grid_size / 2 ; tick++){
				if( tick == 0 )
					continue;
				v1.v[0] = (grid_size / 2.0f) - col - 0.15;
				v1.v[1] = 0.0f;
				v1.v[2] = tick;

				v2.v[0] = (grid_size / 2.0f) - col + 0.15;
				v2.v[1] = 0.0f;
				v2.v[2] = tick;
				glLineWidth(3.5f);
				DrawLine(&v1, &v2);
			}
		}
		else
			glColor4f(0.6f, 0.6f, 0.6f, 0.08f);

		v1.v[0] = (grid_size / 2.0f) - col;
		v1.v[1] = 0.0f;
		v1.v[2] = (grid_size / 2.0f);

		v2.v[0] = (grid_size / 2.0f) - col;
		v2.v[1] = 0.0f;
		v2.v[2] = -(grid_size / 2.0f);
		glLineWidth(1.0f);
		DrawLine(&v1, &v2);
	}


	// Do the row lines
	for(int row = 0; row <= grid_size ; row++){
		if( row == ( (int) grid_size / 2) )
		{
			glColor4f(0.5f, 0.0f, 0.0f, 0.85f);
			for(int tick = (int) -grid_size / 2; tick <= grid_size / 2 ; tick++){
				if( tick == 0 )
					continue;
				v1.v[0] = tick;
				v1.v[1] = 3;
				v1.v[2] = (grid_size / 2.0f) - row + 0.15f;

				v2.v[0] = tick;
				v2.v[1] = 3;
				v2.v[2] = (grid_size / 2.0f) - row - 0.15f;
				glLineWidth(3.5f);
				DrawLine(&v1, &v2);
			}
		}
		else
			glColor4f(0.5f, 0.5f, 0.5f, 0.05f);
		v1.v[0] = (grid_size / 2.0f);
		v1.v[1] = 0.0f;
		v1.v[2] = (grid_size / 2.0f) - row;

		v2.v[0] = -(grid_size / 2.0f);
		v2.v[1] = 0.0f;
		v2.v[2] = (grid_size / 2.0f) - row;
		glLineWidth(1.0f);
		DrawLine(&v1, &v2);
	}
	//glPopMatrix( );

}
void COpenGLWnd::DrawOriginAxes(const float axis_size)
{
	VERTEX origin, vx, vy, vz;
	//const unsigned int axis_size = 30.0;
	const float arrow_size = 0.25;


	origin.v[0] = 0.0f;
	origin.v[1] = 0.0f;
	origin.v[2] = 0.0f;

	vx.v[0] = axis_size;
	vx.v[1] = 0.0f;
	vx.v[2] = 0.0f;

	vy.v[0] = 0.0f;
	vy.v[1] = axis_size;
	vy.v[2] = 0.0f;

	vz.v[0] = 0.0f;
	vz.v[1] = 0.0f;
	vz.v[2] = axis_size;
	
	glLineWidth( 2.5f );
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
		// X
		glColor4f(1.0f, .0f, 0.0f, 1.0f);
		glVertex3f(origin.v[0], origin.v[1], origin.v[2]);
		glVertex3f(vx.v[0], vx.v[1], vx.v[2]);
		// Y
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(origin.v[0], origin.v[1], origin.v[2]);
		glVertex3f(vy.v[0], vy.v[1], vy.v[2]);
		// Z
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(origin.v[0], origin.v[1], origin.v[2]);
		glVertex3f(vz.v[0], vz.v[1], vz.v[2]);

	glEnd();
	glPopMatrix();

	//Draw Arrow points  --->
	glPushMatrix();
	glBegin(GL_TRIANGLES);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(arrow_size/2.0f, axis_size , 0.0f);
		glVertex3f(0.0f, axis_size + arrow_size, 0.0f);
		glVertex3f(-arrow_size/2.0f, axis_size, 0.0f);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(axis_size, -arrow_size/2.0f, 0.0f);
		glVertex3f(axis_size + arrow_size, 0.0f, 0.0f);
		glVertex3f(axis_size, arrow_size/2.0f , 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(arrow_size/2.0f,  0.0f, axis_size);
		glVertex3f(0.0f, 0.0f, axis_size + arrow_size);
		glVertex3f(-arrow_size/2.0f, 0.0f, axis_size);

	glEnd();
	glPopMatrix();
	glLineWidth( 1.0f );
}	
void COpenGLWnd::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	//SetContext( );
	RenderScene( );
	SwapGLBuffers( );
	glFlush( );
}


// COpenGLWnd diagnostics

#ifdef _DEBUG
void COpenGLWnd::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLWnd::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// COpenGLWnd message handlers

void COpenGLWnd::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	int height, width;

	width = cx;

	if( cy == 0 )
		height = 1;
	else 
		height = cy;
	
	glViewport( 0,0, width, height );
	SetFrustum( );
	glFlush( );
}

void COpenGLWnd::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	if( m_hDC )
	{
		delete m_hDC;
	}
	m_hDC = NULL;
}

int COpenGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if ( !InitOpenGL() )
	{
		MessageBox( _T("Error setting up OpenGL!"), _T("Init Error!"), MB_OK | MB_ICONERROR );
		return -1;
	}
	return 0;
}
void COpenGLWnd::RenderScene( )
{
	if( m_RenderScene != NULL )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		SetGLConfig();
		//DrawOriginAxes( );
		//DrawGrid( );
		m_RenderScene( (CSpriteEditorDoc*)(GetDocument()) );
	}
	else
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void COpenGLWnd::SetFrustum( )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0, m_dAspect, m_dNearPlane, m_dFarPlane); 
	glOrtho( -1.0, 1.0, -1.0, 1.0, 0.0, 30.0 );
	glMatrixMode(GL_MODELVIEW);
}
BOOL COpenGLWnd::OnEraseBkgnd(CDC* pDC)
{
	/*
	 *   Avoid erasing the background as it will 
	 *	 cause flickering...
	 */
	return CView::OnEraseBkgnd(pDC);
}

BOOL COpenGLWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// This is needed!!!!
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);
}
