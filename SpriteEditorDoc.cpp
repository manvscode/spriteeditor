// 2DSpriteEditorDoc.cpp : implementation of the CSpriteEditorDoc class
//

#include "stdafx.h"
#include "SpriteEditor.h"

#include "SpriteEditorDoc.h"
#include <gl/gl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpriteEditorDoc

IMPLEMENT_DYNCREATE(CSpriteEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSpriteEditorDoc, CDocument)
END_MESSAGE_MAP()


// CSpriteEditorDoc construction/destruction

CSpriteEditorDoc::CSpriteEditorDoc()
: m_Semaphore( 1, 1 )
{
	// TODO: add one-time construction code here
	m_CurrentFrame = 1;
	m_CreateTextureImage = false;
	m_CreateSpriteFile = false;
}

CSpriteEditorDoc::~CSpriteEditorDoc()
{
	while( m_Frames.size( ) > 0 )
	{
		ImageBitmap image = m_Frames.back( );
		delete [] image.bitmap;
		m_Frames.pop_back( );
	}
}

BOOL CSpriteEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_Name = "Untitled";
	
	m_TextureWidth = 1024;
	m_TextureHeight = 1024;
	m_Playbackspeed = 1;
	m_CreateTextureImage = false;
	m_CreateSpriteFile = false;
	m_CurrentFrame = 1;


	// delete the textures from OpenGL...
	if( m_Frames.size( ) > 0 )
	{
		GLuint *textures = new GLuint[ m_Frames.size( ) ];

		for( unsigned int i = 0; i < m_Frames.size( ); i++ )
			textures[ i ] = i + 1;

		glDeleteTextures( (GLuint) m_Frames.size( ), textures );

		delete [] textures;
	}

	while( m_Frames.size( ) > 0 )
	{
		ImageBitmap image = m_Frames.back( );
		delete [] image.bitmap;
		m_Frames.pop_back( );
	}

	while( m_KeyFrames.size( ) > 0 )
	{
		KeyFrames kf = m_KeyFrames.back( );
		m_KeyFrames.pop_back( );
	}

	return TRUE;
}




// CSpriteEditorDoc serialization

void CSpriteEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) // save
	{
		// avoid saving in unicode...
		ar << m_Name.size( );
		for( unsigned int i = 0; i < m_Name.size( ); i++ )
			ar << m_Name[ i ];

		// save the number of frames...
		ar << m_Frames.size( );

		// save the bitmap data...
		std::vector<ImageBitmap>::const_iterator itr;
		for( itr = m_Frames.begin(); itr != m_Frames.end(); ++itr )
		{
			//unsigned int width;
			//unsigned int height;
			//unsigned short bitsPerPixel;
			//unsigned char *bitmap;
			ar  << itr->width;
			ar << itr->height;
			ar << itr->bitsPerPixel;
			unsigned int size = itr->width * itr->height * itr->bitsPerPixel / 8; 
			//save each bitmap...
			for( unsigned int i = 0; i < size; i++ )
				ar << itr->bitmap[ i ];
		}
		
		ar << m_TextureWidth;
		ar << m_TextureHeight;
		ar << m_Playbackspeed;
		ar << m_CreateTextureImage;
		ar << m_CreateSpriteFile;
		ar << m_CurrentFrame;
	
		// save key frames...
		ar << m_KeyFrames.size( );
		std::vector<CSpriteEditorDoc::KeyFrames>::const_iterator kfItr;
		
		for( kfItr = m_KeyFrames.begin(); kfItr != m_KeyFrames.end(); ++kfItr )
		{
			ar << kfItr->sequenceName.size( );
			for( unsigned int i = 0; i < kfItr->sequenceName.size( ); i++ )
				ar << kfItr->sequenceName[ i ];
			
			ar << kfItr->startFrame;
			ar << kfItr->endFrame;
		}
	}
	else // open
	{
		// wipe anything out...
		OnNewDocument( ); 

		// avoid loading unicode...
		unsigned int name_size = 0;
		ar >> name_size;
		m_Name = ""; //reset to blank
		for( unsigned int i = 0; i < name_size; i++ )
		{
			char c = '\0';
			ar >> c;
			m_Name.push_back( c );
		}
		
		unsigned int size = 0;
		ar >> size;

		// load the bitmap data...
		for( unsigned int frame = 0; frame < size; frame++ )
		{
			ImageBitmap image;
			//unsigned int width;
			//unsigned int height;
			//unsigned short bitsPerPixel;
			//unsigned char *bitmap;
			ar  >> image.width;
			ar >> image.height;
			ar >> image.bitsPerPixel;
			unsigned int size = image.width * image.height * image.bitsPerPixel / 8;

			image.bitmap = new unsigned char[ size ];

			// load each bitmap...
			for( unsigned int i = 0; i < size; i++ )
				ar >> image.bitmap[ i ];

			m_Frames.push_back( image );
		}

		ar >> m_TextureWidth;
		ar >> m_TextureHeight;
		ar >> m_Playbackspeed;
		ar >> m_CreateTextureImage;
		ar >> m_CreateSpriteFile;
		ar >> m_CurrentFrame;

		// load key frames...
		unsigned int numberOfKeyFrames = 0;
		ar >> numberOfKeyFrames;
	
		for( unsigned int keyFrame = 0; keyFrame < numberOfKeyFrames; keyFrame++ )
		{
			KeyFrames kf;
			
			unsigned int seqname_size = 0;
			ar >> seqname_size;
			kf.sequenceName = ""; //reset to blank
			for( unsigned int i = 0; i < seqname_size; i++ )
			{
				char c = '\0';
				ar >> c;
				kf.sequenceName.push_back( c );
			}


			ar >> kf.startFrame;
			ar >> kf.endFrame;
			m_KeyFrames.push_back( kf );
		}

		UpdateAllViews( NULL );
	}

	
}


