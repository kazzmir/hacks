#define ALLEGRO_NO_FIX_ALIASES

#include "allegro.h"
#include <time.h>
#include "trigtable.h"
#include <stdlib.h>

#define max_c 20
#define cq 3
#define screen_x 640
#define screen_y 480
#define max_x 2000
#define max_y 2000
#define BORDER 20
#define max_orb 15
#define orb_rad 17
#define max_flint 500
#define max_explode 500

#define div_x (max_x/screen_x)
#define div_y (max_y/screen_y)

struct exploder {

        int x, y;
        int life;
        double dx, dy;
        double fx, fy;
        double speed;
        bool alive;

};

struct flint_rec {

        int x, y;
        int c;
        bool alive;

};

struct orb_rec {

        int x, y;
        double fx, fy, dx, dy;
        int c;
        bool alive;

};

struct planet_rec {

        int x, y;
        double magma;

};

int grand_clock=0;

BITMAP * work;
orb_rec orb[max_orb];
bool left_click;
bool right_click;
planet_rec planet;
flint_rec flint[ max_flint ];
exploder expl_[ max_explode ];

void set_pal() {

        RGB change;
        change.g = 10;
        change.b = 0;
        change.r = 63;
        for ( int x = 31; x >= 15; x-- ) {

                set_color( x, &change );
                //change.r-=3;
                //change.g--;
                change.g+=3;

        }

}

volatile static int speed_counter = 0;
static void inc_speed_counter(){
	speed_counter += 1;
}

void init() {

        set_trig();
        allegro_init();
        srandom( 2 );
        install_timer();
        install_keyboard();
        set_color_depth( 8 );
        install_mouse();
        position_mouse( 320, 200 );

        set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_x, screen_y, screen_x, screen_y );
        set_pal();

        text_mode(-1);
        work = create_bitmap( screen_x, screen_y );
        clear( work );
	
        install_int_ex( inc_speed_counter, BPS_TO_TIMER( 60 ) );

        for ( int x = 0; x < max_orb; x++ )
                orb[x].alive = false;
        for ( int x = 0; x < max_explode; x++ )
                expl_[x].alive = false;

        planet.x = random() % ( max_x-300) + 200;
        planet.y = random() % ( max_y-250) + 200;
        planet.magma = random() % 800 / 200 + 2.0;

}


bool right_pressed()
{return ( mouse_b & 2 );}

bool left_pressed()
{return ( mouse_b & 1 );}

int tsqr( int r ) {

        return r*r;

}


int tsqrt( int q ) {

        if ( q <= 0 )
                return 0;

        int min = 0;
        while ( tsqr( min ) < q )
                min+=30;
        while ( tsqr( min ) > q )
                min -= 10;
        while ( tsqr( min ) < q )
                min++;
        return min;

}


/*
int dist( int x1, int y1, int x2, int y2 )
{

   int a = tsqr( y1 - y2 );
   int b = tsqr( x1 - x2 );

   return tsqrt( a + b );

}
*/

void make_exploder( int x, int y, double wx, double wy ) {

        int q = 0;
        while ( q < max_explode && expl_[q].alive )
                q++;
        if ( q >= max_explode )
                return;
        expl_[q].alive = true;
        expl_[q].x = x;
        expl_[q].y = y;
        expl_[q].fx = x;
        expl_[q].fy = y;
        expl_[q].dx = wx;
        expl_[q].dy = wy;
        expl_[q].life = random() % 3 + 50;
        expl_[q].speed = sqrt( wx*wx+wy*wy );

}


int get_orb() {

        int q = 0;
        while ( q < max_orb && orb[q].alive )
                q++;
        if ( q >= max_orb )
                return -1;
        return q;

}


bool good_space( int x, int y, int q ) {

        for ( int m = 0; m < max_orb; m++ )
        if ( m != q && orb[m].alive ) {

                if ( dist( orb[m].x, orb[m].y, x, y ) <= orb_rad*2 )
                        return true;

        }
        return false;

}


void make_orb() {

        /*
        if ( random() % 50 != random() % 50 )
           return;
        */

        int q = get_orb();
        if ( q == -1 )
                return;
        orb[q].alive = true;
        do {
                orb[q].x = random() % (max_x-50) + 30;
                orb[q].y = random() % (max_y-100) + 50;
        }while ( good_space(orb[q].x,orb[q].y,q) );
        orb[q].fx = orb[q].x;
        orb[q].fy = orb[q].y;
        orb[q].dx = random() % 700 / 200;
        orb[q].dy = random() % 700 / 200;

}


int posneg() {

        int q = random() % 2;
        if ( q == 0 )
                return 1;
        else
                return -1;

}


void make_expl( int ax, int ay ) {

        for ( int x = 0; x < random() % 20 + 50; x++ )
                make_exploder( ax, ay, (double)(random() % 800) / (double)200 * posneg(),
                        (double)(random() % 800) / (double)200 * posneg() );

}


bool collide( double ax, double ay, int who ) {

        int x = (int)ax;
        int y = (int)ay;
        if ( x <= BORDER || x >= max_x-BORDER || y <= BORDER || y >= max_y-BORDER ) {
                if ( x >= max_x-BORDER || x <= BORDER )
                        orb[who].dx = -orb[who].dx;
                if ( y >= max_y-BORDER || y <= BORDER )
                        orb[who].dy = -orb[who].dy;
                return true;

        }
        for ( int q = 0; q < max_orb; q++ )
                if ( q != who && orb[q].alive )
                if ( dist( orb[q].x, orb[q].y, x, y ) <= orb_rad*2 ) {
                        double t1, t2;
                t1 = orb[who].dx;
                t2 = orb[who].dy;
                orb[who].dx = orb[q].dx;
                orb[who].dy = orb[q].dy;
                orb[q].dx = t1;
                orb[q].dy = t2;

                make_expl( x, y );

                return true;
        }

        return false;

}


