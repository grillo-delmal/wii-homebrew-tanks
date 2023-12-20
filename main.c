/*
Copyright (c) 2008, Grillo del Mal

Distributed under the 2-Clause BSD License, see LICENSE file.
*/

#include <stdlib.h>
#include <math.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

//Definiciones de estructuras
#include "main.h"

//GRRLIB
#include <GRRLIB.h>

//Font
#include "fonts/font8.h"

//Images
#include "gfx/canon.h"
#include "gfx/tank.h"

Mtx GXmodelView2D;
u8 *tex_font8;
u8 *tex_tank;
u8 *tex_canon;

WPADData wmotes,wnull;
Player players[4];
Bullet bullets[MAX_BULLETS];
int cant_bullets=0;

s32 debug=0;
s32 lang;
s32 pnum=0;

void countevs(int chan, const WPADData *data){}

void reset()
{
	int i;


	//INIALIZACION WIIMOTES
	for(i=0;i<4;i++){

		players[i].active = 0;
		players[i].wd = &wnull;


		players[i].t.color.a	=0xFF ;
		players[i].t.c.color.a	=0xFF ;
		players[i].t.color.r	=0x33 ;
		players[i].t.c.color.r	=0x33 ;
		players[i].t.color.g	=0x33 ;
		players[i].t.c.color.g	=0x33 ;
		players[i].t.color.b	=0x33 ;
		players[i].t.c.color.b	=0x33 ;
	}

	wnull.btns_h=0;
	wnull.btns_l=0;
	wnull.btns_d=0;
	wnull.btns_u=0;

	players[0].t.color.g	=0xBB ;
	players[0].t.c.color.g	=0xBB ;

	players[1].t.color.r	=0xBB ;
	players[1].t.c.color.r	=0xBB ;

	players[2].t.color.b	=0xBB ;
	players[2].t.c.color.b	=0xBB ;

	players[3].t.color.g	=0xBB ;
	players[3].t.c.color.g	=0xBB ;
	players[3].t.color.b	=0xBB ;
	players[3].t.c.color.b	=0xBB ;

	cant_bullets=0;
	pnum=0;

}



void start_up()
{
	int i;

	//Rutinas de inicio
	VIDEO_Init();
	PAD_Init();
	WPAD_Init();

	// Init libfat
	fatInitDefault ();
	
	//inicializacion GRRLIB
	GRRLIB_InitVideo();
	GRRLIB_Start();

	tex_font8=GRRLIB_LoadTexture(font8);
	tex_tank=GRRLIB_LoadTexture(tank);
	tex_canon=GRRLIB_LoadTexture(canon);

	//INIALIZACION WIIMOTES
	for(i=0;i<4;i++){
		WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC_IR); 
		WPAD_SetVRes(i, 640, 480);
	}

	wnull.btns_h=0;
	wnull.btns_l=0;
	wnull.btns_d=0;
	wnull.btns_u=0;

	lang=CONF_GetLanguage();
	reset();
}

void scan_pads(){

	u32 i,res,type;

	PAD_ScanPads ();
	WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
	wmotes = wnull;
	for(i=0;i<4;i++){
		res = WPAD_Probe(i, &type);
		if(res == WPAD_ERR_NONE){
			players[i].wd = WPAD_Data(i);

			wmotes.btns_h|=players[i].wd->btns_h;
			wmotes.btns_l|=players[i].wd->btns_l;
			wmotes.btns_d|=players[i].wd->btns_d;
			wmotes.btns_u|=players[i].wd->btns_u;

		}
		else{
			players[i].wd = &wnull;
		}
	}

	//EXIT GAME
	if(PAD_ButtonsDown(0)&PAD_TRIGGER_Z)
		exit(0);
	if(!((*(u32*)0xCC003000)>>16))
		exit(0);
}

