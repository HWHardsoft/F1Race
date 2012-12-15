/*
 *  Happy easter for Uzebox 
 *  Version 1.0
 *  Copyright (C) 2012  Hartmut Wendt
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "kernel/uzebox.h"
#include "data/F1Race_Tiles7.pic.inc"
//#include "data/fonts.pic.inc"
//#include "data/TitleTiles.pic.inc"
//#include "data/happy_easter_sprites.pic.inc"
//#include "data/patches.inc"
//#include "data/Happy_Easter_tracks.inc"


//#define _FadingX




/* global definitons */
// program modes
enum {
	PM_Intro,		// program mode intro
	PM_Gameplay,	// program mode game play
	PM_Credits,		// credits screen
	PM_Help,		// help screen
	PM_HoF_view,	// show hall of fame
	PM_HoF_edit		// edit name in hall of fame

};


/*
// Road definitions
// upper line vertical values; lower line horizontal values
const char *road_left_darkgreen[] = {
	//00			//01			//02			//03			//04
	Road_dark,		Left_dark_01,	Left_dark_02,	Left_dark_03,	Left_dark_04,
	Left_dark_10,	Left_dark_11,	Left_dark_12,	Left_dark_13,	Left_dark_14,
	Left_dark_20,	Left_dark_21,	Left_dark_22,	Left_dark_23,	Left_dark_24,
	Left_dark_30,	Left_dark_31,	Left_dark_32,	Left_dark_33,	Left_dark_34,
	Left_dark_40,	Left_dark_41,	Left_dark_42,	Left_dark_43,	Grass_dark														
};


// upper line vertical values; lower line horizontal values
const char *road_right_darkgreen[] = {
	//00			//01			//02			//03			//04
	Road_dark,		Right_dark_01,	Right_dark_02,	Right_dark_03,	Right_dark_04,
	Right_dark_10,	Right_dark_11,	Right_dark_12,	Right_dark_13,	Right_dark_14,
	Right_dark_20,	Right_dark_21,	Right_dark_22,	Right_dark_23,	Right_dark_24,
	Right_dark_30,	Right_dark_31,	Right_dark_32,	Right_dark_33,	Right_dark_34,
	Right_dark_40,	Right_dark_41,	Right_dark_42,	Right_dark_43,	Grass_dark														
};

// upper line vertical values; lower line horizontal values
const char *road_left_lightgreen[] = {
	//00			//01			//02			//03			//04
	Road_light,		Left_light_01,	Left_light_02,	Left_light_03,	Left_light_04,
	Left_light_10,	Left_light_11,	Left_light_12,	Left_light_13,	Left_light_14,
	Left_light_20,	Left_light_21,	Left_light_22,	Left_light_23,	Left_light_24,
	Left_light_30,	Left_light_31,	Left_light_32,	Left_light_33,	Left_light_34,
	Left_light_40,	Left_light_41,	Left_light_42,	Left_light_43,	Grass_light														
};


// upper line vertical values; lower line horizontal values
const char *road_right_lightgreen[] = {
	//00			//01			//02			//03			//04
	Road_light,		Right_light_01,	Right_light_02,	Right_light_03,	Right_light_04,
	Right_light_10,	Right_light_11,	Right_light_12,	Right_light_13,	Right_light_14,
	Right_light_20,	Right_light_21,	Right_light_22,	Right_light_23,	Right_light_24,
	Right_light_30,	Right_light_31,	Right_light_32,	Right_light_33,	Right_light_34,
	Right_light_40,	Right_light_41,	Right_light_42,	Right_light_43,	Grass_light														
};
*/
const char track[] = {0,1,2,3,4,4,3,2,1,0,0,-1,-2,-3,-4,-4,-5,-4,-3,-2,-1,0,0,0};

// 8-bit, 255 period LFSR (for generating pseudo-random numbers)
#define PRNG_NEXT() (prng = ((u8)((prng>>1) | ((prng^(prng>>2)^(prng>>3)^(prng>>4))<<7))))
#define MAX(x,y) ((x)>(y) ? (x) : (y))


#define lightgreen 0
#define darkgreen 1
#define widthstep 1

struct EepromBlockStruct ebs;



u8 prng; // Pseudo-random number generator

u8 program_mode;	// program mode (intro, 1 player mode, 2 player mode, ....

bool BMusic_on = true;
u16 iHighscore;
u8 Controller_status2;
u8 autorepeat_cnt;

u8 PosY;
u8 PosX;



