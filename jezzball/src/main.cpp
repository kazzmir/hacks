#include <allegro.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "loadpng/loadpng.h"
#include <jpgalleg.h>
#include "game.h"
#include "mouse.h"
#include "bitmap.h"
#include "common.h"
#include "button.h"
#include "player.h"
#include "funcs.h"
#include "score.h"
#include "fonts.h"

#include <iostream>

using namespace std;

FONT * game_font = font;

#define GAME_SPEED 40

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

char * insults[] = {	"You can do better",
			"Hit the any key",
			"Did you leave the stove on?",
			"Where is the cow?",
			"Lock in",
			"Call it a day",
			"Miss!",
			"You are good. NOT!",
			"Good one, Einstein",
			"The chief cause of problems is solutions.",
			"Super sweet!",
			"Made by Jon Rafkind",
			"Divine retribution, that's what!",
			"How rude.",
			"I've been good all day so far.",
			"I like to say 'quark'!",
			"Tuesdays don't start much worse than this.",
			"I'm happy, but it's not like I'm ecstatic.",
			"I'd explain it, but there's a lot of math.",
			"He's so lazy and selfish.",
			"Nothing I do is my fault.",
			"I like to verb words.",
			"Once again, your stupidity has killed us!",
			"Quiet, fignuts!",
			"Oh yeah, real original. Sit on it, Potsie!",
			"Are you the dying, kid?",
			"Consider yourself zinged!",
			"Save it for Queen Doppelpoppolus!",
			"What you Scrooges need is some eggnog!",
			"...can't fix it laying down, baby.",
			"Nay, the true meaning of Alvis day is drinking.",
			"You have a gift.",
			"Well, as long as the baby doesn't touch my stash.",
			"We've got bigger problems than a butter shortage.",
			"I Am Sofa King We Todd Ed.",
			"One convenient location... in Africa.",
			"You are the Insult Master!",
			"You and your third dimension.",
			"No, the real point is: I don't give a damn.",
			"Reality continues to ruin my life.",
			"Where do we keep all our chainsaws, Mom?",
			"end" };
			
static std::string dataDirectory( "data/" );
std::string Global::dataDirectory( const char * str ){
	return ::dataDirectory + str;
}

volatile int speed_counter = 0;

void inc_speed_counter() {
	speed_counter++;
}
END_OF_FUNCTION( inc_speed_counter )

volatile int time_left = 0;
void dec_time_left(){
	time_left--;
}
END_OF_FUNCTION( dec_time_left )

void blend_palette( int * pal, int mp, int sc, int ec ) {

	ASSERT( pal );
	ASSERT( mp != 0 );

	int sc_r = getr( sc );
	int sc_g = getg( sc );
	int sc_b = getb( sc );

	int ec_r = getr( ec );
	int ec_g = getg( ec );
	int ec_b = getb( ec );

	for ( int q = 0; q < mp; q++ ) {
		float j = (float)( q ) / (float)( mp );
		int f_r = (int)( 0.5 + (float)( sc_r ) + (float)( ec_r-sc_r ) * j );
		int f_g = (int)( 0.5 + (float)( sc_g ) + (float)( ec_g-sc_g ) * j );
		int f_b = (int)( 0.5 + (float)( sc_b ) + (float)( ec_b-sc_b ) * j );
		pal[q] = makecol( f_r, f_g, f_b );
	}

}

void init(){
	allegro_init();
	// jpgalleg_init(); 
	loadpng_init();
	// agup_init( abeos_theme );
	install_timer();
	install_keyboard();
	install_mouse();
	install_sound( DIGI_AUTODETECT, MIDI_NONE, NULL );
	set_color_depth( 16 );
	set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0 );

	// game counter
	LOCK_VARIABLE( speed_counter );
	LOCK_FUNCTION( (void *)inc_speed_counter );
	install_int_ex( inc_speed_counter, MSEC_TO_TIMER( GAME_SPEED ) );

	// 1 second counter
	LOCK_VARIABLE( time_left );
	LOCK_FUNCTION( (void *)dec_time_left );
	install_int_ex( dec_time_left, BPS_TO_TIMER( 1 ) );

	srand( time( NULL ) );

}

