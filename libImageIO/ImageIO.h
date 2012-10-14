#pragma once
#ifndef __IMAGEIO_H__
#define __IMAGEIO_H__
#ifdef __cplusplus
extern "C" {
namespace ImageIO {
#endif 

/*	ImageIO.h
 *
 *	Various Data Structures and Image loading routines.
 *
 *	Coded by Joseph A. Marrero
 *	http://www.l33tprogrammer.com/
 */

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif




typedef unsigned char byte;
typedef unsigned int uint;

typedef enum tagImageIOResult {
	FAILURE = 0,
	SUCCESS = 1
} Result;



/*
 *	Windows Bitmap
 */
#define	BITMAP_ID	0x4D42

#ifndef WIN32
#define WORD		unsigned short // should be 2 bytes
#define DWORD		unsigned long // should be 4 bytes
#define LONG		signed long

#define BI_RGB			0L
#define BI_RLE8			1L
#define BI_RLE4			2L
#define BI_BITFIELDS	3L
#define BI_JPEG			4L
#define BI_PNG			5L

#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;						// Specifies File Type; Must be BM (0x4D42)
	DWORD	bfSize;						// Specifies the size in bytes of the bitmap
	WORD	bfReserved1;				// Reserved; Must be zero!
	WORD	bfReserved2;				// Reserved; Must be zero!
	DWORD	bfOffBits;					// Specifies the offset, in bytes, from the 
										// beginning to the bitmap bits
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	DWORD	biSize;						// Specifies number of bytes required by the structure
	LONG	biWidth;					// Specifies the width of the bitmap, in pixels
	LONG	biHeight;					// Specifies the height of the bitmap, in pixels
	WORD	biPlanes;					// Specifies the number of color planes, must be 1
	WORD	biBitCount;					// Specifies the bits per pixel, must be 1, 4,
										// 8, 16, 24, or 32
	DWORD	biCompression;				// Specifies the type of compression
	DWORD	biSizeImage;				// Specifies the size of the image in bytes
	LONG	biXPelsPerMeter;			// Specifies the number of pixels per meter in x axis
	LONG	biYPelsPerMeter;			// Specifies the number of pixels per meter in y axis
	DWORD	biClrUsed;					// Specifies the number of colors used by the bitmap
	DWORD	biClrImportant;				// Specifies the number of colors that are important
} BITMAPINFOHEADER;
#pragma pack(pop)
#else
#include <windows.h>
#endif

/*
 *	Targa
 */
#pragma pack(push, 4)
typedef struct tagTARGAFILEHEADER{
	unsigned char imageIDLength;		// number of characters in identification field;
										// 0 denotes no identification is included.
	unsigned char colorMapType;			// type of color map; always 0
	unsigned char imageTypeCode;		//  0  -  No image data included.
										//	1  -  Uncompressed, color-mapped images.
										//	2  -  Uncompressed, RGB images.
										//	3  -  Uncompressed, black and white images.
										//	9  -  Runlength encoded color-mapped images.
										//  10 -  Runlength encoded RGB images.
										//  11 -  Compressed, black and white images.
										//  32 -  Compressed color-mapped data, using Huffman, Delta, and
										//	  	  runlength encoding.
										//  33 -  Compressed color-mapped data, using Huffman, Delta, and
										//		  runlength encoding.  4-pass quadtree-type process.
	short int colorMapOrigin;			// origin of color map (lo-hi); always 0
	short int colorMapLength;			// length of color map (lo-hi); always 0
	short int colorMapEntrySize;		// color map entry size (lo-hi); always 0;
	short int imageXOrigin;				// x coordinate of lower-left corner of image
										// (lo-hi); always 0
	short int imageYOrigin;				// y coordinate of lower-left corner of image
										// (lo-hi); always 0
	short int imageWidth;				// width of image in pixels (lo-hi)
	short int imageHeight;				// height of image in pixels (lo-hi)
	unsigned char bitCount;				// number of bits; 16, 24, 32
	unsigned char imageDescriptor;		// 24 bit = 0x00; 32-bit = 0x08
} TARGAFILEHEADER;

typedef struct tagTGAFILE{
	unsigned char imageTypeCode; // either 2 or 3
	short int imageWidth;
	short int imageHeight;
	unsigned char bitCount;
	unsigned char *imageData;
}TGAFILE;
#pragma pack(pop)

/*
 *  Startup and shutdown...
 */
void initialize( );
void deinitialize( );


/*
 *		Image IO Functions
 */
Result loadBitmapFile( const char *filename, BITMAPINFOHEADER *bitmapInfoHeader, byte **bitmap );
Result writeBitmapFile( const char *filename, const uint width, const uint height, const uint bitsPerPixel, byte *imageData );
void destroyBMPImageData( unsigned char *imageData );

Result loadTGAFile( const char *filename, TGAFILE *tgaFile );
Result writeTGAFile( const char *filename, TGAFILE *tgaFile );
void destroyTGAImageData( TGAFILE *tgaFile );

/*
 *		Functions to resize an image's pixels using Bresenham's method
 */
typedef enum tagResizeAlgorithm {
	ALG_NEARESTNEIGHBOR,
	ALG_BILINEAR,
	ALG_BILINEAR_SHARPER,
	ALG_BICUBIC,
} ResizeAlgorithm;

void resizeImage( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
				  const uint dstWidth, const uint dstHeight, byte *dstBitmap, const uint bitsPerPixel,
				  const ResizeAlgorithm algorithm );
/* uses nearest neighbor... */
void fastResizeImage( const uint srcWidth, const uint srcHeight, const uint srcBitsPerPixel, const byte *srcBitmap,
					  const uint dstWidth, const uint dstHeight, const uint dstBitsPerPixel, byte *dstBitmap );
/* bi-linear: nearest neighbor with bilinear interpolation */
void bilinearResizeImageRGBA( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
							  const uint dstWidth, const uint dstHeight, byte *dstBitmap,
							  const uint byteCount );
void bilinearSharperResizeImageRGBA( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
									const uint dstWidth, const uint dstHeight, byte *dstBitmap,
									const uint byteCount );
void bilinearResizeImageRGB( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
							 const uint dstWidth, const uint dstHeight, byte *dstBitmap,
							 const uint byteCount );
void bilinearSharperResizeImageRGB( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
								   const uint dstWidth, const uint dstHeight, byte *dstBitmap,
								   const uint byteCount );

 /* uses a cubic B-Spline */
void bicubicResizeImageRGBA( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
							 const uint dstWidth, const uint dstHeight, byte *dstBitmap,
							 const uint byteCount );
void bicubicResizeImageRGB( const uint srcWidth, const uint srcHeight, const byte *srcBitmap,
							const uint dstWidth, const uint dstHeight, byte *dstBitmap,
							const uint byteCount );
/*
 *		RGB and RGBA macros
 */
/* RGBA */
typedef struct tagRGBA {
	byte r;
	byte g;
	byte b;
	byte a;
} RGBA;
#define getRGBA(r,g,b,a)	( r << 24 || g << 16 || b << 8 || a ) // 4 bytes
#define getR32(color)		( color && 0xFF000000 >> 24 )
#define getG32(color)		( color && 0x00FF0000 >> 16 )
#define getB32(color)		( color && 0x0000FF00 >> 8 )
#define getA32(color)		( color && 0x000000FF )
/* RGB */
typedef struct tagRGB {
	byte r;
	byte g;
	byte b;
} RGB;
#define getRGB(r,g,b)		( r << 16 || g << 8 || b )
#define getR16(color)		( color && 0xFF0000 >> 16 )
#define getG16(color)		( color && 0x00FF00 >> 8 )
#define getB16(color)		( color && 0x0000FF )


/* RGB to BGR */
void swapRBinRGB( const uint width, const uint height, const uint byteCount, byte *bitmap );
/*
 *  Image Flipping Routines.
 */
void flipImageHorizontally( const uint width, const uint height, const uint byteCount, byte *bitmap );
void flipImageVertically( const uint width, const uint height, const uint byteCount, byte *bitmap );
/*
 *	Flip image vertically/horizontally without a copy...
 */
void flipXImage( const uint width, const uint height, const byte *srcBitmap, byte *dstBitmap, const uint byteCount );
void flipYImage( const uint width, const uint height, const byte *srcBitmap, byte *dstBitmap, const uint byteCount );



#ifndef __cplusplus
#define convertRGBtoBGR( width, height, byteCount, bitmap ) \
				( swapRBinRGB( width, height, byteCount, bitmap ) )
#define convertBGRtoRGB( width, height, byteCount, bitmap ) \
				( swapRBinRGB( width, height, byteCount, bitmap ) )
#else
inline void convertRGBtoBGR( const uint width, const uint height, const uint byteCount, byte *bitmap )
{ swapRBinRGB( width, height, byteCount, bitmap ); }
inline void convertBGRtoRGB( const uint width, const uint height, const uint byteCount, byte *bitmap )
{ swapRBinRGB( width, height, byteCount, bitmap ); }
#endif

typedef RGBA Pixel32;
typedef RGB Pixel24;

/*
 *	Various image processing filters...
 */
#ifndef _NO_IMAGE_PROCESSING
void detectEdges( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const uint k );
void extractColor( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const RGB *color, const uint k );
void convertToGrayscale( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap );
void convertToColorscale( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const RGB *color );
void modifyContrast( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const int contrast );
void modifyBrightness( const uint width, const uint height, const uint bitsPerPixel, const byte *srcBitmap, byte *dstBitmap, const int brightness );
#endif
/*
 *	Miscellaneous Utility functions...
 */
/* (x,y) to bitmap array index mapping macros */
#define iPosition( x, y, byteCount, width )		((uint) ((width) * (y) * (byteCount) + (x) * (byteCount)))
#define iPos		iPosition
#define iPos1( x, y, width )					((uint) ( (width) * (y) + (x) ))
#define iPos2( x, y, width )					((uint) ( ((width) * (y) << 1) + ((x) << 1 ) ))
#define iPos3( x, y, width )					((uint) ( iPosition( x, y, 3, width ) ))
#define iPos4( x, y, width )					((uint) ( ((width) * (y) << 2) + ((x) << 2 ) ))
#define linear_interpolate( alpha, x2, x1 ) ( x1 + alpha * ( x2 - x1 ) )
#define lerp linear_interpolate
#define bilinear_interpolate( alpha, beta, x1, x2, x3, x4 )		(lerp( beta, lerp( alpha, x1, x2 ), lerp( alpha, x3, x4 ) ))
#define bilerp bilinear_interpolate


#define linear_interpolate( alpha, x2, x1 ) ( x1 + alpha * ( x2 - x1 ) )
#define lerp linear_interpolate
#define bilinear_interpolate( alpha, beta, x1, x2, x3, x4 )		(lerp( beta, lerp( alpha, x1, x2 ), lerp( alpha, x3, x4 ) ))
#define bilerp bilinear_interpolate

#define P(x)	( (x) > 0 ? (x) : 0 )
#define R(x)	( (1.0/6) * (P((x)+2) * P((x)+2) * P((x)+2) - 4.0 * P((x)+1) * P((x)+1) * P((x)+1) + 6.0 * P((x)) * P((x)) * P(x) - 4.0 * P((x)-1) * P((x)-1) * P((x)-1)) )

#ifdef __cplusplus
}
} // end of ImageIO namespace
#endif
#endif
