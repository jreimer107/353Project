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
#include <stdio.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "driver_defines.h"
#include "gpio_port.h"
#include "serial_debug.h"
#include "validate.h"
#include "i2c.h"
#include "MCP23017.h"
#include "ece353_hw2_fonts.h"
#include "music.h"

#define WAVE_SIZE 3

#define MCP PF0
#define PWM PF2
#define RESET SW1_M

#define DOWN_THRESHOLD	0x400
#define UP_THRESHOLD	0xC00
#define RIGHT_THRESHOLD	0x400
#define LEFT_THRESHOLD	0xC00

#define SPAWN_DELAY 10
#define WAVE_DELAY 50


#define NUM_SIDES 		4
#define SPAWN_SIZE 		40
#define NUM_X_SPOTS 	COLS / SPAWN_SIZE
#define NUM_Y_SPOTS		ROWS / SPAWN_SIZE
#define FIRST_X 		SPAWN_SIZE / 2
#define LAST_X			FIRST_X + (NUM_X_SPOTS - 1) * SPAWN_SIZE
#define FIRST_Y			SPAWN_SIZE / 2
#define EXTRA_Y			SPAWN_SIZE / (NUM_Y_SPOTS + 1)
#define LAST_Y			FIRST_Y + (NUM_Y_SPOTS - 1) * SPAWN_SIZE

#define TOP		1
#define BOTTOM	3
#define LEFT	2
#define RIGHT	4

#define BUTTON_M		0xFF
#define UP_BUTTON		0x01
#define DOWN_BUTTON		0x02
#define LEFT_BUTTON		0x04
#define RIGHT_BUTTON	0x08

#define HIGHSCORE_ADDR 	256
#define ADC_M			0x0FFF
#define GAME_OVER		0xFF



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
void debounce_reset(void);
bool fire_on_press(void);
void draw(void);
void spawn(void);
void update_game(uint8_t killed);

#endif