int menu(){
	static int i,pnum=0,pcolorsel[4]={0,0,0,0};

	for(i=0;i<4;i++){
		if(players[i].wd->btns_d & WPAD_BUTTON_A){
			if(!players[i].active){
				players[i].active = 1;
				players[i].score = 0;
				init_tank(i);
				pnum++;		
			}
			else{
				players[i].active = 0;
				players[i].score = 0;		
				pnum--;	
			}
		}

		if(players[i].active){

			if( players[i].wd->btns_d & WPAD_BUTTON_UP ){
				switch(pcolorsel[i]){
					case 0:
						if(players[i].t.color.r < 0xCC) 
							players[i].t.color.r+=0x11;
						break;
					case 1:
						if(players[i].t.color.g < 0xCC) 
							players[i].t.color.g+=0x11;
						break;
					case 2:
						if(players[i].t.color.b < 0xCC) 
							players[i].t.color.b+=0x11;
						break;
					case 3:
						if(players[i].t.c.color.r < 0xCC) 
							players[i].t.c.color.r+=0x11;
						break;
					case 4:
						if(players[i].t.c.color.g < 0xCC) 
							players[i].t.c.color.g+=0x11;
						break;
					case 5:
						if(players[i].t.c.color.b < 0xCC) 
							players[i].t.c.color.b+=0x11;
						break;
				}
			}


			if( players[i].wd->btns_d & WPAD_BUTTON_DOWN ){
				switch(pcolorsel[i]){
					case 0:
						if(players[i].t.color.r > 0x22) 
							players[i].t.color.r-=		0x11;
						break;
					case 1:
						if(players[i].t.color.g > 0x22)
							players[i].t.color.g-=		0x11;
						break;
					case 2:
						if(players[i].t.color.b > 0x22)
							players[i].t.color.b-=		0x11;
						break;
					case 3:
						if(players[i].t.c.color.r > 0x22)
							players[i].t.c.color.r-=	0x11;
						break;
					case 4:
						if(players[i].t.c.color.g > 0x22)
							players[i].t.c.color.g-=	0x11;
						break;
					case 5:
						if(players[i].t.c.color.b > 0x22)
							players[i].t.c.color.b-=	0x11;
						break;
				}
			}

			if( players[i].wd->btns_d & WPAD_BUTTON_LEFT ){
				if(pcolorsel[i]>0){
					pcolorsel[i] = pcolorsel[i]-1;
				}
				else{
					pcolorsel[i] = 5;
				}
			}
			if( players[i].wd->btns_d & WPAD_BUTTON_RIGHT ){
				pcolorsel[i] = (pcolorsel[i]+1)%6;
			}

			
			GRRLIB_Rectangle(
				50 + TANK_SIZE*2*i + font8_char_width*(pcolorsel[i]%3),
				50 + FONT_SIZE*font8_char_high*7.5,
				FONT_SIZE*font8_char_width,FONT_SIZE*font8_char_width,GRRLIB_Splitu32(0xFF0000FF),1);


			if( pcolorsel[i]/3 == 0 ){
				GRRLIB_Printf(
					50 + TANK_SIZE*2*i,
					50 + FONT_SIZE*font8_char_high*7.5,
					tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"%X%X%X",players[i].t.color.r/0x10,players[i].t.color.g/0x10,players[i].t.color.b/0x10);
			}
			else{
				GRRLIB_Printf(
					50 + TANK_SIZE*2*i,
					50 + FONT_SIZE*font8_char_high*7.5,
					tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"%X%X%X",players[i].t.c.color.r/0x10,players[i].t.c.color.g/0x10,players[i].t.c.color.b/0x10);
			}
			GRRLIB_DrawImg(
				50 + TANK_SIZE*2*i, 
				50 + FONT_SIZE*font8_char_high*9, 
				TANK_SIZE, TANK_SIZE, tex_tank,
				0,1,1,
				players[i].t.color);

			GRRLIB_DrawImg(
				50 + TANK_SIZE*2*i, 
				50 + FONT_SIZE*font8_char_high*9, 
				TANK_SIZE, TANK_SIZE, tex_canon ,
				0,1,1,
				players[i].t.c.color);


		}

	}

	if(lang==CONF_LANG_SPANISH){
		GRRLIB_Printf(50,50,		tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a por Grillo");

		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*3,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Presiona + (MAS) para comenzar");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*4,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Para salir al loader presiona la tecla HOME o Start");


		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*6,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			" Jugadores = %c %c %c %c",
						players[0].active ?'1':' ',
						players[1].active ?'2':' ',
						players[2].active ?'3':' ',
						players[3].active ?'4':' ');


		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*13,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"D-pad = Movimiento");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*14,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"    B = Disparar");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*15,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			" Home = Salir");
	}
	else if(lang==CONF_LANG_FRENCH){
		GRRLIB_Printf(50,50,		tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a par Grillo");

		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*2,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Appuyez sur + (Plus) pour commencer.");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*3,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Pour retourner sur le loader,");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*4,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"appuyez sur Home ou sur Start.");


		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*6,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			" Joueurs = %c %c %c %c",
						players[0].active ?'1':' ',
						players[1].active ?'2':' ',
						players[2].active ?'3':' ',
						players[3].active ?'4':' ');


		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*13,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Flèches directionnelles =  Déplacer le tank");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*14,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"                      B = Tirer");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*15,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"                   Home = Retourner sur le loader");
	}

	else
	{
		GRRLIB_Printf(50,50,		tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a by Grillo");

		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*3,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Press + (Plus) to start");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*4,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"To exit to the Loader, press Home Button or Start");


		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*6,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			" Players = %c %c %c %c",
						players[0].active ?'1':' ',
						players[1].active ?'2':' ',
						players[2].active ?'3':' ',
						players[3].active ?'4':' ');


		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*13,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"D-pad = Move the tank");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*14,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"    B = Shoot");
		GRRLIB_Printf(50,50 + FONT_SIZE*font8_char_high*16,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			" Home = Exit to Loader");

	}

	if (( wmotes.btns_d & WPAD_BUTTON_PLUS ) && (pnum >=2)) return ST_GAME;
	if ( wmotes.btns_d & WPAD_BUTTON_HOME ){ exit(0);}

	return ST_STATUS_MENU;
}



