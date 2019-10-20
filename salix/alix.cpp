#define ALLEGRO_NO_FIX_ALIASES

#include "allegro.h"
#include "trigtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GAME_SPEED 35 
#define screen_x 640
#define screen_y 480
#define max_star 2500
#define MAX_BULLET 100
#define MAX_SHIP 9
#define MAX_POWER 10
#define BULLET_MOVE 2
#define FOCUS 2.63
#define BASECOLOR 70

#define expl_color 240
#define max_expl 150

#define max_sample 8
#define snd_death_1 0
#define snd_death_2 1
#define snd_weapon_1 2
#define snd_weapon_2 3
#define snd_weapon_3 4
#define snd_weapon_4 5
#define snd_explode 6
#define snd_weapon_5 7

void init();
volatile int speed_counter;

SAMPLE * snd[ max_sample ];

enum PowerKind{ RED, BLUE, GREEN, HEALTH, SHIELD };

class Camera;
class SpaceShip;
class Bullet;
void DiffAngle( int & change, int stay );

int line_hit( int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy /*, int *ix, int *iy*/ ) {
	float r, s;
	float denom = (bx-ax)*(dy-cy)-(by-ay)*(dx-cx);

	r = (float)((ay-cy)*(dx-cx)-(ax-cx)*(dy-cy))/denom;
	s = (float)((ay-cy)*(bx-ax)-(ax-cx)*(by-ay))/denom;

	if ( r < 0.0 || s < 0.0 || r > 1.0 || s > 1.0 )
		return 0;

	/*
	*ix=ax+(int)(r*(float)(bx-ax)+0.5);
	*iy=ay+(int)(r*(float)(by-ay)+0.5);
	*/

	return 1;

} // end lineIntersect

bool point_in_triangle( int x1, int y1, int x2, int y2, int x3, int y3, int mx, int my )
{

   int ang = random() % 360;

   int mx2, my2;
   mx2 = (int)(mx + tcos[ ang ] * 15000 );
   my2 = (int)(my + tsine[ ang ] * 15000 );

   int count = 0;
   if ( line_hit( x1, y1, x2, y2, mx, my, mx2, my2 ) &&
	line_hit( mx, my, mx2, my2, x1, y1, x2, y2 ) )
      count++;
   if ( line_hit( x2, y2, x3, y3, mx, my, mx2, my2 ) &&
	line_hit( mx, my, mx2, my2, x2, y2, x3, y3 ) )
      count++;
   if ( line_hit( x1, y1, x3, y3, mx, my, mx2, my2 ) &&
	line_hit( mx, my, mx2, my2, x1, y1, x3, y3 ) )
      count++;

   if ( count == 1 || count == 3 )
   {

      ang = ( ang + 180 ) % 360;
      mx2 = (int)(mx + tcos[ ang ] * 15000 );
      my2 = (int)(my + tsine[ ang ] * 15000 );

      count = 0;
      if ( line_hit( x1, y1, x2, y2, mx, my, mx2, my2 ) )
	 count++;
      if ( line_hit( x2, y2, x3, y3, mx, my, mx2, my2 ) )
	 count++;
      if ( line_hit( x1, y1, x3, y3, mx, my, mx2, my2 ) )
	 count++;

      if ( count == 1 || count == 3 )
	 return true;
      else
	 return false;

   }
   else
      return false;

}

struct StarNode{
	int x, y, co;
};

class Camera{
public:

	Camera( int x, int y, int mx, int my):
		cen_x( x ),
		cen_y( y ),
		MAX_X( mx ),
		MAX_Y( my ){
		work = create_bitmap( screen_x, screen_y );
		clear( work ); 
		for ( int q = 0; q < max_star; q++ ){
			star[q].x = random() % MAX_X;
			star[q].y = random() % MAX_Y;
			star[q].co = random() % 16 + 16;
		}

	}

	int CX( int x ){
		return (int)((x - cen_x)/FOCUS);
	}

	int CY( int y ){
		return (int)((y - cen_y)/FOCUS);
	}

	bool Visible( int x, int y ){
		if ( x >= cen_x && x <= (int)(cen_x+screen_x*FOCUS) &&
			y >= cen_y && y <= (int)(cen_y + screen_y*FOCUS) )
				return true;
		return false;
	}

	void MoveXY( int x, int y ){
		cen_x = (int)(x-320*FOCUS);
		cen_y = (int)(y-200*FOCUS);
		if ( cen_x < 0 )
			cen_x = 0;
		if ( cen_y < 0 )
			cen_y = 0;
		if ( cen_x >= MAX_X-(int)(640*FOCUS) )
			cen_x = MAX_X-(int)(640*FOCUS);
		if ( cen_y >= MAX_Y-(int)(480*FOCUS) )
			cen_y = MAX_Y-(int)(480*FOCUS);
	}

	void StarDraw(){
		for ( int q = 0; q < max_star; q++ )
			if ( Visible( star[q].x, star[q].y ) )
				putpixel( work, CX(star[q].x),
						CY(star[q].y),
						star[q].co );
	}

	void Draw(){
		blit( work, screen, 0, 0, 0, 0, 640, 480 );
		clear( work );
	}

	virtual ~Camera(){
		destroy_bitmap( work );
	}

BITMAP * work;
int cen_x, cen_y;
int MAX_X, MAX_Y;
StarNode star[ max_star ];

};

void DiffAngle( int & change, int stay ){

		#define adder 4
		if ( stay == change )
			return;
		if ( stay <= 180 && change <= 180 ){
			if ( stay > change )
				change+=adder;
			else
				change-=adder;
		}
		else
		if ( stay <= 90 && change >= 270 )
			change+=adder;
		else
		if ( stay >= 270 && change <= 90 )
			change-=adder;
		else
		if ( stay >= 180 && change >= 180 ){
			if ( stay > change )
				change+=adder;
			else
				change-=adder;
		}
		else
		if ( abs(stay-change)<180 )
			if ( stay > change )
				change+=adder;
			else
				change-=adder;

		if ( abs(stay-change)<adder )
			change = stay;

	}

struct explosion_rec{

   int x, y;
   double vx, vy;
   double wx, wy;
   int radius;
   int max_rad;
   bool alive;

};
struct explosion_rec expl_[ max_expl ];

void draw_expl( Camera * cam ){

	for ( int r = 0; r< max_expl; r++ )
		if ( expl_[r].alive ){
			expl_[r].vx += expl_[r].wx;
			expl_[r].vy += expl_[r].wy;
			expl_[r].x = (int)expl_[r].vx;
			expl_[r].y = (int)expl_[r].vy;
			int color = expl_[r].radius;
color = (int)( ( (double)(expl_[r].max_rad - color ) / (double)expl_[r].max_rad ) * 100.0 );
			color = color / ( 100 / 63 );
			if ( color > 63 )
				color = 63;
			if ( color < 0 )
				color = 0;
			if ( cam->Visible( expl_[r].x, expl_[r].y ) )
				circlefill( cam->work, cam->CX(expl_[r].x), cam->CY(expl_[r].y), expl_[r].radius, expl_color-color );

			expl_[r].radius--;
			if ( expl_[r].radius <= 0 )
			expl_[r].alive = false;
	}
}

void make_explosion( int x, int y, double walkx, double walky, int rad ){

   int found = 0;
   while ( expl_[ found ].alive && found < max_expl )
      found++;

   if ( found >= max_expl )
      return;

   expl_[found].alive = true;
   expl_[found].x = x;
   expl_[found].y = y;
   expl_[found].vx = x;
   expl_[found].vy = y;
   expl_[found].radius = rad;
   expl_[found].max_rad = rad;
   expl_[found].wx = walkx;
   expl_[found].wy = walky;

}

void make_sane( int x, int y ){
	for ( int q = 0; q < 20; q++ ){
		double f = random() % 8 + 3;
		int a = random() % 360;
		make_explosion( x, y, tcos[a]*f,tsine[a]*f,random()%9+6 );
	}

}

void make_small( int x, int y ){
	for ( int q = 0; q < 3; q++ ){
		double f = random() % 4 + 2;
		int a = random() % 360;
		make_explosion( x, y, tcos[a]*f,tsine[a]*f,random()%7+3 );
	}
}

void explosion_init(){
	for ( int r = 0; r< max_expl; r++ )
		expl_[r].alive = false;

	RGB change; 
	for ( int r = expl_color; r > expl_color-10; r-- ){
		change.r = 63;
		change.b = 63;
		change.g = 63;
		set_color( r, &change );
	}

	for ( int r = expl_color-10; r > expl_color - 32; r-- ){
		change.g = 63 - ( expl_color-10-r );
		change.b = 0;
		change.r = 63;
		set_color( r, &change );
	}
	for ( int r = expl_color-32; r > expl_color-63; r-- ){
		change.r = 55- ( expl_color-32-r );
		change.b = 0;
		change.g = 32 - ( expl_color-32-r );
		set_color( r, &change );
	}
}

