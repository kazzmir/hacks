#define ALLEGRO_NO_FIX_ALIASES

#include "allegro.h"
#include <time.h>
#include <stdlib.h>
#include "trigtable.h"

#define FOCUS 0.75
#define MAX_OBJ 2
#define screen_x 640
#define screen_y 480
#define MAX_X 1500
#define MAX_Y 1500
#define MAX_STAR 1000
#define MAX_BLOCK 70
#define GRAVITY 0.095
#define BOXGRAVITY 0.082
#define expl_color 240
#define MAX_EXPL 100

class AllegroCamera {
        public:
                AllegroCamera() {
                        map = create_bitmap( 640, 240 );
                }

                ~AllegroCamera() {
                        destroy_bitmap( map );
                }

                void Draw( BITMAP * work, int screen ) {

                        blit( map, work, 0, 0, 0, screen*240, 640, 240 );
                        clear( map );

                }

                void setCenter( const int cx, const int cy ) {
                        x = cx - 320;
                        y = cy - 120;
                }                                 // end setCenter

                const int getScreenX( const int sx ) {
                        return (int)((double)(sx - x)*FOCUS);
                }                                 // end getScreenX

                const int getScreenY( const int sy ) {
                        return (int)((double)(sy - y)*FOCUS);
                }                                 // end getScreenY

                // data
                BITMAP * map;
                int x, y;
};

class ExplosionClass {
        public:
                ExplosionClass( int cx, int cy, double walkx, double walky, int rad ):
                x( cx ),
                        y( cy ),
                        vx( cx ),
                        vy( cy ),
                        wx( walkx ),
                        wy( walky ),
                        radius( rad ),
                        max_rad( rad )
                        {}

                bool update() {
                        vx += wx;
                        vy += wy;
                        x = (int)vx;
                        y = (int)vy;

                        radius--;
                        if ( radius <= 0 ) return true;
                        return false;
                }

                void Draw( AllegroCamera * ac ) {

                        int color = radius;
                        color = (int)( ( (double)(max_rad - color ) / (double)max_rad ) * 100.0 );
                        color =(int)((double)color / ( 100.0 / 63.0 ));
                        if ( color > 63 )
                                color = 63;
                        if ( color < 0 )
                                color = 0;
                        circlefill( ac->map, ac->getScreenX(x), ac->getScreenY(y), radius, expl_color-color );
                }

                int x, y;
                double vx, vy;
                double wx, wy;
                int radius;
                int max_rad;

};

class StarClass {
        public:
                StarClass() {
                        x = random() % MAX_X;
                        y = random() % MAX_Y;
                        brightness = random() % 9;
                }

                void Draw( AllegroCamera * ac ) {
                        putpixel( ac->map,
                                ac->getScreenX( x ),
                                ac->getScreenY( y ),
                                31-8+brightness );
                }

                int x, y, brightness;

};

class SuperObject {
        public:
                SuperObject( const int c, const int x, const int y ):
                color( c ),
                        actualx( x ),
                        actualy( y ),
                        virtualx( x ),
                        virtualy( y ),
                        dx( 0 ),
                dy( 0 ) {
                }                                 // end constructor

                bool intersect( int x1, int y1, int x2, int y2 ){ return false; }

                void Draw( AllegroCamera * ac ){}

                bool isVisible( AllegroCamera * ac ){ return false; }

                double virtualx, virtualy;
                double dx, dy;
                int actualx, actualy;
                int color;

};                                                // end class SuperObject

class Block: public SuperObject {
        public:

                Block( const int c, const int x, const int y ):
                SuperObject( c, x, y ),
                        lastToucher( -1 ),
                ang( 45 ) {
                }                                 // end Block

                void Draw( AllegroCamera *ac ) {
                        int points[ 8 ];

                        for ( int q = 0; q < 4; q++ ) {
                                points[q*2] = ac->getScreenX( (int)(actualx+tcos[(ang+q*90)%360]*14.6 ) );
                                points[q*2+1] = ac->getScreenY( (int)(actualy+tsine[(ang+q*90)%360]*14.6 ) );
                        }

                        polygon( ac->map, 4, points, 32+color*8 );

                }                                 // end Draw

                bool cornerTouch( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 ) {
                        if ( x1 <= x4 &&
                                y1 <= y4 &&
                                x2 >= x3 &&
                                y2 >= y3 ) return true;
                        return false;
                }