void drawExtra( Player * player, Bitmap & work, int filled ){

	char name_lives[ 512 ];
	// snprintf( name_lives, 512, "Name: %s Lives: %d", player->getName().c_str(), player->getLives() );
	snprintf( name_lives, 512, "Lives: %d", player->getLives() );
	
	char score_str[ 128 ];
	snprintf( score_str, 128, "Score: %d", player->getScore() );
	
	char buf[ 512 ];
	snprintf( buf, 512, "Percent Filled %d%% ", filled );
	
	char time_str[ 128 ];
	if ( time_left < 0 ) time_left = 0;
	snprintf( time_str, 128, "Level: %d Time: %d  ", player->getLevel(), time_left );

	int score_y = 479 - text_height(game_font);
	work.printf( 1, 1, Bitmap::makeColor(200,200,200), game_font, "%s", name_lives );
	work.printf( 1, score_y, Bitmap::makeColor(255,255,255), game_font, "%s", score_str );

	work.printf( 1, 1, Bitmap::makeColor(200,200,200), game_font, "%s", name_lives );

	int score_x = 350;
	work.printf( score_x, 1, Bitmap::makeColor(200,200,200), game_font, "%s", buf );

	int fx = 50 + text_length( game_font, score_str );
	// work.rectangleFill( fx, score_y, fx+text_length(game_font,time_str), 479, Bitmap::makeColor(16,16,64) );
	work.printf( fx, score_y, Bitmap::makeColor(255,255,255), game_font, "%s", time_str );

}

/* getAllMusic:
 * Finds all files with a certain extension
 * songs: vector of file names
 * pattern: extension to search for
 */
void getAllMusic( vector< char * > * songs, const char * pattern ) {

	struct al_ffblk song_files;
	char tmp_pattern[ 512 ];
	strncpy( tmp_pattern, pattern, 512 );
	fix_filename_slashes( tmp_pattern );
	int success = al_findfirst( tmp_pattern, &song_files, FA_ARCH );

	while ( success == 0 ) {

		songs->push_back( strdup( song_files.name ) );
		success = al_findnext( &song_files );

	}

	al_findclose( &song_files );
}

char * getPicture(){
	vector< char * > songs;
	// getAllMusic( &songs, "./pics/*.jpg" );
	getAllMusic( &songs, Global::dataDirectory( "/pics/*.png" ).c_str()  );
	getAllMusic( &songs, Global::dataDirectory( "/pics/*.tga" ).c_str() );
	getAllMusic( &songs, Global::dataDirectory( "/pics/*.bmp" ).c_str() );
	getAllMusic( &songs, Global::dataDirectory( "/pics/*.pcx" ).c_str() );
	getAllMusic( &songs, Global::dataDirectory( "/pics/*.lbm" ).c_str() );

	if ( songs.empty() ) return "";

	static char current_pic[ 1024 ];
	char * cur = songs[ rnd( songs.size() ) ];
	snprintf( current_pic, 1024, Global::dataDirectory( "/pics/%s" ).c_str(), cur );
	fix_filename_slashes( current_pic );

	for ( vector< char * >::iterator it = songs.begin(); it != songs.end(); it++ ){
		free( *it );
	}

	return current_pic;
}

