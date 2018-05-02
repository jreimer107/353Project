#include "main.h"
#include "actors.h"
#include "eeprom.h"
#include "images.h"
#include "launchpad_io.h"
#include "lcd.h"
#include "ps2.h"
#include "timers.h"




char group[] = "Group??";
char individual_1[] = "John	Reimer";
char individual_2[] = "Luke Richmond";

///////////////////////////
// Global declared next //
/////////////////////////

extern actor_t* actors;
actor_t* hero;

uint16_t ps2_x, ps2_y;

//Flags set by interrupt handlers
volatile bool TimerA_Done = false;
volatile bool TimerB_Done = false;
volatile bool ADC_Done = false;
volatile uint8_t poll_button = 0;
TIMER0_Type* gp_timer;
TIMER0_Type* gp_timer2;
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
	
	//// setup GPIO for LED drive ////
	lp_io_init();
	lp_io_clear_pin(RED_BIT);
	lp_io_clear_pin(GREEN_BIT);
	
	//// Setup ADC to convert on PS2 joystick using SS2 and interrupts ////
	
	ps2_initialize_SS2();
	
	//Initialize EEPROM
	eeprom_init();
	
	
	

	gpio_enable_port(GPIOF_BASE);
	gpio_config_digital_enable(GPIOF_BASE, PF0 |PF2);
	gpio_config_enable_input(GPIOF_BASE, PF0);
	gpio_config_falling_edge_irq(GPIOF_BASE, PF0);
	NVIC_SetPriority(GPIOF_IRQn, 0);
  	NVIC_EnableIRQ(GPIOF_IRQn);
	portf->ICR |= GPIO_ICR_GPIO_M;
	//Initialize port expander
	if(mcp_init() == false){
		while(1){}
	}
	

	play_sequence(0);
	pwm_timer_config(TIMER1_BASE);
	gpio_config_enable_output(GPIOF_BASE,PF2);
	gpio_config_alternate_function(GPIOF_BASE, PF2);

	gpio_config_port_control(GPIOF_BASE,PF2,GPIO_PCTL_PF2_T1CCP0);
	
	
}

//Red LED
//Toggle every 20 ISR executions (5Hz)
void TIMER0A_Handler(void)
{
    TimerA_Done = true;
    gp_timer->ICR |= TIMER_ICR_TATOCINT;
}

//Green LED
//Prescalar 1
//Toggle every 20 executions (2.5 Hz)
void TIMER0B_Handler(void)
{
    TimerB_Done = true;
    gp_timer->ICR |= TIMER_ICR_TBTOCINT;
}

void TIMER1B_Handler(void) {
	gp_timer2->ICR |= TIMER_ICR_TBTOCINT;
	next_in_sequence();
}

void ADC0SS2_Handler(void)
{
    ADC_Done = true;
    myadc->ISC = ADC_ISC_IN2; // Ack the conversion
}

void GPIOF_Handler(void) {
    poll_button = TEAR_RATE;
    //clear icr of gpiof
    portf->ICR |= GPIO_ICR_GPIO_M;
    //read Gpiob of port expander
    mcp_byte_read(I2C1_BASE, GPIOBMCP, &buttons_current);
}

