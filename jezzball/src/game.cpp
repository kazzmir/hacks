#include "game.h"
#include "box.h"
#include "bitmap.h"
#include "landscape.h"
#include "mouse.h"
#include "ball.h"
#include "common.h"
#include "funcs.h"

#include <iostream>

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

#define SIZE 12

Mover::Mover( int _x1, int _y1, int _x2, int _y2, int _dir, int _color ):
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2 ),
dir( _dir ),
color( _color ){
	Bitmap * b = new Bitmap( x2-x1, y2-y1 );
	b->fill( color );
	my_box = new Box( x1, y1, x2, y2, b );
}

void Mover::Draw( Bitmap * work ){
	my_box->draw( work );
}

int Mover::getXFront(){
	switch( dir ){
		case MOVE_LEFT : return x1;
		case MOVE_RIGHT : return x2;
		case MOVE_UP : return (x1+x2)/2;
		case MOVE_DOWN : return (x1+x2)/2;
	}
	return x1;
}

int Mover::getYFront(){

	switch( dir ){
		case MOVE_LEFT : return (y1+y2)/2;
		case MOVE_RIGHT : return (y1+y2)/2;
		case MOVE_UP : return y1;
		case MOVE_DOWN : return y2;
	}
	return y1;
}

void Mover::move(){
	// bool cy = false;
	switch( dir ){
		case MOVE_LEFT : {
			x1--;
			/*
			if ( land->getMap( x1, y1 ) == 1 )
				cy = true;
			*/
			break;
		}
		case MOVE_RIGHT : {
			x2++;
			/*
			if ( land->getMap( x2, y2 ) == 1 )
				cy = true;
			*/
			break;
		}
		case MOVE_UP : {
			y1--;
			/*
			if ( land->getMap( x1, y1 ) == 1 )
				cy = true;
			*/
			break;
		}
		case MOVE_DOWN : {
			y2++;
			/*
			if ( land->getMap( x2, y2 ) == 1 )
				cy = true;
			*/
			break;
		}
	}
	delete my_box;
	Bitmap * b = new Bitmap( x2 - x1, y2 - y1 );
	b->fill( color );
	my_box = new Box( x1, y1, x2, y2, b );

	// return land->collide( my_box );
}

Box * Mover::getBox(){
	return my_box;
}

void Mover::loseBox(){
	cout<<"Lose"<<endl;
	my_box = NULL;
}

Mover::~Mover(){
	if ( my_box )
		delete my_box;
}

Game::Game( int level, const char * background ){

	Error = false;

	debug
	back = new Bitmap( background, 600, 400, 1.0 );
	debug
	Error = back->getError();
	init( level );

}

Game::Game( int level, Bitmap background ){

	Error = false;

	back = new Bitmap( background, 600, 400, 1.0 );
	Error = back->getError();
	init( level );
}

bool Game::error(){
	return Error;
}

void Game::init( int level ){

	#ifdef DEBUG
	init_boxes();
	#endif

	bink = load_sample( Global::dataDirectory( "/bink.wav" ).c_str() );

	land = new Landscape( SIZE, back );
	balls.clear();
	makeBalls( level );

	left_up = NULL;
	right_down = NULL;

}

void Game::playBink(){
	if ( bink ){
		play_sample( bink, 255, 128, 1000, 0 );
	}
}

void Game::makeBalls( int level ){

	for ( int q = 0; q < level; q++ ){

		bool cy = true;
		do{
			cy = true;
			int xx = rnd( land->getWidth() - SIZE * 4 ) + SIZE * 2;
			int yy = rnd( land->getHeight() - SIZE * 4 ) + SIZE * 2;
			Ball * b = new Ball( xx, yy, rnd( 4 ) * 90 + 45, 0.9, SIZE );
			for ( vector< Box * >::iterator it = balls.begin(); it != balls.end() && cy; it++ )
				if ( (*it)->collide( b ) ){
					cy = false;
				}


			if ( cy ){
				balls.push_back( b );
			}
			else {
				delete b;
			}

		} while ( !cy );
	}
}

void Game::handleMover( Mover ** who ){

	if ( *who ){
		Mover * use = *who;

		use->move();

		// if ( land->collide( use->getBox() ) != NULL ){
		int x = land->getMapX( use->getXFront() );
		int y = land->getMapY( use->getYFront() );
		// cout<<"X: "<<x<<" Y: "<<y<<endl;
		// circlefill( screen, use->getXFront(), use->getYFront(), 3, makecol( 0, 255, 0 ) );
		// vsync();
		if ( land->getMap(x,y) != 0 ){ 
			land->addBox( new Box( *use->getBox() ) );
			land->renegotiate( &balls );
			// use->loseBox();
			delete use;
			*who = NULL;
		}
	}

}
	
int Game::percentFilled(){
	return land->percentFilled();
}

