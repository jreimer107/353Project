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
GPIOA_Type* portf;
ADC0_Type* myadc;

uint8_t buttons_current = 0;
bool buttons_pressed[4] = {false, false, false, false};
bool tear_fired;

const uint8_t num_enemies[] = {WAVE1, WAVE2, WAVE3, WAVE4, WAVE5, WAVE5, WAVE6, WAVE7, WAVE8, WAVE9, WAVE10};


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
	gpio_config_digital_enable(GPIOF_BASE, PF0);
	gpio_config_enable_input(GPIOF_BASE, PF0);
	gpio_config_falling_edge_irq(GPIOF_BASE, PF0);
	NVIC_SetPriority(GPIOF_IRQn, 0);
  NVIC_EnableIRQ(GPIOF_IRQn);
	portf->ICR |= GPIO_ICR_GPIO_M;
	//Initialize port expander
	if(mcp_init() == false){
		while(1){}
	}
	
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
		DisableInterrupts();
		init_serial_debug(true, true);
		EnableInterrupts();
    hero_init();
    hero = actors;
    gp_timer = (TIMER0_Type*)TIMER0_BASE;
    myadc = (ADC0_Type*)ADC0_BASE;
    portf = (GPIOA_Type*)GPIOF_BASE;
    initialize_hardware();
    gp_timer_start_16(TIMER0_BASE, 7, 1, TICKS, TICKS);

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
    while (1) {
        if (TimerA_Done) {
            update_red_led();
            TimerA_Done = false;
        }

        if (TimerB_Done) {
            update_green_led();
            get_ps2_value(ADC0_BASE);
            update_game(update_actors());
            //Shoot tears
            if (poll_button) {
                mcp_byte_read(I2C1_BASE, GPIOBMCP, &buttons_current);
								printf("%d ", buttons_current);
                debounce_buttons();
                tear_fired = fire_on_press();
                if (tear_fired) poll_button = TEAR_RATE;
                else poll_button--;
            }
            TimerB_Done = false;
        }

        if (ADC_Done) {
            //Read new adc values
            ps2_x = myadc->SSFIFO2 & 0x0FFF;
            ps2_y = myadc->SSFIFO2 & 0x0FFF;
            ADC_Done = false;
        }

        draw_actors();
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

//UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3
//Tears will keep firing as long as buttons are held.
void debounce_buttons(void) {
    int i;
    static uint8_t button_count[4];

    //Get update button counters based on current state of buttons
    for (i = 0; i < 4; i++) {
        if ((~buttons_current) & (1 << i)) { //If button is currently pressed
            //Check if have had enough iterations to fire tear
            if (button_count[i] == TEAR_RATE - 1) { //Indicate that a button has been pressed sufficiently long
                buttons_pressed[i] = true;
            }
            //Increment circular counter
            button_count[i] = (button_count[i] + 1) % TEAR_RATE;
        } 
				else button_count[i] = 1; //Else reset count
    }
}

bool fire_on_press(void) {
    //Update hero direction
    if (buttons_pressed[0]) {
        hero->ud = UP_d;
        buttons_pressed[0] = false;
    }
    if (buttons_pressed[1]) {
        hero->ud = DOWN_d;
        buttons_pressed[1] = false;
    }
    if (buttons_pressed[2]) {
        hero->lr = LEFT_d;
        buttons_pressed[2] = false;
    }
    if (buttons_pressed[3]) {
        hero->lr = RIGHT_d;
        buttons_pressed[3] = false;
    }

    //If the direction isn't null-null we need to spawn a tear going in that direction
    if (!(hero->lr == IDLE_lr && hero->ud == IDLE_ud)) {
        create_actor(TEAR, hero->x_loc, hero->y_loc, hero->lr, hero->ud);
			hero->lr = IDLE_lr;
			hero->ud = IDLE_ud;
        return true;
    }

    return false;
}

void update_game(uint8_t killed) {
    static uint8_t wave = 1;
    static uint8_t spawned = 0;
    static uint8_t dead = 0;
    static uint8_t spawn_wait = 0;
    static uint8_t wave_wait = 0;

    dead += killed;

    if (dead < num_enemies[wave]) { //wave in progress
        if (spawned < num_enemies[wave - 1]) {
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
}

void spawn() {
    //top = 0, bottom = 1, left = 2, right = 3
    uint16_t x, y;
    uint8_t side, spot, type;
    lr_t lr;
    ud_t ud;

    lr = rand() % 3;
    ud = rand() % 3;
    type = rand() % 4 + 2;

    side = rand() % 4;
    if (side < 2) {
        spot = rand() % 6;
        x = 20 + 40 * spot;
        if (side == 0) y = 20; //top
        else y = 300; //bottom
    } 
	else {
        spot = rand() % 8;
        y = 20 + 40 * spot;
        if (side == 2) x = 20; //left
        else x = 220; //right
    }
    create_actor(type, x, y, lr, ud);
}