/*	ImageIO.c
 *
 *	Various Data Structures and Image loading routines.
 *
 *	Coded by Joseph A. Marrero
 *	http://www.l33tprogrammer.com/
 */

#ifdef __cplusplus
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include "ImageIO.h"
namespace ImageIO {
#else
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "ImageIO.h"
#endif 

void initialize( )
{
#ifdef WIN32
	int i = _set_SSE2_enable( 1 );
	if( i )
		fprintf( stdout, "SSE2 enabled.\n");
	else
		fprintf( stdout, "SSE2 not enabled; processor does not support SSE2.\n");
#endif
}

void deinitialize( )
{
#ifdef WIN32
	_set_SSE2_enable( 0 );
#endif
}

unsigned char *loadBitmapFile( const char *filename, BITMAPINFOHEADER *bitmapInfoHeader )
{
	FILE				*filePtr;
	BITMAPFILEHEADER	bitmapFileHeader;
	unsigned char		*bitmapImage = NULL;	// bitmap image data
	register unsigned int		imageIdx = 0;
	unsigned char		temp = 0;				// swap variable
	unsigned short colorMode = 0;

	if( (filePtr = fopen( filename, "rb" )) == NULL )
		return NULL;

	fread( &bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr );

	if( bitmapFileHeader.bfType != BITMAP_ID ){
		fclose(filePtr);
		return NULL;
	}

	fread( bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr );
	fseek( filePtr, bitmapFileHeader.bfOffBits, SEEK_SET );
	
	bitmapImage = (unsigned char *) malloc( bitmapInfoHeader->biSizeImage );
	
	// check if allocation failed...
	if( bitmapImage == NULL ){
		fclose( filePtr );	
		return NULL;
	}

	fread( bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr );

	if( bitmapImage == NULL ){
		free( bitmapImage );
		fclose( filePtr );
		return NULL;
	}

	colorMode = bitmapInfoHeader->biBitCount >> 3;
	bitmapInfoHeader->biSizeImage = colorMode * bitmapInfoHeader->biWidth * bitmapInfoHeader->biHeight * sizeof(unsigned char);
	
	if( colorMode > 2 ) // Swap BGR to RGB...
		for( imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += colorMode ){
			assert( imageIdx + 2 < bitmapInfoHeader->biSizeImage );
			temp = bitmapImage[ imageIdx ];
			bitmapImage[ imageIdx ] = bitmapImage[ imageIdx + 2 ];
			bitmapImage[ imageIdx + 2 ] = temp;
		}
	else // Swap GGGBBBBBARRRRRGG to ARRRRRGGGGGBBBBB,
		for( imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += colorMode ){
			temp = bitmapImage[ imageIdx ];
			bitmapImage[ imageIdx ] = bitmapImage[ imageIdx + 1 ];
			bitmapImage[ imageIdx + 1 ] = temp;
		}

	fclose( filePtr );
	return bitmapImage;
}

int writeBitmapFile( const char *filename, unsigned int width, unsigned int height, unsigned int bitsPerPixel, unsigned char *imageData )
{
	FILE				*filePtr;
	BITMAPFILEHEADER	bitmapFileHeader;
	BITMAPINFOHEADER	bitmapInfoHeader;
	unsigned int		imageIdx = 0;
	unsigned char		tempRGB; // swap variable
	unsigned int byteCount = bitsPerPixel >> 3;

	if( (filePtr = fopen(filename, "wb")) == NULL )
		return 0;

	// Define the BitmapFileHeader
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = BITMAP_ID;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	// Define the BitmapInfoHeader
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = bitsPerPixel;	
	bitmapInfoHeader.biCompression = BI_RGB;		// No compression
	bitmapInfoHeader.biSizeImage = width * height * byteCount * sizeof(unsigned char);  // w * h * (RGB bytes)
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;

	// swap RGB to BGR...
	for( imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += byteCount ){
		tempRGB = imageData[ imageIdx ];
		imageData[ imageIdx ] = imageData[ imageIdx + 2 ];
		imageData[ imageIdx + 2 ] = tempRGB;
	}

	fwrite( &bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr );
	fwrite( &bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr );
	fwrite( imageData, bitmapInfoHeader.biSizeImage, 1, filePtr );

	fclose( filePtr );
	return 1;
}

void destroyBMPImageData(  unsigned char *imageData )
{
	free( imageData );
	#ifdef _DEBUG_IMAGEIO
	imageData = NULL;
	#endif
}

int loadTGAFile( const char *filename, TGAFILE *tgaFile )
{
	FILE			*filePtr;
	unsigned char	ucharBad;		// garbage unsigned char data
	short int		sintBad;		// garbage short int data
	long			imageSize;		// size of the Targa image
	int				colorMode;		// 4 for RGBA or 3 for RGB
	register long	imageIdx;
	register unsigned char	colorSwap;		// Swap variable

	if( (filePtr = fopen( filename, "rb" )) == NULL )
		return 0;

	// read in first two bytes we don't need
	fread( &ucharBad, sizeof(unsigned char), 1 , filePtr );
	fread( &ucharBad, sizeof(unsigned char), 1 , filePtr );

	//read in image type
	fread( &tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);
	
	if( /*(tgaFile->imageTypeCode != 1) &&*/
		(tgaFile->imageTypeCode != 2) && (tgaFile->imageTypeCode != 3) ){
		fclose( filePtr );
		return 0;
	}

	// read 13 bytes of data we don't need
	fread( &sintBad, sizeof(short int), 1, filePtr );
	fread( &sintBad, sizeof(short int), 1, filePtr );
	fread( &ucharBad, sizeof(unsigned char), 1, filePtr );
	fread( &sintBad, sizeof(short int), 1, filePtr );
	fread( &sintBad, sizeof(short int), 1, filePtr );

	fread( &tgaFile->imageWidth, sizeof(short int), 1 , filePtr );
	fread( &tgaFile->imageHeight, sizeof(short int), 1, filePtr );
	fread( &tgaFile->bitCount, sizeof(unsigned char), 1, filePtr );
	

	//read in one byte of data we don't need
	fread( &ucharBad, sizeof(unsigned char), 1, filePtr );

	//colorMode-> 3 = BGR, 4 = BGRA
	colorMode = tgaFile->bitCount >> 3;
	

	imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode * sizeof(unsigned char);

	tgaFile->imageData = (unsigned char *) malloc( sizeof(unsigned char) * imageSize );
	
	// check if allocation failed...
	if( tgaFile->imageData == NULL )
		return 0;

	fread( tgaFile->imageData, imageSize, 1, filePtr );

	// indexed color mode not handled!!!

	if( colorMode >= 3 ) // 32 bpp or 24 bpp
	{	// Swap BGR to RGB
		for( imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode )
		{
			colorSwap = tgaFile->imageData[ imageIdx ];
			tgaFile->imageData[ imageIdx ] = tgaFile->imageData[ imageIdx + 2 ];
			tgaFile->imageData[ imageIdx + 2 ] = colorSwap;
		}
	}
	else if( colorMode >= 2 ) // 16 bpp
	{ 
		// Swap GGGBBBBBARRRRRGG to ARRRRRGGGGGBBBBB, where each R,G,B, A is a bit
		for( imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode )
		{
			
			tgaFile->imageData[ imageIdx ] = tgaFile->imageData[ imageIdx + 1 ];
			tgaFile->imageData[ imageIdx + 1 ] = tgaFile->imageData[ imageIdx ];
		}
	}

	// For 8 bpp, nothing to swap.


	fclose( filePtr );
	return 1;
}

int writeTGAFile( const char *filename, TGAFILE *tgaFile )
{
	FILE			*filePtr;
	unsigned char	ucharBad = 0;		// garbage unsigned char data
	short int		sintBad = 0;		// garbage short int data
	long			imageSize;		// size of the Targa image
	int				colorMode;		// 4 for RGBA or 3 for RGB
	register long	imageIdx;
	register unsigned char	colorSwap;		// Swap variable


	if( ( filePtr = fopen( filename, "wb" ) ) == NULL )
		return 0;
	
	//skip first two bytes...
	fwrite( &ucharBad, sizeof(unsigned char), 1, filePtr );
	fwrite( &ucharBad, sizeof(unsigned char), 1, filePtr );

	// write in image type
	assert( tgaFile->imageTypeCode == 0x2 || tgaFile->imageTypeCode == 0x3 ); // must be 2 or 3
	fwrite( &tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr );

	// write 13 bytes of data we don't need
	fwrite( &sintBad, sizeof(short int), 1, filePtr );
	fwrite( &sintBad, sizeof(short int), 1, filePtr );
	fwrite( &ucharBad, sizeof(unsigned char), 1, filePtr );
	fwrite( &sintBad, sizeof(short int), 1, filePtr );
	fwrite( &sintBad, sizeof(short int), 1, filePtr );

	fwrite( &tgaFile->imageWidth, sizeof(short int), 1 , filePtr );
	fwrite( &tgaFile->imageHeight, sizeof(short int), 1, filePtr );
	fwrite( &tgaFile->bitCount, sizeof(unsigned char), 1, filePtr );
	

	//write in one byte of data we don't need
	fwrite( &ucharBad, sizeof(unsigned char), 1, filePtr );

	//colorMode-> 3 = BGR, 4 = BGRA
	colorMode = tgaFile->bitCount / 8;
	imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;	

	
	if( colorMode > 2 ) // 32 bpp or 24 bpp
	{	// Swap RGB to BGR
		for( imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode )
		{
			colorSwap = tgaFile->imageData[ imageIdx ];
			tgaFile->imageData[ imageIdx ] = tgaFile->imageData[ imageIdx + 2 ];
			tgaFile->imageData[ imageIdx + 2 ] = colorSwap;
		}
	}
	else { // 16 bpp
		// Swap ARRRRRGGGGGBBBBB to GGGBBBBBARRRRRGG, whereeach R,G,B, A is a bit
		for( imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode )
		{
			
			tgaFile->imageData[ imageIdx ] = tgaFile->imageData[ imageIdx + 1 ];
			tgaFile->imageData[ imageIdx + 1 ] = tgaFile->imageData[ imageIdx ];
		}
	}
	fwrite( tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr );

	fclose( filePtr );
	return 1;
}

void destroyTGAImageData( TGAFILE *tgaFile )
{
	free( tgaFile->imageData );
	#ifdef _DEBUG_IMAGEIO
	tgaFile->imageData = NULL;
	#endif
}

/*
 *  Image stretching functions...
 */
void resizeImage( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
				  const uint dstWidth, const uint dstHeight, byte *dstBitmap, const uint bitsPerPixel,
				  const ResizeAlgorithm algorithm )
{
	uint byteCount = bitsPerPixel >> 3;
	assert( srcBitmap != NULL || dstBitmap != NULL );
	assert( srcWidth != 0 || srcHeight != 0 );
	assert( byteCount > 2 );

	switch( algorithm )
	{
		case ALG_NEARESTNEIGHBOR:
			/* forces both bitmaps to have the same bit depth */
			fastResizeImage( srcWidth, srcHeight, bitsPerPixel, srcBitmap,
				             dstWidth, dstHeight, bitsPerPixel, dstBitmap );
			break;
		case ALG_BILINEAR:
			switch( byteCount )
			{
				case 0:
				case 1:
				case 2:
					assert( FALSE ); // unsupported bpp's
					break;
				case 3:
						bilinearResizeImageRGB( srcWidth, srcHeight, srcBitmap, dstWidth, dstHeight, dstBitmap, byteCount );
						break;
				case 4:
						bilinearResizeImageRGBA( srcWidth, srcHeight, srcBitmap, dstWidth, dstHeight, dstBitmap, byteCount );
						break;
				default: break;
			}
			break;
		case ALG_BILINEAR_SHARPER:
			switch( byteCount )
			{
				case 0:
				case 1:
				case 2:
					assert( FALSE ); // unsupported bpp's
					break;
				case 3:
						bilinearSharperResizeImageRGB( srcWidth, srcHeight, srcBitmap, dstWidth, dstHeight, dstBitmap, byteCount );
						break;
				case 4:
						bilinearSharperResizeImageRGBA( srcWidth, srcHeight, srcBitmap, dstWidth, dstHeight, dstBitmap, byteCount );
						break;
				default: break;
			}
			break;
		case ALG_BICUBIC: 
			switch( byteCount )
			{
				case 0:
				case 1:
				case 2:
					assert( FALSE ); // unsupported bpp's
					break;
				case 3:
						bicubicResizeImageRGB( srcWidth, srcHeight, srcBitmap, dstWidth, dstHeight, dstBitmap, byteCount );
						break;
				case 4:
						bicubicResizeImageRGBA( srcWidth, srcHeight, srcBitmap, dstWidth, dstHeight, dstBitmap, byteCount );
						break;
				default: break;
			}
			break;
		default:
			assert( FALSE ); // bad algorithm...
	}
}
//Algorithm 5 : Resizing a bitmap
//
//    * First we resize the width of the bitmap:
//    * For every pixel ( i , j ) on the output bitmap
//          o Compute the ci using formula (R5)
//          o Extract the (R,G,B) components of pixel E(ci) and E(ci)+1 in source bitmap using (R6)
//          o Mark pixel ( i , j ) on the output bitmap with color given by formula (R7)
//    * Repeat the exact same process to resize the height of the bitmap

/* uses nearest neighbor... */
void fastResizeImage( const uint srcWidth, const uint srcHeight, const uint srcBitsPerPixel, const byte *srcBitmap,
					  const uint dstWidth, const uint dstHeight, const uint dstBitsPerPixel, byte *dstBitmap ) // uses nearest neighbor
{
	uint srcByteCount = srcBitsPerPixel >> 3;
	uint dstByteCount = dstBitsPerPixel >> 3;
	register uint x = 0;
	register uint y = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	//uint srcY, srcX; //don't remove
	uint dstPos, srcPos;
	register uint srcYtimesWidth;
	register uint yTimesWidth;

	assert( srcBitsPerPixel != 0 || dstBitsPerPixel != 0 );
	assert( srcBitmap != NULL || dstBitmap != NULL );
	assert( srcWidth != 0 || srcHeight != 0 );
	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;
	
	// don't remove...
	// x_in = x_out * (w_in / w_out), using nearest neighbor
	//for( y = 0; y < dstHeight; y++ )
	//{
	//	srcY = (uint) y * verticalStretchFactor;
	//	assert( srcY < srcHeight );

	//	for( x = 0; x < dstWidth; x++ )
	//	{
	//		srcX = (uint) x * horizontalStretchFactor;
	//		assert( srcX < srcWidth );

	//		dstPos = y * dstWidth * dstByteCount + x * dstByteCount;
	//		srcPos = (uint) srcY * srcWidth * srcByteCount + srcX * srcByteCount;

	//		memcpy( &dstBitmap[ dstPos ], &srcBitmap[ srcPos ], srcByteCount * sizeof(byte) );
	//	}
	//}

	// optimized version of above code...
	for( y = 0; y < dstHeight; y++ )
	{
		srcYtimesWidth = ((uint)(y * verticalStretchFactor)) * srcWidth * srcByteCount;
		yTimesWidth = y * dstWidth * dstByteCount;

		for( x = 0; x < dstWidth; x++ )
		{
			dstPos = yTimesWidth + x * dstByteCount;
			srcPos = srcYtimesWidth + ((uint)(x * horizontalStretchFactor)) * srcByteCount;		

			memcpy( &dstBitmap[ dstPos ], &srcBitmap[ srcPos ], dstByteCount * sizeof(byte) );
		}
	}

}



/* bi-linear: nearest neighbor with bilinear interpolation */
void bilinearResizeImageRGBA( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
				  const uint dstWidth, const uint dstHeight, byte *dstBitmap,
				  const uint byteCount )
{
	register uint x = 0;
	register uint y = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	register uint srcY = 0;
	register uint srcX = 0;
	register uint dstPos,
		 neighborPos1,
		 neighborPos2,
		 neighborPos3,
		 neighborPos4;
	register byte n1R, n1G, n1B, n1A,	/* color vectors */
				  n2R, n2G, n2B, n2A,
				  n3R, n3G, n3B, n3A,
				  n4R, n4G, n4B, n4A;
	uint largestSrcIndex = srcWidth * srcHeight * byteCount;

	assert( byteCount == 4 );

	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;  /* stretch vector */
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;

	for( y = 0; y < dstHeight; y++ )
	{
		srcY = (uint) (y * verticalStretchFactor);
		if( srcY == 0 ) srcY = 1;

		for( x = 0; x < dstWidth; x++ )
		{
			srcX = (uint) (x * horizontalStretchFactor);
			dstPos = y * dstWidth * byteCount + x * byteCount;

			neighborPos1 = (srcY) * srcWidth * byteCount + (srcX) * byteCount;
			neighborPos2 = (srcY) * srcWidth * byteCount + (srcX+1) * byteCount;
			neighborPos3 = (srcY+1) * srcWidth * byteCount + (srcX) * byteCount;
			neighborPos4 = (srcY+1) * srcWidth * byteCount + (srcX+1) * byteCount;
			if( neighborPos1 >= largestSrcIndex ) neighborPos1 = largestSrcIndex - byteCount;
			if( neighborPos2 >= largestSrcIndex ) neighborPos2 = largestSrcIndex - byteCount;
			if( neighborPos3 >= largestSrcIndex ) neighborPos3 = largestSrcIndex - byteCount;
			if( neighborPos4 >= largestSrcIndex ) neighborPos4 = largestSrcIndex - byteCount;

			// source pixel...
			n1R = srcBitmap[ neighborPos1 ];
			n1G = srcBitmap[ neighborPos1 + 1 ];
			n1B = srcBitmap[ neighborPos1 + 2 ];
			n1A = srcBitmap[ neighborPos1 + 3 ];

			// source pixel's neighbor2...
			n2R = srcBitmap[ neighborPos2 ];
			n2G = srcBitmap[ neighborPos2 + 1 ];
			n2B = srcBitmap[ neighborPos2 + 2 ];
			n2A = srcBitmap[ neighborPos2 + 3 ];
				
			// source pixel's neighbor3...
			n3R = srcBitmap[ neighborPos3 ];
			n3G = srcBitmap[ neighborPos3 + 1 ];
			n3B = srcBitmap[ neighborPos3 + 2 ];
			n3A = srcBitmap[ neighborPos3 + 3 ];

			// source pixel's neighbor4...
			n4R = srcBitmap[ neighborPos4 ];
			n4G = srcBitmap[ neighborPos4 + 1 ];
			n4B = srcBitmap[ neighborPos4 + 2 ];
			n4A = srcBitmap[ neighborPos4 + 3 ];

			dstBitmap[ dstPos ] = (byte) bilerp( 0.5, 0.5, n4R, n3R, n2R, n1R );
			dstBitmap[ dstPos + 1 ] = (byte) bilerp( 0.5, 0.5, n4G, n3G, n2G, n1G );
			dstBitmap[ dstPos + 2 ] = (byte) bilerp( 0.5, 0.5, n4B, n3B, n2B, n1B );
			dstBitmap[ dstPos + 3 ] = (byte) bilerp( 0.5, 0.5, n4A, n3A, n2A, n1A );
		}
	}
}

void bilinearSharperResizeImageRGBA( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
									const uint dstWidth, const uint dstHeight, byte *dstBitmap,
									const uint byteCount )
{
	register uint x = 0;
	register uint y = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	register uint srcY = 0;
	register uint srcX = 0;
	register uint dstPos, srcPos,
		 neighborPos1,
		 neighborPos2,
		 neighborPos3,
		 neighborPos4;
	register byte R, G, B, A,
			 n1R, n1G, n1B, n1A,	/* color vectors */
			 n2R, n2G, n2B, n2A,
			 n3R, n3G, n3B, n3A,
			 n4R, n4G, n4B, n4A;
	uint largestSrcIndex = srcWidth * srcHeight * byteCount;

	assert( byteCount == 4 );

	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;  /* stretch vector */
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;

	

	for( y = 0; y < dstHeight; y++ )
	{
		srcY = (uint) (y * verticalStretchFactor);
		if( srcY == 0 ) srcY = 1;

		for( x = 0; x < dstWidth; x++ )
		{
			srcX = (uint) (x * horizontalStretchFactor);
			dstPos = y * dstWidth * byteCount + x * byteCount;

			neighborPos1 = (srcY-1) * srcWidth * byteCount + (srcX-1) * byteCount;
			neighborPos2 = (srcY-1) * srcWidth * byteCount + (srcX+1) * byteCount;
			neighborPos3 = (srcY+1) * srcWidth * byteCount + (srcX-1) * byteCount;
			neighborPos4 = (srcY+1) * srcWidth * byteCount + (srcX+1) * byteCount;
			srcPos = (uint) srcY * srcWidth * byteCount + srcX * byteCount;

			if( neighborPos1 < 0 ) neighborPos1 = 0;
			if( neighborPos2 < 0 ) neighborPos2 = 0;
			if( neighborPos3 < 0 ) neighborPos3 = 0;
			if( neighborPos4 < 0 ) neighborPos4 = 0;
			if( neighborPos1 >= largestSrcIndex ) neighborPos1 = largestSrcIndex - byteCount;
			if( neighborPos2 >= largestSrcIndex ) neighborPos2 = largestSrcIndex - byteCount;
			if( neighborPos3 >= largestSrcIndex ) neighborPos3 = largestSrcIndex - byteCount;
			if( neighborPos4 >= largestSrcIndex ) neighborPos4 = largestSrcIndex - byteCount;

			// source pixel...
			R = srcBitmap[ srcPos ];
			G = srcBitmap[ srcPos + 1 ];
			B = srcBitmap[ srcPos + 2 ];
			A = srcBitmap[ srcPos + 3 ];

			// source pixel's neighbor1...
			n1R = srcBitmap[ neighborPos1 ];
			n1G = srcBitmap[ neighborPos1 + 1 ];
			n1B = srcBitmap[ neighborPos1 + 2 ];
			n1A = srcBitmap[ neighborPos1 + 3 ];

			// source pixel's neighbor2...
			n2R = srcBitmap[ neighborPos2 ];
			n2G = srcBitmap[ neighborPos2 + 1 ];
			n2B = srcBitmap[ neighborPos2 + 2 ];
			n2A = srcBitmap[ neighborPos2 + 3 ];
				
			// source pixel's neighbor3...
			n3R = srcBitmap[ neighborPos3 ];
			n3G = srcBitmap[ neighborPos3 + 1 ];
			n3B = srcBitmap[ neighborPos3 + 2 ];
			n3A = srcBitmap[ neighborPos3 + 3 ];

			// source pixel's neighbor4...
			n4R = srcBitmap[ neighborPos4 ];
			n4G = srcBitmap[ neighborPos4 + 1 ];
			n4B = srcBitmap[ neighborPos4 + 2 ];
			n4A = srcBitmap[ neighborPos4 + 3 ];

			// comes out a little more burry than I expected, so I lerp between R and the 
			// output from the bilerp of the other 4 samples.
			dstBitmap[ dstPos ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4R, n3R, n2R, n1R ), R );
			dstBitmap[ dstPos + 1 ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4G, n3G, n2G, n1G ), G );
			dstBitmap[ dstPos + 2 ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4B, n3B, n2B, n1B ), B );
			dstBitmap[ dstPos + 3 ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4A, n3A, n2A, n1A ), A );
		}
	}
}