/************************************INICIO JUEGO**********************************************/

void init_tank(int i)
{
	players[i].t.v = 0;
	players[i].t.c.bullets = 5;
	players[i].t.c.reload = 0;
	players[i].t.inmune = CANON_RELOAD_TIME*2;
	switch(i){
		case 0:
			players[i].t.px = 50;
			players[i].t.py = 250;
			players[i].t.angulo = -PI/4;
			players[i].t.c.angulo = -PI/4;
			break;
		case 1:
			players[i].t.px = 450;
			players[i].t.py = 250;
			players[i].t.angulo = -3*PI/4;
			players[i].t.c.angulo = -3*PI/4;
			break;
		case 2:
			players[i].t.px = 50;
			players[i].t.py = 50;
			players[i].t.angulo = PI/4;
			players[i].t.c.angulo = PI/4;
			break;
		case 3:
			players[i].t.px = 450;
			players[i].t.py = 50;
			players[i].t.angulo = 3*PI/4;
			players[i].t.c.angulo = 3*PI/4;
			break;
	}

}

int colision(double xb,double yb, double xt, double yt, double rb, double rt){
	double a = xt - xb;
	double b = yt - yb;
	if( (a*a + b*b) < (rb +rt)*(rb +rt)){
		return 1;
	}
	else return 0;
}


