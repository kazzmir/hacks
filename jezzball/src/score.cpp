#include <allegro.h>
#include <stdio.h>
#include "score.h"

#ifndef debug
#include <iostream>
using namespace std;
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

Highscore::Highscore(){

	filename = "score.jzb";

	if ( !exists( filename ) )
		createList();
	else	readScores();

	sortScores();

}

int Highscore::maxScores(){
	return 10;
}

char * Highscore::getName( int i ){
	return scores[ maxScores() - i - 1 ].name;
}

int Highscore::getScore( int i ){
	return scores[ maxScores() - i - 1 ].score;
}

bool Highscore::eligible( int w ){	
	for ( int q = 0; q < maxScores(); q++ )
		if ( w >= scores[q].score )
			return true;
	return false;
}

void Highscore::addScore( const char * name, int high ){

	struct score_list * set;
	set = &scores[0];

	for ( int q = 1; q < maxScores(); q++ )
		if ( scores[q].score < set->score )
			set = &scores[q];

	sprintf( set->name, "%s", name );
	set->score = high;

	sortScores();
	sync();

}

void Highscore::sync(){
	FILE * score_file;
	score_file = fopen( filename, "wb" );

	if ( ! score_file ){
		return;
	}

	for ( int q = 0; q < maxScores(); q++ ){
		if ( feof( score_file ) ) break;

		fwrite( scores[q].name, sizeof(char), 256, score_file );
		fwrite( &scores[q].score, sizeof(int), 1, score_file );
	}

	fclose( score_file );
}

void Highscore::createList(){

	for ( int q = 0; q < maxScores(); q++ ){
		sprintf( scores[q].name, "Yo mama" );
		scores[q].score = q * q * 5000;
	}

	sortScores();
	sync();

}

int scoreCompare( const void * e1, const void * e2 ){
	
	struct score_list * s1, * s2;
	s1 = (score_list *)e1;
	s2 = (score_list *)e2;
	if ( s1->score < s2->score )
		return -1;
	if ( s1->score > s2->score )
		return 1;
	return 0;
}

void Highscore::sortScores(){
	qsort( scores, maxScores(), sizeof( score_list ), scoreCompare );
}

void Highscore::readScores(){
	FILE * score_file;
	score_file = fopen( filename, "rb" );

	if ( ! score_file ){
		createList();
		return;
	}

	for ( int q = 0; q < maxScores(); q++ ){
		fread( scores[q].name, sizeof(char), 256, score_file );
		fread( &scores[q].score,  sizeof(int), 1, score_file );
	}
	
	fclose( score_file ); 
}

Highscore::~Highscore(){
	sortScores();
	sync();
}