class SpaceShip{
public:
	SpaceShip( int x, int y, int c, int max_sp, int rad, int menergy, int sh, int sw, double max_life ):
		rx( x ),
		ry( y ),
		fx( x ),
		fy( y ),
		dx( 0 ),
		dy( 0 ),
		MAX_LIFE( max_life ),
		life( max_life ),
		SHOT_ENERGY( sh ),
		SHOT_WAIT( sw ),
		MAX_ENERGY( menergy ),
		MAX_SPEED( max_sp ),
		radius( rad ),
		same_rad( rad ),
		speed( 0 ),
		shield(0),
		energy( menergy ),
		color( c ){
			for ( int q = 0; q < 3; q++ )
				shot_counter[q] = 0;
			ang = random() % 360;
			make_explosion( rx, ry, 0, 0, radius*8/7 );
	}

	virtual void ShootFirst( Bullet **& blist ){}
	virtual void ShootSecond( Bullet **& blist ){}
	virtual void ShootThree( Bullet **& blist ){}
	virtual void Draw( Camera * cam ){} 

	void Energize(){
		if ( energy < MAX_ENERGY )
			energy++;
	}

	void Govern(){

		if ( dx > 0 ){

			if ( fabs(tcos[ang]) > 0 )
				if ( dx > fabs(tcos[ang])*MAX_SPEED )
					dx = fabs(tcos[ang])*MAX_SPEED;

		} else {

			if ( -fabs(tcos[ang]) < 0 )
				if ( dx < -fabs(tcos[ang])*MAX_SPEED )
					dx = -fabs(tcos[ang])*MAX_SPEED; 

		}

		if ( dy > 0 ){

			if ( fabs(tsine[ang]) > 0 )
				if ( dy > fabs(tsine[ang])*MAX_SPEED )
					dy = fabs(tsine[ang])*MAX_SPEED;

		} else {

			if ( -fabs(tsine[ang]) < 0 )
				if ( dy < -fabs(tsine[ang])*MAX_SPEED )
					dy = -fabs(tsine[ang])*MAX_SPEED; 

		}

	}

	void accelerate(){
		dx += tcos[ang];
		dy += tsine[ang];
		Govern();
	}

	void deccelerate(){
		dx -= tcos[ang];
		dy -= tsine[ang];
		Govern();
	}

	void realmove( int lx, int ly ){
		fx += dx;
		fy += dy;
		rx = (int)fx;
		ry = (int)fy;
		if ( rx < 0 ){
			rx = lx;
			fx = rx;
		}
		if ( rx > lx ){
			rx = 0;
			fx = rx;
		}
		if ( ry < 0 ){
			ry = ly;
			fy = ry;
		}
		if ( ry > ly ){
			ry = 0;
			fy = ry;
		} 
	}

	virtual void Move( int lx, int ly, Bullet **& blist, SpaceShip * slist ){
		for ( int q = 0; q < 3; q++ )
			if ( shot_counter[q] )
				shot_counter[q]--;
		accelerate();
		if ( dist( rx, ry, destx, desty ) < 10 ){
			destx = random() % lx;
			desty = random() % ly;
			want_ang = gang( rx, ry, destx, desty );
		}
		if ( random() % 150 == random() % 150 ){
			destx = slist->rx;
			desty = slist->ry;
		}
		if ( random() % 10 == random() % 10 )
			want_ang = gang( rx, ry, destx, desty );
		DiffAngle( ang, want_ang ); 
		if ( ang < 0 )
			ang += 360;
		ang %= 360;

		if ( abs(ang-gang( rx, ry, slist->rx, slist->ry ) ) < 10 ){
			if ( random() % 5 == 0 )
				if ( !shot_counter[0] ){
					ShootFirst( blist );
					shot_counter[0] = SHOT_WAIT;
				}
			if ( random() % 5 == 0 )
				if ( !shot_counter[0] ){
					ShootSecond( blist );
					shot_counter[0] = SHOT_WAIT;
				}
			if ( random() % 5 == 0 )
				if ( !shot_counter[0] ){
					ShootThree( blist );
					shot_counter[0] = SHOT_WAIT;
				}
		} 

		realmove( lx, ly );

	}

	void ShowEnergy( Camera * cam ){

		if ( shield > 0 )
			for ( int q = 0; q < (int)shield; q++){
				int c;
				c = color+1+ ( q * 8 / 100 );
				rectfill( cam->work, 580, 100*3+10-q*3, 590, 100*3+10-q*3-1, c );
			}


		for ( int q = 0; q < (int)life; q++ ){
			int c;
			c = 18 + ( q * 13 / (int)MAX_LIFE );
			rectfill( cam->work, 600, (int)(MAX_LIFE)*3+20-q*3, 610, (int)(MAX_LIFE)*3+20-q*3-1, c );

		}
		for ( int q = 0; q <= energy/2; q++ ){
			int c = 40;
			if ( q > 10 )
				c = 44;
			if ( q > 20 )
				c = 48;
			if ( q == MAX_ENERGY/2 )
				c = 31;
				rectfill( cam->work, 620, 180-q*3, 630, 180-q*3-1, c );
		}
	}

	virtual ~SpaceShip(){
		play_sample( snd[snd_death_1+random()%2],255,128,1000,false );
	}

int rx, ry;
double fx, fy;
double dx, dy;
double speed;
int ang;
int color;
int energy;
int radius;
double life;
int shot_counter[3];

double shield;
int same_rad;
double MAX_LIFE;
int MAX_ENERGY;
double MAX_SPEED;
int SHOT_WAIT;
int SHOT_ENERGY;

int destx, desty, want_ang;
int power;
PowerKind kind;


};

class Bullet{
public:

	Bullet( int x, int y, int ang, int c, int bs, int br, double str, int sn ):
		rx( x ),
		ry( y ),
		fx( x ),
		fy( y ),
		angle( ang ),
		color( c ),
		strength( str ),
		BULLET_RAD( br ),
		speed( bs ),
		life( 250 ){
			angle += 360;
			angle %= 360;
			if ( sn >= 0 )
				play_sample( snd[sn],230,128,1000,false );
		}

	int mdist( int x1, int y1, int x2, int y2 ){
		return ( abs(x1-x2) + abs(y1-y2) );
	}

	virtual bool interact( SpaceShip * sh ){
		if ( sh == NULL )
			return false;

		if ( mdist( sh->rx, sh->ry, rx, ry ) > (BULLET_RAD+sh->radius+2*(sh->shield>0))*2 )
			return false;

		if ( dist( sh->rx, sh->ry, rx, ry ) <= BULLET_RAD + sh->radius+2*(sh->shield>0) ){
			sh->fx += tcos[angle]*speed*2;
			sh->fy += tsine[angle]*speed*2;
			sh->rx = (int)sh->fx;
			sh->ry = (int)sh->fy;
			if ( sh->shield > 0 )
				sh->shield -= strength;
			else
				sh->life -= strength;
			if ( sh->energy > 0 )
				sh->energy--;
			make_small( rx, ry );
			return true;
		}
		return false;
	}

	virtual bool Move( SpaceShip ** slist, int lx, int ly ){
		fx += tcos[angle]*speed;
		fy += tsine[angle]*speed;
		rx = (int)fx;
		ry = (int)fy;
		if ( rx < 0 ){
			rx = lx;
			fx = rx;
		}
		if ( rx > lx ){
			rx = 0;
			fx = rx;
		}
		if ( ry < 0 ){
			ry = ly;
			fy = ry;
		}
		if ( ry > ly ){
			ry = 0;
			fy = ry;
		}
		for ( int q = 0; q < MAX_SHIP; q++ )
			if ( slist[q] != NULL )
				if ( interact( slist[q] ) ){
					play_sample( snd[snd_explode],220,128,1000,false );
					return true;
				}
		life--;
		if ( life <= 0 ) return true;
		return false;
	}

	virtual void Draw( Camera * cam ){}

	~Bullet(){}

int rx, ry;
double fx, fy;
double speed;
int angle;
int color;
int life;
double strength;
int BULLET_RAD;

};

class LargeBullet: public Bullet{
public:

	LargeBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 8.1, 6, 3, snd_weapon_1 ){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){
			int st = life % 10;
			for ( int q = 0; q < (int)(BULLET_RAD/FOCUS); q++ ){
				circlefill( cam->work, cam->CX(rx),cam->CY(ry),
					(int)(BULLET_RAD/FOCUS)-q, color+st );
				st++;
				if ( st > 9 )
					st = 0;
			}
		}
	}

};

class TrainBullet: public Bullet{
public:

	TrainBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 8.8, 6, 3, snd_weapon_2 ){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){

			circlefill( cam->work, cam->CX(rx), cam->CY(ry), 2, color+9-life%3 );
			int ang = life % 3 * 45;
			for ( int q = 0; q < 12; q++ ){
				circlefill( cam->work, cam->CX((int)(rx+tcos[ang]*13)),
						cam->CY((int)(ry+tsine[ang]*13)), 1, color+4 );
				ang += 45;
				ang %= 360;
			}

		}
	}

};

