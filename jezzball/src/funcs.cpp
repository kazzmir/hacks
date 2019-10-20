#define ALLEGRO_NO_FIX_ALIASES
#include "funcs.h"
#include <math.h>
#include <allegro.h>
#include <stdlib.h>

double Tcos( int ang ){
	return cos( ang * AL_PI / 180.0 );
}

double Tsine( int ang ){
	return sin( ang * AL_PI / 180.0 );
}

int rnd( int q ){

	if ( q <= 0 ) return 0;
	return rand() % q;

}