                void testContact( Block * other ) {
                        int x1 = actualx - 10;
                        int y1 = actualy - 10;
                        int x2 = actualx + 10;
                        int y2 = actualy + 10;

                        int x3 = other->actualx - 10;
                        int y3 = other->actualy - 10;
                        int x4 = other->actualx + 10;
                        int y4 = other->actualy + 10;

                        if ( cornerTouch( x1, y1, x2, y2, x3, y3, x4, y4 ) ) {

                                double cx = (virtualx + other->virtualx) / 2;
                                double cy = (virtualy + other->virtualy) / 2;
                                double sdx = dx;
                                double sdy = dy;

                                if ( sqrt(dx*dx+dy*dy) > sqrt(other->dx*other->dx+other->dy*other->dy) )
                                        other->color = color;
                                else
                                        color = other->color;

                                dx = other->dx;
                                dy = other->dy;
                                other->dx = sdx;
                                other->dy = sdy;

                                while ( cornerTouch( x1,y1,x2,y2,x3,y3,x4,y4 ) ) {

                                        virtualx += dx;
                                        virtualy += dy;
                                        actualx = (int)( virtualx + 0.5 );
                                        actualy = (int)( virtualy + 0.5 );
                                        other->virtualx += other->dx;
                                        other->virtualy += other->dy;
                                        other->actualx = (int)( other->virtualx + 0.5 );
                                        other->actualy = (int)( other->virtualy + 0.5 );

                                        x1 = actualx - 10;
                                        y1 = actualy - 10;
                                        x2 = actualx + 10;
                                        y2 = actualy + 10;

                                        x3 = other->actualx - 10;
                                        y3 = other->actualy - 10;
                                        x4 = other->actualx + 10;
                                        y4 = other->actualy + 10;
                                }

                                /*
                                dx = virtualx - cx;
                                dy = virtualy - cy;
                                other->dx = other->virtualx - cx;
                                other->dy = other->virtualy - cy;

                                virtualx += dx;
                                virtualy += dy;

                                other->dx += other->dx;
                                other->dy += other->dy;*/
                        }                         // end if

                }                                 // end testContact

                void update( Block **others ) {
                        for ( int i = 0; i < MAX_BLOCK; i++ )
                                if ( others[ i ] != this )
                                        testContact( others[ i ] );

                        if ( virtualx > MAX_X )
                                virtualx -= MAX_X;
                        if ( virtualx < 0 )
                                virtualx += MAX_X;
                        if ( virtualy > MAX_Y )
                                virtualy -= MAX_Y;
                        if ( virtualy < 0 )
                                virtualy += MAX_Y;

                        ang = (ang+(int)sqrt(dx*dx+dy*dy)*2+360 ) % 360;
                        virtualx += dx;
                        virtualy += dy;

                        if ( dx > BOXGRAVITY )
                                dx -= BOXGRAVITY;
                        else if ( dx < -BOXGRAVITY )
                                dx += BOXGRAVITY;
                        else
                                dx = 0;

                        if ( dy > BOXGRAVITY )
                                dy -= BOXGRAVITY;
                        else if ( dy < -BOXGRAVITY )
                                dy += BOXGRAVITY;
                        else
                                dy = 0;

                        actualx = (int)( virtualx + 0.5 );
                        actualy = (int)( virtualy + 0.5 );
                }                                 // end update

                int lastToucher;
                int countDown;
                int ang;

};                                                // end Block

class Ship: public SuperObject {
        public:
                Ship( int h, int sx, int sy ):
                SuperObject( h, sx, sy ),
                        ang( 0 ),
                want_ang( 1 ) {
                        ang = random() % 360;
                        want_ang = random() % 360;
                        rad = 10;
                }

                bool intersect( int x1, int y1, int x2, int y2 ){ return false; }