/*** function prototypes *****************************************************************/
void init(void);
void set_PM_mode(u8 mode);
void msg_window(u8 x1, u8 y1, u8 x2, u8 y2);
u8 set_def_EEPROM(void);
void load_def_EEPROM(void);
void save_def_EEPROM(void);
u8 GetTile(u8 x, u8 y);
void draw_road_line(char road_left_up, char road_right_up, char road_left_dwn, char road_right_dwn, u8 XOffset, u8 road_y, u8 color);
void copy_buf(unsigned char *BUFA, unsigned char *BUFB, unsigned char ucANZ);
void fill_buf(u8 *BUFA, u8 content, u8 ucANZ);
int get_highscore(u8 entry);
u8 check_highscore(void);
void copy_highsore(u8 entry_from, u8 entry_to);
void clear_highsore(u8 entry);
u8 set_def_highscore(void);
u8 view_highscore_entry(u8 x, u8 y, u8 entry, u8 load_data);
void edit_highscore_entry(u8 entry, u8 cursor_pos, u8 b_mode);
void show_highscore_char(u8 entry, u8 position, u8 cursor_on);
void draw_road(u8 ani_road, u8 XOffset);


void init(void)
// init program
{
  // init tile table
  // SetTileTable(BGTiles);
  // init font table
  SetFontTilesIndex(BGTILES_SIZE);
  // init Sprites
  //SetSpritesTileTable(spriteTiles);	

  // init music	
  //InitMusicPlayer(patches);
  // load into screen
  set_PM_mode(PM_Gameplay);
  
  // init random generator

  Controller_status2 = DetectControllers() & 0x0C;	

  //Use block 24
  /*
  ebs.id = 24;
  if (!isEepromFormatted())
     return;

  if (EEPROM_ERROR_BLOCK_NOT_FOUND == EepromReadBlock(24,&ebs))
  {
	set_def_highscore();
  }	
  */ 		

}



int main(){
int ibuttons=0,ibuttons_old;
u8 uc1, uc2=0;

  // init program
  init();        
  // proceed game	
  while(1)
  {
    WaitVsync(3);	  
    // get controller state
    ibuttons_old = ibuttons;
	ibuttons = ReadJoypad(0);
    switch(program_mode)
	{
	  // proceed intro mode	
	  case PM_Intro:
	
        break;

	  // proceed game_play	
	  case PM_Gameplay:
    
	   break;


	}


  }
  

} 


void set_PM_mode(u8 mode) {
// set parameters, tiles, background etc for choosed program mode
u8 uc1, uc2;

	#ifdef _FadingX
	 FadeOut(1, true);
	#endif
			
	switch (mode)
	{

	  case	PM_Intro:

		break;

	  
	  case	PM_Gameplay:
		StopSong();
		   		
  		// init tile table
  		screenSections[0].tileTableAdress=BGTiles;


		// blank screen - no music
		StopSong();
		ClearVram();
	
		WaitVsync(2);

		// Draw background
		//Fill(0,0,24,28,0);
		for (uc1 = 0; uc1 < 24; uc1++) DrawMap2(uc1,2,Sky);

		//	DrawMap2(0,0,Left_Dark);
		uc1 = 2;
		//SetTile(1,1,Left_Dark[(int)uc1]);
		//for (uc1 = 2; uc1 < 16; uc1++) SetTile(uc1,1,pgm_read_byte(&Left_Dark[uc1 + 2]));
/*
		// Draw bottom
		for (uc1 = 0; uc1 < 78; uc1++) {
		  draw_road_line(uc1 + 1, 0, uc1, 0, 10, 0);
		  WaitVsync(10);
		  draw_road_line(uc1, 0, uc1, 0, 10, 0);
		  WaitVsync(10);
		  draw_road_line(uc1, 0, uc1 + 1, 0, 10, 0);
		  WaitVsync(10);

        }
*/

//		draw_road_line(10, 42, 15, 40, 10, 0);
//		WaitVsync(60);
		

		for(uc1 ;; uc1++) {
		  /* 	
		  if (uc1 & 0x01) draw_road(uc1,0);
		  else draw_road(uc1 + 150,12);
		  */
		  draw_road(uc1,0);
		  WaitVsync(2);
		  
		  draw_road(uc1 + 150,12);

		  WaitVsync(2);
        } 




		// init variables
		
		iHighscore = 0;
		/*
		SetSpriteVisibility(true);
		if (BMusic_on) {
  			//start sound track
  			SetMasterVolume(130);
  			WaitVsync(10);
  			StartSong(Eastern_game_song);		
        }*/
		break;
		



	}

	#ifdef _FadingX
    FadeIn(1, true);
	#endif

	program_mode = mode;

}




