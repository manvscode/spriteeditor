// 2DSpriteEditorDoc.h : interface of the CSpriteEditorDoc class
//


#pragma once
#include <afxmt.h>
#include <vector>
#include <string>
#include <ImageIO.h>


class CSpriteEditorDoc : public CDocument
{
protected: // create from serialization only
	CSpriteEditorDoc();
	DECLARE_DYNCREATE(CSpriteEditorDoc)

// Attributes
public:
	typedef struct tagImageBitmap {
		unsigned int width;
		unsigned int height;
		unsigned short bitsPerPixel;
		unsigned char *bitmap;
	} ImageBitmap;

	typedef struct tagKeyFrames {
		std::string sequenceName;
		unsigned short startFrame;
		unsigned short endFrame;
	} KeyFrames;

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	virtual ~CSpriteEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void setName( const std::string &name );
	std::string getName( ) const;

	void setTextureWidth( unsigned int width );
	unsigned int getTextureWidth( ) const;
	void setTextureHeight( unsigned int height );
	unsigned int getTextureHeight( ) const;

	void addFrame( const CSpriteEditorDoc::ImageBitmap &imageBitmap );
	void removeFrame( unsigned int frame );
	
	unsigned int getNumberOfFrames( ) const
	{ return (unsigned int) m_Frames.size( ); }

	void setCurrentFrame( unsigned int frame );
	const unsigned int getCurrentFrame( ) const
	{ return m_CurrentFrame; }

	const std::vector<CSpriteEditorDoc::ImageBitmap> &getFrames( ) const
	{ return m_Frames; }

	std::vector<CSpriteEditorDoc::ImageBitmap> getCopyOfFrames( ) const;// can't use copy-constructor

	void setKeyFrame( unsigned int Index, const CSpriteEditorDoc::KeyFrames &keyframe );
	void addKeyFrame( const CSpriteEditorDoc::KeyFrames &keyframe );
	void removeKeyFrame( unsigned int Index );

	const std::vector<CSpriteEditorDoc::KeyFrames> getKeyFrames( ) const
	{ return m_KeyFrames; }
	
	std::vector<CSpriteEditorDoc::KeyFrames> &getKeyFrames( )
	{ return m_KeyFrames; }

	static CSpriteEditorDoc *getDoc( );

	


protected:
	std::string m_Name;
	std::vector<CSpriteEditorDoc::ImageBitmap> m_Frames;
	std::vector<CSpriteEditorDoc::KeyFrames> m_KeyFrames;
	unsigned int m_TextureWidth, m_TextureHeight;
	unsigned int m_Playbackspeed;
	bool m_CreateTextureImage, m_CreateSpriteFile;
	unsigned int m_CurrentFrame;

	CSemaphore m_Semaphore;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


