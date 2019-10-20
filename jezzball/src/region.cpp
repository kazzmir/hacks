#include "region.h"
#include "ball.h"

Region::Region(){
}


Region::Region( int _x1, int _y1, int _x2, int _y2 ):
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2){

	take_up = (x2-x1) * (y2-y1);

}

Region::Region( const Region & cr ){
	x1 = cr.x1;
	y1 = cr.y1;
	x2 = cr.x2;
	y2 = cr.y2;
	take_up = cr.take_up;
}

bool collide( Ball * ball ){

	return false;
}

/* this method makes or breaks jezzball 
 */
Region & Region::intersect( Ball * ball ){
	return *this;
}

int Region::space(){
	return take_up;
}

vector< Region * > * Region::getRegions(){
	return NULL;
}

Region::~Region(){
}

