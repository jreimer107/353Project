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
#define MIMIC_HEALTH 2

//Speeds (lower = faster)	Change this|
#define PLAYER_SPEED 	3
#define TEAR_SPEED 		1
#define ZOMBIE_SPEED 	2
#define BAT_SPEED 		2
#define SLIME_SPEED 	3
#define MIMIC_SPEED 	1

#define MAX_ENEMIES 6
#define MAX_TEARS 10
#define HERO_INVINCIBILITY	50
#define TEAR_DAMAGE 1
//Two Tear Rates needed
#define TEAR_RATE 50

//Enemy constants
#define PREFERENCE_MAX 20
#define PREFERENCE_CUTOFF PREFERENCE_MAX / 2
#define SLIME_COUNT 50
#define HOP_THRESHOLD 0x7F

typedef enum {IDLE_lr, RIGHT_d, LEFT_d} lr_t;
typedef enum {IDLE_ud, UP_d, DOWN_d} ud_t;

typedef struct hero_t {
    lr_t lr;
    ud_t ud;
    uint16_t x_loc;
    uint16_t y_loc;
    int8_t health;
    uint8_t count;
    uint8_t move_count;
} hero_t;

typedef struct enemy_t {
	Updater_t update;
    lr_t lr;
    ud_t ud;
    uint16_t x_loc;
    uint16_t y_loc;
    int8_t health;
    uint8_t count;
    uint8_t move_count;
} enemy_t;

typedef struct tear_t {
    uint16_t x_loc;
    uint16_t y_loc;
    uint8_t move_count;
} tear_t;


uint8_t update_actors(void);
void update_tears(void);
uint8_t update_enemies(void);

bool update_hero(void);
bool update_tear(tear_t*);

typedef bool (*Updater_t)(enemy_t*);
bool update_zombie(enemy_t*);
bool update_bat(enemy_t*);
bool update_slime(enemy_t*);
bool update_mimic(enemy_t*);

lr_t at_edge_lr(uint16_t, uint8_t);
ud_t at_edge_ud(uint16_t, uint8_t);
bool detect_collision(uint16_t,uint16_t, uint8_t, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t);
actor_t* create_actor(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud);

#endif