                void Draw( AllegroCamera * ac ) {

                        int x1 = actualx + (int)( tcos[ ang ] * rad + 0.5 );
                        int y1 = actualy + (int)( tsine[ ang ] * rad + 0.5 );
                        int x2 = actualx + (int)( tcos[ (ang+150)%360 ] * rad + 0.5 );
                        int y2 = actualy + (int)( tsine[(ang+150)%360 ] * rad + 0.5 );
                        int x3 = actualx + (int)( tcos[ (ang+210)%360 ] * rad + 0.5 );
                        int y3 = actualy + (int)( tsine[(ang+210)%360 ] * rad + 0.5 );

                        triangle( ac->map,
                                ac->getScreenX( x1 ),
                                ac->getScreenY( y1 ),
                                ac->getScreenX( x2 ),
                                ac->getScreenY( y2 ),
                                ac->getScreenX( x3 ),
                                ac->getScreenY( y3 ),
                                32+color*8 );

                        //if ( actualx == sx && actualy == sy )
                        //textprintf( ac->map, font, 1, 1, 31, "X:%d Y:%d", actualx, actualy );
                }

                bool isVisible( AllegroCamera * ac ){ return false; }
                int Allegiance(){ return color; }

                bool touch( Block * bl ) {

                        int x1 = actualx + (int)( tcos[ ang ] * rad + 0.5 );
                        int y1 = actualy + (int)( tsine[ ang ] * rad + 0.5 );
                        int x2 = actualx + (int)( tcos[ (ang+150)%360 ] * rad + 0.5 );
                        int y2 = actualy + (int)( tsine[(ang+150)%360 ] * rad + 0.5 );
                        int x3 = actualx + (int)( tcos[ (ang+210)%360 ] * rad + 0.5 );
                        int y3 = actualy + (int)( tsine[(ang+210)%360 ] * rad + 0.5 );

                        int ax1 = bl->actualx-10;
                        int ay1 = bl->actualy-10;
                        int ax2 = bl->actualx+10;
                        int ay2 = bl->actualy-10;
                        int ax3 = bl->actualx-10;
                        int ay3 = bl->actualy+10;
                        int ax4 = bl->actualx+10;
                        int ay4 = bl->actualy+10;

                        if ( dist( x1, y1, ax1, ay1 ) > (rad+10)*2 ) return false;

                        if (    ptriangle( ax1, ay1, ax2, ay2, ax3, ay3, x1, y1 ) ||
                                ptriangle( ax1, ay1, ax2, ay2, ax3, ay3, x2, y2 ) ||
                                ptriangle( ax1, ay1, ax2, ay2, ax3, ay3, x3, y3 ) ||

                                ptriangle( ax2, ay2, ax3, ay3, ax4, ay4, x1, y1 ) ||
                                ptriangle( ax2, ay2, ax3, ay3, ax4, ay4, x2, y2 ) ||
                                ptriangle( ax2, ay2, ax3, ay3, ax4, ay4, x3, y3 ) ||

                                ptriangle( x1, y1, x2, y2, x3, y3, ax1, ay1 ) ||
                                ptriangle( x1, y1, x2, y2, x3, y3, ax2, ay2 ) ||
                                ptriangle( x1, y1, x2, y2, x3, y3, ax3, ay3 ) ||
                                ptriangle( x1, y1, x2, y2, x3, y3, ax4, ay4 ) )
                                return true;
                        return false;

                        if (    lineIntersect( x1, y1, x2, y2, ax1, ay1, ax2, ay2 ) ||
                                lineIntersect( x1, y1, x2, y2, ax1, ay1, ax3, ay3 ) ||
                                lineIntersect( x1, y1, x2, y2, ax2, ay2, ax4, ay4 ) ||
                                lineIntersect( x1, y1, x2, y2, ax3, ay3, ax4, ay4 ) ||

                                lineIntersect( x2, y2, x3, y3, ax1, ay1, ax2, ay2 ) ||
                                lineIntersect( x2, y2, x3, y3, ax1, ay1, ax3, ay3 ) ||
                                lineIntersect( x2, y2, x3, y3, ax2, ay2, ax4, ay4 ) ||
                                lineIntersect( x2, y2, x3, y3, ax3, ay3, ax4, ay4 ) ||

                                lineIntersect( x3, y3, x1, y1, ax1, ay1, ax2, ay2 ) ||
                                lineIntersect( x3, y3, x1, y1, ax1, ay1, ax3, ay3 ) ||
                                lineIntersect( x3, y3, x1, y1, ax2, ay2, ax4, ay4 ) ||
                                lineIntersect( x3, y3, x1, y1, ax3, ay3, ax4, ay4 ) )
                                return true;
                        return false;

                }