bool playGame( Player * player ){

	/*
	int pic = rnd( GIRLS_LAST );
	Game game( player->getLevel(), Bitmap( (BITMAP *)girls[ pic ].dat ) );
	*/

	char * pic = getPicture();
	if ( strcasecmp( pic, "" ) == 0 ) return true;

	Game * game = new Game( player->getLevel(), Bitmap( getPicture() ) );
	while ( game->error() ){
		delete game;
		game = new Game( player->getLevel(), Bitmap( getPicture() ) );
	}

	int num_insults = 0;
	char ** qqq = insults;
	while ( strcasecmp( *qqq++, "end" ) != 0 ) num_insults++;

	BITMAP * mouses = mouse_sprite;

	Mouse mouse;
	Bitmap work( 640, 480 );
	Bitmap Screen( screen );
	int offset_x = 30;
	int offset_y = 30;

	int show_insults = -1;
	int insult = 0;
	int insult_x = 0;
	int insult_y = 0;
	int insult_color = 0;

	// sub-bitmap is deleted before parent bitmap which
	// coincidentally works out well for us :)
	Bitmap sub_work( work, offset_x, offset_y, 600, 460 );
	bool quit = false;
	speed_counter = 0;
	while ( speed_counter < 15 );
	speed_counter = 0;
	time_left = player->getLevel() * 25 + 25; 
	while ( !quit ){
		if ( key[ KEY_ESC ] ) {
			set_mouse_sprite( mouses );
			delete game;
			return true;
		}
		// if ( key[ KEY_R ] ) quit = true;

		if ( show_insults == -1 && rnd( 40 ) == rnd( 40 ) ){
			show_insults = 60;
			insult = rnd( num_insults );
			// insult_x = rnd( 200 ) + 100;
			insult_x = rnd( 640 - text_length( game_font, insults[ insult ] ) - 50 ) + 25;
			insult_y = rnd( 300 ) + 100;
			insult_color = Bitmap::makeColor( rnd( 64 ) + 192, rnd( 64 ) + 192, rnd( 64 ) + 192 );
		}

		// quit = key[ KEY_ESC ];
		bool redraw_name = false;
		while ( speed_counter > 0 ){
			// if ( key[ KEY_R ] ) quit = true;
			/*
			if ( mouse.move() )
				mouse.draw( &Screen );
			*/
			mouse.update();
			for ( int q = 0; q < 5; q++ )
				if ( game->runGame( &mouse, offset_x, offset_y ) ){
					player->incLives( -1 );
					redraw_name = true;
				}
			speed_counter--;
		}
		quit = player->getLives() == 0; 
		if ( game->percentFilled() >= 80 ) quit = true;
		game->Draw( &sub_work );

		drawExtra( player, work, game->percentFilled() );

		if ( show_insults >= 0 ){

			// drawing_mode( DRAW_MODE_TRANS, NULL, 0, 0 );

			int blend = 0;
			if ( show_insults > 30 )
				blend = 64 + (60 - show_insults)*6;
			else	blend = 64 + show_insults*6;

			set_trans_blender( 0, 0, 0, blend );

			char * cur_insult = insults[ insult ];
			int length = text_length( game_font, cur_insult );
			int height = text_height( game_font );

			Bitmap temp( length, height );
			temp.fill( Bitmap::MaskColor ); 

			temp.printf( 0, 0, insult_color, game_font, cur_insult );
			temp.drawTrans( insult_x, insult_y, &work );

			// drawing_mode( DRAW_MODE_SOLID, NULL, 0, 0 );
			show_insults--;
		}

		show_mouse( work.getBitmap() );
		work.Blit( 0, 0, &Screen );
		work.fill( Bitmap::makeColor( 16, 16, 64 ) );
		// show_mouse( screen );
		show_mouse( Screen.getBitmap() );
		while (speed_counter == 0){
			// if ( key[ KEY_R ] ) quit = true;
			mouse.update(); 
			rest( 1 );
		}
	}
	set_mouse_sprite( mouses );
	show_mouse( NULL );

	int bonus = time_left * time_left;
	
	char end_message[ 512 ];

	if ( player->getLives() > 0 ){
		player->incScore( bonus );
		snprintf( end_message, 512, "Filled %d%% Time Left: %d Bonus: %d", game->percentFilled(), time_left, bonus );
	} else {
		snprintf( end_message, 512, "You lose!!!!!!" );
	}

	int max_colors = 256;
	int num_colors = 8;
	int colors[ max_colors ];
	/*
	colors[0] = Bitmap::makeColor( rnd( 128 ) + 128, rnd( 128 ) + 128, rnd( 128 ) + 128 );
	colors[64] = Bitmap::makeColor( rnd( 128 ) + 128, rnd( 128 ) + 128, rnd( 128 ) + 128 );
	colors[64*2] = Bitmap::makeColor( rnd( 128 ) + 128, rnd( 128 ) + 128, rnd( 128 ) + 128 );
	colors[64*3] = Bitmap::makeColor( rnd( 128 ) + 128, rnd( 128 ) + 128, rnd( 128 ) + 128 );
	colors[255] = colors[0];
	blend_palette( 
	*/

	int use_colors = max_colors / num_colors;

	colors[0] = Bitmap::makeColor( rnd( 128 ) + 128, rnd( 128 ) + 128, rnd( 128 ) + 128 );
	for ( int q = 0; q < num_colors-1; q++ ){

		int start = q * use_colors;
		int end = start + use_colors;
		colors[end] = Bitmap::makeColor( rnd( 128 ) + 128, rnd( 128 ) + 128, rnd( 128 ) + 128 );
		blend_palette( colors+start, use_colors, colors[start], colors[end] );
	}
	colors[ max_colors - 1 ] = colors[0];
	int s = (num_colors-1) * use_colors;
	int e = max_colors - 1;
	blend_palette( colors+s, use_colors-1, colors[s], colors[e] );

	SAMPLE * endSound;
	if ( player->getLives() > 0 ){
		endSound = load_sample( Global::dataDirectory( "win.wav" ).c_str() );
	} else {
		endSound = load_sample( Global::dataDirectory( "laugh.wav" ).c_str() );
	}

	play_sample( endSound, 255, 128, 1000, 1 );

	// Screen.printf( 100, 100, Bitmap::makeColor(255,255,255), game_font, "%s", end_message );
	speed_counter = 0;
	int effect = 0;
	int l_count = 0;
	while ( speed_counter < 60 ){
		vsync();
		Screen.printf( 100, 100, colors[ effect ], game_font, "%s", end_message );
		effect = (effect+2) % max_colors;
		while( speed_counter == l_count )
			rest( 1 );
		l_count = speed_counter;
	}

	stop_sample( endSound );
	destroy_sample( endSound );

	delete game;

	return false;

	// return key[ KEY_ESC ];
}

