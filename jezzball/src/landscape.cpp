#include "landscape.h"
#include "box.h"
#include "bitmap.h"
#include <string.h>

#include <iostream>

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

Landscape::Landscape( int _size, Bitmap * _picture ){

	picture = _picture;
	size = _size;
	// background = _background;

	int width = picture->getWidth() / size; 
	// int pwidth = (width + 1 ) * size;
	int height = picture->getHeight() / size;
	// int pheight = (height + 1 ) * size;

	max_x = width+2; 
	max_y = height+2;

	map = new Box**[ max_y ];
	for ( int q = 0; q < max_y; q++ ){
		map[q] = new Box*[ max_x ];
		for ( int x = 0; x < max_x; x++ )
			map[q][x] = 0;
	}

	// background = new Bitmap( max_x*size, max_y*size );

	// background->fill( Bitmap::makeColor( 32, 32, 32 ) );

	// plain = new Bitmap( size, size );
	// plain->fill( Bitmap::makeColor( 16, 16, 64 ) );

	/*
	addBox( new Box( 0, 0, pwidth, size, new Bitmap( *plain ) ) );
	addBox( new Box( 0, 0, size, pheight, new Bitmap( *plain ) ) );
	addBox( new Box( pwidth-size+2, 0, pwidth, pheight, new Bitmap( *plain ) ) );
	addBox( new Box( 0, pheight-size+2, pwidth, pheight, new Bitmap( *plain ) ) );
	*/
	Box * tmp_box = new Box( 1, 1, 2, 2 );
	for ( int y = 0; y < max_y; y++ ){
		map[y][0] = tmp_box;
		map[y][max_x-1] = tmp_box;
	}
	for ( int x = 0; x < max_x; x++ ){
		map[0][x] = tmp_box;
		map[max_y-1][x] = tmp_box;
	}
	boxes.push_back( tmp_box );

	rebuildMap();

	total_map = countFreeBoxes();
	filled_map = total_map;

}

int Landscape::percentFilled(){
	return (int)( (total_map - filled_map) * 100 / total_map );
}

int Landscape::countFreeBoxes(){
	int total = 0;
	for ( int y = 0; y < max_y; y++ )
		for ( int x = 0; x < max_x; x++ )
			total += map[y][x] == 0 ? 1 : 0;

	return total;
}

int Landscape::getWidth(){
	if ( max_x == 0 )cout<<"Major error at Landscape::getWidth"<<endl;
	return (max_x-1) * size;
}

int Landscape::getHeight(){
	if ( max_y == 0 )cout<<"Major error at Landscape::getHeight"<<endl;
	return (max_y-1) * size;
}

void Landscape::draw( Bitmap * work ){
	// background->blit( 0, 0, work );
	// picture->blit( size, size, background );
	// background->fill( Bitmap::makeColor( 64, 64, 64 ) );
	Bitmap * use = work;
	int rd = 32;
	for ( int y = 0; y < max_y; y++ )
		for ( int x = 0; x < max_x; x++ ){
			// int col = 128 + (x^y) + ((x>>3) & (y>>3)); 
			if ( map[y][x] == 0 ){
				use->rectangleFill( x*size, y*size, (x+1)*size, (y+1)*size, Bitmap::makeColor( 128+x%50-y%20, 128+y%50, 128+(x+y)%50 ) );
			/*
			int col2 = Bitmap::makeColor(255,255,255);
			if ( map[y][x] == 0 )
				background->rectangleFill( x*size, y*size, (x+1)*size, (y+1)*size, Bitmap::makeColor( 128+x%50-y%20, 128+y%50, 128+(x+y)%50 ) );
			else
				background->rectangleFill( x*size, y*size, (x+1)*size, (y+1)*size, col2 );
			*/
				rd = (rd + 23411) & 0xFFFFF;
				int color;
			/*
			if ( map[y][x] != 0 )
				color = Bitmap::makeColor(255,255,255);
			else	color = Bitmap::makeColor(128,128,128);
			*/
				color = Bitmap::makeColor(128,128,128);
				use->rectangle( x*size, y*size, (x+1)*size, (y+1)*size, color );
			} else {
				if ( y > 0 && y < max_y-1 && x > 0 && x < max_x - 1 ){
					int x1 = (x-1) * size;
					int y1 = (y-1) * size;
					Bitmap t( *picture, x1, y1, size, size );
					t.Blit( x*size, y*size, work );
				}
			}
		}
	// int blue = Bitmap::makeColor( 16, 16, 64 );
	/*
	background->rectangleFill( 0, 0, size-1, background->getHeight()-1, blue );
	background->rectangleFill( 0, 0, background->getWidth()-1, size-1, blue );
	background->rectangleFill( background->getWidth()-size, 0, background->getWidth()-1, background->getHeight()-1, blue );
	background->rectangleFill( 0, background->getHeight()-size, background->getWidth()-1, background->getHeight()-1, blue );
	*/

	/*
	for ( vector< Box * >::iterator it = boxes.begin(); it != boxes.end(); it++ ){
		Box * b = *it;
		b->draw( use );
	}
	*/
	// background->Blit( 0, 0, work );
}

