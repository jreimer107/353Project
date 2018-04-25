#ifndef __ACTORS_H__
#define __ACTORS_H__

#include <stdint.h>

#include "main.h"
#include "images.h"

#define HERO 0
#define MISSILE 1
#define ZOMBIE 2
#define BAT 3
#define BLOB 4
#define MIMIC 5

#define PLAYER_HEALTH 3
#define ZOMBIE_HEALTH 3
#define BAT_HEALTH 2
#define BLOB_HEALTH 4
#define MIMIC_HEALTH 5

#define TOP_BORDER 		HERO_HEIGHT / 2
#define BOTTOM_BORDER 	ROWS - TOP_BORDER
#define LEFT_BORDER 	HERO_WIDTH / 2
#define RIGHT_BORDER 	COLS - LEFT_BORDER - 1


#define MAX_ACTORS	50
#define HERO_INVINCIBILITY	10
#define MISSILE_DAMAGE 1

//Enemy constants
#define PREFERENCE_MAX 10
#define PREFERENCE_CUTOFF PREFERENCE_MAX / 2
#define BLOB_COUNT 10
#define HOP_THRESHOLD 0x0F

typedef enum {IDLE_lr, RIGHT_d, LEFT_d} lr_t;
typedef enum {IDLE_ud, UP, DOWN} ud_t;

typedef struct {
	struct actor_t *next;
	uint8_t *bitmap;
	uint8_t *clear_bitmap;
	lr_t lr;
	ud_t ud;
	uint16_t x_loc;
	uint16_t y_loc;
	uint8_t type;
	int8_t health;
	uint8_t count;
	uint8_t height;
	uint8_t width;
} actor_t;

void update_actors(void);
bool update_hero(actor_t*);
bool update_missile(actor_t*);
bool update_zombie(actor_t*);
bool update_bat(actor_t*);
bool update_blob(actor_t*);
bool update_mimic(actor_t*);



#endif