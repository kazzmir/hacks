#ifndef _game_h
#define _game_h

#include <vector>

using namespace std;

class Box;
class Landscape;
class Bitmap;
class Mouse;
struct SAMPLE;

#define MOVE_LEFT 1
#define MOVE_RIGHT 2
#define MOVE_UP 3
#define MOVE_DOWN 4

class Mover{
public:
	Mover( int _x1, int _y1, int _x2, int _y2, int _dir, int _color );

	void Draw( Bitmap * work );
	void move();

	int getXFront();
	int getYFront();

	Box * getBox();
	void loseBox();

	~Mover();

protected:

	int x1, y1, x2, y2;
	int dir;
	int color;
	Box * my_box;
	
};

class Game{
public:

	Game( int level, const char * background );
	Game( int level, Bitmap background );

	bool runGame( Mouse * mouse, int off_x, int off_y );
	void Draw( Bitmap * work );

	int percentFilled();

	bool error();

	~Game();

protected:

	void handleMover( Mover ** who );
	void init( int level );
	void makeBalls( int level );
	void playBink();

	vector< Box * > balls;
	Landscape * land;
	Bitmap * back;

	Mover * left_up, * right_down;

	bool Error;

	SAMPLE * bink;
};

#endif
