#ifndef _region_h
#define _region_h

#include <vector>

using namespace std;

class Ball;

class Region{
public:

	Region();
	Region( int _x1, int _y1, int _x2, int _y2 );
	Region( const Region & cr );

	bool collide( Ball * ball );
	Region & intersect( Ball * ball );
	int space();

	vector< Region * > * getRegions();

	~Region();

protected:
	int x1, y1, x2, y2;
	int take_up;
	vector< Region * > attached;

};

#endif