class MassiveBullet: public Bullet{
public:

	MassiveBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 7.5, 9, 6, snd_weapon_3 ),
		qol(0){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){

			fixed ang1, ang2;
			ang1 = itofix( (angle+90)%360 * 256 / 360 );
			ang2 = itofix( (angle+270)%360 * 256 / 360 );
			int x1 = cam->CX(rx);
			int y1 = cam->CY(ry);
			double x2 = x1;
			double y2 = y1;
			for ( int q = 9; q > 0; q--){
				int cr = color+(q+qol)%10;
				arc( cam->work, x1, y1, ang2, ang1, (int)(BULLET_RAD/FOCUS), cr );
				x2 += tcos[(angle+180)%360];
				y2 += tsine[(angle+180)%360];
				x1 = (int)x2;
				y1 = (int)y2;
			}
			qol++;
		}
	}

int qol;

};

class CoolBullet: public Bullet{
public:

	CoolBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 8.3, 3, 1.7, snd_weapon_4 ){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){

			circlefill( cam->work, cam->CX(rx), cam->CY(ry), 1, color+9 );

		}
	}

};

class SmallBullet: public Bullet{
public:

	SmallBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 8.5, 3, 1, snd_weapon_4 ){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){

			circlefill( cam->work, cam->CX(rx), cam->CY(ry), 1, color+9 );

		}
	}

};


class Gernade: public Bullet{
public:

	Gernade( int x, int y, int ang, int c, int xlife, int destroy, int ex_area ):
		Bullet( x, y, ang, c, 7.1, 15, 3, snd_weapon_2 ),
		sm_ang(0){
			life = xlife;
			semi_life = destroy; 
			semi_area = ex_area;
		}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){

			if ( semi_life > 0 ){
				circlefill( cam->work, cam->CX(rx), cam->CY(ry), (int)(BULLET_RAD/FOCUS), color+6 );
				for ( int q = 0; q <= 90; q+= 45 ){
					int x1 = (int)(rx+tcos[(q+sm_ang)%360]*BULLET_RAD/2);
					int y1 = (int)(ry+tsine[(q+sm_ang)%360]*BULLET_RAD/2);
					int x2 = (int)(rx+tcos[(q+sm_ang+180)%360]*BULLET_RAD/2);
					int y2 = (int)(ry+tsine[(q+sm_ang+180)%360]*BULLET_RAD/2);
					line( cam->work, cam->CX(x1), cam->CY(y1),
						cam->CX(x2), cam->CY(y2), color+9 );

				}
				sm_ang = (sm_ang+1)%360;
			} else {

				for ( int q = 0; q < 9; q++ ){
					int sang = random() % 360;
					int dr = random() % BULLET_RAD;
					make_explosion( (int)(rx+tcos[sang]*dr), (int)(ry+tsine[sang]*dr), 0,0,random()%6+3 );
				}

			}

		}
	}

	virtual bool Move( SpaceShip ** slist, int lx, int ly ){
		if ( semi_life > 0 ){
			fx += tcos[angle]*speed;
			fy += tsine[angle]*speed;
			rx = (int)fx;
			ry = (int)fy;
		}
		if ( rx < 0 ){
			rx = lx;
			fx = rx;
		}
		if ( rx > lx ){
			rx = 0;
			fx = rx;
		}
		if ( ry < 0 ){
			ry = ly;
			fy = ry;
		}
		if ( ry > ly ){
			ry = 0;
			fy = ry;
		}
		for ( int q = 0; q < MAX_SHIP; q++ )
			if ( slist[q] != NULL )
				if ( interact( slist[q] ) ){
					play_sample( snd[snd_explode],220,128,1000,false );
					if ( semi_life > 0 ) return true;
				}
		if ( semi_life > 0 ){
			semi_life--;
			if ( semi_life <= 0 ){
				BULLET_RAD = semi_area;
				strength = 0.7;
				speed = 0;
				play_sample( snd[snd_explode],220,128,1000,false );
			}
		}
		life--;
		if ( life <= 0 ) return true;
		return false;
	}

int sm_ang;
int semi_life;
int semi_area;

};

class MegaBullet: public Bullet{
public:

	MegaBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 7.3, 14, 25, snd_weapon_3 ){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){ 
			make_explosion( rx, ry, 0,0,5 );
		}
	}

};

class FireBullet: public Bullet{
public:

	FireBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 7.8, 5, 4, snd_weapon_2 ){}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){ 
			make_explosion( rx, ry, 0,0,3 );
		}
	}

};

class Fast: public Bullet{
public:

	Fast( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 8.3, 5, 0, -1 ){}

	virtual void Draw( Camera * cam ){}

};

class LaserBullet: public Bullet{
public:

	LaserBullet( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 10.3, 5, 1, snd_weapon_4 ){
			for ( int q = 0; q < 5; q++){
				xp[q*2] = (int)(rx+tcos[angle]*BULLET_RAD*1.25);
				xp[q*2+1] = (int)(ry+tsine[angle]*BULLET_RAD*1.25);
			}

		}

	virtual void Draw( Camera * cam ){
		if ( cam->Visible( rx, ry ) ){
			int x1 = (int)(rx+tcos[angle]*BULLET_RAD);
			int y1 = (int)(ry+tsine[angle]*BULLET_RAD);
			line( cam->work, cam->CX(rx), cam->CY(ry),
					cam->CX(x1), cam->CY(y1),color+9);
			circlefill( cam->work, cam->CX(rx), cam->CY(ry), 1, color+9 );
			for ( int q = 0; q < 5; q++ )
				if ( cam->Visible( xp[q*2], xp[q*2+1] ) )
					putpixel( cam->work, cam->CX(xp[q*2]), cam->CY(xp[q*2+1]), color+9-q );
			for ( int q = 4; q > 0; q-- ){
				xp[q*2] = xp[(q-1)*2];
				xp[q*2+1] = xp[(q-1)*2+1];
			}
			xp[0] = (int)(rx+tcos[angle]*BULLET_RAD*1.25);
			xp[1] = (int)(ry+tsine[angle]*BULLET_RAD*1.25);
		}
	}

int xp[ 5 * 2 ];

};

class Graviton:public Bullet{
public:

	Graviton( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 0, 0, 0.7, snd_weapon_5 ){
			spread = c*4/13;
			power = c*6;
			life = 1;
		}

	virtual void Draw( Camera * cam ){
		int tx[3], ty[3];
		tx[0] = rx;
		ty[0] = ry;
		tx[1] = (int)(rx+tcos[(angle+spread)%360]*power);
		ty[1] = (int)(ry+tsine[(angle+spread)%360]*power);
		tx[2] = (int)(rx+tcos[(angle+360-spread)%360]*power);
		ty[2] = (int)(ry+tsine[(angle+360-spread)%360]*power);
		triangle( cam->work, cam->CX(tx[0]), cam->CY(ty[0]), 
			cam->CX(tx[1]), cam->CY(ty[1]), cam->CX(tx[2]),
				cam->CY(ty[2]), BASECOLOR+4 );
	}

	virtual bool interact( SpaceShip * sh ){
		if ( sh == NULL )
			return false;

		#define polish 8
		int qx[polish+1], qy[polish+1];
		for ( int q = 0; q < polish; q++ ){
			qx[q] = (int)(sh->rx+tcos[q*90]*sh->radius/2);
			qy[q] = (int)(sh->ry+tcos[q*90]*sh->radius/2);
		}
		qx[polish] = sh->rx;
		qy[polish] = sh->ry;
		int tx[3], ty[3];
		tx[0] = rx;
		ty[0] = ry;
		tx[1] = (int)(rx+tcos[(angle+spread)%360]*power);
		ty[1] = (int)(ry+tsine[(angle+spread)%360]*power);
		tx[2] = (int)(rx+tcos[(angle+360-spread)%360]*power);
		ty[2] = (int)(ry+tsine[(angle+360-spread)%360]*power);
		for ( int q = 0; q < polish+1; q++ )
			if ( point_in_triangle( tx[0],ty[0],tx[1],ty[1],tx[2],ty[2],qx[q],qy[q] ) ){
				sh->fx += tcos[angle]*6;
				sh->fy += tsine[angle]*6;
				sh->rx = (int)sh->fx;
				sh->ry = (int)sh->fy;
				if ( sh->shield > 0 )
					sh->shield-= strength;
				else
					sh->life -= strength;
				make_small( qx[q], qy[q] );
				return true; 
			}

		return false;
	}

