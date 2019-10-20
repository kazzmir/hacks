#include "box.h" 
#include "bitmap.h"

#include <iostream>

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

using namespace std;

#ifdef DEBUG
Box * box_array[ MAX_BOXES ];
int box_counter = 0;
#endif
	
Box::Box( int _x1, int _y1, int _x2, int _y2, Bitmap * _pic ):
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2 ),
pic( _pic ){

	#ifdef DEBUG
	my_box_index = box_counter;
	box_array[ box_counter++ ] = this;
	#endif

}

Box::Box( const Box & copy ){
	x1 = copy.getX1();
	y1 = copy.getY1();
	x2 = copy.getX2();
	y2 = copy.getY2();
	pic = NULL;

	#ifdef DEBUG
	my_box_index = box_counter;
	box_array[ box_counter++ ] = this;
	#endif
}

Bitmap * Box::getPic(){
	return pic;
}

void Box::setPic( Bitmap * _pic ){
	if ( pic ) delete pic;
	pic = _pic;
}
	
void Box::setCoord( int _x1, int _y1, int _x2, int _y2 ){
	x1 = _x1;
	y1 = _y1;
	x2 = _x2;
	y2 = _y2;
}

void Box::draw( Bitmap * work ){
	if ( pic )
		pic->draw( getX1(), getY1(), work );
}

int Box::getX1() const{
	return x1;
}

int Box::getY1() const{
	return y1;
}

int Box::getX2() const{
	return x2;
}

int Box::getY2() const{
	return y2;
}

bool Box::collide( Box * other ){

	int zx1 = getX1();
	int zy1 = getY1();
	int zx2 = getX2();
	int zy2 = getY2();

	int zx3 = other->getX1();
	int zy3 = other->getY1();
	int zx4 = other->getX2();
	int zy4 = other->getY2();

	/*
	cout<<"Me X1: "<<zx1<<" Y1:"<< zy1<<" X2:"<<zx2<<" Y2:"<<zy2<<endl;
	cout<<"Him X1: "<<zx3<<" Y1:"<< zy3<<" X2:"<<zx4<<" Y2:"<<zy4<<endl;
	*/

	if ( zx1 < zx3 && zx1 < zx4 &&
		zx2 < zx3 && zx2 < zx4 ) return false;
	if ( zx1 > zx3 && zx1 > zx4 &&
		zx2 > zx3 && zx2 > zx4 ) return false;
	if ( zy1 < zy3 && zy1 < zy4 &&
		zy2 < zy3 && zy2 < zy4 ) return false;
	if ( zy1 > zy3 && zy1 > zy4 &&
		zy2 > zy3 && zy2 > zy4 ) return false;

	return true;

}

void Box::print(){
	cout<<"What.."<<endl;
	return;
	cout<<"X1: "<<this->getX1()<<" Y1: "<<this->getY1()<<" X2: "<<this->getX2()<<" Y2: "<<this->getY2()<<endl;

}


Box::~Box(){

	if ( pic != NULL ) delete pic;

	#ifdef DEBUG
	if ( box_array[ my_box_index ] != this )
		cout<<"Major problems"<<endl;
	box_array[ my_box_index ] = NULL;
	#endif
}

#ifdef DEBUG
void init_boxes(){
	for ( int q = 0; q < MAX_BOXES; q++ )
		box_array[q] = 0;
}

void print_boxes(){
	cout<<"Used "<<box_counter<<" boxes"<<endl;
	for ( int q = 0; q < MAX_BOXES; q++ )
		if ( box_array[q] != 0 )
			cout<<"Box not deleted at: "<<q<<endl;
}
#endif
