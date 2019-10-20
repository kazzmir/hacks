#include "allegro.h"
//#include "trigtable.h"
#include <stdlib.h>
#include <time.h>

#define screen_x 640
#define screen_y 480
#define max_block 5
#define max_shape 5
#define min_shape 3
#define min_x 80
#define min_y 30
#define max_key 9
#define max_x 10
#define max_y 20
#define block_x_size 15
#define block_y_size 20
#define max_time 200
#define max_level 15

BITMAP * work;
int map[max_x][max_y];
int grand_clock = 0;
int hit_change = 0;
int left_key = 0;
int right_key = 0;

int LEVEL = 14;
int total_count = 0;

void inc_timer(){
	grand_clock++;
}

struct column {

        int x, y;
        int blocks;
        int block[ max_block ];

};

void init() {
        allegro_init();
        srandom( time( NULL ) );
        install_timer();
        install_keyboard();
        set_color_depth( 8 );
        set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_x, screen_y, screen_x, screen_y );
        work = create_bitmap( 640, 480 );
        for ( int r = 0; r < max_x; r++ )
                for ( int q = 0; q < max_y; q++ )
                        map[r][q] = 0;

        RGB change;
        change.r = 0;
        change.b = 0;
        change.g = 0;
        set_color( 200, &change );
        for ( int x = 0; x < 15; x++ ) {

                change.r+=4;
                change.g++;
                set_color( 201+x, &change );

        }

	install_int_ex( inc_timer, MSEC_TO_TIMER( 40 ) );
}

void reset( column & me ) {

        me.x = random() % max_x;
        me.y = -1;
        me.blocks = 3;
        for ( int q = 0; q < me.blocks; q++ )
                me.block[q] = random() % max_shape+1;

}


#define down 0
#define right 1
#define dr 2
#define dl 3

int sq[max_x][max_y];
int total = 0;
void check( int x, int y, int col, int dir ) {

        if ( dir == -1 ) {
                total = 1;
                if ( x+1 < max_x )
                        if ( map[x+1][y] == col )
                                check( x+1, y, col, right );
                if ( total >= 3 )
                        sq[x][y] = 1;
                total = 1;
                if ( y+1 < max_y )
                        if ( map[x][y+1] == col )
                                check( x, y+1, col, down );
                if ( total >= 3 )
                        sq[x][y] = 1;
                total = 1;
                if ( x-1 >= 0 && y+1 < max_y )
                        if ( map[x-1][y+1] == col )
                                check( x-1, y+1, col, dl );
                if ( total >= 3 )
                        sq[x][y] = 1;
                total = 1;
                if ( x+1 < max_x && y+1 < max_y )
                        if ( map[x+1][y+1] == col )
                                check( x+1, y+1, col, dr );
                if ( total >= 3 )
                        sq[x][y] = 1;

        }
        else {

                switch( dir ) {
                        case right  : {

                                total++;
                                if ( x+1 < max_x )
                                        if ( map[x+1][y] == col )
                                                check( x+1, y, col, right );
                                if ( total >= 3 )
                                        sq[x][y] = 1;

                                break;
                        }
                        case down   : {
                                total++;
                                if ( y+1 < max_y )
                                        if ( map[x][y+1] == col )
                                                check( x, y+1, col, down );
                                if ( total >= 3 )
                                        sq[x][y] = 1;
                                break;
                        }
                        case dl     : {

                                total++;
                                if ( x-1 >= 0 && y+1 < max_y )
                                        if ( map[x-1][y+1] == col )
                                                check( x-1, y+1, col, dl );
                                if ( total >= 3 )
                                        sq[x][y] = 1;

                                break;
                        }

                        case dr     : {

                                total++;
                                if ( x+1 < max_x && y+1 < max_y )
                                        if ( map[x+1][y+1] == col )
                                                check( x+1, y+1, col, dr );
                                if ( total >= 3 )
                                        sq[x][y] = 1;
                                break;
                        }

                }                                 //switch

        }                                         //else if

}                                                 //function


void draw_em() {

        textprintf( work, font, 400, 10, 31, "LEVEL:%d", max_level-LEVEL );

        rect( work, min_x-2, min_y-2,
                max_x*block_x_size+min_x+2,
                max_y*block_y_size+min_y+2,
                31 );
        rect( work, min_x-1, min_y-1,
                max_x*block_x_size+min_x+1,
                max_y*block_y_size+min_y+1,
                29 );

        for ( int x = 0; x < max_x; x++ )
                for ( int y = 0; y < max_y; y++ )
                        if ( map[x][y] != 0 )
                                rectfill( work, x*block_x_size+min_x, y*block_y_size+min_y,
                                        x*block_x_size+block_x_size+min_x,
                                        y*block_y_size+block_y_size+min_y, map[x][y]*5+32 );

}


