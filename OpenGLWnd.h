#pragma once
#include <gl/gl.h>
#include <gl/glu.h>
#include "Vertex.h"
#include "SpriteEditorDoc.h"
// COpenGLWnd view


// light0
//static GLfloat light_vector[4] = {15.0f, -30.0f, 15.0f, 0.0f}; //Must relate to grid, light point down at a slight angle
static GLfloat light_vector[4] = {0.0f, 0.0f, 10.0f, 0.0f}; //Must relate to grid, light point down at a slight angle

static GLfloat ambient_light[4] = {5.0f, 5.0f, 5.0f, 1.0f}; //not to bright white light

//Generic Material
static GLfloat ambientMaterial[4] = {0.5f, 0.5f, 0.5f, 0.0f};

inline void DrawCube( CSpriteEditorDoc* doc );


class COpenGLWnd : public CView
{
	DECLARE_DYNCREATE(COpenGLWnd)

protected:
	COpenGLWnd();           // protected constructor used by dynamic creation
	virtual ~COpenGLWnd();

public:
	bool SetDCPixelFormat( HDC hDC, DWORD dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER );
	bool InitOpenGL( );
	void SetGLConfig( );
	void SetContext( ) { wglMakeCurrent( m_hDC->GetSafeHdc( ), m_hRC ); }
	void SwapGLBuffers( ) { 	SwapBuffers( m_hDC->GetSafeHdc( ) ); }
	void DrawLine( VERTEX *v1, VERTEX *v2 );
	void DrawGrid( const float grid_size = 25.0 );
	void DrawOriginAxes( const float axis_size = 1.5 );
	virtual void RenderScene( );
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	void SetFrustum( );


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	void (*m_RenderScene)( CSpriteEditorDoc* doc );	// void function pointer to the rendering

	HWND m_hWnd;
	CDC* m_hDC;
	HGLRC	m_hRC;
	double		m_dNearPlane; 
	double		m_dFarPlane; 
	double		m_dAspect;
private:
	GLuint *m_SelectionBuffer;
	GLsizei m_SelectionBufferSize;
	



	
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


inline void DrawCube( CSpriteEditorDoc* doc )
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);

	glPushMatrix( );
		glTranslatef(0.0f, 1.0f, 1.0f);
		glBegin( GL_TRIANGLE_STRIP );
		glColor4f( 1.0f, 0.0f, 0.0f, 0.25f );
			glVertex3f( -1.0f, -1.0f, 0.0f );
			glVertex3f( 1.0f, -1.0f, 0.0f );
			glVertex3f( -1.0f, 1.0f, 0.0f );
			glVertex3f( 1.0f, 1.0f, 0.0f );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
		glColor4f( 1.0f, 0.0f, 0.0f, 0.25f );
			glVertex3f( -1.0f, -1.0f, -2.0f );
			glVertex3f( 1.0f, -1.0f, -2.0f );
			glVertex3f( -1.0f, 1.0f, -2.0f );
			glVertex3f( 1.0f, 1.0f, -2.0f );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
		glColor4f( 0.0f, 1.0f, 0.0f, 0.25f );
			glVertex3f( -1.0f, -1.0f, 0.0f );
			glVertex3f( -1.0f, -1.0f, -2.0f );
			glVertex3f( -1.0f, 1.0f, 0.0f );
			glVertex3f( -1.0f, 1.0f, -2.0f );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
		glColor4f( 0.0f, 1.0f, 0.0f, 0.25f );
			glVertex3f( 1.0f, -1.0f, 0.0f );
			glVertex3f( 1.0f, -1.0f, -2.0f );
			glVertex3f( 1.0f, 1.0f, 0.0f );
			glVertex3f( 1.0f, 1.0f, -2.0f );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
		glColor4f( 0.0f, 0.0f, 1.0f, 0.25f );
			glVertex3f( -1.0f, 1.0f, 0.0f );
			glVertex3f( 1.0f, 1.0f, 0.0f );
			glVertex3f( -1.0f, 1.0f, -2.0f );
			glVertex3f( 1.0f, 1.0f, -2.0f );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
		glColor4f( 0.0f, 0.0f, 1.0f, 0.25f );
			glVertex3f( -1.0f, -1.0f, 0.0f );
			glVertex3f( 1.0f, -1.0f, 0.0f );
			glVertex3f( -1.0f, -1.0f, -2.0f );
			glVertex3f( 1.0f, -1.0f, -2.0f );
		glEnd();
	glPopMatrix( );
}