void update_p(int i){
	int j;
	double fx,fy;

	if(players[i].wd->exp.type == EXP_NUNCHUK){

		if((players[i].wd->exp.nunchuk.js.mag > 0.3)&&(players[i].wd->exp.nunchuk.js.mag <1.5)){
			double ang = players[i].wd->exp.nunchuk.js.ang;
			if	(ang >=  0 && ang <  90){
				ang= -(90 - ang );
			}
			else if	(ang >= 90){
				ang= -(-ang - 270);
			}
			players[i].t.angulo = ang*PI/180;
	
			players[i].t.v = TANK_MAX_SPEED * players[i].wd->exp.nunchuk.js.mag;
		}
		else{
			players[i].t.v=0;
		}
		////PUNTERO DEL TANQUE
		if(players[i].wd->ir.valid) { 
			players[i].cx = players[i].wd->ir.x - SCREEN_MOVE_X;
			players[i].cy = players[i].wd->ir.y - SCREEN_MOVE_Y;
		}
		////ACTUALIZAR ANGULO CAÑON
		players[i].t.c.angulo = atan2(  (players[i].t.py) - players[i].cy, players[i].cx - (players[i].t.px) );

		////DISPARAR CON B
		if ( (players[i].wd->btns_d & WPAD_BUTTON_B) ){ 
			if(players[i].t.c.bullets){
				if(cant_bullets < MAX_BULLETS){
					//crea la bala
					bullets[cant_bullets].owner = i;
					bullets[cant_bullets].del = 0;
					bullets[cant_bullets].px = players[i].t.px + (TANK_SIZE / 2) + (TANK_RADIUS+BULLET_RADIUS+1) * cos(- players[i].t.c.angulo);
					bullets[cant_bullets].py = players[i].t.py + (TANK_SIZE / 2) + (TANK_RADIUS+BULLET_RADIUS+1) * sin(- players[i].t.c.angulo);
					bullets[cant_bullets].angulo = -players[i].t.c.angulo;	
					cant_bullets++;
			
					players[i].t.c.bullets --;
					players[i].t.c.reload += CANON_RELOAD_TIME;
				}
			}
		}
	}
	else{
		////ARRIBA SUBE LA VELOCIDAD + 1
		if ( (players[i].wd->btns_h & WPAD_BUTTON_UP)  && players[i].t.v <  TANK_MAX_SPEED ) players[i].t.v += TANK_ACC;

		////ABAJO BAJA LA VELOCIDAD -1
		if ( (players[i].wd->btns_h & WPAD_BUTTON_DOWN)&& players[i].t.v > -TANK_MAX_SPEED ) players[i].t.v -= TANK_ACC;

		////IZQUIERDA AUMENTA EL ANGULO - PI/180
		if ( (players[i].wd->btns_h & WPAD_BUTTON_LEFT) ){ 
			if((players[i].t.angulo -= TANK_ROT_ANGLE)< -PI){
				players[i].t.angulo = -players[i].t.angulo;
			}
		}
		////DERECHA AUMENTA EL ANGULO + PI/180
		if ( (players[i].wd->btns_h & WPAD_BUTTON_RIGHT) ){ 
			if((players[i].t.angulo += TANK_ROT_ANGLE)> PI){
				players[i].t.angulo = -players[i].t.angulo;
			}
		}
		////PUNTERO DEL TANQUE
		if(players[i].wd->ir.valid) { 
			players[i].cx = players[i].wd->ir.x - SCREEN_MOVE_X;
			players[i].cy = players[i].wd->ir.y - SCREEN_MOVE_Y;
		}
		////ACTUALIZAR ANGULO CAÑON
		players[i].t.c.angulo = atan2(  (players[i].t.py + (TANK_SIZE / 2)) - players[i].cy, players[i].cx - (players[i].t.px + (TANK_SIZE / 2) ) );

		////DISPARAR CON B
		if ( (players[i].wd->btns_d & WPAD_BUTTON_B) ){ 
			if(players[i].t.c.bullets){
				if(cant_bullets < MAX_BULLETS){
					//crea la bala
					bullets[cant_bullets].owner = i;
					bullets[cant_bullets].del = 0;
					bullets[cant_bullets].px = players[i].t.px + (TANK_SIZE / 2) + (TANK_RADIUS+BULLET_RADIUS+1) * cos(- players[i].t.c.angulo);
					bullets[cant_bullets].py = players[i].t.py + (TANK_SIZE / 2) + (TANK_RADIUS+BULLET_RADIUS+1) * sin(- players[i].t.c.angulo);
					bullets[cant_bullets].angulo = -players[i].t.c.angulo;	
					cant_bullets++;
			
					players[i].t.c.bullets --;
					players[i].t.c.reload += CANON_RELOAD_TIME;
				}
			}
		}
		
	}


	//Counters
	if(players[i].t.c.reload){
		players[i].t.c.reload--;
		if( players[i].t.c.reload % CANON_RELOAD_TIME == 0)
			players[i].t.c.bullets++;
	}
	if(players[i].t.inmune)
		players[i].t.inmune--;

	//actualizar variables
	fx = ( players[i].t.px + players[i].t.v*cos(players[i].t.angulo) ) ;
	fy = ( players[i].t.py + players[i].t.v*sin(players[i].t.angulo) ) ;


	if( ! (fx > 0  &&  fx < WIDTH -TANK_SIZE )){
		fx = players[i].t.px;
		fy = players[i].t.py;
		players[i].t.v = 0;
		
	}

	if( !(fy > 0 &&  fy < HEIGHT -TANK_SIZE )){
		fx = players[i].t.px;
		fy = players[i].t.py;
		players[i].t.v = 0;
		
	}


	for(j=0;j<4;j++){			
		if( players[j].active && j != i && players[i].t.inmune == 0){
			if( colision( players[j].t.px, players[j].t.py, fx, fy, TANK_RADIUS, TANK_RADIUS) ){
				fx = players[i].t.px;
				fy = players[i].t.py;
				players[i].t.v = 0;
			}
		}
	}
	players[i].t.px = fx;
	players[i].t.py = fy;

}


