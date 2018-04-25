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

#include "main.h"
#include "lcd.h"
#include "timers.h"
#include "ps2.h"
#include "launchpad_io.h"
#include "images.h"
#include "actors.h"

char group[] = "Group??";
char individual_1[] = "John	Reimer";
char individual_2[] = "Luke Richmond";

///////////////////////////
// Global declared next //
/////////////////////////

//struct missle * m_head = NULL;
//struct missle * m_tail = NULL;
extern actor_t* actors;
actor_t *hero;
//enemy_t *enemy_head = NULL;
//actor_t *actor = NULL;

static uint16_t ps2_x, ps2_y;

volatile bool TimerA_Done = false;
volatile bool TimerB_Done = false; 
volatile bool ADC_Done = false;
volatile bool button_press = false;
TIMER0_Type* gp_timer;
GPIOA_Type* portf;
ADC0_Type* myadc;


//*****************************************************************************
//*****************************************************************************
void initialize_hardware(void) {
	initialize_serial_debug();
	
	//// setup lcd GPIO, config the screen, and clear it ////
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	//// setup the timers ////
	gp_timer_config_16(TIMER0_BASE, TIMER_TAMR_TAMR_PERIOD, false, true);
	
	//// setup GPIO for LED drive ////
	lp_io_init();
	lp_io_clear_pin(RED_BIT);
	lp_io_clear_pin(GREEN_BIT);
	
	//// Setup ADC to convert on PS2 joystick using SS2 and interrupts ////
	ps2_initialize_SS2();
}

//Red LED
//Toggle every 20 ISR executions (5Hz)
void TIMER0A_Handler(void) {
	TimerA_Done = true;
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
}

//Green LED
//Prescalar 1
//Toggle everu 20 executions (2.5 Hz)
void TIMER0B_Handler(void) {
	TimerB_Done = true;
	gp_timer->ICR |= TIMER_ICR_TBTOCINT;
}

void ADC0SS2_Handler(void) {
	ADC_Done = true;
  	myadc->ISC = ADC_ISC_IN2;          // Ack the conversion
}

void GPIOF_Handler(void) {
	button_press = true;
	//clear icr of gpiof
	portf->ICR |= GPIO_ICR_GPIO_M;
	//read interrupt flag register ifr of port expander
}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	//Initialize hero location, timer, and adc.
	hero = actors;
	hero->x_loc = COLS/2;
	hero->y_loc = ROWS/2;
	gp_timer = (TIMER0_Type*)TIMER0_BASE;
	myadc = (ADC0_Type*)ADC0_BASE;
  	initialize_hardware();
	gp_timer_start_16(TIMER0_BASE, 7, 15, TICKS, TICKS);

  put_string("\n\r");
  put_string("************************************\n\r");
  put_string("ECE353 - Spring 2018 HW3\n\r  ");
  put_string(group);
  put_string("\n\r     Name:");
  put_string(individual_1);
  put_string("\n\r     Name:");
  put_string(individual_2);
  put_string("\n\r");  
  put_string("************************************\n\r");

	//Main loop
  while(1){
		if (TimerA_Done) {
			update_red_led();
			//update_missles();
			TimerA_Done = false;
		}
		if (TimerB_Done) {
			update_green_led();
			get_ps2_value(ADC0_BASE);
			update_hero_dir();
			TimerB_Done = false;
		}
		if (ADC_Done) {
			//Read new adc values
			ps2_x = myadc->SSFIFO2 & 0x0FFF;
			ps2_y = myadc->SSFIFO2 & 0x0FFF;
			ADC_Done = false;
		}
		
		draw();
  }
}

//FSM that toggles the red led every 5Hz
void update_red_led(void) {
	static uint8_t countA = 0;
	static bool stateA = false;
	if (!countA) {
		if (stateA) {
			lp_io_clear_pin(RED_BIT);
			stateA = false;
		}
		else {
			lp_io_set_pin(RED_BIT);
			stateA = true;
		}
	}
	countA = (countA + 1) % LED_CYCLE;
}

//FSM that toggles the green led every 2.5Hz.
void update_green_led(void) {
	static bool stateB = false;	
	static uint8_t countB = 0;
	if (!countB) {
		if (stateB) {
			lp_io_clear_pin(GREEN_BIT);
			stateB = false;
		}
		else {
			lp_io_set_pin(GREEN_BIT);
			stateB = true;
		}		
	}
	countB = (countB + 1) % LED_CYCLE;	
}

void update_hero_dir(void) {
		if (ps2_x > LEFT_THRESHOLD) {
		hero->lr = LEFT_d;
	}
	else if (ps2_x < RIGHT_THRESHOLD) {
		hero->lr = RIGHT_d;
	}
	else hero->lr = IDLE_lr;
	if (ps2_y > UP_THRESHOLD) {
		 hero->ud = UP;
	}
	else if (ps2_y < DOWN_THRESHOLD) {
		hero->ud = DOWN;
	}
}

void update_move(void) {
	if ();
}