void bilinearResizeImageRGB( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
				  const uint dstWidth, const uint dstHeight, byte *dstBitmap,
				  const uint byteCount )
{
	register uint x = 0;
	register uint y = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	register uint srcY = 0;
	register uint srcX = 0;
	register uint dstPos, 
		 neighborPos1,
		 neighborPos2,
		 neighborPos3,
		 neighborPos4;
	register byte n1R, n1G, n1B, 	/* color vectors */
				  n2R, n2G, n2B, 
				  n3R, n3G, n3B, 
				  n4R, n4G, n4B;

	assert( byteCount == 3 );

	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;  /* stretch vector */
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;

	
	for( y = 0; y < dstHeight; y++ )
	{
		srcY = (uint) (y * verticalStretchFactor);
		assert( srcY < srcHeight );

		for( x = 0; x < dstWidth; x++ )
		{
			srcX = (uint) (x * horizontalStretchFactor);
			dstPos = y * dstWidth * byteCount + x * byteCount;

			neighborPos1 = (srcY) * srcWidth * byteCount + (srcX) * byteCount;
			neighborPos2 = (srcY) * srcWidth * byteCount + (srcX+1) * byteCount;
			neighborPos3 = (srcY+1) * srcWidth * byteCount + (srcX) * byteCount;
			neighborPos4 = (srcY+1) * srcWidth * byteCount + (srcX+1) * byteCount;

			// source pixel...
			n1R = srcBitmap[ neighborPos1 ];
			n1G = srcBitmap[ neighborPos1 + 1 ];
			n1B = srcBitmap[ neighborPos1 + 2 ];

			// source pixel's neighbor2...
			n2R = srcBitmap[ neighborPos2 ];
			n2G = srcBitmap[ neighborPos2 + 1 ];
			n2B = srcBitmap[ neighborPos2 + 2 ];
				
			// source pixel's neighbor3...
			n3R = srcBitmap[ neighborPos3 ];
			n3G = srcBitmap[ neighborPos3 + 1 ];
			n3B = srcBitmap[ neighborPos3 + 2 ];

			// source pixel's neighbor4...
			n4R = srcBitmap[ neighborPos4 ];
			n4G = srcBitmap[ neighborPos4 + 1 ];
			n4B = srcBitmap[ neighborPos4 + 2 ];

			dstBitmap[ dstPos ] = (byte) bilerp( 0.5, 0.5, n4R, n3R, n2R, n1R );
			dstBitmap[ dstPos + 1 ] = (byte) bilerp( 0.5, 0.5, n4G, n3G, n2G, n1G );
			dstBitmap[ dstPos + 2 ] = (byte) bilerp( 0.5, 0.5, n4B, n3B, n2B, n1B );
		}
	}
}


