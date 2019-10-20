#include "mouse.h" 
#include "bitmap.h"
#include "common.h"
#include <allegro.h>
#include <deque>
#include <iostream>

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

using namespace std;

Mouse::Mouse(){
	last_x = mouse_x;
	last_y = mouse_y;
	current_x = mouse_x;
	current_y = mouse_y;
	last_pic = NULL;

	/*
	vertical = new Bitmap( 4, 4 );
	vertical->fill( Bitmap::makeColor(255,0,0) );
	horizontal = new Bitmap( 4, 4 );
	horizontal->fill( Bitmap::makeColor(0,255,0) );
	*/
	vertical = new Bitmap( Global::dataDirectory( "mice/vert.bmp" ).c_str() );
	horizontal = new Bitmap( Global::dataDirectory( "mice/hori.bmp" ).c_str() );
	picture = vertical;
	dir = MOUSE_VERTICAL;

	set_mouse_sprite( picture->getBitmap() );

	event.clear();
	hold = false;
}

int Mouse::getDir(){
	return dir;
}

int Mouse::getX(){
	return mouse_x + mouse_sprite->w/2;
	// return current_x;
}

int Mouse::getY(){
	return mouse_y + mouse_sprite->h/2;
}

void Mouse::update(){
	if ( (mouse_b & 1) && !hold ){
		event.push_back( MOUSE_EVENT_LEFT_CLICK );
		hold = true;
	}
	if ( ( mouse_b & 2 ) && !hold ){
		event.push_back( MOUSE_EVENT_RIGHT_CLICK );
		if ( picture == vertical )
			picture = horizontal;
		else	picture = vertical;
		if ( dir == MOUSE_VERTICAL )
			dir = MOUSE_HORIZONTAL;
		else	dir = MOUSE_VERTICAL;
		set_mouse_sprite( picture->getBitmap() );
		hold = true;
	}
	if ( !mouse_b ) hold = false;

	current_x = mouse_x;
	current_y = mouse_y;
}

int Mouse::getEvent(){
	if ( event.empty() ) return 0;
	int e = event.front();
	event.pop_front();
	return e;
}

void Mouse::clearEvent(){
	event.clear();
}

bool Mouse::move(){
	current_x = mouse_x;
	current_y = mouse_y;
	return ( last_x != current_x && last_y != current_y );
}

void Mouse::copy( Bitmap * work ){
	debug
	if ( last_pic )
		last_pic->Blit( last_x, last_y, work );
}

void Mouse::draw( Bitmap * work ){
	debug
	copy( work );
	if ( last_pic )
		delete last_pic;
	
	last_x = current_x;
	last_y = current_y;
	last_pic = new Bitmap( *work, last_x, last_y, 4, 4 );
	picture->Blit( current_x, current_y, work );
}
	
void Mouse::getBack( Bitmap * work ){
	debug
	if ( last_pic )
		delete last_pic;
	last_x = current_x;
	last_y = current_y;
	last_pic = new Bitmap( *work, last_x, last_y, 4, 4 );
}

Mouse::~Mouse(){
	delete vertical;
	delete horizontal;
	if ( last_pic )
		delete last_pic;
}