void Landscape::getAllBoxes( vector< Box * > * list, int ** map_list, int x, int y ){

	list->push_back( new Box( x*size, y*size, (x+1)*size, (y+1) * size ) );
	map_list[y][x] = 1;
	if ( x > 0 )
		if ( map_list[y][x-1] == 0 )
			getAllBoxes( list, map_list, x-1, y );

	if ( x < max_x-1 )
		if ( map_list[y][x+1] == 0 )
			getAllBoxes( list, map_list, x+1, y );

	if ( y > 0 )
		if ( map_list[y-1][x] == 0 )
			getAllBoxes( list, map_list, x, y-1 );

	if ( y < max_y-1 )
		if ( map_list[y+1][x] == 0 )
			getAllBoxes( list, map_list, x, y+1 );

}

vector< Box * > * Landscape::getMapList( int ** map_list, int x, int y ){
	vector< Box * > * list = new vector< Box * >;

	getAllBoxes( list, map_list, x, y );
	return list;
}

void Landscape::rebuildBoxes( int ** imap, int x, int y ){

	int y_max, y_min;			  // upper and lower border
	int x_max, x_min;			  // left and right border 

	bool expand_left, expand_right, expand_up, expand_down;
	int j;

	// y and x are reversed for optimization: memory is contigious if y comes first so less page faults

	#define MAX_MAP 3

	x_max = x_min = x;
	y_max = y_min = y;

	// assume we can expand in all directions
	expand_left = expand_right = expand_up = expand_down = true;

	while ( expand_left || expand_right || expand_up || expand_down ) {

		if ( expand_left ) {

			if ( x_min-1 < 0 )
				expand_left = false;
			else {

				for ( j = y_min; j <= y_max; j++ ) {
					if ( imap[j][x_min-1] != 1 ) {
					// if ( map[j][min_x-1] == 0 ) {
					// if ( map[j][x_min-1] == 0 && map[j][x_min-1] < MAX_MAP ) {
						expand_left = false;
						break;
					}
				}		  // for

				if ( expand_left ) {

					x_min--;
					for ( j = y_min; j <= y_max; j++ )
						imap[j][x_min] = 2;
						// map[j][min_x]++;

				}		  // expand_left

			}			  // else

		}				  // expand_left

		if ( expand_right ) {

			if ( x_max+1 >= max_x )
				expand_right = false;
			else {

				for ( j = y_min; j <= y_max; j++ ) {
					if ( imap[j][x_max+1] != 1 ) {
					// if ( map[j][max_x+1] == 0 ) {
					// if ( map[j][x_max+1] == 0 && map[j][x_max+1] < MAX_MAP ) {
						expand_right = false;
						break;
					}
				}

				if ( expand_right ) {
					x_max++;
					for ( j = y_min; j <= y_max; j++ )
						imap[j][x_max] = 2;
						// map[j][max_x]++;
				}

			}

		}

		if ( expand_up ) {

			if ( y_min-1 < 0 )
				expand_up = false;
			else {

				for ( j = x_min; j <= x_max; j++ ) {
					if ( imap[y_min-1][j] != 1 ) {
					// if ( map[min_y-1][j] == 0 ) {
					// if ( map[y_min-1][j] == 0 && map[y_min-1][j] < MAX_MAP ) {
						expand_up = false;
						break;
					}
				}

				if ( expand_up ) {
					y_min--;
					for ( j = x_min; j <= x_max; j++ )
						imap[y_min][j] = 2;
						// map[min_y][j]++;
				}

			}

		}

		if ( expand_down ) {

			if ( y_max+1 >= max_y )
				expand_down = false;
			else {

				for ( j = x_min; j <= x_max; j++ ) {
					if ( imap[y_max+1][j] != 1 ) {
					// if ( map[max_y+1][j] == 0 ) {
					// if ( map[y_max+1][j] == 0 && map[y_max+1][j] < MAX_MAP ) {
						expand_down = false;
						break;
					}
				}

				if ( expand_down ) {
					y_max++;
					for ( j = x_min; j <= x_max; j++ )
						imap[y_max][j] = 2;
						// map[max_y][j]++;
				}

			}  // else

		} // expand_down

	} // while


	Box * junk = new Box( x_min * size, y_min * size, (x_max+1) * size, (y_max+1) * size );
	addBox( junk );

	/*
	for ( int ay = y_min; ay <= y_max; ay++ )
		for ( int ax = x_min; ax <= x_max; ax++ )
			map[ay][ax] = 3;
	*/

	/*
	EBox * junk = new EBox( min_x*size, min_y*size, (max_x)*size, (max_y)*size );
	add( junk );

	for ( int ay = min_y; ay <= max_y; ay++ )
		for ( int ax = min_x; ax <= max_x; ax++ ) 
			if ( map[ay][ax] == 2 ){
				// map[ay][ax] = 2;
				box_map[ay][ax] = junk;
			}
	*/

}