void bilinearSharperResizeImageRGB( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
								   const uint dstWidth, const uint dstHeight, byte *dstBitmap,
								   const uint byteCount )
{
	register uint x = 0;
	register uint y = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	register uint srcY = 0;
	register uint srcX = 0;
	register uint dstPos, srcPos,
		 neighborPos1,
		 neighborPos2,
		 neighborPos3,
		 neighborPos4;
	register byte R, G, B,
			 n1R, n1G, n1B, 	/* color vectors */
			 n2R, n2G, n2B, 
			 n3R, n3G, n3B, 
			 n4R, n4G, n4B;
	uint largestSrcIndex = srcWidth * srcHeight * byteCount;

	assert( byteCount == 3 );

	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;  /* stretch vector */
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;
	
	for( y = 0; y < dstHeight; y++ )
	{
		srcY = (uint) (y * verticalStretchFactor);
		assert( srcY < srcHeight );

		for( x = 0; x < dstWidth; x++ )
		{
			srcX = (uint) (x * horizontalStretchFactor);
			dstPos = y * dstWidth * byteCount + x * byteCount;

			neighborPos1 = (srcY-1) * srcWidth * byteCount + (srcX-1) * byteCount;
			neighborPos2 = (srcY-1) * srcWidth * byteCount + (srcX+1) * byteCount;
			neighborPos3 = (srcY+1) * srcWidth * byteCount + (srcX-1) * byteCount;
			neighborPos4 = (srcY+1) * srcWidth * byteCount + (srcX+1) * byteCount;
			srcPos = (uint) srcY * srcWidth * byteCount + srcX * byteCount;

			if( neighborPos1 < 0 ) neighborPos1 = 0;
			if( neighborPos2 < 0 ) neighborPos2 = 0;
			if( neighborPos3 < 0 ) neighborPos3 = 0;
			if( neighborPos4 < 0 ) neighborPos4 = 0;
			if( neighborPos1 >= largestSrcIndex ) neighborPos1 = largestSrcIndex - byteCount;
			if( neighborPos2 >= largestSrcIndex ) neighborPos2 = largestSrcIndex - byteCount;
			if( neighborPos3 >= largestSrcIndex ) neighborPos3 = largestSrcIndex - byteCount;
			if( neighborPos4 >= largestSrcIndex ) neighborPos4 = largestSrcIndex - byteCount;

			// source pixel...
			R = srcBitmap[ srcPos ];
			G = srcBitmap[ srcPos + 1 ];
			B = srcBitmap[ srcPos + 2 ];

			// source pixel's neighbor1...
			n1R = srcBitmap[ neighborPos1 ];
			n1G = srcBitmap[ neighborPos1 + 1 ];
			n1B = srcBitmap[ neighborPos1 + 2 ];

			// source pixel's neighbor2...
			n2R = srcBitmap[ neighborPos2 ];
			n2G = srcBitmap[ neighborPos2 + 1 ];
			n2B = srcBitmap[ neighborPos2 + 2 ];
				
			// source pixel's neighbor3...
			n3R = srcBitmap[ neighborPos3 ];
			n3G = srcBitmap[ neighborPos3 + 1 ];
			n3B = srcBitmap[ neighborPos3 + 2 ];

			// source pixel's neighbor4...
			n4R = srcBitmap[ neighborPos4 ];
			n4G = srcBitmap[ neighborPos4 + 1 ];
			n4B = srcBitmap[ neighborPos4 + 2 ];

			// comes out a little more burry than I expected, so I lerp between R and the 
			// output from the bilerp of the other 4 samples.
			dstBitmap[ dstPos ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4R, n3R, n2R, n1R ), R );
			dstBitmap[ dstPos + 1 ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4G, n3G, n2G, n1G ), G );
			dstBitmap[ dstPos + 2 ] = (byte) lerp( 0.5, bilerp( 0.5, 0.5, n4B, n3B, n2B, n1B ), B );
		}
	}
}
 /* uses a cubic B-Spline */