void destroy_blocks() {

        bool cy = false;
        for ( int x = 0; x < max_x; x++ )
                for ( int y = 0; y < max_y; y++ )
                        if ( sq[x][y] == 1 )
                                cy = true;
        if ( !cy )
                return;

        for ( int x = 0; x < 15; x++ ) {

                draw_em();

                for ( int qx = 0; qx < max_x; qx++ )
                        for ( int y =0; y < max_y; y++ )
                                if ( sq[qx][y] == 1 )
                                        rectfill( work, qx*block_x_size+min_x, y*block_y_size+min_y,
                                                qx*block_x_size+block_x_size+min_x,
                                                y*block_y_size+block_y_size+min_y, 200+x );

                blit( work, screen, 0, 0, 0, 0, 640, 480 );
                clear( work );

        }

}


void check_squares() {

        for ( int x=0;x<max_x;x++)
                for (int y=0;y<max_y;y++)
                        sq[x][y] = 0;

        int total = 0;
        for ( int x = 0; x< max_x; x++ )
                for ( int y = 0; y < max_y; y++ )
                if ( map[x][y] != 0 ) {
                        total = 0;
                check( x, y, map[x][y], -1 );
        }

        destroy_blocks();

        int count = 0;
        for ( int x = 0; x < max_x; x++ )
                for ( int y = 0; y < max_y; y++ )
                if ( sq[x][y] == 1 ) {
                        map[x][y] = 0;
                count++;
        }
        total_count += count;
        if ( total_count > (max_level-LEVEL)*50 )
                LEVEL--;

        for ( int x = 0; x < max_x; x++ )
        for ( int y = max_y-1; y >=0; y-- ) {

                int z = y;
                while ( z < max_y-1 && map[x][z+1] == 0 ) {

                        int temp = map[x][z];
                        map[x][z] = 0;
                        z++;
                        map[x][z] = temp;

                }

        }

        bool cy = false;
        for ( int j = 0; j < max_x; j++ )
                for ( int k = 0; k < max_y; k++ )
                        if ( sq[j][k] == 1 )
                                cy = true;
        if ( cy )
                check_squares();

}


void move_block( column & me ) {

        if ( map[me.x][me.y+1] == 0 && me.y+1 < max_y )
                me.y++;
        else {

                for ( int z = 0; z < me.blocks; z++ )
                if ( me.y-z < 0 ) {
                        textprintf( screen, font, 100, 100, 31, "You lose");
                        LEVEL = 1;
                        while ( !key[KEY_ESC] );
                }

                if ( me.y != -1 )
                        for ( int q = 0; q < me.blocks; q++ )
                                if ( me.y-q >= 0 )
                                        map[me.x][me.y-q]=me.block[q];
                check_squares();
                reset( me );
        }

}


void draw_map( column me ) {

        for ( int y = 0; y < me.blocks; y++ ) {

                if ( me.y-y >= 0 )
                        rectfill( work, me.x*block_x_size+min_x, (me.y-y)*block_y_size+min_y,
                                me.x*block_x_size+block_x_size+min_x,
                                (me.y-y)*block_y_size+block_y_size+min_y,
                                me.block[y]*5+32 );

        }

        draw_em();

}


void change_block( column & me ) {

        int temp = me.block[0];
        for ( int q = 0; q < me.blocks-1; q++ )
                me.block[q]=me.block[q+1];
        me.block[me.blocks-1]=temp;

}


void move( column & me, int much ) {

        if ( me.x + much < 0 || me.x + much >= max_x )
                return;
        for ( int q = 0; q < me.blocks; q++ )
                if ( me.y-q>=0 )
                        if ( map[me.x+much][me.y-q] != 0 )
                                return;
        me.x+=much;

}


int main() {

        init();
        column act;
        reset( act );
	int time = 0;
        while ( !key[KEY_ESC] && LEVEL > 1 ) {

		bool draw = false;
                // grand_clock++;
		while ( grand_clock > 0 ){
			draw = true;
			grand_clock--;
			time++;
			if ( key[KEY_5_PAD] && hit_change == 0) {
				change_block( act );
				hit_change = 12;
			}
			if ( key[KEY_4_PAD] && left_key == 0 ) {
				move( act, -1 );
				left_key = max_key;
			}
			if ( key[KEY_6_PAD] && right_key == 0 ) {
				move( act, +1 );
				right_key = max_key;
			}
			if ( left_key > 0 )
				left_key--;
			if ( right_key > 0 )
				right_key--;
			if ( hit_change > 0 )
				hit_change--;

			if ( time % (LEVEL+10) == 0 || ( time % 3 == 0 && key[KEY_2_PAD] ) ){
				move_block( act );
			}

		}

		if ( draw ){
			draw_map(act);
			blit( work, screen, 0, 0, 0, 0, 640, 480 );
			clear( work );
		}

		while ( grand_clock == 0 ){
			rest( 1 );
		}
        }
        destroy_bitmap( work );

}


END_OF_MAIN();