void update_b(){

	int i,j,dflag=0;
	for(j=0;j<cant_bullets;j++){
		////ACTUALIZAR POSICION BALA
		bullets[j].px = ( bullets[j].px + BULLET_SPEED*cos(bullets[j].angulo) ) ;
		bullets[j].py = ( bullets[j].py + BULLET_SPEED*sin(bullets[j].angulo) ) ;
		//CHEQUEAR COLISION CON TANQUE
		for(i=0;i<4;i++){
			if( players[i].active && i != bullets[j].owner && players[i].t.inmune == 0 ){
				if( colision( bullets[j].px, bullets[j].py, players[i].t.px+(TANK_SIZE / 2), players[i].t.py+(TANK_SIZE / 2), BULLET_RADIUS, TANK_RADIUS) ){
					players[bullets[j].owner].score+=1;
					init_tank(i);
					bullets[j].del = 1;
					dflag=1;
				}
			}
		}
		//CHEQUEAR POSICION
		if(bullets[j].px < 0 || bullets[j].px > WIDTH || bullets[j].py < 0 || bullets[j].py > HEIGHT){
			bullets[j].del = 1;
			dflag=1;
		}
	}

	if(dflag){
		for(i=0,j=0;j<cant_bullets;i++,j++){
			if(bullets[j].del){
				i++;			
				cant_bullets--;
			}
			bullets[j]=bullets[i];
		}
	}
}