u8 GetTile(u8 x, u8 y)
// get background tile from vram
{

 return (vram[(y * 30) + x] - RAM_TILES_COUNT);

}


/**** R O A D ***************************************************************************/
/*
void draw_road_line(char road_left_up, char road_right_up, char road_left_dwn, char road_right_dwn, u8 road_y, u8 color) {

  u8 u_up;
  u8 u_dwn;
  
  if (road_left_up < 0) road_left_up = 0;
  if (road_left_dwn < 0) road_left_dwn = 0;
  if (road_right_up < 0) road_right_up = 0;
  if (road_right_dwn < 0) road_right_dwn = 0;
  
  for (u8 u1 = 0; u1 < 72; u1 += 3) {

	if (road_left_up > u1) {
       u_up = road_left_up - u1;
	   if (u_up > 3) u_up = 3;

	} else if (road_right_up < u1) {
      u_up = u1 - road_right_up;
	   if (u_up > 3) u_up = 3;

	} else if (road_right_up == 0) {
	   u_up = 3; 
	   
	} else {
	   u_up = 0; 
	   
	}

    
	if (road_left_dwn > u1) {
       u_dwn = road_left_dwn - u1;
	   if (u_dwn > 3) u_dwn = 3;

    } else if (road_right_dwn < u1) {
       u_dwn = u1 - road_right_dwn;
	   if (u_dwn > 3) u_dwn = 3;

	} else if (road_right_dwn == 0) {
	   u_dwn = 3; 

	} else {
	   u_dwn = 0;
	   
    } 

	// draw tile
	if (color == lightgreen) {
	
		if ((road_right_dwn < u1) || (road_right_up < u1)) SetTile(u1 / 3, road_y, pgm_read_byte(&Right_Light[(u_up * 4) + u_dwn + 2]));
    	else SetTile(u1 / 3, road_y, pgm_read_byte(&Left_Light[(u_up * 4) + u_dwn + 2]));

//		if ((road_right_dwn < u1) || (road_right_up < u1)) DrawMap2(u1 / 4, road_y, road_right_lightgreen[(u_up * 5) + u_dwn]);
//    	else DrawMap2(u1 / 4, road_y, road_left_lightgreen[(u_up * 5) + u_dwn]);
    
	}else{

		if ((road_right_dwn < u1) || (road_right_up < u1)) SetTile(u1 / 3, road_y, pgm_read_byte(&Right_Dark[(u_up * 4) + u_dwn + 2]));
    	else SetTile(u1 / 3, road_y, pgm_read_byte(&Left_Dark[(u_up * 4) + u_dwn + 2]));

//		if ((road_right_dwn < u1) || (road_right_up < u1)) DrawMap2(u1 / 4, road_y, road_right_darkgreen[(u_up * 5) + u_dwn]);
//    	else DrawMap2(u1 / 4, road_y, road_left_darkgreen[(u_up * 5) + u_dwn]);	
	
	} 


  }

}
*/


void draw_road_line(char road_left_up, char road_right_up, char road_left_dwn, char road_right_dwn, u8 XOffset, u8 road_y, u8 color) {

  u8 u_up;
  u8 u_dwn;
  
  if (road_left_up < 0) road_left_up = 0;
  if (road_left_dwn < 0) road_left_dwn = 0;
  if (road_right_up < 0) road_right_up = 0;
  if (road_right_dwn < 0) road_right_dwn = 0;
  
  for (u8 u1 = 0; u1 < 60; u1 +=6) {

	if (road_left_up > u1) {
       u_up = road_left_up - u1;
	   if (u_up > 6) u_up = 6;

	} else if (road_right_up < u1) {
      u_up = u1 - road_right_up;
	   if (u_up > 6) u_up = 6;

	} else if (road_right_up == 0) {
	   u_up = 6; 
	   
	} else {
	   u_up = 0; 
	   
	}

    
	if (road_left_dwn > u1) {
       u_dwn = road_left_dwn - u1;
	   if (u_dwn > 6) u_dwn = 6;

    } else if (road_right_dwn < u1) {
       u_dwn = u1 - road_right_dwn;
	   if (u_dwn > 6) u_dwn = 6;

	} else if (road_right_dwn == 0) {
	   u_dwn = 6; 

	} else {
	   u_dwn = 0;
	   
    } 

	// draw tile
	if (color == lightgreen) {
	
		if ((road_right_dwn < u1) || (road_right_up < u1)) SetTile(XOffset + (u1 / 6), road_y, pgm_read_byte(&Right_Light[(u_up * 7) + u_dwn + 2]));
    	else SetTile(XOffset + (u1 / 6), road_y, pgm_read_byte(&Left_Light[(u_up * 7) + u_dwn + 2]));

//		if ((road_right_dwn < u1) || (road_right_up < u1)) DrawMap2(u1 / 4, road_y, road_right_lightgreen[(u_up * 5) + u_dwn]);
//    	else DrawMap2(u1 / 4, road_y, road_left_lightgreen[(u_up * 5) + u_dwn]);
    
	}else{

		if ((road_right_dwn < u1) || (road_right_up < u1)) SetTile(XOffset + (u1 / 6), road_y, pgm_read_byte(&Right_Dark[(u_up * 7) + u_dwn + 2]));
    	else SetTile(XOffset + (u1 / 6), road_y, pgm_read_byte(&Left_Dark[(u_up * 7) + u_dwn + 2]));

//		if ((road_right_dwn < u1) || (road_right_up < u1)) DrawMap2(u1 / 4, road_y, road_right_darkgreen[(u_up * 5) + u_dwn]);
//    	else DrawMap2(u1 / 4, road_y, road_left_darkgreen[(u_up * 5) + u_dwn]);	
	
	} 

  }
}


