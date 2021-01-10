#include <iostream>
#include <allegro.h>
#include <stdlib.h>
#include <time.h>

const int max_y = 180;
const int what = ( 480 - max_y );
const int spot_width = 20;

#define MAX_SPOTS 20

unsigned char back[640][max_y];
unsigned char real[640][max_y];
unsigned int spot[MAX_SPOTS];

unsigned int grand_clock;

void inc_timer(){
	grand_clock++;
}

void init() {
        allegro_init();
        srandom( time( NULL ) );
        install_timer();
        install_keyboard();
        set_color_depth( 8 );
        set_gfx_mode( GFX_AUTODETECT_WINDOWED, 640, 480, 640, 480 );
        install_int_ex( inc_timer, MSEC_TO_TIMER( 8 ) );
}


void set_pal() {

        PALETTE pal;

        int t;

        for (t=0; t<64; t++) {

                pal[t].r = t;
                pal[t].g = 0;
                pal[t].b = 0;

        }
        for (t=0; t<64; t++) {

                pal[t+64].r = 63;
                pal[t+64].g = t;
                pal[t+64].b = 0;

        }
        for (t=0; t<64; t++) {

                pal[t+128].r = 63;
                pal[t+128].g = 63;
                pal[t+128].b = t;

        }
        /*
        for ( t = 0; t < 63; t++ )
        {

           pal[ t+192].r = 63;
           pal[ t+192].g = 63;
           pal[ t+128].b = 60;

        }
        */

        set_palette( pal );

        for (int q=0; q<MAX_SPOTS; q++) {

                spot[ q ] = random() % 640;

        }

}


void draw_back() {

    for ( int x = 0; x < 640; x++ )
        for ( int y = 0; y < max_y; y++ )
            real[x][y] = 0;

    for (int t =0; t<640; t++)
        back[t][max_y-1] = 0;

    for( int t=0; t<MAX_SPOTS; t++) {
        if (spot[t] > 0 && spot[t] < 640){
            back[spot[t]][max_y-1] = 192;
        }

        for (int w =-spot_width/2; w<spot_width/2; w++){
            int x_pos = spot[t] + w;
            if (x_pos > 0 && x_pos < 640){
                back[x_pos][max_y-1] = 192 - abs( w );
            }
        }

    }

    for (int t =0; t<MAX_SPOTS; t++){
        spot[t] = spot[t] + random() % 5 - 2;
        if (spot[t] < 0){
            spot[t] = 639;
        }
        if (spot[t] > 639){
            spot[t] = 0;
        }
    }

}


void show_it(BITMAP* work) {

        const int dec = (int) (192.0 / (float) max_y + 0.5);

        for (int y = 0; y<max_y-1; y++) {

                int ar = back[1][y+1] + back[0][y+1] - dec;
                if ( ar < 0 )
                        ar = 0;

                putpixel( work, 0, y, ar >> 1 );
                real[0][y] = ar >> 1;

                ar =back[640-1][y] + back[640-1][y+1] - dec;
                if ( ar < 0 )
                        ar = 0;
                putpixel( work, 639, y, ar >> 1 );
                real[639][y] = ar >> 1;

                for (int x=1; x<639; x++) {

                        ar = back[x-1][y]+back[x+1][y+1]+back[x-1][y+1]+back[x][y+1] -dec;
                        if ( ar < 0 ) ar = 0;
                        putpixel( work, x, y, ar >> 2 );
                        real[ x ][ y ] = ar >> 2;

                }

        }

        // for debugging
        // hline(work, 0, 0, 640, 191);

}


void copy_it() {

        for (int y =0; y<max_y-1; y++)
        for (int x=0; x<640; x++) {

                //back[x][y] = getpixel( work, x, y );
                back[x][y] = real[x][y];

        }

}


int main() {
    init();
    set_pal();
    BITMAP * work = create_bitmap(640, max_y);

    for (int i = 0; i < 40; i++){
        draw_back();
        show_it(work);
        copy_it();
    }

    while (!key[ KEY_ESC]) {
        while (grand_clock == 0){
            rest( 1 );
        }

        while ( grand_clock > 0 ){
            grand_clock--;
            draw_back();
        }

        show_it(work);
        copy_it();

        blit( work, screen, 0, 0, 0, what, 640, max_y-1 );
    }

    destroy_bitmap(work);
}
END_OF_MAIN();
