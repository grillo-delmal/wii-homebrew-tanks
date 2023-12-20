/*
Copyright (c) 2008, Grillo del Mal

Distributed under the 2-Clause BSD License, see LICENSE file.
*/

#define PI			3.14159265

#define SCREEN_MOVE_X		70
#define SCREEN_MOVE_Y		60
#define HEIGHT			300
#define WIDTH			500

#define BULLET_SPEED		4
#define BULLET_RADIUS		3.5

#define TANK_RADIUS		16
#define TANK_SIZE		32

#define TANK_ACC		0.05
#define TANK_ROT_ANGLE		5*PI/180
#define TANK_MAX_SPEED		3
#define CANON_RELOAD_TIME	120

#define MAX_BULLETS		100

#define ST_PAUSE		0
#define ST_STATUS_MENU		1
#define ST_GAME			2
#define ST_ENDGAME		3

#define FONT_SIZE		1.2

typedef struct{
	double angulo;
	int bullets;
	int reload;
	GXColor color;
} Canon;

typedef struct{
	double px;
	double py;
	double v;
	double angulo;
	int inmune;
	GXColor color;
	Canon c;
} Tank;

typedef struct{
	int active :1;
	Tank t;
	WPADData *wd;
	int score;	
	double cx;
	double cy;
} Player;

typedef struct{
	int owner;
	int del;
	double px;
	double py;
	double angulo;	
} Bullet;

void init_tank(int i);
int colision(double xb,double yb, double xt, double yt, double rb, double rt);