void bicubicResizeImageRGBA( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
							 const uint dstWidth, const uint dstHeight, byte *dstBitmap,
							 const uint byteCount )
{
	register uint x = 0;
	register uint y = 0;
	register int m = 0, n = 0, i = 0, j = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	register float srcY = 0;
	register float srcX = 0;
	register float dy = 0;
	register float dx = 0;
	register uint dstPos, srcPos;
	uint largestSrcIndex = srcWidth * srcHeight * byteCount;
	register uint sumR = 0;
	register uint sumG = 0;
	register uint sumB = 0;
	register uint sumA = 0;

	assert( byteCount == 4 );

	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;  /* stretch vector */
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;

	

	for( y = 0; y < dstHeight; y++ )
	{
		srcY = y * verticalStretchFactor;
		j = (int) floor( srcY );
		if( srcY == 0 ) srcY = 1;
		

		for( x = 0; x < dstWidth; x++ )
		{
			srcX = x * horizontalStretchFactor;
			i = (int) floor( srcX );
			
			dstPos = y * dstWidth * byteCount + x * byteCount;
			sumR = 0;
			sumG = 0;
			sumB = 0;
			sumA = 0;
			dy = srcY - j;
			dx = srcX - i;
			
			for( m = -1; m <= 2; m++ )
				for( n = -1; n <= 2; n++ )
				{
					srcPos = iPosition( i + m, j + n, byteCount, srcWidth );
					if( srcPos > largestSrcIndex ) srcPos = largestSrcIndex - byteCount;

					sumR +=	(uint) srcBitmap[ srcPos ] * R( m - dx ) * R( dy - n );
					sumG +=	(uint) srcBitmap[ srcPos + 1 ] * R( m - dx ) * R( dy - n );
					sumB +=	(uint) srcBitmap[ srcPos + 2 ] * R( m - dx ) * R( dy - n );
					sumA +=	(uint) srcBitmap[ srcPos + 3 ] * R( m - dx ) * R( dy - n );
				}

			dstBitmap[ dstPos ] = (byte) sumR;
			dstBitmap[ dstPos + 1 ] = (byte) sumG;
			dstBitmap[ dstPos + 2 ] = (byte) sumB;
			dstBitmap[ dstPos + 3 ] = (byte) sumA;
		}
	}
}

