#define ALLEGRO_NO_FIX_ALIASES

#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

BITMAP * work;

#define screen_x 640
#define screen_y 480

const double TPI = 3.141592654;

double tcos[ 360 ];
double tsine[360 ];

void set_trig() {
        for ( int r = 0; r<360; r++ ) {
                tcos [ r ]  = cos( (double)(r) * TPI / 180.0 );
                tsine[ r ]  =-sin( (double)(r) * TPI / 180.0 );
        }
}


struct point {
        int x, y;
        double realx, realy;
        int ang;
};

class linex {
        public:
                linex() {
                        for ( int q = 0; q < 2; q++ ) {
                                bl[q].x = random() % 640;
                                bl[q].y = random() % 480;
                                bl[q].realx = bl[q].x;
                                bl[q].realy = bl[q].y;
                                bl[q].ang = random() % 360;
                        }
                }

                void update() {
                        for ( int q = 0; q < 2; q++ ) {
                                bl[q].ang = ( bl[q].ang + random() % 9 - 4 + 360 ) % 360;
                                bl[q].realx += tcos[bl[q].ang];
                                bl[q].realy += tsine[bl[q].ang];
                                bl[q].x = (int)bl[q].realx;
                                bl[q].y = (int)bl[q].realy;
                                if ( bl[q].x < 0 || bl[q].x > 640 || bl[q].y < 0 || bl[q].y > 480 )
                                        bl[q].ang = (bl[q].ang+180)%360;
                        }
                }

                void draw() {
                        line( work, bl[0].x, bl[0].y, bl[1].x, bl[1].y, 200 );
                }

                point bl[2];

};

void blend_palette( PALETTE &pal, int sc, RGB s, int ec, RGB e ) {
        for ( int i = sc; i <= ec; i++ ) {
                float j = (float)( i - sc ) / (float)( ec - sc );

                pal[ i ].r = (int)( 0.5 + (float)( s.r ) + (float)( e.r - s.r ) * j );
                pal[ i ].g = (int)( 0.5 + (float)( s.g ) + (float)( e.g - s.g ) * j );
                pal[ i ].b = (int)( 0.5 + (float)( s.b ) + (float)( e.b - s.b ) * j );

        }                                         // end for
}                                                 // end blend_palette


void set_colors() {

        RGB k1, k2;
        k1.r = 0;
        k1.g = 1;
        k1.b = 1;
        k2.r = 0;
        k2.g = 15;
        k2.b = 63;
        PALETTE suck;
        blend_palette( suck, 100, k1, 200, k2 );
        set_palette( suck );
}


void init() {
        set_trig();
        allegro_init();
        srand(time(NULL));
        install_timer();
        install_keyboard();
        set_color_depth( 8 );
        set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_x, screen_y, screen_x, screen_y );
        work = create_bitmap( screen_x, screen_y );
        clear_to_color( work, 100 );
        set_colors();
}


int main() {

        init();
        #define ML 8
        linex * ls[ ML ];
        int dir = 1;
        int col = 0;
        for ( int q = 0; q < ML; q++ )
                ls[q] = new linex();
        while ( !key[KEY_ESC] ) {

                for ( int q = 0; q < ML; q++ )
                        ls[q]->update();

                for ( int q = 0; q < ML; q++ )
                        ls[q]->draw();
                blit( work, screen, 0, 0, 0, 0, 640, 480 );
                for ( int x = 0; x < 640; x++ )
                for ( int y = 0; y < 480; y++ ) {
                        int z = getpixel( work, x, y );
                        if ( z > 100 )
                                putpixel( work, x, y, z-1 );
                }

                PALETTE gay;
                get_palette( gay );
                for ( int q = 101; q <= 200; q++ )
                        gay[q].r = col;
                set_palette( gay );
                col += dir;
                if ( col >= 63 || col <= 0 )
                        dir *= -1;

        }
        destroy_bitmap( work );

}
