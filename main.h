// Copyright (c) 2015-16, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "driver_defines.h"
#include "gpio_port.h"
#include "serial_debug.h"
#include "validate.h"

#define TICKS 62500
#define LED_CYCLE 20

#define TOP_BORDER			HERO_HEIGHT/2
#define BOTTOM_BORDER		ROWS - TOP_BORDER
#define LEFT_BORDER			HERO_WIDTH/2
#define RIGHT_BORDER		COLS - LEFT_BORDER - 1

#define DOWN_THRESHOLD	0x400
#define UP_THRESHOLD	0xC00
#define RIGHT_THRESHOLD	0x400
#define LEFT_THRESHOLD	0xC00

#define ZOMBIE 0
#define BAT 1
#define BLOB 2
#define MIMIC 3

//Enemy constants
#define PREFERENCE_MAX 		10
#define PREFERENCE_CUTOFF	PREFERENCE_MAX / 2
#define BLOB_COUNT			10
#define HOP_THRESHOLD		0x0F

extern void initialize_serial_debug(void);

typedef enum {IDLE, RIGHT, LEFT} lr_t;
typedef enum {IDLE, UP, DOWN} ud_t;
typedef enum {IDLE, UP, DOWN, RIGHT, LEFT} dir_t;

///////////////////////////////
// Declare any custom types //
/////////////////////////////
typedef struct {
	uint16_t x_loc;
	uint16_t y_loc;
} hero_t;

struct missle {
	uint16_t x_loc;
	uint16_t y_loc;
	struct missle *nxt;
};

typedef struct enemy_t{
	struct enemy_t *next;
	lr_t lr;
	ud_t ud;
	uint16_t x_loc;
	uint16_t y_loc;
	uint8_t type;
	int8_t health;
	uint8_t count;
	uint8_t height;
	uint8_t width;
} enemy_t;


//////////////////////////////
// Function Prototype Next //
////////////////////////////
void update_red_led(void);
void update_green_led(void);
void fire_missle(void);
void remove_missle(void);
void update_missles(void);
void update_hero(void);
void draw(void);


#endif