	virtual bool Move( SpaceShip ** slist, int lx, int ly ){
		bool cy = false;
		for ( int q = 0; q < MAX_SHIP; q++ )
			if ( slist[q] != NULL )
				if ( interact( slist[q] ) ){
					cy = true;
					play_sample( snd[snd_explode],220,128,1000,false ); 
				}
		life--;
		if ( life <= 0 )
			cy = true;
		return cy;
	}

int spread;
int power;

};

class GravitonGreen:public Bullet{
public:

	GravitonGreen( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 0, 0, 1.1, snd_weapon_5 ){
			spread = c*4/13;
			power = c*6;
			life = 1;
		}

	virtual void Draw( Camera * cam ){
		int tx[3], ty[3];
		tx[0] = rx;
		ty[0] = ry;
		tx[1] = (int)(rx+tcos[(angle+spread)%360]*power);
		ty[1] = (int)(ry+tsine[(angle+spread)%360]*power);
		tx[2] = (int)(rx+tcos[(angle+360-spread)%360]*power);
		ty[2] = (int)(ry+tsine[(angle+360-spread)%360]*power);
		triangle( cam->work, cam->CX(tx[0]), cam->CY(ty[0]), 
			cam->CX(tx[1]), cam->CY(ty[1]), cam->CX(tx[2]),
				cam->CY(ty[2]), BASECOLOR+4 );
	}

	virtual bool interact( SpaceShip * sh ){
		if ( sh == NULL )
			return false;

		#define polish 8
		int qx[polish+1], qy[polish+1];
		for ( int q = 0; q < polish; q++ ){
			qx[q] = (int)(sh->rx+tcos[q*90]*sh->radius/2);
			qy[q] = (int)(sh->ry+tcos[q*90]*sh->radius/2);
		}
		qx[polish] = sh->rx;
		qy[polish] = sh->ry;
		int tx[3], ty[3];
		tx[0] = rx;
		ty[0] = ry;
		tx[1] = (int)(rx+tcos[(angle+spread)%360]*power);
		ty[1] = (int)(ry+tsine[(angle+spread)%360]*power);
		tx[2] = (int)(rx+tcos[(angle+360-spread)%360]*power);
		ty[2] = (int)(ry+tsine[(angle+360-spread)%360]*power);
		for ( int q = 0; q < polish+1; q++ )
			if ( point_in_triangle( tx[0],ty[0],tx[1],ty[1],tx[2],ty[2],qx[q],qy[q] ) ){
				sh->fx += tcos[angle]*6;
				sh->fy += tsine[angle]*6;
				sh->rx = (int)sh->fx;
				sh->ry = (int)sh->fy;
				if ( sh->shield > 0 )
					sh->shield-= strength;
				else
					sh->life -= strength;
				make_small( qx[q], qy[q] );
				return true; 
			}

		return false;
	}

	virtual bool Move( SpaceShip ** slist, int lx, int ly ){
		bool cy = false;
		for ( int q = 0; q < MAX_SHIP; q++ )
			if ( slist[q] != NULL )
				if ( interact( slist[q] ) ){
					cy = true;
					play_sample( snd[snd_explode],220,128,1000,false ); 
				}
		life--;
		if ( life <= 0 )
			cy = true;
		return cy;
	}

int spread;
int power;

};

class GravitonUltra:public Bullet{
public:

	GravitonUltra( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 0, 0, 1.6, snd_weapon_5 ){
			spread = c*4/13;
			power = c*6;
			life = 1;
		}

	virtual void Draw( Camera * cam ){
		int tx[3], ty[3];
		tx[0] = rx;
		ty[0] = ry;
		tx[1] = (int)(rx+tcos[(angle+spread)%360]*power);
		ty[1] = (int)(ry+tsine[(angle+spread)%360]*power);
		tx[2] = (int)(rx+tcos[(angle+360-spread)%360]*power);
		ty[2] = (int)(ry+tsine[(angle+360-spread)%360]*power);
		triangle( cam->work, cam->CX(tx[0]), cam->CY(ty[0]), 
			cam->CX(tx[1]), cam->CY(ty[1]), cam->CX(tx[2]),
				cam->CY(ty[2]), BASECOLOR*2+4 );
	}

	virtual bool interact( SpaceShip * sh ){
		if ( sh == NULL )
			return false;

		#define polish 8
		int qx[polish+1], qy[polish+1];
		for ( int q = 0; q < polish; q++ ){
			qx[q] = (int)(sh->rx+tcos[q*90]*sh->radius/2);
			qy[q] = (int)(sh->ry+tcos[q*90]*sh->radius/2);
		}
		qx[polish] = sh->rx;
		qy[polish] = sh->ry;
		int tx[3], ty[3];
		tx[0] = rx;
		ty[0] = ry;
		tx[1] = (int)(rx+tcos[(angle+spread)%360]*power);
		ty[1] = (int)(ry+tsine[(angle+spread)%360]*power);
		tx[2] = (int)(rx+tcos[(angle+360-spread)%360]*power);
		ty[2] = (int)(ry+tsine[(angle+360-spread)%360]*power);
		for ( int q = 0; q < polish+1; q++ )
			if ( point_in_triangle( tx[0],ty[0],tx[1],ty[1],tx[2],ty[2],qx[q],qy[q] ) ){
				sh->fx += tcos[angle]*6;
				sh->fy += tsine[angle]*6;
				sh->rx = (int)sh->fx;
				sh->ry = (int)sh->fy;
				if ( sh->shield > 0 )
					sh->shield-= strength;
				else
					sh->life -= strength;
				make_small( qx[q], qy[q] );
				return true; 
			}

		return false;
	}

	virtual bool Move( SpaceShip ** slist, int lx, int ly ){
		bool cy = false;
		for ( int q = 0; q < MAX_SHIP; q++ )
			if ( slist[q] != NULL )
				if ( interact( slist[q] ) ){
					cy = true;
					play_sample( snd[snd_explode],220,128,1000,false ); 
				}
		life--;
		if ( life <= 0 )
			cy = true;
		return cy;
	}

int spread;
int power;

};

class HomingOne: public Bullet{
public:

	HomingOne( int x, int y, int ang, int c ):
		Bullet( x, y, ang, c, 8.6, 5, 3, snd_weapon_4 ){
			Home = NULL;
			attention = 0;
		}

	virtual void Draw( Camera * cam ){

		int x1 = cam->CX( rx );
		int y1 = cam->CY( ry );
		int x2 = (int)(x1+tcos[(angle+120)%360]*BULLET_RAD);
		int y2 = (int)(y1+tsine[(angle+120)%360]*BULLET_RAD);
		int x3 = (int)(x1+tcos[(angle+240)%360]*BULLET_RAD);
		int y3 = (int)(y1+tsine[(angle+240)%360]*BULLET_RAD);
		triangle( cam->work, x1, y1, x2, y2, x3, y3, color+9 );

	}

	virtual bool interact( SpaceShip * sh ){
		if ( sh == NULL )
			return false;

		if ( Home == NULL && abs(gang(rx,ry,sh->rx,sh->ry)-angle) < 15 )
			Home = sh;

		if ( dist( sh->rx, sh->ry, rx, ry ) <= BULLET_RAD + sh->radius+2*(sh->shield>0) ){
			sh->fx += tcos[angle]*speed*2;
			sh->fy += tsine[angle]*speed*2;
			sh->rx = (int)sh->fx;
			sh->ry = (int)sh->fy;
			sh->life -= strength;
			if ( sh->energy > 0 )
				sh->energy--;
			make_small( rx, ry );
			return true;
		}
		return false;
	}

	virtual bool Move( SpaceShip ** slist, int lx, int ly ){

		if ( Home != NULL && !attention ){
			DiffAngle( angle, gang( rx, ry, Home->rx, Home->ry ) );
			angle+=360;
			angle%=360;
			attention = 10;
		}
		if ( attention > 0 )
			attention--;

		fx += tcos[angle]*speed;
		fy += tsine[angle]*speed;
		rx = (int)fx;
		ry = (int)fy;
		if ( rx < 0 ){
			rx = lx;
			fx = rx;
		}
		if ( rx > lx ){
			rx = 0;
			fx = rx;
		}
		if ( ry < 0 ){
			ry = ly;
			fy = ry;
		}
		if ( ry > ly ){
			ry = 0;
			fy = ry;
		}
		for ( int q = 0; q < MAX_SHIP; q++ )
			if ( slist[q] != NULL )
				if ( interact( slist[q] ) ){
					play_sample( snd[snd_explode],220,128,1000,false ); 
					return true;
				}
		life--;
		if ( life <= 0 ) return true;
		return false;
	} 

SpaceShip * Home;
int attention;

};

class LargeShip: public SpaceShip{
public:

	LargeShip( int x, int y, int c ):
		SpaceShip( x, y, c, 6.2, 30, 100, 6, 4, 70 ){}

