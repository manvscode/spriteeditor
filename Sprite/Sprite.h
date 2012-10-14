#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <map>

class CSprite
{
  public:
	CSprite( );
	virtual ~CSprite( );

  protected:
	  typedef struct tagKeyFrameSequence {
		  std::string sequenceName; // used for debugging
		  unsigned int start; 
		  unsigned int end;
		  float startU, startV;
		  float endU, endV;
	  } KeyFrameSequence;

	unsigned char *m_Texels;
	unsigned int m_Width,
		         m_Height;
	unsigned int m_FrameWidth, m_FrameHeight; // (u,v) coordinates 
	unsigned int m_NumberOfFrames; 
	std::map<unsigned int, KeyFrameSequence> m_KeyFrameMap; // maps a sprite's state to a key frame
															// sequence.
};

#endif