                void interact( Block ** blist ) {
                        for ( int q = 0; q < MAX_BLOCK; q++ )
                        if ( touch( blist[q] ) ) {
                                double sdx = dx;
                                double sdy = dy;
                                dx = blist[q]->dx*0.8;
                                dy = blist[q]->dy*0.8;
                                blist[q]->dx = sdx*1.2;
                                blist[q]->dy = sdy*1.2;
                                while( touch(blist[q]) ) {
                                        blist[q]->virtualx += blist[q]->dx;
                                        blist[q]->virtualy += blist[q]->dy;
                                        blist[q]->actualx = (int)(blist[q]->virtualx + 0.5 );
                                        blist[q]->actualy = (int)(blist[q]->virtualy + 0.5 );
                                        virtualx += dx;
                                        virtualy += dy;
                                        actualx = (int)( virtualx + 0.5 );
                                        actualy = (int)( virtualy + 0.5 );
                                }
                        }
                }

                void ControlMove(ExplosionClass ** expl_array ) {

                #define fox 4.1
                        if ( key[KEY_UP] ) {
                                dx += tcos[ang]/fox;
                                dy += tsine[ang]/fox;

                                int q = 0;
                                while ( q < MAX_EXPL && expl_array[q] != NULL )
                                        q++;
                                if ( q < MAX_EXPL )
                                        expl_array[q] = new ExplosionClass( actualx+(int)(tcos[(ang+180)%360]*8),
                                                actualy+(int)(tsine[(ang+180)%360]*8), 0, 0, 6 );
                        }
                        if ( key[KEY_DOWN] ) {
                                dx -= tcos[ang]/fox;
                                dy -= tsine[ang]/fox;
                        }
                        if ( key[KEY_RIGHT] )
                                ang = (ang-3+360)%360;
                        if ( key[KEY_LEFT] )
                                ang = (ang+3+360)%360;
                }

                void CompMove() {
                        if ( ang == want_ang )
                                want_ang = random() % 360;
                        if ( ang < want_ang )
                                ang++;
                        if ( ang > want_ang )
                                ang--;
                        dx = tcos[ang];
                        dy = tsine[ang];

                }

                void moveD() {

                        virtualx += dx;
                        virtualy += dy;

                        if ( dx > GRAVITY )
                                dx -= GRAVITY;
                        else if ( dx < -GRAVITY )
                                dx += GRAVITY;
                        else
                                dx = 0;
                        if ( dy > GRAVITY )
                                dy -= GRAVITY;
                        else if ( dy < -GRAVITY )
                                dy += GRAVITY;
                        else
                                dy = 0;

                        if ( virtualx > MAX_X )
                                virtualx -= MAX_X;
                        if ( virtualx < 0 )
                                virtualx += MAX_X;
                        if ( virtualy > MAX_Y )
                                virtualy -= MAX_Y;
                        if ( virtualy < 0 )
                                virtualy += MAX_Y;
                        actualx = (int)virtualx;
                        actualy = (int)virtualy;
                }

                int ang;
                int rad;
                int want_ang;

};

void explosion_init() {
        RGB change;
        for ( int r = expl_color; r > expl_color-10; r-- ) {
                change.r = 63;
                change.b = 63;
                change.g = 63;
                set_color( r, &change );
        }

        for ( int r = expl_color-10; r > expl_color - 32; r-- ) {
                change.g = 63 - ( expl_color-10-r );
                change.b = 0;
                change.r = 63;
                set_color( r, &change );
        }
        for ( int r = expl_color-32; r > expl_color-63; r-- ) {
                change.r = 55- ( expl_color-32-r );
                change.b = 0;
                change.g = 32 - ( expl_color-32-r );
                set_color( r, &change );
        }
}

volatile unsigned int speed_counter;
void speed_timer(){
	speed_counter++;
}
END_OF_FUNCTION( speed_timer );

void init() {

        allegro_init();
        install_keyboard();
        set_trig();
        srandom( time( NULL ) );
        install_timer();
        install_keyboard();
        set_color_depth( 8 );

        set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_x, screen_y, 0, 0 );
        explosion_init();

        LOCK_VARIABLE( speed_counter );
	LOCK_FUNCTION( (void *)speed_timer );
	install_int( speed_timer, 30 );
	speed_counter = 0;
}