	virtual void Draw( Camera * cam ){

		#define zanzabar { \
			double wx = sx; \
			double wy = sy; \
			for ( int q = 0; q < 5; q++ ){ \
				circlefill( cam->work, cam->CX( (int)wx ), cam->CY( (int)wy ), 4, color+9 ); \
				wx += tcos[ang]*5; \
				wy += tsine[ang]*5; \
			} \
		}

		if ( cam->Visible( rx, ry ) ){
			int zang = (ang+90)%360;
			double sx = rx+tcos[(zang+180)%360]*(radius-2);
			double sy = ry+tsine[(zang+180)%360]*(radius-2);
			zanzabar
			for ( int q = 0; q < radius/2-1; q++ ){
				circlefill( cam->work, cam->CX(sx), cam->CY(sy), radius/8, color+9 );
				sx += tcos[zang]*radius/8;
				sy += tsine[zang]*radius/8;
			}
			zanzabar
			circle( cam->work, cam->CX(rx), cam->CY(ry), (int)(radius/FOCUS), color+9 );
		}

	}

	virtual void ShootFirst( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < SHOT_WAIT; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new LargeBullet( (int)(fx+tcos[(ang+45)%360]*(radius+12) ),
				(int)(fy+tsine[(ang+45)%360]*(radius+12) ),
				ang-2, color );
			energy -= SHOT_ENERGY;
		}
	}

	virtual void ShootSecond( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < SHOT_WAIT; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new LargeBullet( (int)(fx+tcos[(ang+315)%360]*(radius+12) ),
				(int)(fy+tsine[(ang+315)%360]*(radius+12) ),
				ang+2, color );
			energy -= SHOT_ENERGY;
		}
	}

	virtual void ShootThree( Bullet **& blist ){
		ShootFirst( blist );
		ShootSecond( blist );
	}

};

class QuickShip: public SpaceShip{
public:

	QuickShip( int x, int y, int c ):
		SpaceShip( x, y, c, 9.3, 25, 50, 3, 9, 50 ){}

	virtual void Draw( Camera * cam ){
		if ( !cam->Visible( rx, ry ) )
			return;

		int x1, y1, x2, y2;
		x1 = (int)(rx+tcos[ang]*radius/2);
		y1 = (int)(ry+tsine[ang]*radius/2);
		x2 = (int)(rx+tcos[(ang+180)%360]*radius/2);
		y2 = (int)(ry+tsine[(ang+180)%360]*radius/2);
		line( cam->work, cam->CX( x1 ), cam->CY( y1 ),
				cam->CX( x2 ), cam->CY( y2 ), color+9 );
		x2 = (int)(rx+tcos[(ang+180)%360]*radius/4);
		y2 = (int)(ry+tsine[(ang+180)%360]*radius/4);
		fixed ang1, ang2;
		ang1 = itofix( (ang+90)%360 * 256 / 360 );
		ang2 = itofix( (ang+270)%360 * 256 / 360 );
		arc( cam->work, cam->CX(x2), cam->CY(y2), ang2, ang1, (int)(radius/FOCUS), color+9 );
	}

	virtual void ShootFirst( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < SHOT_WAIT; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new LaserBullet( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color );
			energy -= SHOT_ENERGY;
		}
	}

	virtual void ShootSecond( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*2 ){
			for ( int q = 0; q < SHOT_WAIT*2; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new TrainBullet( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color );
			energy -= SHOT_ENERGY*2;
			shot_counter[0] += SHOT_WAIT;
		}
	}

	virtual void ShootThree( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*4 ){
			for ( int q = 0; q < SHOT_WAIT*3; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new MassiveBullet( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color );
			energy -= SHOT_ENERGY*4;
			shot_counter[0] += SHOT_WAIT*2;
		}
	}

};

class SpikeShip: public SpaceShip{
public:

	SpikeShip( int x, int y, int c ):
		SpaceShip( x, y, c, 5.6, 30, 50, 5, 5, 40 ){
			sm_ang = 0;
		}

	virtual void Draw( Camera * cam ){
		if ( !cam->Visible( rx, ry ) )
			return;
		sm_ang = (sm_ang+1)%360;
		circlefill( cam->work, cam->CX( rx ), cam->CY( ry ), (int)(radius*2/3/FOCUS), color+9 );
		for ( int tang = 0; tang <= 360; tang += 72 ){
			int x1 = cam->CX( (int)(rx+tcos[(tang+sm_ang)%360]*radius/FOCUS) );
			int y1 = cam->CY( (int)(ry+tsine[(tang+sm_ang)%360]*radius/FOCUS) );
			int x2 = cam->CX( (int)(rx+tcos[(tang+sm_ang+72)%360]*radius/FOCUS) );
			int y2 = cam->CY( (int)(ry+tsine[(tang+sm_ang+72)%360]*radius/FOCUS) );
			int x3 = cam->CX( (int)(rx+tcos[(tang+sm_ang+36)%360]*radius*10/3/FOCUS) );
			int y3 = cam->CY( (int)(ry+tsine[(tang+sm_ang+36)%360]*radius*10/3/FOCUS) );
			triangle( cam->work, x1, y1, x2, y2, x3, y3, color + 7 );
		}
	}

	virtual void ShootFirst( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < SHOT_WAIT; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new LaserBullet( (int)(fx+tcos[shoot_ang]*radius*2), (int)(fy+tsine[shoot_ang]*radius*2), shoot_ang, color );
			energy -= SHOT_ENERGY;
		}
	}

	virtual void ShootSecond( Bullet **& blist ){
		ShootFirst( blist );
	}

	virtual void ShootThree( Bullet **& blist ){
		ShootFirst( blist );
	}

	virtual void Move( int lx, int ly, Bullet **& blist, SpaceShip * slist ){
		if ( shot_counter[0] )
			shot_counter[0]--;
		accelerate();
		if ( dist( rx, ry, destx, desty ) < 10 ){
			destx = random() % lx;
			desty = random() % ly;
			want_ang = gang( rx, ry, destx, desty );
		}
		if ( random() % 20 == random() % 20 )
			want_ang = gang( rx, ry, destx, desty );
		DiffAngle( ang, want_ang ); 
		if ( ang < 0 )
			ang += 360;
		ang %= 360;

		if ( random() % 50 == 0 ) 
			if ( !shot_counter[0] ){
				shoot_ang = gang( rx, ry, slist->rx, slist->ry );
				ShootFirst( blist );
				shot_counter[0] = SHOT_WAIT;
			}

		realmove( lx, ly );

	}

int shoot_ang;
int sm_ang;

};

class AngryShip: public SpaceShip{
public:

	AngryShip( int x, int y, int c ):
		SpaceShip( x, y, c, 5.7, 25, 50, 4, 4, 30 ){
			my_life = life;
			circle_color = 0;
		}

	virtual void Draw( Camera * cam ){
		if ( !cam->Visible( rx, ry ) )
			return;

		for ( int q = radius; q > 0; q-- ){
			int c = color+(q+circle_color)%10;
			circlefill( cam->work, cam->CX(rx), cam->CY(ry), (int)(q/FOCUS), c );
		}
		circle_color++;

	}

	virtual void ShootFirst( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < SHOT_WAIT; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new LargeBullet( (int)(fx+tcos[shoot_ang]*radius*2), (int)(fy+tsine[shoot_ang]*radius*2), shoot_ang, color );
			energy -= SHOT_ENERGY;
		}
	}

	virtual void ShootSecond( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			energy -= SHOT_ENERGY;
			int start = random() % 360;
			for ( int z = 0; z < 360; z+=19 ){
				int q = 0;
				while ( q < MAX_BULLET && blist[q] != NULL )
					q++;
				if ( q >= MAX_BULLET ) return;
				blist[q] = new LargeBullet( (int)(fx+tcos[(z+start)%360]*radius*3/2),
					(int)(fy+tsine[(z+start)%360]*radius*3/2 ), z, color );

			}
		}
	}

	virtual void Move( int lx, int ly, Bullet **& blist, SpaceShip * slist ){
		if ( shot_counter[0] )
			shot_counter[0]--;
		accelerate();
		if ( dist( rx, ry, destx, desty ) < 10 ){
			destx = random() % lx;
			desty = random() % ly;
			want_ang = gang( rx, ry, destx, desty );
		}
		if ( random() % 20 == random() % 20 )
			want_ang = gang( rx, ry, destx, desty );
		DiffAngle( ang, want_ang ); 
		if ( ang < 0 )
			ang += 360;
		ang %= 360;

		if ( random() % 200 == 0 ) 
			if ( !shot_counter[0] ){
				shoot_ang = gang( rx, ry, slist->rx, slist->ry );
				ShootFirst( blist );
				shot_counter[0] = SHOT_WAIT;
			}
		if ( fabs(life-my_life)>5 ){
			my_life = life;
			ShootSecond( blist );
		}

		realmove( lx, ly );

	}

int shoot_ang;
double my_life;
int circle_color;

};

class LizardShip: public SpaceShip{
public:

	LizardShip( int x, int y, int c ):
		SpaceShip( x, y, c, 8.1, 25, 30, 4, 4, 30 ){}

