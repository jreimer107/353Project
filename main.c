#include "main.h"
#include "actors.h"
#include "eeprom.h"
#include "images.h"
#include "launchpad_io.h"
#include "lcd.h"
#include "ps2.h"
#include "timers.h"

char group[] = "Group23";
char individual_1[] = "John	Reimer";
char individual_2[] = "Luke Richmond";

///////////////////////////
// Global declared next //
/////////////////////////

extern actor_t* actors;
actor_t* hero;

uint16_t ps2_x, ps2_y;

//Flags set by interrupt handlers
volatile bool GameTimer_Done = false;
volatile bool ADC_Done = false;
volatile uint8_t poll_button = 0;
TIMER0_Type* game_timer;
TIMER0_Type* sound_timer;
GPIOA_Type* portf;
ADC0_Type* myadc;

uint8_t buttons_current = 0;
bool buttons_pressed[4] = {false, false, false, false};
bool tear_fired;

uint8_t wave = 0;

//*****************************************************************************
// 
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
// 
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}


//*****************************************************************************
//*****************************************************************************
void initialize_hardware(void) {
	initialize_serial_debug();
	
	//// setup lcd GPIO, config the screen, and clear it ////
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	//// setup the timers ////
	gp_timer_config_16(TIMER0_BASE, TIMER_TAMR_TAMR_PERIOD, false, true);
	pwm_timer_config(TIMER1_BASE);
	
	//// Setup ADC to convert on PS2 joystick using SS2 and interrupts ////
	ps2_initialize_SS2();
	
	//Initialize EEPROM
	eeprom_init();
	
	//Enables GPIO Port F for use with the port expander/buttons
	//and PWM/sound
	gpio_enable_port(GPIOF_BASE);
	gpio_config_digital_enable(GPIOF_BASE, MCP | PWM | RESET);
	gpio_config_enable_input(GPIOF_BASE, MCP | RESET);
	gpio_config_enable_output(GPIOF_BASE, PWM);
	gpio_config_alternate_function(GPIOF_BASE, PWM);
	gpio_config_port_control(GPIOF_BASE, PWM, GPIO_PCTL_PF2_T1CCP0);
	gpio_config_enable_pullup(GPIOF_BASE, RESET);
	gpio_config_falling_edge_irq(GPIOF_BASE, MCP);
	NVIC_SetPriority(GPIOF_IRQn, 0);
	NVIC_EnableIRQ(GPIOF_IRQn);
	portf->ICR |= GPIO_ICR_GPIO_M;
	//Initialize port expander
	if(mcp_init() == false){
		while(1){}
	}
	//Plays nothing to remove garbage data.
	play_sequence(NOTHING);
}

//Handler for the game timer, sets a flag
//Parameters: None
//Returns: None
void TIMER0B_Handler(void) {
    GameTimer_Done = true;
    game_timer->ICR |= TIMER_ICR_TBTOCINT;
}

//Handler for the music timer, updates current music sequence to next note
//Parameters: None
//Returns: None
void TIMER1B_Handler(void) {
	sound_timer->ICR |= TIMER_ICR_TBTOCINT;
	next_in_sequence();
}

//Handler for ADC interrupts
//Parameters: None
//Returns: None
void ADC0SS2_Handler(void) {
    ADC_Done = true;
    myadc->ISC = ADC_ISC_IN2; // Ack the conversion
}

//Handler for port expander button interrupts
//Parameters: None
//Returns: None
void GPIOF_Handler(void) {
    poll_button = TEAR_RATE;
    //clear icr of gpiof
    portf->ICR |= GPIO_ICR_GPIO_M;
    //read Gpiob of port expander
    mcp_byte_read(I2C1_BASE, GPIOBMCP, &buttons_current);
}