void bicubicResizeImageRGB( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
							const uint dstWidth, const uint dstHeight, byte *dstBitmap,
							const uint byteCount )
{
	register uint x = 0;
	register uint y = 0;
	register int m = 0, n = 0, i = 0, j = 0;
	float horizontalStretchFactor = 0;
	float verticalStretchFactor = 0;
	register float srcY = 0;
	register float srcX = 0;
	register float dy = 0;
	register float dx = 0;
	register uint dstPos, srcPos;
	uint largestSrcIndex = srcWidth * srcHeight * byteCount;
	register uint sumR = 0;
	register uint sumG = 0;
	register uint sumB = 0;
	assert( byteCount == 4 );

	horizontalStretchFactor = (float) srcWidth / (float) dstWidth;  /* stretch vector */
	verticalStretchFactor = (float) srcHeight / (float) dstHeight;

	

	for( y = 0; y < dstHeight; y++ )
	{
		srcY = y * verticalStretchFactor;
		j = floor( srcY );
		if( srcY == 0 ) srcY = 1;
		

		for( x = 0; x < dstWidth - 2; x++ )
		{
			srcX = x * horizontalStretchFactor;
			i = (int) floor( srcX );
			
			dstPos = y * dstWidth * byteCount + x * byteCount;
			sumR = 0;
			sumG = 0;
			sumB = 0;
			dy = srcY - j;
			dx = srcX - i;
			
			for( m = -1; m <= 2; m++ )
				for( n = -1; n <= 2; n++ )
				{
					srcPos = iPosition( i + m, j + n, byteCount, srcWidth );
					if( srcPos > largestSrcIndex ) srcPos = largestSrcIndex - byteCount;

					sumR +=	(uint) srcBitmap[ srcPos ] * R( m - dx ) * R( dy - n );
					sumG +=	(uint) srcBitmap[ srcPos + 1 ] * R( m - dx ) * R( dy - n );
					sumB +=	(uint) srcBitmap[ srcPos + 2 ] * R( m - dx ) * R( dy - n );
				}

			dstBitmap[ dstPos ] = (byte) sumR;
			dstBitmap[ dstPos + 1 ] = (byte) sumG;
			dstBitmap[ dstPos + 2 ] = (byte) sumB;
		}
	}
}

