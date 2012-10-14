#ifndef _SPRITEFILE_H_
#define _SPRITEFILE_H_


namespace SpriteIO {

#define SPRITEFILE_IDCODE	(0xdeadbeef)

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;


typedef struct tagSpriteFileHeader {
	uint uiBitsPerPixel;
	ushort usByteCount;
	uint uiWidth;
	uint uiHeight;
	uint uiFrameWidth;
	uint uiFrameHeight;
	uint uiNumberOfFrames;
	uint uiNumberOfStates;
	uint uiSize;
	char description[ 128 ];
	uint uiOffsetToTexels;
	byte *bTexels;
} SpriteFileHeader;

//typedef struct tagSpriteKeyFrame {
//	char sequenceName[ 64 ];
//	uint uiStartFrame;
//	uint uiEndFrame;
//} SpriteKeyFrame;

bool writeSpriteFile( const char *filename, const SpriteFileHeader &header );
bool openSpriteFile( const char *filename, SpriteFileHeader &header );

} //end of namespace
#endif