// CSpriteEditorDoc diagnostics

#ifdef _DEBUG
void CSpriteEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSpriteEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


void CSpriteEditorDoc::setName( const std::string &name )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	m_Name = name;
	singleLock.Unlock( );
}

std::string CSpriteEditorDoc::getName( ) const
{
	return m_Name;
}

void CSpriteEditorDoc::setTextureWidth( unsigned int width )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	m_TextureWidth = width;
	singleLock.Unlock( );
}

unsigned int CSpriteEditorDoc::getTextureWidth( ) const
{
	return m_TextureWidth;
}

void CSpriteEditorDoc::setTextureHeight( unsigned int height )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	m_TextureHeight = height;
	singleLock.Unlock( );
}

unsigned int CSpriteEditorDoc::getTextureHeight( ) const
{
	return m_TextureHeight;
}

void CSpriteEditorDoc::addFrame( const CSpriteEditorDoc::ImageBitmap &imageBitmap )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );


	m_Frames.push_back( imageBitmap );
	singleLock.Unlock( );
}

void CSpriteEditorDoc::removeFrame( unsigned int frame )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	ASSERT( frame != 0 ); // 0 is default texture... causes problems
	std::vector<ImageBitmap>::iterator itr;
	
	int idx = 1;
	for( itr = m_Frames.begin( ); itr != m_Frames.end( ); ++itr )
		if( frame == idx++ )
		{
			delete [] itr->bitmap;
			m_Frames.erase( itr );
			break;
		}

	singleLock.Unlock( );
}

void CSpriteEditorDoc::setCurrentFrame( unsigned int frame )
{
	CSingleLock singleLock( &m_Semaphore );
	singleLock.Lock( );
	ASSERT( frame != 0 ); // 0 is default texture... causes problems
	m_CurrentFrame = frame;
	singleLock.Unlock( );
}

CSpriteEditorDoc *CSpriteEditorDoc::getDoc( )
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	return (CSpriteEditorDoc *) pFrame->GetActiveDocument();
}

void CSpriteEditorDoc::setKeyFrame( unsigned int Index, const CSpriteEditorDoc::KeyFrames &keyframe )
{
	ASSERT( Index >= 0 && Index < m_KeyFrames.size( ) );
	KeyFrames &kf = m_KeyFrames[ Index ];
	kf.sequenceName = keyframe.sequenceName;
	kf.startFrame = keyframe.startFrame;
	kf.endFrame = keyframe.endFrame;
}

void CSpriteEditorDoc::addKeyFrame( const CSpriteEditorDoc::KeyFrames &keyframe )
{
	m_KeyFrames.push_back( keyframe );
}

void CSpriteEditorDoc::removeKeyFrame( unsigned int Index )
{
	if( m_KeyFrames.size( ) > 0 )
		m_KeyFrames.erase( m_KeyFrames.begin( ) + Index );
}

std::vector<CSpriteEditorDoc::ImageBitmap> CSpriteEditorDoc::getCopyOfFrames( ) const// can't use copy-constructor
{
	std::vector<CSpriteEditorDoc::ImageBitmap> copy;
	std::vector<CSpriteEditorDoc::ImageBitmap>::const_iterator itr;
	unsigned int size = 0;

	for( itr = m_Frames.begin( ); itr != m_Frames.end( ); ++itr )
	{
		ImageBitmap cpyBmp;

		cpyBmp.bitsPerPixel = itr->bitsPerPixel;
		cpyBmp.width = itr->width;
		cpyBmp.height = itr->height;
		size = cpyBmp.width * cpyBmp.height * (cpyBmp.bitsPerPixel >> 3) * sizeof(BYTE);
		cpyBmp.bitmap = new BYTE[ size ];

		for( unsigned int i = 0; i < size; i++ )
			cpyBmp.bitmap[ i ] = itr->bitmap[ i ];		
	
		copy.push_back( cpyBmp );
	}

	return copy;	
}

// CSpriteEditorDoc commands