/*
 *	Swap Red and blue colors in RGB abd RGBA functions
 */
void swapRBinRGB( const uint width, const uint height, const uint byteCount, byte *bitmap ) /* RGB to BGR */
{
	register uint y;
	register uint x;
	assert( byteCount != 0 );
	assert( bitmap != NULL );

	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
		{
			uint pos = width * y * byteCount + x * byteCount;
			//swap
			//byte temp = bitmap[ pos ];
			//bitmap[ pos ] = bitmap[ pos + 2 ];
			//bitmap[ pos + 2 ] = bitmap[ pos ];
			
			// fast swap using XOR...
			bitmap[ pos ] = bitmap[ pos ] ^ bitmap[ pos + 2 ];
			bitmap[ pos + 2 ] = bitmap[ pos + 2 ] ^ bitmap[ pos ];
			bitmap[ pos ] = bitmap[ pos ] ^ bitmap[ pos + 2 ]; 
		}
}

/*
 *  Image Flipping Routines. These are slow... :(
 */
void flipImageHorizontally( const uint width, const uint height, const uint byteCount, byte *bitmap )
{
	register uint x = 0;
	register uint y = 0;
	byte *srcBitmap = (byte *) malloc( sizeof(byte) * width * height * byteCount );
	memcpy( srcBitmap, bitmap, sizeof(byte) * width * height * byteCount );
	
	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
			memcpy( &bitmap[ iPosition(width - x - 1, y, byteCount, width) ], &srcBitmap[ iPosition(x, y, byteCount, width) ], sizeof(byte) * byteCount );

	free( srcBitmap );
}

void flipImageVertically( const uint width, const uint height, const uint byteCount, byte *bitmap )
{
	register uint x = 0;
	register uint y = 0;
	byte *srcBitmap = (byte *) malloc( sizeof(byte) * width * height * byteCount );
	memcpy( srcBitmap, bitmap, sizeof(byte) * width * height * byteCount );
	
	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
			memcpy( &bitmap[ iPosition(x, height - y - 1, byteCount, width) ], &srcBitmap[ iPosition(x, y, byteCount, width) ], sizeof(byte) * byteCount );

	free( srcBitmap );
}
/*
 *	Flip image vertically/horizontally without a copy...
 */