	virtual void Draw( Camera * cam ){
		if ( !cam->Visible( rx, ry ) )
			return;

		int x1, y1, x2, y2, x3, y3, x4, y4;
		x1 = (int)(rx+tcos[(ang+90)%360]*radius/2);
		y1 = (int)(ry+tsine[(ang+90)%360]*radius/2);

		x2 = (int)(x1+tcos[(ang+180)%360]*radius);
		y2 = (int)(y1+tsine[(ang+180)%360]*radius);

		x3 = (int)(x2+tcos[(ang+270)%360]*radius);
		y3 = (int)(y2+tsine[(ang+270)%360]*radius);

		x4 = (int)(x3+tcos[ang]*radius);
		y4 = (int)(y3+tsine[ang]*radius); 

		line( cam->work, cam->CX( x1 ), cam->CY( y1 ),
				cam->CX( x2 ), cam->CY( y2 ), color+9 );
		line( cam->work, cam->CX( x2 ), cam->CY( y2 ),
				cam->CX( x3 ), cam->CY( y3 ), color+9 );
		line( cam->work, cam->CX( x3 ), cam->CY( y3 ),
				cam->CX( x4 ), cam->CY( y4 ), color+9 );

		x1 = (int)(rx+tcos[ang]*radius/2);
		y1 = (int)(ry+tsine[ang]*radius/2);
		fixed ang1, ang2;
		ang2 = itofix( (ang+90)%360 * 256 / 360 );
		ang1 = itofix( (ang+270)%360 * 256 / 360 );
		arc( cam->work, cam->CX(x1), cam->CY(y1), ang2, ang1, (int)(radius/FOCUS/2), color+9 );
		circlefill( cam->work, cam->CX(x2), cam->CY(y2), 2, color+9 );
		circlefill( cam->work, cam->CX(x3), cam->CY(y3), 2, color+9 );
	}

	virtual void ShootFirst( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < SHOT_WAIT; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new SmallBullet( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color );
			energy -= SHOT_ENERGY;
		}
	}

	virtual void ShootSecond( Bullet **& blist ){
		ShootFirst( blist );
	}

	virtual void ShootThree( Bullet **& blist ){
		ShootFirst( blist );
	}

};

class DeltaShip: public SpaceShip{
public:

	DeltaShip( int x, int y, int c ):
		SpaceShip( x, y, c, 9.0, 30, 100, 4, 2, 60 ){

			/*
			RedProc[0] = &ShootRedOne;
			RedProc[1] = &ShootRedTwo;
			RedProc[2] = &ShootRedThree;
			RedProc[3] = &ShootRedFour;
			RedProc[4] = &ShootRedFive;
			RedProc[5] = &ShootRedSix;

			BlueProc[0] = &ShootBlueOne;
			BlueProc[1] = &ShootBlueTwo;
			*/

			kind = RED;
			power = 1;

		}

	virtual void Draw( Camera * cam ){
		if ( !cam->Visible( rx, ry ) )
			return;

		if ( shield > 0 )
			circle( cam->work, cam->CX(rx), cam->CY(ry), radius+2, color+1+( (int)(shield*8/100) ) );

		int x1 = (int)(rx+tcos[ang]*radius*5/2/2);
		int y1 = (int)(ry+tsine[ang]*radius*5/2/2); 

		int x2 = (int)(x1+tcos[(ang+180+20)%360]*radius*5/2);
		int y2 = (int)(y1+tsine[(ang+180+20)%360]*radius*5/2);
		int x3 = (int)(x1+tcos[(ang+180-20)%360]*radius*5/2);
		int y3 = (int)(y1+tsine[(ang+180-20)%360]*radius*5/2);
		int x4 = (int)(rx+tcos[(ang+180)%360]*radius*5/2/4);
		int y4 = (int)(ry+tsine[(ang+180)%360]*radius*5/2/4);

		int c = color+2;
		for ( int q = 0; q < radius; q += 5 ){ 
			triangle( cam->work, cam->CX(x1), cam->CY(y1), 
				cam->CX(x2), cam->CY(y2), 
				cam->CX(x3), cam->CY(y3), c );

			x2 = (int)(x1+tcos[(ang+180+20)%360]*(radius-q)*5/2);
			y2 = (int)(y1+tsine[(ang+180+20)%360]*(radius-q)*5/2);
			x3 = (int)(x1+tcos[(ang+180-20)%360]*(radius-q)*5/2);
			y3 = (int)(y1+tsine[(ang+180-20)%360]*(radius-q)*5/2);

			c++;
			if ( c > color+9 )
				c = color+9;
		}

		x2 = (int)(x1+tcos[(ang+180+20)%360]*radius*5/2);
		y2 = (int)(y1+tsine[(ang+180+20)%360]*radius*5/2);
		x3 = (int)(x1+tcos[(ang+180-20)%360]*radius*5/2);
		y3 = (int)(y1+tsine[(ang+180-20)%360]*radius*5/2);
		x4 = (int)(rx+tcos[(ang+180)%360]*radius*5/2/4);
		y4 = (int)(ry+tsine[(ang+180)%360]*radius*5/2/4);

		triangle( cam->work, cam->CX(x2), cam->CY(y2), 
				cam->CX(x3), cam->CY(y3), 
				cam->CX(x4), cam->CY(y4), 0 );
	}