//*****************************************************************************
//*****************************************************************************
int main(void) {
    //Initialize hero location, timer, and adc.
	char message[20];
	uint8_t high_score;
	uint8_t killed;
	uint8_t prev_wave = 0;
	uint8_t prev_health = 0;
	DisableInterrupts();
	init_serial_debug(true, true);
	EnableInterrupts();
    hero_init();
    hero = actors;
    gp_timer = (TIMER0_Type*)TIMER0_BASE;
	gp_timer2 = (TIMER0_Type*)TIMER1_BASE;
    myadc = (ADC0_Type*)ADC0_BASE;
    portf = (GPIOA_Type*)GPIOF_BASE;
    initialize_hardware();
    gp_timer_start_16(TIMER0_BASE, 7, 5, TICKS, TICKS);

    //Main loop
	spawn();
	play_sequence(3);
    while (1) {
		if(wave != prev_wave || prev_health != hero->health){
			prev_wave = wave;
			prev_health = hero->health;
			sprintf(message,"Score: %d Health: %d",wave,hero->health);
			lcd_print_stringXY(message,0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
		}
		GPIOF -> DATA = gp_timer2 -> TAV;
      	
		if (TimerA_Done) {
            TimerA_Done = false;
        }

        if (TimerB_Done) {
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
            TimerB_Done = false;
        }

        if (ADC_Done) {
            //Read new adc values
            ps2_x = myadc->SSFIFO2 & ADC_M;
            ps2_y = myadc->SSFIFO2 & ADC_M;
            ADC_Done = false;
        }
		//draw_actors();
    }
	lcd_clear_screen(LCD_COLOR_BLACK);
	eeprom_byte_read(I2C1_BASE, HIGHSCORE_ADDR, &high_score);
	if (wave > high_score) {
		eeprom_byte_write(I2C1_BASE, HIGHSCORE_ADDR, wave);
		high_score = wave;
	}
	sprintf(message,"High Score: %d",high_score);
	lcd_print_stringXY(message,0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	sprintf(message,"Your Score: %d",wave);
	lcd_print_stringXY(message,0,9,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	while(1){}
}

//UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3
//Tears will keep firing as long as buttons are held.
void debounce_buttons(void) {
    static uint8_t button_count = 0;
	
		if (buttons_current != BUTTON_M) {
			//if (button_count == TEAR_RATE - 4) play_freq(TIMER1_BASE, 1000);
			//else if (button_count == TEAR_RATE - 3) play_freq(TIMER1_BASE, 500);
			//else if (button_count == TEAR_RATE - 2) play_freq(TIMER1_BASE, 400);
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
				//play_freq(TIMER1_BASE, 0);
			}
			button_count = (button_count + 1) % TEAR_RATE; 
		}
		//else play_freq(TIMER1_BASE, 0);
}

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



bool fire_on_press(void) {
	
    //If the direction isn't null-null we need to spawn a tear going in that direction
    if (!(hero->lr == IDLE_lr && hero->ud == IDLE_ud)) {
        create_actor(TEAR, hero->x_loc, hero->y_loc, hero->lr, hero->ud);
		play_sequence(1);
		hero->lr = IDLE_lr;
		hero->ud = IDLE_ud;
        return true;
    }

    return false;
}

void update_game(uint8_t killed) {
    static uint8_t spawned = 0;
    static uint8_t dead = 0;
    static uint8_t spawn_wait = 0;
    static uint8_t wave_wait = 0;

    dead += killed;

	if ((dead % 4) == 0){
		spawn();
		dead = 1;
		//spawned = 0;
		wave++; //for high score
	}
	  
	
/*	
    if (dead < num_enemies[0]) { //wave in progress
        if (spawned < num_enemies[0]) {
            if (spawn_wait >= SPAWN_DELAY && spawned - dead < MAX_ACTORS) {
                spawn();
                spawn_wait = 0;
                spawned++;
            } 
						else spawn_wait++;
        }
    } 
		else { //wave over
        if (wave_wait < WAVE_DELAY) wave_wait++;
        else {
            wave_wait = 0;
            wave++;
            spawned = 0;
            dead = 0;
        }
    }
*/
}

void spawn() {
    uint16_t x, y;
	uint8_t prev_spots[NUM_SIDES];
    uint8_t i, side, spot, type;
    lr_t lr;
    ud_t ud;

	for (i = 0; i < WAVE_SIZE; i++) {
		lr = rand() % UDLR_SIZE;
		ud = rand() % UDLR_SIZE;
		type = rand() % NUM_ENEMY_TYPES + ENEMY_OFFSET;
		side = rand() % NUM_SIDES;
		
		if (side % 2) {
			do {
				spot = rand() % NUM_X_SPOTS;
			} while(spot == prev_spots[0] || spot == prev_spots[1]);
			x = FIRST_X + SPAWN_SIZE * spot;
			if (side == TOP) y = FIRST_Y; //top
			else y = LAST_Y; //bottom
		}	 
		else {
			do {
				spot = rand() % NUM_Y_SPOTS;
			} while(spot == prev_spots[2] || spot == prev_spots[3]);
			y = FIRST_Y + SPAWN_SIZE * spot;
			if (side == LEFT) x = FIRST_X; //left
			else x = LAST_X; //right
		}
		create_actor(type, x, y, lr, ud);
	}



}