void draw_road(u8 ani_road, u8 XOffset) {
  u8 roadline;
  u8 halfwidth = 20;
  char roadposX = 27;
  char XL_UP, XL_DWN;
  char XR_UP, XR_DWN;
  char DDX;
  double curve; 

  const u8 gras_color[3][12] = {{1,1,1,0,0,1,1,0,1,0},
  								{0,0,0,1,1,0,0,1,0,1},
  								{0,0,0,1,1,0,0,1,0,1}};

for(roadline = 0; roadline < 10; roadline++) {
    curve = (char)track[((ani_road + roadline) / 10)];
    curve = curve / 10;
	DDX = (char)(roadline * curve);
	XL_DWN = roadposX - halfwidth;
	XR_DWN = roadposX + halfwidth;	
	halfwidth = halfwidth - widthstep;
	roadposX += DDX;	

	XL_UP = roadposX - halfwidth;
	XR_UP = roadposX + halfwidth;
	halfwidth = halfwidth - widthstep;
	roadposX += DDX;

	
	if (ani_road & 0x02) draw_road_line(XL_UP, XR_UP, XL_DWN, XR_DWN, XOffset, 25 - roadline, gras_color[0][roadline]);
	else draw_road_line(XL_UP, XR_UP, XL_DWN, XR_DWN, XOffset, 25 - roadline, gras_color[1][roadline]);

}
 

}






/**** A N I M A T I O N S ***************************************************************/



/**** S T U F F *************************************************************************/


void msg_window(u8 x1, u8 y1, u8 x2, u8 y2) {
// draw a window with frame and black backgound on the screen

    // window backgound
	Fill(x1 + 1, y1 + 1, x2 - x1 - 1, y2 - y1 - 1,401);
	// upper frame
	Fill(x1 + 1, y1, x2 - x1 - 1, 1,411);
	// lower frame
	Fill(x1 + 1, y2, x2 - x1 - 1, 1,410);
	// left frame
	Fill(x1 , y1 + 1, 1, y2 - y1 - 1,409);
	// right frame
	Fill(x2, y1 + 1, 1 , y2 - y1 - 1,409);
	// upper, left corner
	SetTile(x1,y1,405);
	// upper, right corner
	SetTile(x2,y1,406);
	// lower, left corner
	SetTile(x1,y2,407);
	// lower, right corner
	SetTile(x2,y2,408);
}	








/**
copy a buffer into another buffer 
@param source buffer
@param target buffer
@param count of copied bytes
@return none
*/
void copy_buf(unsigned char *BUFA, unsigned char *BUFB, unsigned char ucANZ)
{
 for(;ucANZ>0 ; ucANZ--)
 {
  *(BUFB++) = *(BUFA++);
 }   
}


/**
fill a buffer 
@param target buffer
@param byte to fill
@param count of copied bytes
@return none
*/
void fill_buf(u8 *BUFA, u8 content, u8 ucANZ)
{
 for(;ucANZ>0 ; ucANZ--)
 {
  *(BUFA++) = content;
 }   
}


