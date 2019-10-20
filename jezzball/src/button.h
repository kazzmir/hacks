#ifndef _button_h
#define _button_h

#include <string>
#include <allegro.h>
using namespace std; 

class Work;
class Mouse;
class Bitmap;

class Button{
public:

	Button( const char * _name, int _x, int _y, FONT * font ); 
	void Update( Bitmap * work );
	bool Selected( int mx, int my );

	virtual ~Button();

protected:

	bool select;
	bool update;
	Bitmap * on, * off;

	string name;

	// drawing coords
	int x, y;

	// bounding box
	int x1, y1, x2, y2;

};

#endif