void find_col( int & me ) {

        int tk = me;
        if ( me >= 0 )
                tk = 41+72;
        if ( me > 5 )
                tk = 41;
        if ( me > 15 )
                tk = 44;
        if ( me > 25 )
                tk = 67;
        if ( me >= 35 )
                tk = 31;
        me = tk;

}


void draw_expl() {

        for ( int q = 0; q < max_explode; q++ )
        if ( expl_[q].alive ) {

                int col = (int)( (double)(expl_[q].life*3/2) / expl_[q].speed );
                find_col( col );
                circlefill( work, expl_[q].x/div_x, expl_[q].y/div_y, 1, col );

        }

}


void move_expl() {

        for ( int q = 0;q<max_explode; q++ )
        if ( expl_[q].alive ) {

                expl_[q].fx += expl_[q].dx;
                expl_[q].fy += expl_[q].dy;
                expl_[q].x = (int)expl_[q].fx;
                expl_[q].y = (int)expl_[q].fy;
                expl_[q].life--;
                if ( expl_[q].life <= 0 )
                        expl_[q].alive = false;

        }

}


void magnetize( double & ax, double & ay, int who ) {

        for ( int q = 0; q < max_orb; q++ )
        if ( orb[q].alive && q != who ) {

                int ds = dist( orb[who].x, orb[who].y, orb[q].x,orb[q].y );
                int jang = gang( orb[who].x,orb[who].y,orb[q].x,orb[q].y );
                if ( ds < 1 )
                        ds = 1;
                ax += tcos[jang]*cq*(1.0/(double)ds);
                ay += tsine[jang]*cq*(1.0/(double)ds);

        }
        /*
        int ds = dist( orb[who].x, orb[who].y, planet.x,planet.y );
        int jang = gang( orb[who].x,orb[who].y,planet.x,planet.y );
        if ( ds < 1 )
           ds = 1;
        ax += tcos[jang]*planet.magma*(1.0/(double)ds);
        ay += tsine[jang]*planet.magma*(1.0/(double)ds);
        */

}


void make_flint(int x,int y) {

        int q = 0;
        while ( q < max_flint && flint[q].alive )
                q++;
        if ( q >= max_flint )
                return;
        flint[q].alive = true;
        flint[q].c = max_c;
        flint[q].x = x;
        flint[q].y = y;
}


void move_orb() {

        for ( int q = 0; q < max_orb; q++ )
        if ( orb[q].alive ) {

                magnetize( orb[q].dx, orb[q].dy, q );
                double mx = orb[q].fx + orb[q].dx;
                double my = orb[q].fy + orb[q].dy;
                if ( !collide(mx,my,q) ) {
                        orb[q].fx = mx;
                        orb[q].fy = my;
                }
                orb[q].x = (int)orb[q].fx;
                orb[q].y = (int)orb[q].fy;

                if ( grand_clock % 2 == 0 )
                        make_flint( orb[q].x, orb[q].y );

        }

}


void show_orb() {

        for ( int q = 0; q< max_orb; q++ )
                if ( orb[q].alive );
        //circlefill( work, orb[q].x, orb[q].y, orb_rad, 42 );

}


void kill_orb() {

        int q = 0;
        int time = 0;
        /*
        while ( q < max_orb && orb[q].alive == false )
           q++;
        if ( q >= max_orb )
           return;
        */
        do {
                q = random() % max_orb;
                time++;
        }while ( time < 10000 && !orb[q].alive );
        if ( time >= 10000 )
                return;
        orb[q].alive = false;

}


void show_flint() {
        for ( int q = 0; q < max_flint; q++ )
        if ( flint[q].alive ) {

                int a;
                if ( flint[q].c > 15 )
                        a = 31;
                else
                        a = 15+flint[q].c;
                int x = flint[q].x / div_x;
                int y = flint[q].y / div_y;
                //putpixel( work, x, y, a );
                circlefill( work, x, y, (a/7)+1, a );
                flint[q].c--;
                if ( flint[q].c <= 0 )
                        flint[q].alive = false;

        }
}


void show_planet() {
        circlefill( work, planet.x, planet.y, 40, 32+72 );
}


int main() {

        init();

        for ( int x = 0; x < max_orb; x++ )
                make_orb();
        while ( !key[KEY_ESC] ) {
            bool draw = false;
            while ( speed_counter > 0 ){
                    speed_counter -= 1;

                    draw = true;

                if ( random() % 200 == random() % 200 ) {
                        kill_orb();
                        make_orb();
                }

                grand_clock++;
                if ( grand_clock > 10000 )
                        grand_clock = 0;

                if ( left_pressed() && !left_click ) {
                        make_orb();
                        left_click = true;
                }
                if ( !left_pressed() )
                        left_click = false;
                if ( right_pressed() && !right_click ) {
                        kill_orb();
                        right_click = true;
                }
                if ( !right_pressed() )
                        right_click = false;

                move_orb();
                move_expl();
            }

            if (draw){
                //show_planet();
                draw_expl();
                show_flint();
                //show_orb();
                blit( work, screen, 0, 0, 0, 0, 640, 480 );
                clear( work );
            }

            while (speed_counter == 0){
                rest(1);
            }
        }

        destroy_bitmap( work );

}


END_OF_MAIN();
