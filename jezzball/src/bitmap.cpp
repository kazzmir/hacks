#include <allegro.h>
#include "bitmap.h"
#include <stdarg.h>
#include <iostream>

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

Bitmap::Bitmap( int x, int y ):
error( false ){
	my_bitmap = create_bitmap( x, y );
	clear( my_bitmap );
	own = true;
}

Bitmap::Bitmap( BITMAP * who ):
error( false ){
	my_bitmap = who;
	own = false;
}

Bitmap::Bitmap( const char * load_file ):
error( false ){
	my_bitmap = load_bitmap( load_file, NULL );
	if ( !my_bitmap ){
		my_bitmap = create_bitmap( 100, 100 );
		clear( my_bitmap );
		cout<<"Could not load "<<load_file<<endl;
		error = true;
	}
	own = true;
}

Bitmap::Bitmap( const char * load_file, int sx, int sy ):
error( false ){
	BITMAP * temp = load_bitmap( load_file, NULL );
	// my_bitmap = load_bitmap( load_file, NULL );
	my_bitmap = create_bitmap( sx, sy );
	clear( my_bitmap );
	if ( !temp ){
		cout<<"Could not load "<<load_file<<endl;
		error = true;
	} else {
		stretch_blit( temp, my_bitmap, 0, 0, temp->w, temp->h, 0, 0, my_bitmap->w, my_bitmap->h );
		destroy_bitmap( temp );
	}
	own = true;
}

Bitmap::Bitmap( const char * load_file, int sx, int sy, double accuracy ):
error( false ){
	BITMAP * temp = load_bitmap( load_file, NULL );
	if ( !temp ){
		cout<<"Could not load "<<load_file<<endl;
		my_bitmap = create_bitmap( sx, sy );
		clear( my_bitmap );
		error = true;
	} else {
		if ( temp->w > sx || temp->h > sy ){
			double bx = temp->w / sx;
			double by = temp->h / sy;
			double use;
			use = bx > by ? bx : by;
			int fx = (int)(sx / use);
			int fy = (int)(sy / use);
			my_bitmap = create_bitmap( fx, fy );
			
			stretch_blit( temp, my_bitmap, 0, 0, temp->w, temp->h, 0, 0, my_bitmap->w, my_bitmap->h );
			
			destroy_bitmap( temp );
		} else my_bitmap = temp;
	}
	own = true;
}

Bitmap::Bitmap( const Bitmap & copy, int sx, int sy ):
error( false ){
	BITMAP * temp = copy.my_bitmap;
	my_bitmap = create_bitmap( sx, sy );
	clear( my_bitmap );
	stretch_blit( temp, my_bitmap, 0, 0, temp->w, temp->h, 0, 0, my_bitmap->w, my_bitmap->h );
	own = true;
}

Bitmap::Bitmap( const Bitmap & copy, int sx, int sy, double accuracy ):
error( false ){
	BITMAP * temp = copy.my_bitmap;
	if ( temp->w > sx || temp->h > sy ){
		double bx = (double)temp->w / (double)sx;
		double by = (double)temp->h / (double)sy;
		double use;
		use = bx > by ? bx : by;
		int fx = (int)(temp->w / use);
		int fy = (int)(temp->h / use);
		my_bitmap = create_bitmap( fx, fy );
		own = true;
		if ( !my_bitmap ){
			allegro_message("Could not create bitmap\n");
			own = false;
			error = true;
			return;
		}
	
		stretch_blit( temp, my_bitmap, 0, 0, temp->w, temp->h, 0, 0, my_bitmap->w, my_bitmap->h );
		// destroy_bitmap( temp );
	} else {
		my_bitmap = create_bitmap( temp->w, temp->h );
		blit( temp, my_bitmap, 0, 0, 0, 0, temp->w, temp->h );
		own = true;
	}
}


Bitmap::Bitmap( const Bitmap & copy ):
error( false ){
	BITMAP * his = copy.getBitmap();
	my_bitmap = create_bitmap( his->w, his->h );
	::blit( his, my_bitmap, 0, 0, 0, 0, his->w, his->h );
	own = true;
}

Bitmap::Bitmap( const Bitmap & copy, int x, int y, int width, int height ):
error( false ){
	BITMAP * his = copy.getBitmap();
	my_bitmap = create_sub_bitmap( his, x, y, width, height );
	if ( !my_bitmap ){
		cout<<"Could not create sub-bitmap"<<endl;
		my_bitmap = create_bitmap( 10, 10 );
		clear( my_bitmap );
	}
	own = true;
}

Bitmap::~Bitmap(){
	if ( own )
		destroy_bitmap( my_bitmap );
}

BITMAP * Bitmap::getBitmap() const{
	return my_bitmap;
}

bool Bitmap::getError(){
	return error;
}

int Bitmap::getWidth(){
	return my_bitmap->w;
}

int Bitmap::getHeight(){
	return my_bitmap->h;
}

void Bitmap::circlefill( int x, int y, int radius, int color ){
	::circlefill( my_bitmap, x, y, radius, color );
}
	
void Bitmap::printf( int x, int y, int color, FONT * f, const char * str, ... ){

	char buf[512];
	va_list ap;

	va_start(ap, str);
	uvszprintf(buf, sizeof(buf), str, ap);
	va_end(ap);

	textout_ex( my_bitmap, f, buf, x, y, color, -1);
}

void Bitmap::rectangle( int x1, int y1, int x2, int y2, int color ){
	rect( my_bitmap, x1, y1, x2, y2, color );
}

void Bitmap::rectangleFill( int x1, int y1, int x2, int y2, int color ){
	rectfill( my_bitmap, x1, y1, x2, y2, color );
}

int Bitmap::getPixel( int x, int y ){
	if ( x >= 0 && x < my_bitmap->w && y >= 0 && y <= my_bitmap->h )
		return _getpixel16( my_bitmap, x, y );
	return -1;
}

int Bitmap::makeColor( int r, int g, int b ){
	return makecol16( r, g, b );
}

void Bitmap::fill( int color ){
	clear_to_color( my_bitmap, color );
}
	
void Bitmap::draw( int x, int y, Bitmap * where ){
	draw_sprite( where->my_bitmap, my_bitmap, x, y );
}

void Bitmap::drawTrans( int x, int y, Bitmap * where ){
	draw_trans_sprite( where->my_bitmap, my_bitmap, x, y );
}

void Bitmap::Blit( int x, int y, Bitmap * where ){
	::blit( my_bitmap, where->my_bitmap, 0, 0, x, y, my_bitmap->w, my_bitmap->h );
}
