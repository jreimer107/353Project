#ifndef __ACTORS_H__
#define __ACTORS_H__

#include <stdint.h>

#include "main.h"
#include "images.h"
#include "lcd.h"

#define HERO 0
#define TEAR 1
#define ZOMBIE 2
#define BAT 3
#define SLIME 4
#define MIMIC 5

#define PLAYER_HEALTH 3
#define ZOMBIE_HEALTH 3
#define BAT_HEALTH 2
#define SLIME_HEALTH 4
#define MIMIC_HEALTH 5

//Speeds (lower = faster)	Change this|
#define MAX_SPEED		10
#define PLAYER_SPEED 	MAX_SPEED - 3 + 1
#define TEAR_SPEED 		MAX_SPEED - 1 + 1
#define ZOMBIE_SPEED 	MAX_SPEED - 2 + 1
#define BAT_SPEED 		MAX_SPEED - 3 + 1
#define SLIME_SPEED 	MAX_SPEED - 4 + 1
#define MIMIC_SPEED 	MAX_SPEED - 5 + 1

#define MAX_ACTORS	50
#define HERO_INVINCIBILITY	10
#define TEAR_DAMAGE 1
#define TEAR_RATE 5

//Enemy constants
#define PREFERENCE_MAX 10
#define PREFERENCE_CUTOFF PREFERENCE_MAX / 2
#define SLIME_COUNT 10
#define HOP_THRESHOLD 0x0F

typedef enum {IDLE_lr, RIGHT_d, LEFT_d} lr_t;
typedef enum {IDLE_ud, UP_d, DOWN_d} ud_t;

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
	struct actor_t *next;
} actor_t;

uint8_t update_actors();
bool update_hero(actor_t*);
bool update_tear(actor_t*);
bool update_zombie(actor_t*);
bool update_bat(actor_t*);
bool update_slime(actor_t*);
bool update_mimic(actor_t*);
void hero_init(void);
void draw_actors(void);
void destroy(actor_t*);

lr_t at_edge_lr(actor_t*);
ud_t at_edge_ud(actor_t*);
bool detect_collision(actor_t *a , actor_t *b);
void destroy(actor_t*);
actor_t* create_actor(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud);



#endif