bool Game::runGame( Mouse * mouse, int off_x, int off_y ){

	handleMover( &left_up );
	handleMover( &right_down );

	bool cy = false;

	for ( vector< Box * >::iterator it = balls.begin(); it != balls.end(); it++ ){

		Box * b = *it;
		((Ball *)b)->move();

		/* Dont need a collision for this */
		if ( left_up )
			if ( b->collide( left_up->getBox() ) ){
				delete left_up;
				left_up = NULL;
				cy = true;
			}

		if ( right_down )
			if ( b->collide( right_down->getBox() ) ){
				delete right_down;
				right_down = NULL;
				cy = true;
			}

		Box * collide = land->collide( b );
		if ( collide != NULL ){
			((Ball *)b)->collision( collide );
			playBink();
		}
	}

	for ( vector< Box * >::iterator it = balls.begin(); it != balls.end(); it++ ){
		Box * first = *it;
		for ( vector< Box * >::iterator zz = balls.begin(); zz != balls.end(); zz++ ){
			Box * second = *zz;
			if ( first != second ){
				if ( first->collide( second ) ){
					Box tmp_first( *first );
					Box tmp_second( *second );
					((Ball *)first)->collision( &tmp_second );
					((Ball *)second)->collision( &tmp_first );

					playBink();
				}
			}
		}
	}

	int e = mouse->getEvent();
	if ( e != 0 ){
		int red = Bitmap::makeColor( 200, 0, 0 );
		int blue = Bitmap::makeColor( 0, 0, 200 );
			
		int x1 = mouse->getX() - off_x;
		int y1 = mouse->getY() - off_y;
		if ( e == MOUSE_EVENT_LEFT_CLICK && land->onMap(x1,y1)) {

			if ( left_up == NULL ){
				switch( mouse->getDir() ){
					// up
					case MOUSE_VERTICAL : {
						/*
						int x1 = mouse->getX() - off_x;
						int y1 = mouse->getY() - off_y;
						if ( !land->onMap(x1,y1) ) break;
						*/
						int mx1 = land->getCoordX( x1 )+1;
						int my1 = land->getCoordY( y1 )-1;
						int mx2 = mx1 + land->getSize()-1;
						int my2 = my1 + 1;


						left_up = new Mover( mx1, my1, mx2, my2, MOVE_UP, red  );
						if ( right_down )
							if ( right_down->getBox()->collide( left_up->getBox() ) ){
								delete left_up;
								left_up = NULL;
							}

						break;
					}
					// left
					case MOUSE_HORIZONTAL : {
						/*
						int x1 = mouse->getX();
						int y1 = mouse->getY();
						if ( !land->onMap(x1,y1) ) break;
						*/
						int mx1 = land->getCoordX( x1 )-1;
						int my1 = land->getCoordY( y1 )+1;
						int mx2 = mx1+1;
						int my2 = my1 + land->getSize()-1;
						left_up = new Mover( mx1, my1, mx2, my2, MOVE_LEFT, red );
						if ( right_down )
							if ( right_down->getBox()->collide( left_up->getBox() ) ){
								delete left_up;
								left_up = NULL;
							}


						break;
					}
				}
			}

			if ( right_down == NULL ){
			
				switch( mouse->getDir() ){
					// down
					case MOUSE_VERTICAL : {
				
						/*
						int x1 = mouse->getX();
						int y1 = mouse->getY();
						if ( !land->onMap(x1,y1) ) break;
						*/
						int mx1 = land->getCoordX( x1 )+1;
						int my1 = land->getCoordY( y1 )+1;
						int mx2 = mx1 + land->getSize()-1;
						int my2 = my1 + 2; 


						right_down = new Mover( mx1, my1, mx2, my2, MOVE_DOWN, blue  );
						
						if ( left_up )
							if ( left_up->getBox()->collide( right_down->getBox() ) ){
								delete right_down;
								right_down = NULL;
							}


						break;
					}
					// right
					case MOUSE_HORIZONTAL : {
				
						/*
						int x1 = mouse->getX();
						int y1 = mouse->getY();
						if ( !land->onMap(x1,y1) ) break;
						*/
						int mx1 = land->getCoordX( x1 )+1;
						int my1 = land->getCoordY( y1 )+1;
						int mx2 = mx1+1;
						int my2 = my1 + land->getSize()-1;
						right_down = new Mover( mx1, my1, mx2, my2, MOVE_RIGHT, blue );
						
						if ( left_up )
							if ( left_up->getBox()->collide( right_down->getBox() ) ){
								delete right_down;
								right_down = NULL;
							}


						break;
					}
				}

			}
		} // if left_click
	} // if e != 0


	return cy;

}

void Game::Draw( Bitmap * work ){
	land->draw( work );
	for ( vector< Box * >::iterator it = balls.begin(); it != balls.end(); it++ )
		(*it)->draw( work );

	vector< Box * > temp;
	if ( left_up ) temp.push_back( left_up->getBox() );
	if ( right_down ) temp.push_back( right_down->getBox() );

	for ( vector< Box * >::iterator it = temp.begin(); it != temp.end(); it++ )
		(*it)->draw( work );
}

Game::~Game(){
	delete land;
	// Must delete back AFTER land
	delete back;

	if ( bink != NULL ){
		destroy_sample( bink );
	}

	for ( vector< Box * >::iterator it = balls.begin(); it != balls.end(); it++ )
		delete *it;

	if ( left_up ) delete left_up;
	if ( right_down ) delete right_down;

	#ifdef DEBUG
	print_boxes();
	#endif
}