//Calls all initializations before entering main game loop.
//Parameters: None
//Returns: End's in while loop
int main(void) {
  //Initialize hero location, timer, and adc.
	char message[20];
	uint8_t high_score;
	uint8_t killed;
	uint8_t prev_wave = 0;
	uint8_t prev_health = 0;//used to prevent rerendering of score on the top of the screen
	actor_t* curr_actor; //used for the free at end
	actor_t* next_actor; //used for free at end
	
	/*
	//Required Putty Print
	put_string("\n\r");
	put_string("************************************\n\r");
	put_string("ECE353 - Spring 2018 Project\n\r  ");
	put_string(group);
	put_string("\n\r     Name:");
	put_string(individual_1);
	put_string("\n\r     Name:");
	put_string(individual_2);
	put_string("\n\r");  
	put_string("************************************\n\r");
	*/
	
	//Creates hero
    hero_init();
    hero = actors;
    //Defines bases for drivers
	game_timer = (TIMER0_Type*)TIMER0_BASE;
	sound_timer = (TIMER0_Type*)TIMER1_BASE;
    myadc = (ADC0_Type*)ADC0_BASE;
    portf = (GPIOA_Type*)GPIOF_BASE;
    initialize_hardware();
	//Kicks off timer that controls game
    gp_timer_start_16(TIMER0_BASE, 7, 5, TICKS, TICKS);
    
	spawn();				//An intial wave of monsters spawns
	play_sequence(SONG); 	//Song starts
	
	//Main loop
    while (1) {
		//Update wave counter and print new game status message
		if(wave != prev_wave || prev_health != hero->health){
			prev_wave = wave;
			prev_health = hero->health;
			sprintf(message,"Score: %d Health: %d",wave,hero->health);
			lcd_print_stringXY(message,0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
		}
		
		//Update game state, including actors and spawns
        if (GameTimer_Done) {
			debounce_reset();
			get_ps2_value(ADC0_BASE);
			killed = update_actors();
			if (killed == GAME_OVER) break;
			update_game(killed);
            //Shoot tears
            if (poll_button) {
                mcp_byte_read(I2C1_BASE, GPIOBMCP, &buttons_current);
                debounce_buttons();
                tear_fired = fire_on_press();
                if (tear_fired) poll_button = TEAR_RATE;
                else poll_button--;
            }
            GameTimer_Done = false;
        }
	
		//Read new adc values
        if (ADC_Done) {           
            ps2_x = myadc->SSFIFO2 & ADC_M;
            ps2_y = myadc->SSFIFO2 & ADC_M;
            ADC_Done = false;
        }
			
    }
	//checks if a new high score has been achieved
	lcd_clear_screen(LCD_COLOR_BLACK);
	eeprom_byte_read(I2C1_BASE, HIGHSCORE_ADDR, &high_score);
	if (wave > high_score) {
		eeprom_byte_write(I2C1_BASE, HIGHSCORE_ADDR, wave);
		high_score = wave;
	}
	//prints game over screen to board
	sprintf(message,"High Score: %d",high_score);
	lcd_print_stringXY(message,0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	sprintf(message,"Your Score: %d",wave);
	lcd_print_stringXY(message,0,9,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	
	//frees remaining actors
	while(curr_actor != NULL){
		next_actor = curr_actor->next;
		free(curr_actor);
		curr_actor = next_actor;
	}
	while(1){}
}

//Debounce button function, tests if button should be considered pressed
//Used to determine if tear should be shot from Steve
//Parameters: None
//Returns: None
void debounce_buttons(void) {
    static uint8_t button_count = 0;
	
	//Change in button state detected
	if (buttons_current != BUTTON_M) {
		if (button_count >= TEAR_RATE - 1) {
			if (~buttons_current & UP_BUTTON) { //UP
				hero->ud = UP_d;
			}
			else if (~buttons_current & DOWN_BUTTON) { //DOWN
				hero->ud = DOWN_d;
			}
			if (~buttons_current & LEFT_BUTTON) { //LEFT
				hero->lr = LEFT_d;
			}
			else if (~buttons_current & RIGHT_BUTTON) { //RIGHT
				hero->lr = RIGHT_d;
			}
		}
		button_count = (button_count + 1) % TEAR_RATE; 
	}
}

//Used to reset the high score counter
//Parameters: None
//Returns: None
void debounce_reset(void) {
	static uint8_t button_count;
	
	if (lp_io_read_pin(SW1_BIT)) {
		if (button_count == 3) {
			eeprom_byte_write(I2C1_BASE,256,0);
		}
		button_count = (button_count + 1) % 4;
	}
	else button_count = 0;
}


//Responsible for creating tears
//Parameters: None
//Returns:
//True if tear was fired
bool fire_on_press(void) {
	
    //If the direction isn't null-null we need to spawn a tear going in that direction
    if (!(hero->lr == IDLE_lr && hero->ud == IDLE_ud)) {
    create_actor(TEAR, hero->x_loc, hero->y_loc, hero->lr, hero->ud);
		play_sequence(TEAR_SOUND);
		hero->lr = IDLE_lr;
		hero->ud = IDLE_ud;
        return true;
    }

    return false;
}

//Decides when new waves of enemies need to be spawned
//Parameters:
//killed (uint8_t)			Number of enemies killed this update
//Returns: None
void update_game(uint8_t killed) {
	static uint8_t dead = 0;

    dead += killed;

	if ((dead % (WAVE_SIZE + 1)) == 0){
		spawn();
		dead = 1;
		wave++; //for high score
	}
}

//Responsible for creating monsters when a new wave starts
//Randomly chooses a monster type and position
//Equally chooses between each of the four sides of the screen
//Spawns a monster on the perimeter in 40x40 pixel blocks
//Parameters: None
//Returns: None
void spawn(void) {
    uint16_t x, y;
	//Need to keep track of previous wave's spawn locations so that 
	//no two enemies appear in the same place
	static uint16_t prev_x[WAVE_SIZE];
	static uint16_t prev_y[WAVE_SIZE];
    uint8_t i, j, side, type;
    lr_t lr;
    ud_t ud;
	bool match;

	for (i = 0; i < WAVE_SIZE; i++) {
		match = true;
		
		//Randomly choose type and side to spawn on
		lr = rand() % UDLR_SIZE;
		ud = rand() % UDLR_SIZE;
		type = rand() % NUM_ENEMY_TYPES + ENEMY_OFFSET;
		side = rand() % NUM_SIDES;
		
		//Based on the side chosen, spawn the monster in one of the 40x40 spaces
		if (side % 2) {	//ODD: 1(top) or 3(bottom)
			//x coordinate is randomized
			while(match) {	//Don't choose previously used spaces
				x = FIRST_X + SPAWN_SIZE * (rand() % NUM_X_SPOTS);
				match = false;
				for (j = 0; j < WAVE_SIZE; j++) {
					if (x == prev_x[j]) match = true;
				}
			}
			//Y coordinate is decided by side
			if (side == TOP && x != FIRST_X) y = SPAWN_SIZE; //top
			else y = LAST_Y; //bottom
		}	 
		else {	//EVEN: 2(left) or 4(right)
			//y coordinate is randomized
			while(match) { //Don't choose previously used spaces
				y = FIRST_Y + SPAWN_SIZE * (rand() % NUM_Y_SPOTS);
				match = false;
				for (j = 0; j < WAVE_SIZE; j++) {
					if (y == prev_y[j]) match = true;
				}
			}
			//x coordinate is decided by side
			if (side == LEFT && y != FIRST_Y) x = FIRST_X; //left
			else x = LAST_X; //right
		}
		//Record spawn location to prevent overlapping spawns
		prev_x[i] = x;
		prev_y[i] = y;		
		create_actor(type, x, y, lr, ud);
	}
}