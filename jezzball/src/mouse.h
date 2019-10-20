#ifndef _mouse_h
#define _mouse_h

#define MOUSE_EVENT_LEFT_CLICK 1
#define MOUSE_EVENT_RIGHT_CLICK 2

#define MOUSE_VERTICAL 1
#define MOUSE_HORIZONTAL 2

#include <deque>

using namespace std;

class Bitmap;

class Mouse{
public:
	Mouse();

	bool move();
	void draw( Bitmap * work );
	void copy( Bitmap * work );
	void update();

	void getBack( Bitmap * work ); 
	int getDir();

	int getX();
	int getY();

	void clearEvent();
	int getEvent();

	~Mouse();

protected:
	int last_x, last_y;
	int current_x, current_y;
	Bitmap * last_pic;
	Bitmap * picture;
	Bitmap * vertical;
	Bitmap * horizontal;

	deque< int > event;
	bool hold;
	int dir;
};

#endif