/*
int get_highscore(u8 entry) {
// get the actual highscore from eeprom
  // check the value for entry	
  if (entry > 2) return(0);
	
   // read the eeprom block
  if (!isEepromFormatted() || EepromReadBlock(24, &ebs))
        return(0);   
  return((ebs.data[(entry * 10)+8] * 256) + ebs.data[(entry * 10)+9]);
}



u8 check_highscore(void) {
// check the actual highsore
u8 a;
int i1;
   // read the eeprom block
  if (!isEepromFormatted() || EepromReadBlock(24, &ebs))
        return(0);   
  for(a=0; a<3; a++) {
    i1 = (ebs.data[(a * 10)+8] * 256) + ebs.data[(a * 10)+9];
    if (iHighscore > i1) return(a + 1);
  }

  // highscore is lower as saved highscores 
  return(0);
}



void copy_highsore(u8 entry_from, u8 entry_to) {
// copy a highscore entry to another slot
u8 a;
   // read the eeprom block
  for(a=0; a<10; a++) {
    ebs.data[(entry_to * 10) + a] = ebs.data[(entry_from * 10) + a];
  } 
}


void clear_highsore(u8 entry) {
// clear the name in actual entry and set the score to highscore
u8 a;
  // clear name 
  for(a=0; a<8; a++) {
    ebs.data[(entry * 10) + a] = 0x20;
  }   
  // set score
  ebs.data[(entry * 10) + 8] = iHighscore / 256;
  ebs.data[(entry * 10) + 9] = iHighscore % 256;
}



u8 set_def_highscore(void) {
// write the default highscore list in the EEPROM
  // entry 1
  ebs.data[0] = 'H';
  ebs.data[1] = 'A';
  ebs.data[2] = 'R';
  ebs.data[3] = 'T';
  ebs.data[4] = 'M';
  ebs.data[5] = 'U';
  ebs.data[6] = 'T';
  ebs.data[7] = ' ';
  ebs.data[8] = 0x07;
  ebs.data[9] = 0xD0;	
  // entry 2
  ebs.data[10] = 'M';
  ebs.data[11] = 'A';
  ebs.data[12] = 'R';
  ebs.data[13] = 'I';
  ebs.data[14] = 'E';
  ebs.data[15] = ' ';
  ebs.data[16] = ' ';
  ebs.data[17] = ' ';
  ebs.data[18] = 0x05;
  ebs.data[19] = 0xDC;	
  // entry 3
  ebs.data[20] = 'L';
  ebs.data[21] = 'U';
  ebs.data[22] = 'K';
  ebs.data[23] = 'A';
  ebs.data[24] = 'S';
  ebs.data[25] = ' ';
  ebs.data[26] = ' ';
  ebs.data[27] = ' ';
  ebs.data[28] = 0x03;
  ebs.data[29] = 0xE8;	
  return(EepromWriteBlock(&ebs));
}


u8 view_highscore_entry(u8 x, u8 y, u8 entry, u8 load_data) {
// shows an entry of the higscore
u8 a,c;

  // read the eeprom block
  if (load_data)
  {
    if (!isEepromFormatted() || EepromReadBlock(24, &ebs))
        return(1);   
  }
  entry--;
  for(a = 0; a < 8;a++) {
	c = ebs.data[a + (entry * 10)];  
	PrintChar(x + a, y, c);  
  }
  PrintInt(x + 13, y, (ebs.data[(entry * 10)+8] * 256) + ebs.data[(entry * 10)+9], true);
  return(0);
}



void edit_highscore_entry(u8 entry, u8 cursor_pos, u8 b_mode) {
// edit and view and char in the name of choosed entry    
entry--;
u8 c = ebs.data[(entry * 10) + cursor_pos];
  // proceed up & down button
  if (b_mode == BTN_UP) {
     c++;
     if (c > 'Z') c = ' '; 
     else if (c == '!') c = 'A';
  }
  if (b_mode == BTN_DOWN) {		 
     c--;      
     if (c == 0x1F) c = 'Z';
	 else if (c < 'A') c = ' ';
  }
  ebs.data[(entry * 10) + cursor_pos] = c;

}


void show_highscore_char(u8 entry, u8 position, u8 cursor_on) {
// shows a char of edited name
u8 c = ebs.data[(entry * 10) + position];
    if (cursor_on) PrintChar(8 + position, (entry * 2) + 12, '_');   // show '_'
    else if (c == ' ') PrintChar(8 + position, (entry * 2) + 12, ' ');	// space
    else PrintChar(8 + position, (entry * 2) + 12, c); 	
	
}

*/