	void ShootRedOne( Bullet **& blist ){
		if ( energy > SHOT_ENERGY+1 ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, 31-9 );
			energy -= SHOT_ENERGY+1;
		}
	}

	void ShootRedTwo( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*3/2 ){
			for ( int q = 0; q < 2; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[(ang+12)%360]*radius*2), (int)(fy+tsine[(ang+12)%360]*radius*2), ang, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[(ang+360-12)%360]*radius*2), (int)(fy+tsine[(ang+360-12)%360]*radius*2), ang, 31-9 );
			energy -= SHOT_ENERGY*3/2;
		}
	}

	void ShootRedThree( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*5/2 ){
			for ( int q = 0; q < 3; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[(ang+16)%360]*radius*2), (int)(fy+tsine[(ang+16)%360]*radius*2), ang, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[(ang+360-16)%360]*radius*2), (int)(fy+tsine[(ang+360-16)%360]*radius*2), ang, 31-9 );
			energy -= SHOT_ENERGY*5/2;
		}
	}

	void ShootRedFour( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*3/2 ){
			for ( int q = 0; q < 2; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[(ang+25)%360]*radius*2), (int)(fy+tsine[(ang+25)%360]*radius*2), (ang+1)%360, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[(ang+360-25)%360]*radius*2), (int)(fy+tsine[(ang+360-25)%360]*radius*2), (ang+359)%360, 31-9 );
			energy -= SHOT_ENERGY*3/2;
		}
		if ( energy > SHOT_ENERGY && shot_counter[1]==0){
			for ( int q = 0; q < 2; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 31-9 );
			shot_counter[1] = 3;
			energy -= SHOT_ENERGY;
		}
	}

	void ShootRedFive( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*3 ){
			for ( int q = 0; q < 5; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[(ang+35)%360]*radius*2), (int)(fy+tsine[(ang+35)%360]*radius*2), (ang+3)%360, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[(ang+360-35)%360]*radius*2), (int)(fy+tsine[(ang+360-35)%360]*radius*2), (ang+360-3)%360, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 31-9 );
			energy -= SHOT_ENERGY*3;
			shot_counter[0] += 2;
		}
	}

	void ShootRedSix( Bullet **& blist ){
		if ( energy >= SHOT_ENERGY+1 ){
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new CoolBullet( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, 31-9 );
			energy -= SHOT_ENERGY+1;
		}
		if ( energy > SHOT_ENERGY*5/2 && shot_counter[1]==0 ){
			for ( int q = 0; q < 5; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[(ang+50)%360]*radius*2), (int)(fy+tsine[(ang+50)%360]*radius*2), (ang+2)%360, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[(ang+360-50)%360]*radius*2), (int)(fy+tsine[(ang+360-50)%360]*radius*2), (ang+360-2)%360, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[(ang+360-15)%360]*radius*2), (int)(fy+tsine[(ang+360-15)%360]*radius*2), ang, 31-9 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new FireBullet( (int)(fx+tcos[(ang+15)%360]*radius*2), (int)(fy+tsine[(ang+15)%360]*radius*2), ang, 31-9 );
			energy -= SHOT_ENERGY*5/2;
			shot_counter[1] += 3;
		}
	}

	void ShootBlueOne( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Graviton( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 30 );
			energy -= SHOT_ENERGY;
			shot_counter[0] = 0;
		}
	}

	void ShootBlueTwo( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Graviton( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 40 );
			energy -= SHOT_ENERGY;
			shot_counter[0] = 0;
		}
	}

	void ShootBlueThree( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Graviton( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 60 );
			energy -= SHOT_ENERGY;
			shot_counter[0] = 0;
		}
	}

	void ShootBlueFour( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new GravitonGreen( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 60 );
			energy -= SHOT_ENERGY;
			shot_counter[0] = 0;
		}
	}

	void ShootBlueFive( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new GravitonGreen( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 75 );
			energy -= SHOT_ENERGY;
			shot_counter[0] = 0;
		}
	}

	void ShootBlueSix( Bullet **& blist ){
		if ( energy > SHOT_ENERGY ){
			for ( int q = 0; q < 1; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new GravitonUltra( (int)(fx+tcos[ang]*radius*5/2), (int)(fy+tsine[ang]*radius*5/2), ang, 75 );
			energy -= SHOT_ENERGY;
			shot_counter[0] = 0;
		}
	}

	void ShootGreenOne( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*4 ){
			for ( int q = 0; q < 4; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color, 80, 60, 90 );
			energy -= SHOT_ENERGY*4;
			shot_counter[0] += SHOT_WAIT*8;
		}
	}

	void ShootGreenTwo( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*4 ){
			for ( int q = 0; q < 4; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color, 100, 60, 95 );
			energy -= SHOT_ENERGY*4;
			shot_counter[0] += SHOT_WAIT*8;
		}
	}

	void ShootGreenThree( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*4 ){
			for ( int q = 0; q < 4; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color, 120, 60, 105 );
			energy -= SHOT_ENERGY*4;
			shot_counter[0] += SHOT_WAIT*8;
		}
	}

	void ShootGreenFour( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*7 ){
			for ( int q = 0; q < 5; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), (ang+15)%360, color, 110, 60, 90 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), (ang+360-15)%360, color, 110, 60, 90 );
			energy -= SHOT_ENERGY*7;
			shot_counter[0] += SHOT_WAIT*9;
		}
	}

	void ShootGreenFive( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*9 ){
			for ( int q = 0; q < 6; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), (ang+18)%360, color, 110, 60, 90 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), (ang+360-18)%360, color, 110, 60, 90 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color, 130, 65, 95 );
			energy -= SHOT_ENERGY*9;
			shot_counter[0] += SHOT_WAIT*9;
		}
	}

	void ShootGreenSix( Bullet **& blist ){
		if ( energy > SHOT_ENERGY*10 ){
			for ( int q = 0; q < 6; q++ )
				deccelerate();
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), (ang+17)%360, color, 140, 60, 110 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), (ang+360-17)%360, color, 140, 60, 110 );
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Gernade( (int)(fx+tcos[ang]*radius*2), (int)(fy+tsine[ang]*radius*2), ang, color, 150, 65, 130 );
			energy -= SHOT_ENERGY*10;
			shot_counter[0] += SHOT_WAIT*9;
		}
	}

	virtual void ShootFirst( Bullet **& blist ){
		int z = kind;

		switch( z ){
		case RED        :       {

				switch( power ){
				case 1  :       ShootRedOne( blist );break;
				case 2  :       ShootRedTwo( blist );break;
				case 3  :       ShootRedThree( blist );break;
				case 4  :       ShootRedFour( blist );break;
				case 5  :       ShootRedFive( blist );break;
				case 6  :       ShootRedSix( blist );break;
				}
				break;
			} //RED 
		case BLUE       :       {
				switch( power ){
				case 1  :       ShootBlueOne( blist );break;
				case 2  :       ShootBlueTwo( blist );break;
				case 3  :       ShootBlueThree( blist );break;
				case 4  :       ShootBlueFour( blist );break;
				case 5  :       ShootBlueFive( blist );break;
				case 6  :       ShootBlueSix( blist );break;
				break;
				}
				break;
			} 
		case GREEN      :       {
				switch( power ){
				case 1  :       ShootGreenOne( blist );break;
				case 2  :       ShootGreenTwo( blist );break;
				case 3  :       ShootGreenThree( blist );break;
				case 4  :       ShootGreenFour( blist );break;
				case 5  :       ShootGreenFive( blist );break;
				case 6  :       ShootGreenSix( blist );break;
				}
				break;
			}
		} //switch(z)

		/*
		switch( z ){
		RED     :       RedProc[power]( blist );break;
		BLUE    :       BlueProc[power]( blist );break;
		}
		*/

	}

	virtual void ShootSecond( Bullet **& blist ){
		if ( energy >= MAX_ENERGY*3/4 && life > 5+1 ){
			for ( int q = 0; q < SHOT_WAIT*6; q++ )
				deccelerate();
			int zang = ang;
			zang = ( zang + 15 ) % 360;
			for ( int z = 0; z < 3; z++ ){
				int q = 0;
				while ( q < MAX_BULLET && blist[q] != NULL )
					q++;
				if ( q >= MAX_BULLET ) return;
				blist[q] = new MegaBullet( (int)(fx+tcos[zang]*radius*2), (int)(fy+tsine[zang]*radius*2), zang, color );
				zang = ( zang + 360 - 15 ) % 360;
			} // for ( int z )
			energy -= MAX_ENERGY*3/4;
			shot_counter[0] += SHOT_WAIT*8;
			life-=5;
		}
	}

	virtual void ShootThree( Bullet **& blist ){
		if ( energy > 1 ){
			int q = 0;
			while ( q < MAX_BULLET && blist[q] != NULL )
				q++;
			if ( q >= MAX_BULLET ) return;
			blist[q] = new Fast( (int)(fx+tcos[(ang+180)%360]*radius), (int)(fy+tsine[(ang+180)%360]*radius), ang, color );
			energy -= 1;
			shot_counter[0] = 2;
		}
	}
/*
funcptr RedProc[ 6 ];
funcptr BlueProc[ 6 ];
funcptr GreenProc[ 6 ];
*/

};

class PowerUp{
public:

	PowerUp( PowerKind kind, int zx, int zy ):
		power( kind ),
		x( zx ),
		y( zy ),
		life( 170 ),
		power_rad( 10 ){
			int q = power;
			switch( q ){
			case RED        :       c = 40;break;
			case BLUE       :       c = 32;break;
			case GREEN      :       c = 49;break;
			case HEALTH     :       c = 31;break;
			case SHIELD     :       c = 57;break;
			}
		}


	void Draw( Camera * cam ){
		circlefill( cam->work, cam->CX(x),cam->CY(y), power_rad, c );
	}

	bool Interact( SpaceShip * slist ){
		if ( dist( slist->rx, slist->ry, x, y ) < power_rad+slist->radius ){
			if ( slist->kind == power ){
				if ( slist->power < 6 )
					slist->power++;
				else if ( slist->life < slist->MAX_LIFE )
					slist->life += 1;
			} else {
				if ( power == HEALTH ){
					slist->life+=20;
					if ( slist->life > slist->MAX_LIFE )
						slist->life = slist->MAX_LIFE;
					return true;
				}
				if ( power == SHIELD ){
					slist->shield += 20;
					if ( slist->shield > 100 )
						slist->shield = 100;
					return true;
				}
				slist->kind = power;
				slist->power = 1;
			}
			return true;
		}
		life--;
		if ( life <= 0 )
			return true;
		return false;
	}

PowerKind power;
int x, y;
int life;
int c;
int power_rad;

};

class Human{
public:

	Human( SpaceShip * who, PowerKind my_kind, int my_power ){
		mine = who;
		mine->power = my_power;
		if ( mine->power < 1 )
			mine->power = 1;
		mine->kind = my_kind;
	}

	virtual void Move( int lx, int ly, Bullet **& blist ){ 

		if ( key[ KEY_UP ] || key[ KEY_W ] )
			mine->accelerate();
		if ( key[ KEY_DOWN ] || key[ KEY_X ] )
			mine->deccelerate();
		if ( key[ KEY_LEFT] )
			mine->ang+=3;
		if ( key[ KEY_RIGHT] )
			mine->ang-=3;
		int mx,my;
		get_mouse_mickeys( &mx, &my );
		mine->ang += -mx;
		mine->ang += 360;
		mine->ang %= 360;
		if ( key[ KEY_A ] )
			mine->power = random() % 6+1;
		if ( key[ KEY_LCONTROL ] || mouse_b&1 )
			if ( !mine->shot_counter[0] ){
				mine->shot_counter[0] = mine->SHOT_WAIT;
				mine->ShootFirst( blist );
			} 
		if ( key[ KEY_LSHIFT ] )
			if ( !mine->shot_counter[0] ){
				mine->shot_counter[0] = mine->SHOT_WAIT;
				mine->ShootSecond( blist );
			}
		if ( key[ KEY_ALT ] )
			if ( !mine->shot_counter[0] ){
				mine->shot_counter[0] = mine->SHOT_WAIT;
				mine->ShootThree( blist );;
			} 

		for ( int q = 0; q < 3; q++ )
			if ( mine->shot_counter[q] )
				mine->shot_counter[q]--;
		mine->realmove( lx, ly );

	}

SpaceShip * mine;

};