void print(){
	int i;

	GRRLIB_Rectangle(  SCREEN_MOVE_X,SCREEN_MOVE_Y,WIDTH, HEIGHT, GRRLIB_Splitu32(0xFF0000FF) ,0); 

	//Dibujar JUGADORES
	for(i=0;i<4;i++){
		if(players[i].active){
			if((players[i].t.inmune%15) < 8){
				//TANQUE
				GRRLIB_DrawImg(
					SCREEN_MOVE_X + players[i].t.px, 
					SCREEN_MOVE_Y + players[i].t.py, 
					TANK_SIZE, TANK_SIZE, tex_tank,
					players[i].t.angulo,1,1,
					players[i].t.color);

				GRRLIB_DrawImg(
					SCREEN_MOVE_X + players[i].t.px, 
					SCREEN_MOVE_Y + players[i].t.py, 
					TANK_SIZE, TANK_SIZE, tex_canon ,
					-players[i].t.c.angulo,1,1,
					players[i].t.color);

			}
			//PUNTERO
			GRRLIB_Rectangle(
				SCREEN_MOVE_X + players[i].cx, 
				SCREEN_MOVE_Y + players[i].cy,
				7,7,players[i].t.color,0);
		}
	}
	//Dibujar Balas
	for(i=0;i<cant_bullets; i++){
		GRRLIB_Rectangle(
			SCREEN_MOVE_X + bullets[i].px, 
			SCREEN_MOVE_Y + bullets[i].py,
			5,5,players[bullets[i].owner].t.color,1);
	}

	
	if(lang==CONF_LANG_SPANISH){
		GRRLIB_Printf(25,25,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a por Grillo");
		for(i=0;i<4;i++){
			if(players[i].active){
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*3,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Jugador: %d",i+1);
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*4,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Balas: %d",players[i].t.c.bullets);
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*5,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Puntaje: %d",players[i].score);
			}
	
		}
			
	}
	else if(lang==CONF_LANG_FRENCH){
		GRRLIB_Printf(25,25,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a par Grillo");
		for(i=0;i<4;i++){
			if(players[i].active){
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*3,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Joueur: %d",i+1);
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*4,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Balles: %d",players[i].t.c.bullets);
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*5,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Score:  %d",players[i].score);
			}
	
		}
			
	}
	else{
		GRRLIB_Printf(25,25,	tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),1,
			"TANKS V0.03a by Grillo");
		for(i=0;i<4;i++){
			if(players[i].active){
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*3,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Player %d",i+1);
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*4,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Bullets: %d",players[i].t.c.bullets);
				GRRLIB_Printf(25 + (145)*i,335 + FONT_SIZE*font8_char_high*5,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
					"Score: %d",players[i].score);
			}
	
		}
	}


}

int check_scores()
{
	int i;
	for(i=0;i<4;i++){
		if(players[i].active){
			if(players[i].score >= 10){
				return i;
			}
		}
	}
	return -1;
	
}

int game(){
	int i;
	//ACTUALIZAR JUGADORES
	for(i=0;i<4;i++){
		if(players[i].active){
			update_p(i);
			if(players[i].score >= 10){
				return ST_ENDGAME;
			}
		}
	}
	//ACTUALIZAR BALAS
	update_b();
	//DIBUJAR Cosas ;D
	print();

	if ( wmotes.btns_d & WPAD_BUTTON_HOME ) return ST_PAUSE;
	return ST_GAME;
}

/************************************FIN JUEGO**********************************************/
int pause(){
	print();
	if(lang==CONF_LANG_SPANISH){
		GRRLIB_Rectangle(640/2-245, 480/2-50, 490, 100, GRRLIB_Splitu32(0x880000AA),1);
		GRRLIB_Printf(640/2 -  80 ,480/2 - 20, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),4,"Pausa");
		GRRLIB_Printf(640/2 - 225 ,480/2 + 20, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Presione A para continuar o Home para salir");
		
	}
	else if(lang==CONF_LANG_FRENCH){
		GRRLIB_Rectangle(640/2-200, 480/2-50, 400, 136, GRRLIB_Splitu32(0x880000AA),1);
		GRRLIB_Printf(640/2 -  80 ,480/2 - 20, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),4,"Pause");
		GRRLIB_Printf(640/2 - 169 ,480/2 + 20, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Appuyez sur A pour continuer ou");
		GRRLIB_Printf(640/2 - 118 ,480/2 + 20 + FONT_SIZE*font8_char_high, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"sur Home pour quitter.");
		
	}

	else{
		GRRLIB_Rectangle(640/2-245, 480/2-50, 490, 100, GRRLIB_Splitu32(0x880000AA),1);
		GRRLIB_Printf(640/2 - 80 ,480/2 - 20, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),4,"Pause");
		GRRLIB_Printf(640/2 -195 ,480/2 + 20, tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Press A to continue or Home to quit");
	}

	if ( wmotes.btns_d & WPAD_BUTTON_HOME ){ return ST_ENDGAME; }
	if ( wmotes.btns_d & WPAD_BUTTON_A    ){ return ST_GAME; }

	return ST_PAUSE;
}


int print_end(int j){
	static int flag=0;

	if(lang==CONF_LANG_SPANISH){

		GRRLIB_Printf(50,50,		tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a por Grillo");

		GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*3 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Gano el Jugador %d, presiona Home para salir,",j+1);
		GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*4 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"+ (MAS) para reiniciar o A para ver los creditos.");
		switch(flag){
		case 1:
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*6 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Gracias a todos los que hicieron esto posible:");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*7 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" bushing, marcan y todo el equipo Twiizers");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*8 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" Todos los que han ayudado con los proyectos:");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*9 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"    devkitPro, ogclib, wiiuse, GRRLIB y libwiisprite");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*10,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"y todos los que se me olvido saludar XD");
		break;
		case 2:
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*6,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Cambios de la version");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*7,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Actualizado a GRRLIB 3.0.1.a (algo modificada)");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*8,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Arreglado Bug de coloreo de balas");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*9,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Pintado de tanques mas logico");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*10,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Deteccion de lenguaje de la consola (grax roto)");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*11,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Agregado soporte para Nunchuck");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*13,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" *Si presionas A otra vez pasaras a la version en ingles y luego a la en frances");
		break;
		case 3:
			lang=CONF_LANG_ENGLISH;
			flag=0;
			break;
		default:
		break;
		}
	}
	else if(lang==CONF_LANG_FRENCH){

		GRRLIB_Printf(50,50,		tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a per Grillo");

		GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*3 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Le joueur %d, a gagné. Appuyez sur Home pour quitter,",j+1);
		GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*4 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"sur + pour recommencer ou sur A pour regarder les crédits.");
		switch(flag){
		case 1:
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*6 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Merci à toutes les personnes qui ont rendu cela");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*7 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"possible, tout particulièrement:");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*8 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" Brushing, Marcan et tous ceux de la Team Twiizer");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*9 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" Je remercie également toutes les personnes qui ont");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*10 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" développé les projets suivants:");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*11 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"    devkitPro, ogclib, wiiuse, GRRLIB et libwiisprite");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*12,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"et tout ceux que j'ai oubliés XD !");
		break;
		case 2:
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*6,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Nouveautées de la version 0.3b");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*7,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Mise à jour vers GRRLIB 3.0.1.a");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*8,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Le bug de la coloration des balles est réparé");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*9,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Le bug sur la coloration des tanks est réparé");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*10,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Détection de ces langues (merci Roto)");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*11,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -On peut désormais jouer avec un Nunchuck");

			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*13,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Cette traduction française n'a pas été faite par moi,");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*14,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"puisque je ne parle pas français. S'il y a une erreur");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*15,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"quelconque, ce n'est pas moi qu'il faut blâmer mais");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*16,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"plutôt la personne qui a bien voulu faire la traduction.");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*17,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Si vous avez trouvé une erreur, veuillez contacter");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*18,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"le traducteur à cette adresse: miniskull_@hotmail.com");
		break;
		case 3:
			lang=CONF_LANG_SPANISH;
			flag=0;
			break;
		default:
		break;
		}
	}
	else{

		GRRLIB_Printf(50,50,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"TANKS V0.3.1a by Grillo");

		GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*3 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"Player %d wins, press Home to quit, + to restart",j+1);
		GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*4 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
			"or A to watch the credits.");

		switch(flag){
		case 1:
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*6 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Thanks to everyone who made this possible, specially:");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*7 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" bushing, marcan & all the people from Team Twiizers");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*8 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" All the people who maintain the following projects:");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*9 ,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"    devkitPro, ogclib, wiiuse, GRRLIB & libwiisprite");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*10,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" and everyone who I forgot XD");
		break;
		case 2:
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*6,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"Version 0.03a Changes");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*7,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Updated to GRRLIB 3.0");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*8,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Fixed bullet coloring Bug");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*9,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Fixed Tank coloring");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*10,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Spanish/English language detection (thx roto)");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*11,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" -Added Nunchuck Support");

			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*13,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				"*Next time I'll ask someone to check my english before");
			GRRLIB_Printf(25,50 + FONT_SIZE*font8_char_high*14,tex_font8,font8_char_width,font8_char_high,GRRLIB_Splitu32(0xFFFFFFFF),FONT_SIZE,
				" releasing xD.");
		
		break;
		case 3:
			lang=CONF_LANG_FRENCH;
			flag=0;
			break;
		default:
		break;
		}
	}	

	if ( wmotes.btns_d & WPAD_BUTTON_A ){ flag=(flag+1)%4;}
	if ( wmotes.btns_d & WPAD_BUTTON_HOME ){ exit(0);}
	if ( wmotes.btns_d & WPAD_BUTTON_PLUS ){reset(); return ST_STATUS_MENU;}
	
	
	return ST_ENDGAME;

}


int update_game(int status){
	static int old_status;
	int ret=status;

	switch(status){
		case ST_PAUSE:
			ret=pause();
			if(ret==ST_PAUSE)
				return ST_PAUSE;
			else if( ret < 0 )
				status=old_status;
			else
				status = ret;
		break;

		case ST_STATUS_MENU:
			ret = menu();
		break;

		case ST_GAME:
			ret = game();
		break;

		case ST_ENDGAME:
			ret = print_end(check_scores());
		break;
	}

	old_status = status;
	return ret;
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	int status = ST_STATUS_MENU;

	start_up();

	GRRLIB_FillScreen(GRRLIB_Splitu32(0xFF000000));
	GRRLIB_Render();

	while(1==1) {
		//SCAN CONTROLERS
		scan_pads();

		//UPDATE GAME
		GRRLIB_FillScreen(GRRLIB_Splitu32(0xFF000000));
		status = update_game(status);
		GRRLIB_Render();
		//VIDEO_WaitVSync();
	}
	return 0;
}


