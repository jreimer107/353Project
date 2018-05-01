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
#include <stdbool.h>

#include "TM4C123.h"
#include "driver_defines.h"
#include "gpio_port.h"
#include "serial_debug.h"
#include "validate.h"
#include "i2c.h"
#include "MCP23017.h"

#define TICKS 62500
#define LED_CYCLE 20

#define WAVE1 3
#define WAVE2 6
#define WAVE3 10
#define WAVE4 15
#define WAVE5 22
#define WAVE6 30
#define WAVE7 40 
#define WAVE8 55
#define WAVE9 75
#define WAVE10	100

#define NUM_SIDES		4
#define SPAWN_SIZE 		40
#define SPAWN_RADIUS 	SPAWN_SIZE / 2
#define NUM_SPAWNS_V	COLS / SPAWN_SIZE
#define NUM_SPAWNS_H	ROWS / SPAWN_SIZE
#define FIRST_H			SPAWN_SIZE / 2
#define FIRST_V			SPAWN_SIZE / 2
#define LAST_H			SPAWN_RADIUS + NUM_SPAWNS_H * SPAWN_SIZE
#define LAST_V			SPAWN_RADIUS + NUM_SPAWNS_V * SPAWN_SIZE			

#define DOWN_THRESHOLD	0x400
#define UP_THRESHOLD	0xC00
#define RIGHT_THRESHOLD	0x400
#define LEFT_THRESHOLD	0xC00

#define ADC_M			0x0FFF

#define BUTTONS_M 		0xFF
#define BUTTONS_UP 		0x02	//0xFE
#define BUTTONS_DOWN 	0x04	//0xFD
#define BUTTONS_LEFT 	0x04	//0xFB
#define BUTTONS_RIGHT	0x08	//0xF7



#define SPAWN_DELAY 10
#define WAVE_DELAY 50



extern void initialize_serial_debug(void);

///////////////////////////////
// Declare any custom types //
/////////////////////////////


//////////////////////////////
// Function Prototype Next //
////////////////////////////
void update_red_led(void);
void update_green_led(void);
void update_hero_dir(void);
void debounce_buttons(void);
bool fire_on_press(void);
void draw(void);
void spawn(void);
void update_game(uint8_t killed);


#endif