void set_colors(){
#define reset { change.r = 0; change.b = 0; change.g = 0; b+=10; }

	RGB change;
	int b = BASECOLOR-10;
	reset
	for ( int q = b; q < b+10; q++ ){
		change.r+=6;
		set_color(q, &change ); 
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.b+=6;
		change.g+=1;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.g+=6;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.g+=6;
		change.r+=6;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.g+=6;
		change.b+=6;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.r+=6;
		change.b+=3;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.r+=5;
		change.g+=2;
		change.b+=1;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.r+=2;
		change.g+=4;
		change.b+=2;
		set_color(q, &change ); 
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.r+=1;
		change.g+=4;
		change.b+=4;
		set_color(q, &change );
	}
	reset
	for ( int q = b; q < b+10; q++ ){
		change.r+=2;
		change.g+=5;
		change.b+=1;
		set_color(q, &change );
	}

}

void init(){

	#define dot printf(".");
	allegro_init();
	printf("Loading game.");
	install_sound( DIGI_AUTODETECT, MIDI_NONE, "" );
	snd[ snd_death_1 ]        = load_sample( "death.wav" );dot
	snd[ snd_death_2 ]        = load_sample( "death2.wav" );dot
	snd[ snd_weapon_1 ]       = load_sample( "weapon1.wav" );dot
	snd[ snd_weapon_2 ]       = load_sample( "weapon2.wav" );dot
	snd[ snd_weapon_3 ]       = load_sample( "weapon3.wav" );dot
	snd[ snd_weapon_4 ]       = load_sample( "lf3.wav" );dot
	snd[ snd_explode ]        = load_sample( "qexpl.wav");dot
	snd[ snd_weapon_5 ]       = load_sample( "grav.wav");dot
	printf("\n");

	snd[ snd_death_1 ]->priority = 255;
	snd[ snd_death_2 ]->priority = 255;
	snd[ snd_weapon_1 ]->priority = 250;
	snd[ snd_weapon_2 ]->priority = 250;
	snd[ snd_weapon_3 ]->priority = 252;
	snd[ snd_weapon_4 ]->priority = 250;
	snd[ snd_explode ]->priority = 248;
	snd[ snd_weapon_5 ]->priority = 246;

	set_trig();
	srandom( time( NULL ) );
	install_timer();
	install_keyboard();
	install_mouse();
	set_color_depth( 8 );

	set_gfx_mode( GFX_AUTODETECT_WINDOWED, screen_x, screen_y, screen_x, screen_y );
	set_colors();
	explosion_init();
        printf("Init complete\n");
}

END_OF_FUNCTION( init );

void GetShip( SpaceShip *& Slist, Camera * cam, int c ){

	int q = random() % 5;
	switch( q ){
	case 0  :       Slist = new LargeShip( random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR+c*10 );break;
	case 1  :       Slist = new QuickShip( random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR+c*10 );break;
	case 2  :       Slist = new LizardShip(random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR+c*10 );break;
	case 3  :       Slist = new SpikeShip( random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR+c*10 );break;
	case 4  :       Slist = new AngryShip( random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR+c*10 );break;
	}
}

void YIELD(){
	struct timeval tv;
        tv.tv_sec = 0;  
        tv.tv_usec = 1; 
        select( 0, NULL, NULL, NULL, &tv );
}

void inc_speed_counter(){
	speed_counter++;
}
END_OF_FUNCTION( inc_speed_counter );

int main(){

	init();

	speed_counter = 0;
	LOCK_VARIABLE( speed_counter );
	LOCK_FUNCTION( (void *)inc_speed_counter );
	install_int_ex( inc_speed_counter, MSEC_TO_TIMER( GAME_SPEED ) );

	Camera * cam = new Camera( 0, 0, 3500, 3500 );
	SpaceShip ** Slist = new SpaceShip*[ MAX_SHIP ];
	PowerUp ** Plist = new PowerUp*[ MAX_POWER ];
	for ( int q = 0; q < MAX_POWER; q++ )
		Plist[q] = NULL;
	Slist[0] = new DeltaShip( random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR );
	for ( int q = 1; q < MAX_SHIP; q++ )
		GetShip( Slist[q], cam, q );
	Human * player = new Human( Slist[0], RED, 1 );
	Bullet ** Blist = new Bullet*[ MAX_BULLET ];
	for ( int q = 0; q < MAX_BULLET; q++ )
		Blist[q] = NULL;
	int clock = 0;
        printf("Main loop running\n");
        speed_counter = 0;
	while ( !key[KEY_ESC] ){

		bool dirty = false;
		if ( speed_counter == 0 ) YIELD();
                while ( speed_counter > 0 ){
                    dirty = true;	
                    clock++;
                    if ( clock > 100 )
                        clock = 0;

                    for ( int q = 0; q < MAX_BULLET; q++ ){
                        for ( int z = 0; z < BULLET_MOVE; z++ )
                            if ( Blist[q] != NULL )
                                if ( Blist[q]->Move( Slist, cam->MAX_X, cam->MAX_Y ) ){
                                    delete Blist[q];
                                    Blist[q] = NULL;
                                }
                    }

                    player->Move( cam->MAX_X, cam->MAX_Y, Blist );
                    for ( int q = 1; q < MAX_SHIP; q++ )
                        if ( Slist[q] != NULL )
                            Slist[q]->Move( cam->MAX_X, cam->MAX_Y, Blist, player->mine );


                    cam->MoveXY( player->mine->rx, player->mine->ry );

                    if ( player->mine->life <= 0 ){
                        int p = player->mine->power;
                        PowerKind k = player->mine->kind;
                        delete player;
                        delete Slist[0];
                        Slist[0] = new DeltaShip( random() % cam->MAX_X, random()% cam->MAX_Y, BASECOLOR );
                        player = new Human( Slist[0], k, p-1 );
                    }

                    for ( int q = 1; q < MAX_SHIP; q++ )
                        if ( Slist[q] != NULL )
                            if ( Slist[q]->life <= 0 ){
                                make_sane( Slist[q]->rx, Slist[q]->ry );
                                if ( random() % 2 == random() % 2 ){
                                    int z = 0;
                                    while ( z < MAX_POWER && Plist[z] != NULL )z++;
                                    if ( z < MAX_POWER ){
                                        int tr = random()%5;
                                        PowerKind aa;
                                        switch( tr ){
                                            case 0  :       aa = RED;break;
                                            case 1  :       aa = BLUE;break;
                                            case 2  :       aa = HEALTH;break;
                                            case 3  :       aa = GREEN;break;
                                            case 4  :       aa = SHIELD;break;
                                            default :       aa = HEALTH;break;
                                        }
                                        Plist[z] = new PowerUp( aa, Slist[q]->rx, Slist[q]->ry );
                                    }
                                }
                                delete Slist[q];
                                Slist[q] = NULL;
                            }


                    speed_counter--;

                    for ( int q = 0; q < MAX_POWER; q++ )
                        if ( Plist[q] != NULL )
                            if ( Plist[q]->Interact( player->mine ) ){
                                delete Plist[q];
                                Plist[q] = NULL;
                            }


                }

                if ( dirty ){
                    cam->StarDraw();
                    draw_expl( cam );

                    for ( int q = 0; q < MAX_BULLET; q++ )
                        if( Blist[q] != NULL )
                            Blist[q]->Draw( cam );
                    for ( int q = 0; q < MAX_SHIP; q++ ){
                        if ( Slist[q] != NULL ){
                            if ( clock % 2 == 0 )
                                Slist[q]->Energize();
                            Slist[q]->Draw( cam );
                        }
                    }
                    for ( int q = 0; q < MAX_POWER; q++ )
                        if ( Plist[q] != NULL ) Plist[q]->Draw(cam);

                    player->mine->ShowEnergy( cam );

                    for ( int q = 1; q < MAX_SHIP; q++ )
                        if ( Slist[q] == NULL )
                            if ( random() % 1600 == random() % 1600 )
                                GetShip( Slist[q], cam, q );

                    int total = 0;
                    for ( int q = 0; q < max_expl; q++ )
                        if ( expl_[q].alive )
                            total++;
                    textprintf( cam->work, font, 1, 1, 31, "Explosions:%d", total );
                    cam->Draw();
                }
	}
	
	for ( int q = 0; q < max_sample; q++ )
		destroy_sample( snd[q] );
	delete cam;
	for ( int q = 0; q < MAX_SHIP; q++ )
		if ( Slist[q] != NULL )
			delete Slist[q];
	delete[] Slist;
	for ( int q = 0; q < MAX_BULLET; q++ )
		if ( Blist[q] != NULL )
			delete Blist[q];
	delete[] Blist;
	for ( int q = 0; q < MAX_POWER; q++ )
		if ( Plist[q] != NULL )
			delete Plist[q];
	delete[] Plist;

	return 1;

}
END_OF_MAIN();
