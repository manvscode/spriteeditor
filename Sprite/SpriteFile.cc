#include <fstream>
#include <cassert>
#include <cstdio>
#include <iostream>
#include "SpriteFile.h"

namespace SpriteIO {

bool writeSpriteFile( const char *filename, const SpriteFileHeader &header )
{
	std::ofstream out;
	out.open( filename, std::ios_base::out | std::ios_base::binary );

	out << SPRITEFILE_IDCODE;
	out << header.uiBitsPerPixel;
	out << header.usByteCount;
	out << header.uiWidth;
	out << header.uiHeight;
	out << header.uiFrameWidth;
	out << header.uiFrameHeight;
	out << header.uiNumberOfFrames;
	out << header.uiNumberOfStates;
	out << header.uiSize;

	for( uint i = 0; i < 128; i++ )
		out << header.description[ i ];

	SpriteFileHeader *tempHeader = const_cast<SpriteFileHeader *>( &header );

	tempHeader->uiOffsetToTexels = static_cast<uint>( out.tellp( ) ) + sizeof(header.uiOffsetToTexels);
	out << header.uiOffsetToTexels;

	out.write( (char *) header.bTexels, sizeof(unsigned char) * header.uiSize );

	out.close( );
	return out.good( );
}

bool openSpriteFile( const char *filename, SpriteFileHeader &header )
{
	std::ifstream in;
	in.open( filename, std::ios_base::in | std::ios_base::binary );
	
	int fileCode = 0;
	in >> fileCode;

	if( fileCode != SPRITEFILE_IDCODE )
	{
		std::cerr << "File, " << filename << ", is not a sprite file." << std::endl;
		return false;
	}
	
	in >> header.uiBitsPerPixel;
	in >> header.usByteCount;
	in >> header.uiWidth;
	in >> header.uiHeight;
	in >> header.uiFrameWidth;
	in >> header.uiFrameHeight;
	in >> header.uiNumberOfFrames;
	in >> header.uiNumberOfStates;
	in >> header.uiSize;

	for( uint i = 0; i < 128; i++ )
		in >> header.description[ i ];

	header.uiOffsetToTexels = in.tellg( );

	in.read( (char *) header.bTexels, sizeof(unsigned char) * header.uiSize );

	in.close( );
	return in.bad( );
}

} // end of namespace