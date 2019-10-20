#ifndef _box_h_
#define _box_h_

class Bitmap;

class Box{
public:

	Box( int _x1, int _y1, int _x2, int _y2, Bitmap * _pic = 0 );
	Box( const Box & copy );

	virtual bool collide( Box * other );
	virtual void setPic( Bitmap * _pic );
	virtual Bitmap * getPic();
	virtual void setCoord( int _x1, int _y1, int _x2, int _y2 );

	virtual void draw( Bitmap * work );
	virtual int getX1() const;
	virtual int getY1() const;
	virtual int getX2() const;
	virtual int getY2() const;

	virtual void print();

	virtual ~Box();
	
protected:

	int x1, y1, x2, y2;
	Bitmap * pic;

	int my_box_index;

};

#ifdef DEBUG
#define MAX_BOXES 1000000
extern Box * box_array[ MAX_BOXES ];
void init_boxes();
void print_boxes();
#endif

#endif
