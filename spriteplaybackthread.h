#pragma once

#include "SpriteEditorDoc.h"


// CSpritePlaybackThread

class CSpritePlaybackThread : public CWinThread
{
	DECLARE_DYNCREATE(CSpritePlaybackThread)

protected:
	UINT m_PlaybackSpeed; // in milliseconds...
	HWND m_hwndParent;
	CSpriteEditorDoc *m_pDoc;
	BOOL m_isAnimating;


public:

	CSpritePlaybackThread( HWND hwndParent/*, CMy2DSpriteEditorDoc *pDoc*/ );
	virtual ~CSpritePlaybackThread();
	
	

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void setPlaybackSpeed( UINT speed );
	void setAnimation( const BOOL animate = TRUE );
	

protected:
	DECLARE_MESSAGE_MAP()
	CSpritePlaybackThread( );
	void killThread( );


public:

	
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
};