void flipXImage( const uint width, const uint height, const byte *srcBitmap, byte *dstBitmap, const uint byteCount )
{
	register uint j, i;
	register uint j_times_width = 0;
	register uint b = 0;
	register byte *temp = (byte *) malloc( width * byteCount * sizeof(byte) ); // temp scan line, just in case src = dst

	for( j = 0; j < height; j++ )
	{
		j_times_width = j* width * byteCount; // avoids doing this twice
		
		memcpy( &temp[ 0 ], &srcBitmap[ j_times_width ], width * byteCount * sizeof(byte) );

		for( i = 0; i < width; i++ )
			memcpy( &dstBitmap[ j_times_width + (width - 1 - i) * byteCount ], &temp[ i * byteCount ], byteCount * sizeof(byte) );
	}

	free( temp );
}

void flipYImage( const uint width, const uint height, const byte *srcBitmap, byte *dstBitmap, const uint byteCount )
{
	register uint j, i;
	register uint i_times_bytecount = 0;
	register uint width_times_bytecount = 0;
	register uint b = 0;
	register byte *temp = (byte *) malloc( height * byteCount * sizeof(byte) ); // temp column line, just in case src = dst

	for( i = 0; i < width; i++ )
	{
		i_times_bytecount = i * byteCount;
		width_times_bytecount = width * byteCount;

		for( j = 0; j < height; j++ )
			memcpy( &temp[ j * byteCount ], &srcBitmap[ j * width_times_bytecount + i_times_bytecount ], byteCount * sizeof(byte) );

		for( j = 0; j < height; j++ )
			memcpy( &dstBitmap[ (height - 1 - j) * width_times_bytecount + i_times_bytecount ], &temp[ j * byteCount ], byteCount * sizeof(byte) );
	}

	free( temp );
}


//Algorithm 1 : Edge Detection
//
//    * For every pixel ( i , j ) on the source bitmap
//          o Extract the (R,G,B) components of this pixel, its right neighbour (R1,G1,B1), and its bottom neighbour (R2,G2,B2)
//          o Compute D(C,C1) and D(C,C2) using (R1)
//          o If D(C,C1) OR D(C,C2) superior to a parameter K, then we have an edge pixel !
// Edge Detection: k is the maximum color distance that signifies an edge
void detectEdges( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const uint k )
{
	uint byteCount = bitsPerPixel >> 3; // 4 ==> RGBA32, 3==>RGB32 , 2 ==> RGB16
	register uint y;
	register uint x;
	assert( bitsPerPixel != 0 );
	assert( srcBitmap != NULL || dstBitmap != NULL );

	for( y = 0; y < height - 1; y++ )
		for( x = 0; x < width - 1; x++ )
		{
			// r = right, b = bottom
			uint pos = width * y * byteCount + x * byteCount;
			uint rpos = width * y * byteCount + (x + 1) * byteCount;
			uint bpos = width * (y + 1) * byteCount + x * byteCount;

			// this pixel
			byte R = srcBitmap[ pos ];
			byte G = srcBitmap[ pos + 1 ];
			byte B = srcBitmap[ pos + 2 ];

			// right neighbor
			byte rR = srcBitmap[ rpos ];
			byte rG = srcBitmap[ rpos + 1 ];
			byte rB = srcBitmap[ rpos + 2 ];

			// bottom neighbor
			byte bR = srcBitmap[ bpos ];
			byte bG = srcBitmap[ bpos + 1 ];
			byte bB = srcBitmap[ bpos + 2 ];

			//uint D1 = sqrt( (R-rR)*(R-rR) + (G-rG)*(G-rG) + (B-rB)*(B-rB) );
			//uint D2 = sqrt( (R-bR)*(R-bR) + (G-bG)*(G-bG) + (B-bB)*(B-bB) );
			
			// if any of the distances are greater than k, put a white pixel in the destination
			if( (R-rR)*(R-rR) + (G-rG)*(G-rG) + (B-rB)*(B-rB) >= k*k ||
				(R-bR)*(R-bR) + (G-bG)*(G-bG) + (B-bB)*(B-bB) >= k*k ) //if( D1 > k || D2 > k ) 
			{
				dstBitmap[ pos ] = 0xFF;
				dstBitmap[ pos + 1 ] = 0xFF;
				dstBitmap[ pos + 2 ] = 0xFF;
			}
			else { // otherwise place a black pixel;
				dstBitmap[ pos ] = 0x00;
				dstBitmap[ pos + 1 ] = 0x00;
				dstBitmap[ pos + 2 ] = 0x00;			
			}

		}
}

//Algorithm 2 : Color extraction
//
//    * For every pixel ( i , j ) on the source bitmap
//          o Extract the C = (R,G ,B) components of this pixel.
//          o Compute D(C,C0) using (R1)
//          o If D(C,C0) inferior to a parameter K, we found a pixel which colour's matches the colour we are looking for. We mark it in white. Otherwise we leave it in black on the output bitmap.
// Color Extraction: Marks white all the pixels that are no greater than k distance to the color.
void extractColor( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const RGB *color, const uint k )
{
	uint byteCount = bitsPerPixel >> 3; // 4 ==> RGBA32, 3==>RGB32 , 2 ==> RGB16
	register uint y;
	register uint x;
	assert( bitsPerPixel != 0 );
	assert( srcBitmap != NULL || dstBitmap != NULL );

	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
		{
			uint pos = width * y * byteCount + x * byteCount;
			// this pixel
			byte R = srcBitmap[ pos ];
			byte G = srcBitmap[ pos + 1 ];
			byte B = srcBitmap[ pos + 2 ];

			if( (R-color->r)*(R-color->r) + (G-color->g)*(G-color->g) + (B-color->b)*(B-color->b) <= k*k )//if( sqrt( (R-color->r) + (G-color->g) + (B-color->b) ) <= k )
			{
				dstBitmap[ pos ] = 0xFF;
				dstBitmap[ pos + 1 ] = 0xFF;
				dstBitmap[ pos + 2 ] = 0xFF;
			}
			else {
				dstBitmap[ pos ] = 0x00;
				dstBitmap[ pos + 1 ] = 0x00;
				dstBitmap[ pos + 2 ] = 0x00;
			}
		}
}

