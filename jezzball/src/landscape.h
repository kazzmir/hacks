#ifndef _landscape_h
#define _landscape_h

#include <vector>

using namespace std;

class Box;
class Bitmap;

class Landscape{
public:

	Landscape( int size, Bitmap * _picture );

	void addBox( Box * box );
	Box * collide( Box * box );
	Bitmap * getBackground();

	int getWidth();
	int getHeight();

	Box * getMap( int x, int y );
	int getCoordX( int x );
	int getCoordY( int y );
	bool onMap( int x, int y );

	int getMapX( int x );
	int getMapY( int y );
			
	void renegotiate( vector< Box * > * balls );

	int percentFilled();

	int getSize();

	void draw( Bitmap * work );

	~Landscape();

protected:

	vector< Box * > * getMapList( int ** map_list, int x, int y );
	void getAllBoxes( vector< Box * > * list, int ** map_list, int x, int y );
	int countFreeBoxes();
	void rebuildBoxes( int ** map, int x, int y );
	void rebuildMap();

	vector< Box * > boxes;
	Bitmap * background;
	Bitmap * picture;
	Bitmap * plain;
	int size;
	Box *** map;
	int max_x, max_y;
	int total_map, filled_map;

};

#endif
