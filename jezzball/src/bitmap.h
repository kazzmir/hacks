#ifndef _bitmap_h
#define _bitmap_h

#include <allegro.h>

class Bitmap{
public:

	Bitmap( int x, int y );
	Bitmap( BITMAP * who );
	Bitmap( const char * load_file );
	Bitmap( const char * load_file, int sx, int sy );
	Bitmap( const char * load_file, int sx, int sy, double accuracy );
	Bitmap( const Bitmap & copy );
	Bitmap( const Bitmap & copy, int sx, int sy );
	Bitmap( const Bitmap & copy, int sx, int sy, double accuracy );
	Bitmap( const Bitmap & copy, int x, int y, int width, int height );
	~Bitmap();

	int getWidth();
	int getHeight();

	void printf( int x, int y, int color, FONT * f, const char * str, ... );
	void rectangle( int x1, int y1, int x2, int y2, int color );
	void rectangleFill( int x1, int y1, int x2, int y2, int color );
	void draw( int x, int y, Bitmap * where );
	void drawTrans( int x, int y, Bitmap * where );
	void Blit( int x, int y, Bitmap * where );
	void fill( int color );
	void circlefill( int x, int y, int radius, int color );

	bool getError();

	BITMAP * getBitmap() const;
	int getPixel( int x, int y );

	static int makeColor( int r, int g, int b );

	static const int MaskColor = MASK_COLOR_16;

protected:
	BITMAP * my_bitmap;
	bool own;
	bool error;

};

#endif