//Algorithm 3-1 : Grayscale conversion
//
//    * For every pixel ( i , j ) on the source bitmap
//          o Extract the C = (R,G ,B) components of this pixel.
//          o Compute Grayscale(C) using (R4)
//          o Mark pixel ( i , j ) on the output bitmap with color (Grayscale(C), Grayscale(C), Grayscale(C)).
void convertToGrayscale( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap )
{
	uint byteCount = bitsPerPixel >> 3; // 4 ==> RGBA32, 3==>RGB32 , 2 ==> RGB16
	register uint y;
	register uint x;
	assert( bitsPerPixel != 0 );
	assert( srcBitmap != NULL || dstBitmap != NULL );

	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
		{
			uint pos = width * y * byteCount + x * byteCount;
			// this pixel
			byte R = srcBitmap[ pos ];
			byte G = srcBitmap[ pos + 1 ];
			byte B = srcBitmap[ pos + 2 ];

			uint colorAverage = (R + G + B) / 3;
			
			dstBitmap[ pos ] = colorAverage;
			dstBitmap[ pos + 1 ] = colorAverage;
			dstBitmap[ pos + 2 ] = colorAverage;
		}
}

//Algorithm 3-2 : Colorscale conversion
//
//    * For every pixel ( i , j ) on the source bitmap
//          o Extract the C = (R,G ,B) components of this pixel.
//          o Compute Grayscale(C) using (R4)
//          o Mark pixel ( i , j ) on the output bitmap with color (Grayscale(C), Grayscale(C), Grayscale(C)).

void convertToColorscale( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const RGB *color )
{
	uint byteCount = bitsPerPixel >> 3; // 4 ==> RGBA32, 3==>RGB32 , 2 ==> RGB16
	register uint y;
	register uint x;
	assert( (color->r != 0 && color->g != 0 && color->b != 0) ||
			(color->r != 0 || color->g != 0 || color->b != 0) ); // no black, because 0 vector
	assert( srcBitmap != NULL || dstBitmap != NULL );
	assert( bitsPerPixel != 0 );

	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
		{
			uint pos = width * y * byteCount + x * byteCount;
			// this pixel
			byte R = srcBitmap[ pos ];
			byte G = srcBitmap[ pos + 1 ];
			byte B = srcBitmap[ pos + 2 ];

			//float a = sqrt(R*R + G*G + B*B);
			//float b = sqrt((float)color->r*color->r + (float)color->g*color->g + (float)color->b*color->b);
			//float colorScaled = (R*color->r + G*color->g + B*color->b) / ( a * b );
			
			float colorScaled = (float) ( (R*color->r + G*color->g + B*color->b) / ( sqrt(R*R + G*G + B*B) * sqrt(color->r*color->r + color->g*color->g + color->b*color->b) ));
			
			dstBitmap[ pos ] = (byte) colorScaled * R;
			dstBitmap[ pos + 1 ] = (byte) colorScaled * G;
			dstBitmap[ pos + 2 ] = (byte) colorScaled * B;
		}
}

//Algorithm 4 : Light or contrast modification
//
//    * Generate the look-up table for the desired transform (Light or contrast or any other). For each index j from 0 to 255 is stored its image by the transform in transform_list[ j ].
//    * For every pixel ( i , j ) on the source bitmap
//          o Extract the C = (R,G ,B) components of this pixel.
//          o Mark pixel ( i , j ) on the output bitmap with color (transform_list[R], transform_list[G], transform_list[B]).
void modifyContrast( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const int contrast )
{
	uint byteCount = bitsPerPixel >> 3; // 4 ==> RGBA32, 3==>RGB32 , 2 ==> RGB16
	register uint y;
	register uint x;
	register uint colorIndex;
	uint transform[ 256 ];
	assert( srcBitmap != NULL || dstBitmap != NULL );
	assert( bitsPerPixel != 0 );

	for( colorIndex = 0; colorIndex < 256; colorIndex++ )
	{
		float slope = (float) tan(contrast);
		if( colorIndex < (uint) (128.0f + 128.0f*slope) && colorIndex > (uint) (128.0f-128.0f*slope) )
			transform[ colorIndex ] = (uint) ((colorIndex - 128) / slope + 128);
		else if( colorIndex >= (uint) (128.0f + 128.0f*slope) )
			transform[ colorIndex ] = 255;
		else // colorIndex <= 128 -128*slope 
			transform[ colorIndex ] = 0;
	}

	
	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
		{
			uint pos = width * y * byteCount + x * byteCount;
			// this pixel
			byte R = srcBitmap[ pos ];
			byte G = srcBitmap[ pos + 1 ];
			byte B = srcBitmap[ pos + 2 ];

			dstBitmap[ pos ] = transform[ R ];
			dstBitmap[ pos + 1 ] = transform[ G ];
			dstBitmap[ pos + 2 ] = transform[ B ];
		}
}

void modifyBrightness( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const int brightness )
{
	uint byteCount = bitsPerPixel >> 3; // 4 ==> RGBA32, 3==>RGB32 , 2 ==> RGB16
	register uint y;
	register uint x;
	register uint colorIndex;
	uint transform[ 256 ];
	assert( srcBitmap != NULL || dstBitmap != NULL );
	assert( bitsPerPixel != 0 );

	for( colorIndex = 0; colorIndex < 256; colorIndex++ )
	{
		transform[ colorIndex ] = colorIndex + brightness;
		if( transform[ colorIndex ] > 255 )
			transform[ colorIndex ] = 255;
		if( transform[ colorIndex ] < 0 )
			transform[ colorIndex ] = 0;

	}

	
	for( y = 0; y < height; y++ )
		for( x = 0; x < width; x++ )
		{
			uint pos = width * y * byteCount + x * byteCount;
			// this pixel
			byte R = srcBitmap[ pos ];
			byte G = srcBitmap[ pos + 1 ];
			byte B = srcBitmap[ pos + 2 ];

			dstBitmap[ pos ] = transform[ R ];
			dstBitmap[ pos + 1 ] = transform[ G ];
			dstBitmap[ pos + 2 ] = transform[ B ];
		}
}


#ifdef __cplusplus
} // end of ImageIO namespace
#endif
