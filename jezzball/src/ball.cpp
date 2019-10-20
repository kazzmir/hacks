#include "ball.h"
#include "funcs.h"
#include "bitmap.h"

#include <iostream>
using namespace std;

Ball::Ball( const int _x, const int _y, const int _ang, const double _speed, int _size ):
// Box( _x, _y, _x+size, _y+size ),
Box( -1, -1, -1, -1 ),
size( _size ),
last_x( _x ),
last_y( _y ),
actualx( _x ),
actualy( _y ),
virtualx( _x ),
virtualy( _y ),
ang( _ang ),
speed( _speed ){ 

	Bitmap tmp( size, size );
	tmp.fill( Bitmap::MaskColor );

	tmp.circlefill( size/2, size/2, size/3, makecol(255,255,255) );

	// pic = new Bitmap( tmp );
	setPic( new Bitmap( tmp ) );
	last_pic = NULL;

	// collide = new ECollide( tmp.bitmap(),

}

Ball::~Ball(){

	if ( last_pic ) delete last_pic;

}

int Ball::getX1() const{
	return actualx;
}

int Ball::getY1() const{
	return actualy;
}

int Ball::getX2() const{
	return actualx + size;
}

int Ball::getY2() const{
	return actualy + size;
}

void Ball::incSpeed( float inc ){
	speed += inc;
}

/*
bool Ball::collide( Box * other ){

	
}
*/

void Ball::collision( Box * other ){

	int zx1 = this->getX1();
	int zy1 = this->getY1();
	int zx2 = this->getX2();
	int zy2 = this->getY2();

	int ax1 = other->getX1();
	int ay1 = other->getY1();
	int ax2 = other->getX2();
	int ay2 = other->getY2();

	int dir = 0;
	#define BALL_LEFT 1
	#define BALL_RIGHT 2
	#define BALL_UP 3
	#define BALL_DOWN 4
	// int eps = size / 5 < 2 ? size / 5 : 2;

	int min_eps = size;
	dir = 0;
	// while ( dir == 0 ){
		int t = zx2 - ax1;
		if ( t < min_eps && t >= 0 ){
			min_eps = t;
			dir = BALL_RIGHT;
		}
		t = ay2 - zy1;
		if ( t < min_eps && t >= 0 ){
			min_eps = t;
			dir = BALL_UP;
		}
		t = ax2 - zx1;
		if ( t < min_eps && t >= 0 ){
			min_eps = t;
			dir = BALL_LEFT;
		}
		t = zy2 - ay1;
		if ( t < min_eps && t >= 0 ){
			min_eps = t;
			dir = BALL_DOWN;
		}

		/*
		if ( !dir && zx2 - ax1 <= eps && zx2 - ax1 >= 0 )
			dir = BALL_RIGHT;
		if ( !dir && ay2 - zy1 <= eps && ay2 - zy1 >= 0 )
			dir = BALL_UP;
		if ( !dir && ax2 - zx1 <= eps && ax2 - zx1 >= 0 )
			dir = BALL_LEFT;
		if ( !dir && zy2 - ay1 <= eps && zy2 - ay1 >= 0 )
			dir = BALL_DOWN;

		eps += 2;
		*/
	// }
	switch( dir ){
		case BALL_LEFT : {
			if ( ang < 180 ) ang = 180 - ang;
			else ang = 540 - ang;
			break;
		}
		case BALL_RIGHT : {
			if ( ang < 90 ) ang = 180 - ang;
			else ang = 360 - ang + 180;
			/* 
			if ( ang < 90 ) ang = 360 - ang;
			else ang -= 90;
			*/
			break;
		}
		case BALL_DOWN : {
			ang = 360 - ang;
			/*
			if ( ang < 270 ) ang = 360 - ang;
			else ang -= 270;
			*/
			break;
		}
		case BALL_UP : {
			ang = 360 - ang;
			/*
			if ( ang < 90 ) ang = 360 - ang;
			else ang 
			*/
			break;
		}
		default : {
			cout<<"WHAT THE HECK"<<endl;
			break;
		}
	}

	/*
	actualx = last_x;
	actualy = last_y;
	virtualx = actualx;
	virtualy = actualy;
	*/
	while ( this->collide( other ) ){
		virtualx += Tcos( ang ) * speed;
		virtualy += Tsine( ang ) * speed;
		actualx = (int)virtualx;
		actualy = (int)virtualy;
	}
	// ang = (ang + 90) % 360;
	// ang = (180 - ang + 360) % 360;
}

void Ball::move( ){
	last_x = actualx;
	last_y = actualy;
	virtualx += Tcos( ang ) * speed;
	virtualy += Tsine( ang ) * speed;
	actualx = (int)virtualx;
	actualy = (int)virtualy;
}

void Ball::draw( Bitmap * work ){
	/*
	if ( last_pic ){
		last_pic->draw( last_x, last_y, background );
		delete last_pic;
	}
	last_pic = new Bitmap( *work, last_x, last_y, size, size );
	*/
	pic->draw( getX1(), getY1(), work );
}

