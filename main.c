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

char group[] = "Group??";
char individual_1[] = "John	Reimer";
char individual_2[] = "Luke Richmond";

///////////////////////////
// Global declared next //
/////////////////////////
hero_t hero;

struct missle * m_head = NULL;
struct missle * m_tail = NULL;

enemy_t *enemy_head = NULL;

static uint16_t ps2_x, ps2_y;

volatile bool TimerA_Done = false;
volatile bool TimerB_Done = false; 
volatile bool ADC_Done = false;
TIMER0_Type* gp_timer;
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

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	//Initialize hero location, timer, and adc.
	hero.x_loc = COLS/2;
	hero.y_loc = ROWS/2;
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
			update_missles();
			TimerA_Done = false;
		}
		if (TimerB_Done) {
			update_green_led();
			get_ps2_value(ADC0_BASE);
			update_hero();
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

void update_hero(void) {
	if (ps2_x > LEFT_THRESHOLD && hero.x_loc > LEFT_BORDER) hero.x_loc--;
	else if (ps2_x < RIGHT_THRESHOLD && hero.x_loc < RIGHT_BORDER) hero.x_loc++;
	if (ps2_y > UP_THRESHOLD && hero.y_loc > TOP_BORDER) hero.y_loc--;
	else if (ps2_y < DOWN_THRESHOLD && hero.y_loc < BOTTOM_BORDER) hero.y_loc++;
}


void update_missles(void) {
	struct missle * m_curr = NULL;
	static uint8_t button_count = 0;
	
	//Check for button presses for new missile
	//Button needs to be held for 40ms (4 interrupts) before missile fires
	if (lp_io_read_pin(SW1_BIT)) {
		//Make sure missile only fires once per button press, gets stuck at 5.
		if (button_count <= 4) button_count++;
		//Fire missile
		if (button_count == 4) fire_missle();
	}
	else button_count = 0;
		
	//Update missile positions, remove oldest missile if off screen
	if (m_head->y_loc < MISSLE_HEIGHT/2) remove_missle();
	m_curr = m_head;
	while (m_curr) {
		m_curr->y_loc--;
		m_curr = m_curr->nxt;
	}		
}

//Adds new missle object to tail of linked list
void fire_missle(void) {
	struct missle *newMissle = malloc(sizeof(struct missle));
	newMissle->x_loc = hero.x_loc;
	newMissle->y_loc = hero.y_loc - HERO_HEIGHT/2;
	newMissle->nxt = NULL;
	if (m_head == NULL) m_head = newMissle;
	else m_tail->nxt = newMissle;
	m_tail = newMissle;
}

//Removes oldest missile from head of linked list.
void remove_missle(void) {
	//Draw blank missile bitmap to erase
	lcd_draw_image(
		m_head->x_loc,      // X Pos
		MISSLE_WIDTH,   		// Image Horizontal Width
		MISSLE_HEIGHT/2,    // Y Pos
		MISSLE_HEIGHT,  		// Image Vertical Height
		missleErase,      	// Image
		LCD_COLOR_YELLOW,    // Foreground Color
		LCD_COLOR_BLACK     // Background Color
	);
	m_head = m_head->nxt;
}

void update_enemies(void) {
	enemy_t *curr_enemy;
	while (curr_enemy) {
		if (curr_enemy->type == ZOMBOID) {
			uint8_t direction_preference = rand() % PREFERENCE_MAX;
			if (direction_preference > PREFERENCE_CUTOFF) {
				if (curr_enemy->x_loc > hero.x_loc) curr_enemy->x_loc--;
				else if (curr_enemy->x_loc < hero.x_loc) curr_enemy->x_loc++;
			}
			else {
				if (curr_enemy->y_loc > hero.y_loc) curr_enemy->y_loc--;
				else if (curr_enemy->y_loc < hero.y_loc) curr_enemy->y_loc++;
			}
		}
		else if (curr_enemy->type == BATTI)	 {
			
	}
}





void draw(void) {
	struct missle * m_curr = NULL;
	//Draw hero
	lcd_draw_image(
    hero.x_loc,            // X Pos
    HERO_WIDTH,   					// Image Horizontal Width
    hero.y_loc,            // Y Pos
    HERO_HEIGHT,  					// Image Vertical Height
    heroBitmap,       			// Image
    LCD_COLOR_BLUE,      	// Foreground Color
    LCD_COLOR_BLACK     		// Background Color
  );
		
	//Draw missiles
	m_curr = m_head;
	while(m_curr) {
		lcd_draw_image(
			m_curr->x_loc,      // X Pos
			MISSLE_WIDTH,   		// Image Horizontal Width
			m_curr->y_loc,      // Y Pos
			MISSLE_HEIGHT,  		// Image Vertical Height
			missleBitmap,      	// Image
			LCD_COLOR_YELLOW,    // Foreground Color
			LCD_COLOR_BLACK     // Background Color
		);
		m_curr = m_curr->nxt;
	}
}