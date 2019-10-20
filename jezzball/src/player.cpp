#include "player.h"
#include <string>

using namespace std;

Player::Player( int _lives ):
level( 1 ),
score( 0 ),
lives( _lives ){
	// name = _name;
	name = "";
}

void Player::incScore( int _score ){
	score += _score;
}

int Player::getScore(){
	return score;
}

void Player::incLevel(){
	level++;
}

int Player::getLevel(){
	return level;
}

void Player::incLives( int _lives ){
	lives += _lives;
	if ( lives < 0 ) lives = 0;
}

int Player::getLives(){
	return lives;
}

string & Player::getName(){
	return name;
}

void Player::setName( string & who ){
	name = who;
}

