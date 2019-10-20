#ifndef _ball_h
#define _ball_h

#include "box.h"

class Bitmap;

class Ball: public Box{
public:

	Ball( const int _x, const int _y, const int _ang, const double _speed, int _size );

	virtual int getX1() const;
	virtual int getY1() const;
	virtual int getX2() const;
	virtual int getY2() const;
	virtual void draw( Bitmap * work );
	void incSpeed( float inc );

	void move();
	// virtual bool collide( Box * other );
	void collision( Box * other );

	virtual ~Ball();

protected:
	int size;
	int last_x, last_y;
	int actualx, actualy;
	double virtualx, virtualy;
	int ang;
	double speed;
	Bitmap * last_pic;

};

#endif
