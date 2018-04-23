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
		MISSLE_WIDTH,   	// Image Horizontal Width
		MISSLE_HEIGHT/2,    // Y Pos
		MISSLE_HEIGHT,  	// Image Vertical Height
		missleErase,      	// Image
		LCD_COLOR_YELLOW,    // Foreground Color
		LCD_COLOR_BLACK     // Background Color
	);
	m_head = m_head->nxt;
}

void update_enemies(void) {
	enemy_t *curr_enemy = enemy_head;
	enemy_t *prev_enemy = NULL;
	while (curr_enemy) {
		//enemy dead
		if (curr_enemy->health <= 0) {
			if (prev_enemy) prev_enemy->next = curr_enemy->next;
			else enemy_head = curr_enemy->next;
			remove_enemy(curr_enemy);
			free(curr_enemy);
			curr_enemy = prev_enemy->next;
			continue;
		}


		//Update enemy position/movement
		//Zombies follow the player, with some sway.
		if (curr_enemy->type == ZOMBIE) { //Zombies have a random direction preference to make them sway towards the player
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

		//Bats bounce around the screen.
		else if (curr_enemy->type == BAT)	 {
			//If given only one direction they will repeatedy go back and forth
			//If given two directions they will bounce around like tv screensavers
			lr_t edge_lr = at_edge_lr(curr_enemy);
			ud_t edge_ud = at_edge_ud(curr_enemy);
			//Detect if at edge of screen, switch movement direction
			if (curr_enemy->lr = LEFT) {
				curr_enemy->x_loc--;
				if (edge_lr == LEFT) curr_enemy->lr = RIGHT;
			}
			else if (curr_enemy->lr = RIGHT) {
				curr_enemy->x_loc++;
				if (edge_lr == RIGHT) curr_enemy->lr = LEFT;
			}

			if (curr_enemy->ud = UP) {
				curr_enemy->y_loc--;
				if (edge_ud == UP) curr_enemy->ud = DOWN;
			}
			else if (curr_enemy->ud = DOWN) {
				curr_enemy->y_loc++;
				if (edge_ud == RIGHT) curr_enemy->ud = UP;
			}
		}

		//Blobs periodically hop around.
		else if (curr_enemy->type == BLOB) {
			//Count reached, switch between moving and idle.
			if (curr_enemy->count == BLOB_COUNT) {
				if (curr_enemy->lr != IDLE || curr_enemy->ud != IDLE) {
					curr_enemy->lr = IDLE;
					curr_enemy->ud == IDLE;
				}
				else {	//We are idle, need to pick a direction to hop;
					uint8_t hop_dir = rand();
					//If we are odd, hop horizontal. Else hop vertical
					if (hop_dir % 2) {
						//If we are greater or equal to threshold, hop right. Else hop left.
						if (hop_dir > HOP_THRESHOLD) curr_enemy->lr = RIGHT;
						else curr_enemy->lr = RIGHT;
					}
					else {
						//If we are greater or equal to threshold, hop down. Else hop up.
						if (hop_dir > HOP_THRESHOLD) curr_enemy->ud = DOWN;
						else curr_enemy->ud = UP;
					}
				}
				curr_enemy->count = 0;
			}
			else curr_enemy->count++;

			//Actually move.
			lr_t edge_lr = at_edge_lr(curr_enemy, BLOB_WIDTH);
			ud_t edge_ud = at_edge_ud(curr_enemy, BLOB_HEIGHT);
			if (curr_enemy->lr == LEFT && edge_lr != LEFT) curr_enemy->x_loc--;
			else if (curr_enemy->lr == RIGHT && edge_lr != RIGHT) curr_enemy->x_loc++;
			else if (curr_enemy->ud == UP && edge_lr != UP) curr_enemy->y_loc--;
			else if (curr_enemy->ud == DOWN && edge_lr != DOWN) curr_enemy->y_loc++;
		}

		//Mimics, surprisingly, mimic the movement of the player, but slower (same speed, faster? variants?)
		else if (curr_enemy->type = MIMIC) {
			lr_t edge_lr = at_edge_lr(curr_enemy, MIMIC_WIDTH);
			ud_t edge_ud = at_edge_ud(curr_enemy, MIMIC_HEIGHT);
			if (ps2_x > LEFT_THRESHOLD && edge_lr != LEFT) curr_enemy->x_loc--;
			else if (ps2_x < RIGHT_THRESHOLD && edge_lr != RIGHT) curr_enemy->x_loc++;
			if (ps2_y > UP_THRESHOLD && edge_ud != UP) curr_enemy->y_loc--;
			else if (ps2_y < DOWN_THRESHOLD && edge_ud != DOWN) curr_enemy->y_loc++;
		}

		//Update next enemy
		prev_enemy = curr_enemy;
		curr_enemy = curr_enemy->next;
	}
}

void remove_enemy(enemy_t enemy) {
	uint8_t width;

	lcd_draw_image(
		enemy->x_loc,	 // X Pos
		MISSLE_WIDTH,	  // Image Horizontal Width
		enemy->y_loc, // Y Pos
		MISSLE_HEIGHT,	 // Image Vertical Height
		missleErase,	   // Image
		LCD_COLOR_YELLOW,  // Foreground Color
		LCD_COLOR_BLACK	// Background Color
	);
}



//Detects if an enemy is at an edge.
//returns LEFT if the enemy is at the left edge of the screen
//returns RIGHT if the enemy is at the right edge of the screen
//else returns IDLE.
lr_t at_edge_lr(enemy_t enemy) {
	if (enemy->x_loc < enemy->width / 2) return LEFT;
	if (enemy->y_loc > COLS - enemy->width / 2) return RIGHT;
	return IDLE;
}

//Detects if an enemy is at an edge.
//returns UP if the enemy is at the top edge of the screen
//returns DOWN if the enemy is at the bottom edge of the screen
//Else returns IDLE.
ud_t at_edge_ud(enemy_t enemy, uint8_t enemy_height) {
	if (enemy->y_loc < enemy->height / 2) return UP;
	if (enemy->y_loc > ROWS - enemy->height / 2) return DOWN;
	return IDLE;
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