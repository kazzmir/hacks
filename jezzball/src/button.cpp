#include "button.h"
#include "bitmap.h"
#include "mouse.h"
#include <allegro.h>

Button::Button( const char * _name, int _x, int _y, FONT * f ):
select( false ),
update( true ),
x( _x ),
y( _y ){

	name = name;
	int w, h;
	w = text_length( f, _name ) + 10;
	h = text_height( f ) + 5;

	on = new Bitmap( w, h );
	off = new Bitmap( w, h );

	on->fill( Bitmap::MaskColor );
	off->fill( Bitmap::MaskColor );

	on->printf( 5, 2, Bitmap::makeColor( 255, 255, 0 ), f, "%s", _name );
	off->printf( 5, 2, Bitmap::makeColor( 128, 0, 64 ), f, "%s", _name );

	x1 = x;
	y1 = y;
	x2 = x + w;
	y2 = y + h;
	
}

void Button::Update( Bitmap * work ){

	if ( !update ) return;
	update = false;

	if ( select )
		on->draw( x, y, work );
	else 	off->draw( x, y, work );
}

bool Button::Selected( int mx, int my ){

	bool cy = mx >= x1 && mx <= x2 && my >= y1 && my <= y2;
	if ( cy != select )
		update = true;
	select = cy;

	return select; 
}

Button::~Button(){

	delete on;
	delete off;

}