int main() {
        init();
        AllegroCamera ** camera = new AllegroCamera*[ 2 ];
        Ship ** piece = new Ship*[ MAX_OBJ ];
        StarClass ** star = new StarClass*[ MAX_STAR ];
        Block ** blocks = new Block*[ MAX_BLOCK ];
        ExplosionClass ** expl = new ExplosionClass*[ MAX_EXPL ];
        for ( int q = 0; q < MAX_EXPL; q++ )
                expl[q] = NULL;
        for ( int q = 0; q < MAX_STAR; q++ )
                star[q] = new StarClass();
        for ( int q = 0; q < 2; q++ )
                camera[q] = new AllegroCamera();
        for ( int q = 0; q < MAX_OBJ; q++ )
                piece[q] = new Ship( q, random() % MAX_X, random() % MAX_Y );
        for ( int q = 0; q < MAX_BLOCK; q++ ) {
                blocks[ q ] = new Block( random() % 3, random() % MAX_X, random() % MAX_Y );
                blocks[ q ]->dx = (random() % 10) - 5;
                blocks[ q ]->dy = (random() % 10) - 5;
        }

        camera[ 0 ]->setCenter( piece[ 0 ]->actualx, piece[ 0 ]->actualy );
        camera[ 1 ]->setCenter( piece[ 1 ]->actualx, piece[ 1 ]->actualy );

        while ( !key[KEY_ESC] ) {
            if (speed_counter > 0){
                unsigned int use = speed_counter;
                speed_counter = 0;
                for (unsigned int i = 0; i < use; i++){

                    for ( int q = 0; q < MAX_EXPL; q++ )
                        if( expl[q] != NULL )
                            if ( expl[q]->update() ) {
                                delete expl[q];
                                expl[q] = NULL;
                            }
                    for ( int q = 0; q < MAX_OBJ; q++ ) {
                        /*
                           if ( q != 0 )
                           piece[q]->CompMove();
                           else piece[q]->ControlMove( expl );
                           */
                        if ( q == 0 )
                            piece[q]->ControlMove( expl );
                        piece[q]->moveD();
                    }
                    for ( int q = 0; q < MAX_BLOCK; q++ )
                        blocks[ q ]->update( blocks );
                    for ( int q = 0; q < MAX_OBJ; q++ )
                        piece[q]->interact( blocks );
                    for ( int q = 0; q < 2; q++ )
                        camera[ q ]->setCenter( piece[ q ]->actualx, piece[ q ]->actualy );
                    for ( int q = 0; q < MAX_STAR; q++ ) {
                        star[q]->Draw( camera[0] );
                        star[q]->Draw( camera[1] );
                    }
                    for ( int q = 0; q < MAX_EXPL; q++ )
                        if( expl[q] != NULL ) {
                            expl[q]->Draw( camera[0] );
                            expl[q]->Draw( camera[1] );
                        }
                    for ( int q = 0; q < MAX_OBJ; q++ ) {
                        piece[q]->Draw( camera[0] );
                        piece[q]->Draw( camera[1] );
                    }                                 // end for
                    for ( int q = 0; q < MAX_BLOCK; q++ ) {
                        blocks[ q ]->Draw( camera[ 0 ] );
                        blocks[ q ]->Draw( camera[ 1 ] );
                    }                                 // end for

                    line( camera[1]->map, 0, 0, 640, 0, 31 );
                    for ( int q = 0; q < 2; q++ )
                        camera[q]->Draw( screen, q );
                }
            }
        }
        textprintf( screen, font, 320, 200, 31, "QUITTING" );

        for ( int q = 0; q < MAX_OBJ; q++ )
                delete piece[q];
        delete[] piece;
        for ( int q = 0; q < MAX_STAR; q++ )
                delete star[q];
        delete[] star;
        for ( int q = 0; q < MAX_EXPL; q++ )
                if ( expl[q] != NULL )
                        delete expl[q];
        delete[] expl;
        for ( int q = 0; q < MAX_BLOCK; q++ )
                delete blocks[q];
        delete[] blocks;
        for ( int q = 0; q < 2; q++ )
                delete camera[q];
        delete[] camera;
        textprintf( screen, font, 320, 210, 31, "Final Shutdown" );

}
END_OF_MAIN()
