#ifndef _player_h
#define _player_h

#include <string>
using namespace std;

class Player{
public:
	Player( int _lives );

	void incScore( int _score );
	int getScore();
	void incLevel();
	int getLevel();
	void incLives( int _lives );
	int getLives();

	string & getName();
	void setName( string & what );

protected:
	string name;
	int level;
	int score;
	int lives;
};

#endif