int do_menu(){

	show_mouse( NULL );
	Bitmap Screen( screen );
	Bitmap work( 640, 480 );
	work.fill( Bitmap::makeColor( 16, 16, 64 ) );

	Button new_game( "New Game", 100, 100, game_font );
	Button quit( "Quit", 100, 150, game_font );
	Highscore score;
	/*
	new_game.Draw( work, false );
	quit.Draw( work, false );
	*/

	// work.printf( 300, 100, Bitmap::makeColor(255,255,255), game_font, "%s: %d", score.getName(0), score.getScore(0) );
	work.printf( 300, 100 - text_height(game_font) - 10, Bitmap::makeColor(255,255,255), game_font, "Top 10" );
	for ( int q = 0; q < score.maxScores(); q++ )
		work.printf( 300, 100 + q * text_height( game_font ), Bitmap::makeColor(255,255,255), game_font, "%s: %d", score.getName(q), score.getScore(q) );

	work.Blit( 0, 0, &Screen );
	show_mouse( Screen.getBitmap() );

	bool stop = false;
	while ( !stop ){

		// show_mouse( NULL );
		new_game.Update( &Screen );
		quit.Update( &Screen );
		// show_mouse( Screen.getBitmap() );
		int mx = mouse_x;
		int my = mouse_y;
		if ( new_game.Selected( mx, my ) && (mouse_b&1) )
			return 1;
		if ( quit.Selected( mx, my ) && (mouse_b&1) )
			return 0;

		rest( 1 );
	}

	return 0;

}

string enterName( Player * player ){
	string so_far = "";
	Bitmap Screen( screen );
	vsync();
	Screen.fill( Bitmap::makeColor(16,16,64) );

	int enter_x = 50;

	Screen.printf( enter_x, 100, Bitmap::makeColor(255,255,255), game_font, "Score: %d", player->getScore() );
	Screen.printf( enter_x, 200, Bitmap::makeColor(255,255,255), game_font, "Enter your name: " );
	int length = 0;
	bool quit = false;
	clear_keybuf();
	while ( !quit ){

		int c = readkey();
		
		char duh[ 512 ];
		sprintf( duh, "Enter your name: %s", so_far.c_str() );

		if ( c >> 8 == KEY_ENTER || c >> 8 == KEY_ENTER_PAD )
			quit = true;
		else {
			// cout<<"C "<< (c>>8) << " backspace "<< KEY_BACKSPACE<<endl;
			if ( c >> 8 == KEY_BACKSPACE ){
				if ( length > 0 ) length--;
				so_far.resize( length );
			} else if ( length < 30 ){
				length++;
				so_far += c & 0xff;
			}
		}
		int x1 = enter_x;
		int y1 = 200;
		int x2 = x1 + text_length( game_font, duh );
		int y2 = y1 + text_height( game_font );
		vsync();
		Screen.rectangleFill( x1, y1, x2, y2, Bitmap::makeColor(16,16,64) );
		Screen.printf( enter_x, 200, Bitmap::makeColor(255,255,255), game_font, "Enter your name: %s", so_far.c_str() );
		
	}
	return so_far;
}

int main( int argc, char ** argv ){
	
	init();

	cout << "Jezzball by Jon Rafkind" << endl;
	cout << "jezzball [-d data_directory]" << endl;
	cout << "-d sets the data directory. default is ./data" << endl;

	for ( int i = 1; i < argc; i++ ){
		if ( strcasecmp( "-d", argv[ i ] ) == 0 ){
			i++;
			if ( i < argc ){
				dataDirectory = std::string( argv[ i ] ) + "/";
			}
		}
	}

	Bitmap Screen( screen );
	cout << "Loading data from " << dataDirectory <<endl;
	DATAFILE * fonts = load_datafile( Global::dataDirectory( "/fonts.dat" ).c_str() );

	if ( ! fonts ){
		game_font = font;
	} else {
		int use_font = PUMP_PCX;
		game_font = (FONT * )fonts[ use_font ].dat;
	}

	int menu_ok = 1;
	while ( menu_ok ){

		menu_ok = do_menu();

		switch( menu_ok ){

			case 1 : {
				Player player( 3 );
				bool quit = false;
				while ( !quit ){
					quit = playGame( &player );
					player.incLevel();
					if ( player.getLives() != 0 )
						player.incLives( 1 );
					quit |= player.getLives() <= 0;
				}
				Highscore score;
				if ( score.eligible( player.getScore() ) ){
					string name = enterName( &player );
					score.addScore( name.c_str(), player.getScore() );
				}
			} // case 1
		} // switch
	}
	
	if ( fonts ){
		unload_datafile( fonts );
	}
	// agup_shutdown();

	cout<<"Exiting normally"<<endl;

	return 0;

}
END_OF_MAIN()