void Landscape::renegotiate( vector< Box * > * balls ){

	vector< vector< Box * > * > lists;

	int * temp_map[ max_y ];

	for ( int y = 0; y < max_y; y++ )
		temp_map[y] = new int[ max_x ];

	for ( int y = 0; y < max_y; y++ )
		for ( int x = 0; x < max_x; x++ )
			if ( map[y][x] == 0 )
				temp_map[y][x] = 0;
			else 	temp_map[y][x] = 1;

	for ( int y = 0; y < max_y; y++ )
		for ( int x = 0; x < max_x; x++ )
			if ( temp_map[y][x] == 0 )
				lists.push_back( getMapList( temp_map, x, y ) );

	for ( vector< vector< Box * > * >::iterator it = lists.begin(); it != lists.end(); it++ ){
		vector< Box * > * cur = *it;

		bool cy = true;

		for ( vector< Box * >::iterator ball = balls->begin(); ball != balls->end() && cy; ball++ ){
			Box * ba = *ball;

			for ( vector< Box * >::iterator cur_box = cur->begin(); cur_box != cur->end() && cy; cur_box++ ){

				Box * cur_map_box = *cur_box;
				if ( cur_map_box->collide( ba ) ){
					cy = false;
				}
			}

		}

		if ( cy ){
			for ( vector< Box * >::iterator bb = cur->begin(); bb != cur->end(); /**/ ){
				addBox( *bb );
				bb = cur->erase( bb );
			}
		}
	}

	filled_map = 0;
	for ( vector< vector< Box * > * >::iterator it = lists.begin(); it != lists.end(); it++ ){
		vector< Box * > * l = *it;
		for ( vector< Box * >::iterator aa = l->begin(); aa != l->end(); aa++ ){
			filled_map++;
			delete *aa;
		}

		delete l;
	}

	for ( int y = 0; y < max_y; y++ )
		delete[] temp_map[y];
	
	rebuildMap();

}

void Landscape::rebuildMap(){

	int * temp_map[ max_y ];

	for ( int y = 0; y < max_y; y++ )
		temp_map[y] = new int[ max_x ];

	for ( vector< Box * >::iterator it = boxes.begin(); it != boxes.end(); it++ )
		delete *it;

	boxes.clear();

	for ( int y = 0; y < max_y; y++ )
		for ( int x = 0; x < max_x; x++ ){
			if ( map[y][x] == 0 )
				temp_map[y][x] = 0;
			else 	temp_map[y][x] = 1;

			map[y][x] = 0;
		}


	for ( int y = 0; y < max_y; y++ )
		for ( int x = 0; x < max_x; x++ )
			if ( temp_map[y][x] == 1 )
				rebuildBoxes( temp_map, x, y );

	for ( int q = 0; q < max_y; q++ )
		delete[] temp_map[q];
}
	
Box * Landscape::getMap( int x, int y ){
	if ( x >= 0 && x < max_x && y >= 0 && y < max_y )
		return map[y][x];
	return 0;
}

bool Landscape::onMap( int x, int y ){
	return !( ( x/size >= max_x-1 ) || (y/size >= max_y-1) || ( x < 0 ) || ( y < 0 ) );
}

int Landscape::getMapX( int x ){
	return x / size;
}

int Landscape::getMapY( int y ){
	return y / size;
}

int Landscape::getCoordX( int x ){
	return (x / size) * size;
}

int Landscape::getCoordY( int y ){
	return (y / size) * size;
}

int Landscape::getSize(){
	return size;
}

void Landscape::addBox( Box * box ){

	/*
	int gx1, gy1, gx2, gy2;
	gx1 = getMapX( box->getX1() ) * size;
	gy1 = getMapY( box->getY1() ) * size;
	gx2 = getMapX( box->getX2() ) * size + size;
	gy2 = getMapY( box->getY2() ) * size + size; 
	box->setCoord( gx1, gy1, gx2, gy2 );
	*/

	for ( int y = box->getY1() / size; y < box->getY2() / size; y++ )
		for ( int x = box->getX1() / size; x < box->getX2() / size; x++ )
			if ( x >= 0 && x < max_x && y >= 0 && y < max_y ) {
				map[y][x] = box;

				/*
				if ( y > 1 && y < max_y-1 && x > 1 && x < max_x-1 )
					// box->setPic( new Bitmap( *picture, (x-1)*size, (y-1)*size, size, size ) );
					box->setPic( new Bitmap( *picture, box->getX1(), box->getY1(), box->getX2()-box->getX1(), box->getY2()-box->getY1() ) );
				*/
			}
	// if ( box->getX1() - size > size
	
	boxes.push_back( box );
}

Box * Landscape::collide( Box * box ){
	for ( vector< Box * >::iterator it = boxes.begin(); it != boxes.end(); it++ ){
		Box * mine = *it;
		if ( mine->collide( box ) )
			return mine;
	}
	return NULL;
}

Bitmap * Landscape::getBackground(){
	cout<<"ERROR: getBackground"<<endl;
	return background;
}

Landscape::~Landscape(){
	// delete picture;
	// delete background;
	// delete plain;

	for ( vector< Box * >::iterator it = boxes.begin(); it != boxes.end(); it++ )
		delete *it;

	for ( int q = 0; q < max_y; q++ )
		delete[] map[q];
	delete[] map;
}
