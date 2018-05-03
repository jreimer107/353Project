#ifndef __ACTORS_H__
#define __ACTORS_H__

#include <stdint.h>

#include "main.h"
#include "images.h"
#include "lcd.h"
#include "eeprom.h"
#include "music.h"

//Types
#define HERO 0
#define TEAR 1
#define ZOMBIE 2
#define BAT 3
#define SLIME 4
#define MIMIC 5

#define NUM_TYPES		6
#define NUM_ENEMY_TYPES 4
#define ENEMY_OFFSET 	NUM_TYPES - NUM_ENEMY_TYPES

//Healths
#define PLAYER_HEALTH 3
#define ZOMBIE_HEALTH 3
#define BAT_HEALTH 2
#define SLIME_HEALTH 4
#define MIMIC_HEALTH 2

//Speeds
#define MAX_SPEED		10
#define PLAYER_SPEED 	3
#define TEAR_SPEED 		1
#define ZOMBIE_SPEED 	2
#define BAT_SPEED 		2
#define SLIME_SPEED 	1
#define MIMIC_SPEED 	1

//Colors
#define TEAR_COLOR LCD_COLOR_BLUE
#define STEVE_COLOR LCD_COLOR_MAGENTA
#define STEVE_COLOR_HURT LCD_COLOR_RED
#define ZOMBIE_COLOR LCD_COLOR_GREEN2
#define BAT_COLOR LCD_COLOR_ORANGE
#define SLIME_COLOR LCD_COLOR_YELLOW
#define MIMIC_COLOR LCD_COLOR_BROWN


#define MAX_ACTORS	50
#define HERO_INVINCIBILITY	128
#define TEAR_DAMAGE 1
#define TEAR_RATE 50

//Enemy constants
#define PREFERENCE_MAX 20
#define PREFERENCE_CUTOFF PREFERENCE_MAX / 2
#define SLIME_COUNT 100
#define HOP_THRESHOLD 0x7F

//Enumerations for facing and edge touching
#define UDLR_SIZE 3
typedef enum {IDLE_lr, RIGHT_d, LEFT_d} lr_t;
typedef enum {IDLE_ud, UP_d, DOWN_d} ud_t;

//Actor struct that defines all moving things in the game
typedef struct actor_t {
	uint8_t *bitmap;
	lr_t lr;
	ud_t ud;
	uint16_t x_loc;
	uint16_t y_loc;
	uint8_t type;
	int8_t health;
	uint8_t count;
	uint8_t height;
	uint8_t width;
	uint8_t move_count;
	uint16_t color;
	struct actor_t *next;
} actor_t;

//Main update function. Calls helper functions based on actor type.
//Helper functions must complete four tasks:
//1. Update the position of the actor.
//2. Update the actor's state based on any collisions.
//3. Complete any type-specific actions.
//4. Report the aliveness of the actor.
//Parameters: None
//Returns:
//Number of enemies killed
uint8_t update_actors(void);

//Controls hero collision and movement
//Parameters:
//hero (*actor_t)				Pointer to main character (Steve!)
//Returns:
//true if hero has died
bool update_hero(actor_t*);

//Missile dies on contact with enemy, travels in straight line(including diagonal), and hurts enemies.
//Parameters:
//tear (*actor_t)				Pointer to tear to be updated
//Returns:
//True if tear has "died"
bool update_tear(actor_t*);

//Zombies randomly sway towards player.
//Parameters:
//zombie (*actor_t)				Pointer to zombie to be updated
//Returns:
//True if zombie has died
bool update_zombie(actor_t*);

//Bats bounce around the screen.
//Parameters:
//bat (*actor_t)				Pointer to bat to be updated
//Returns:
//True if bat has died
bool update_bat(actor_t*);

//Slimes only move every so often. They have a counter that tells them when to move and when not to.
//Parameters:
//slime (*actor_t)				Pointer to slime to be updated
//Returns:
//True if slime has died
bool update_slime(actor_t*);

//Mimics, surprisingly, mimic the movement of the player, but faster
//Parameters:
//mimic (*actor_t)				Pointer to mimic to be updated
//Returns:
//True if mimic has died
bool update_mimic(actor_t*);

//Creates the main character at the start of the game
//Parameters: None
//Returns: None
void hero_init(void);

//Detects if an actor is at an edge.
//Parameters:
//actor (*actor_t)			Pointer to current actor
//Returns:
//LEFT if the actor is at the left edge of the screen
//RIGHT if the actor is at the right edge of the screen
//else IDLE
lr_t at_edge_lr(actor_t*);

//Detects if an actor is at an edge.
//Parameters:
//actor (*actor_t)			Pointer to current actor
//Returns:
//UP if the actor is at the top edge of the screen
//DOWN if the actor is at the bottom edge of the screen
//returns IDLE.
ud_t at_edge_ud(actor_t*);

//Detects if two actors are colliding
//Parameters:
//a (*actor_t)			First actor for collision test
//b (*actor_t)			Second actor for collision test
//Returns:
//True if colliding
bool detect_collision(actor_t *a , actor_t *b);

//Creates new montsters and tears by setting their initial values
//Parameters:
//type (uint8_t)		Decides which actor will be spawned	
//x (uint16_t)			X location of actor
//y (uint16_t)			Y location of actor
//lr (lr_t)					Direction the actor is facing, left/right
//ud (ud_t)					Direction the actor is facing, up/down
//Returns:
//Pointer to the actor that was created
actor_t* create_actor(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud);



#endif