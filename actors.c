#include "actors.h"

actor_t *actors; //Hero should be head node;
extern uint16_t ps2_x, ps2_y;


//Main update function. Calls helper functions based on actor type.
//Helper functions must complete four tasks:
//1. Update the position of the actor.
//2. Update the actor's state based on any collisions.
//3. Complete any type-specific actions.
//4. Report the aliveness of the actor.
uint8_t update_actors() {
	actor_t *curr_actor = actors;
	actor_t *prev_actor = NULL;
	bool kill;
	uint8_t killed = 0;
	while (curr_actor) {
		if (curr_actor->type == HERO) {
			kill = update_hero(curr_actor);
		}
		else if (curr_actor->type == TEAR) {
			kill = update_tear(curr_actor);
		}
		else if (curr_actor->type == ZOMBIE) {
			kill = update_zombie(curr_actor);
		}
		else if (curr_actor->type == BAT) {
			kill = update_bat(curr_actor);
		}
		else if (curr_actor->type == SLIME) {
			kill = update_slime(curr_actor);
		}
		else if (curr_actor->type == MIMIC) {
			kill = update_mimic(curr_actor);
		}

		//Remove actor from list if dead
		if (kill) {
			destroy(curr_actor);	//unrender
			if (prev_actor) {
				prev_actor->next = curr_actor->next;
			}
			else {
				actors = curr_actor->next;
			}
			free(curr_actor);
			curr_actor = prev_actor;
			killed++;
		}

		prev_actor = curr_actor;
		curr_actor = curr_actor->next;
	}
	return killed;
}

//Hero has an invincibility counter that prevents it from being damaged continuously.
//It also fires tears.
bool update_hero(actor_t *hero) {
	static uint8_t button_count = 0;
	lr_t edge_lr = at_edge_lr(hero);
	ud_t edge_ud = at_edge_ud(hero);

	if (!hero->health) return true;

	//Update collisions
	if (!hero->count) { 
		actor_t *enemy = actors->next;	//First actor is hero
		while(enemy) {
			if (enemy->type != TEAR && detect_collision(hero, enemy)) {
				hero->count = HERO_INVINCIBILITY;
				hero->health--;
			}
			enemy = enemy->next;
		}
	}
	//Invincibility count is active, do not check collisions
	else hero->count--;

	//Move on speed interval
	if (hero->move_count = PLAYER_SPEED) {
		if (ps2_x > LEFT_THRESHOLD) {
			if (edge_lr != LEFT_d) hero->x_loc--;
		}
		else if (ps2_x < RIGHT_THRESHOLD) {
			if (edge_lr != RIGHT_d) hero->x_loc++;
		}
		if (ps2_y > UP_THRESHOLD) {\
			 if (edge_ud != UP_d) hero->y_loc--;
		}
		else if (ps2_y < DOWN_THRESHOLD) {
			if (edge_ud != DOWN_d) hero->y_loc++;
		}
		hero->move_count = 0;
	}
	else hero->move_count++;

	//Report aliveness (false for alive)
	return false;
}

//Missile dies on contact with enemy, travels in straight line, and hurts enemies.
bool update_tear(actor_t *tear) {
	actor_t *enemy = actors->next;

	//Check enemy collision
	while(enemy) {
		if (enemy->type != HERO && enemy->type != TEAR && detect_collision(tear, enemy)) {
			enemy->health -= TEAR_DAMAGE;
			return true;
		}
		enemy = enemy->next;
	}

	//Check if off screen
	if (at_edge_lr(tear) != IDLE_lr || at_edge_ud(tear) != IDLE_ud) {
		return true;
	}
	
	//Update position
	if(tear->move_count == TEAR_SPEED) {
		if (tear->lr == LEFT_d) tear->x_loc--;
		else tear->x_loc++;
		if (tear->ud == UP_d) tear->y_loc--;
		else tear->y_loc++;
		tear->move_count = 0;
	}
	else tear->move_count++;

	//If we get here tear is still alive
	return false;
}

//Zombies randomly sway towards player.
bool update_zombie(actor_t *zombie) {
	uint8_t direction_preference = rand() % PREFERENCE_MAX;
	actor_t *hero = actors; //Hero is head of actors list.

	//If dead, return dead.
	if (zombie->health <= 0) return true;
	
	//Move on speed interval
	if (zombie->move_count == ZOMBIE_SPEED) {
		if (direction_preference > PREFERENCE_CUTOFF) {
			if (zombie->x_loc > hero->x_loc) zombie->x_loc--;
			else if (zombie->x_loc < hero->x_loc) zombie->x_loc++;
		}
		else {
			if (zombie->y_loc > hero->y_loc) zombie->y_loc--;
			else if (zombie->y_loc < hero->y_loc) zombie->y_loc++;
		}
		zombie->move_count = 0;
	}
	else zombie->move_count++;

	return false;
}

//Bats bounce around the screen.
bool update_bat(actor_t *bat) {
	//If given only one direction they will repeatedy go back and forth
	//If given two directions they will bounce around like tv screensavers
	lr_t edge_lr = at_edge_lr(bat);
	ud_t edge_ud = at_edge_ud(bat);

	if (bat->health <= 0) return true;


	//Detect if at edge of screen, switch movement direction
	if (edge_lr == LEFT_d) bat->lr = RIGHT_d;
	if (edge_lr == RIGHT_d) bat->lr = LEFT_d;
	if (edge_ud == UP_d) bat->ud = DOWN_d;
	if (edge_ud == RIGHT_d) bat->ud = UP_d;

	//Move at speed inteval
	if (bat->move_count == BAT_SPEED)
		if (bat->lr == LEFT_d) bat->x_loc--;
		else if (bat->lr == RIGHT_d) bat->x_loc++;
		if (bat->ud == UP_d) bat->y_loc--;
		else if (bat->ud == DOWN_d) bat->y_loc++;
		bat->move_count = 0;
	}
	else bat->move_count++;

	return false;
}

//Slimes only move every so often. They have a counter that tells them when to move and when not to.
bool update_slime(actor_t *slime) {
	lr_t edge_lr = at_edge_lr(slime);
	ud_t edge_ud = at_edge_ud(slime);

	if (slime->health <= 0) return true;

	//Count reached, switch between moving and idle.
	if (slime->count == SLIME_COUNT) {
		if (slime->lr != IDLE_lr || slime->ud != IDLE_ud) {
			slime->lr = IDLE_lr;
			slime->ud = IDLE_ud;
		}
		else { //We are idle, need to pick a direction to hop;
			uint8_t hop_dir = rand();
			//If we are odd, hop horizontal. Else hop vertical
			if (hop_dir % 2) {
				//If we are greater or equal to threshold, hop right. Else hop left.
				if (hop_dir > HOP_THRESHOLD) slime->lr = RIGHT_d;
				else slime->lr = RIGHT_d;
			}
			else {
				//If we are greater or equal to threshold, hop down. Else hop up.
				if (hop_dir > HOP_THRESHOLD) slime->ud = DOWN_d;
				else slime->ud = UP_d;
			}
		}
		slime->count = 0;
	}
	else slime->count++;

	//Actually move.
	if (slime->move_count == SLIME_SPEED) {
		if (slime->lr == LEFT_d && edge_lr != LEFT_d) slime->x_loc--;
		else if (slime->lr == RIGHT_d && edge_lr != RIGHT_d) slime->x_loc++;
		else if (slime->ud == UP_d && edge_lr != UP_d) slime->y_loc--;
		else if (slime->ud == DOWN_d && edge_lr != DOWN_d) slime->y_loc++;
		slime->move_count = 0;
	}
	else slime->move_count++;

	return false;
}

//Mimics, surprisingly, mimic the movement of the player, but slower (same speed, faster? variants?)
bool update_mimic(actor_t *mimic) {
	lr_t edge_lr = at_edge_lr(mimic);
	ud_t edge_ud = at_edge_ud(mimic);
	
	if (mimic->health <= 0) return true;
	
	//move on speed interval
	if (mimic->move_count >= MIMIC_SPEED) {
		if (ps2_x > LEFT_THRESHOLD && edge_lr != LEFT_d) mimic->x_loc--;
		else if (ps2_x < RIGHT_THRESHOLD && edge_lr != RIGHT_d) mimic->x_loc++;
		if (ps2_y > UP_THRESHOLD && edge_ud != UP_d) mimic->y_loc--;
		else if (ps2_y < DOWN_THRESHOLD && edge_ud != DOWN_d) mimic->y_loc++;
		mimic->move_count = 0;
	}
	else mimic->move_count++;
	
	return false;
}

actor_t* create_actor(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud) {
	actor_t *actor = malloc(sizeof(actor));
	if (type == TEAR) {
		actor->bitmap = (uint8_t*)tearBitmap;
		//actor->clear_bitmap = (uint8_t*)tearErase;
		actor->height = TEAR_HEIGHT;
		actor->width = TEAR_WIDTH;
		//actor->speed = MAX_SPEED - TEAR_SPEED + 1;
	}
	else if (type == ZOMBIE) {
		actor->bitmap = (uint8_t*)zombieBitmap;
		//actor->clear_bitmap = (uint8_t*)zombieErase;
		actor->height = ZOMBIE_HEIGHT;
		actor->width = ZOMBIE_WIDTH;
		actor->health = ZOMBIE_HEALTH;
		//actor->speed = MAX_SPEED - ZOMBIE_SPEED + 1;	
	}
	else if (type == BAT) {
		actor->bitmap = (uint8_t*)batBitmap;
		//actor->clear_bitmap = (uint8_t*)batErase;
		actor->height = BAT_HEIGHT;
		actor->width = BAT_WIDTH;
		actor->health = BAT_HEALTH;
		//actor->speed = MAX_SPEED - BAT_SPEED + 1;	
	}
	else if (type == SLIME) {
		actor->bitmap = (uint8_t*)slimeBitmap;
		//actor->clear_bitmap = (uint8_t*)slimeErase;
		actor->height = SLIME_HEIGHT;
		actor->width = SLIME_WIDTH;
		actor->health = SLIME_HEALTH;
		//actor->speed = MAX_SPEED - SLIME_SPEED + 1;		
	}
	else if (type == MIMIC) {
		actor->bitmap = (uint8_t*)mimicBitmap;
		//actor->clear_bitmap = (uint8_t*)mimicErase;
		actor->height = MIMIC_HEIGHT;
		actor->width = MIMIC_WIDTH;
		actor->health = MIMIC_HEALTH;
		//actor->speed = MAX_SPEED - MIMIC_SPEED + 1;
	}

	actor->lr = lr;
	actor->ud = ud;
	actor->x_loc = x;
	actor->y_loc = y;
	actor->type = type;
	actor->count = 0;
	actor->move_count = 0;

	//Link into list after head
	actor->next = actors->next;	//new_actor->next = hero->next
	actors->next = actor;	//hero->next = new_actor

	return actor;
}

//Detects if an actor is at an edge.
//returns LEFT if the actor is at the left edge of the screen
//returns RIGHT if the actor is at the right edge of the screen
//else returns IDLE.
lr_t at_edge_lr(actor_t *actor) {
	if (actor->x_loc < actor->width / 2 + 1) return LEFT_d;
	if (actor->x_loc > COLS - actor->width / 2 - 1) return RIGHT_d;
	return IDLE_lr;
}

//Detects if an actor is at an edge.
//returns UP if the actor is at the top edge of the screen
//returns DOWN if the actor is at the bottom edge of the screen
//Else returns IDLE.
ud_t at_edge_ud(actor_t *actor) {
	if (actor->y_loc < actor->height / 2  + 1) return UP_d;
	if (actor->y_loc > ROWS - actor->height / 2 - 1) return DOWN_d;
	return IDLE_ud;
}

bool detect_collision(actor_t *a, actor_t *b) {
	uint16_t a_top, a_bottom, a_right, a_left;
	uint16_t b_top, b_bottom, b_right, b_left;
	a_top = a->y_loc - a->height;
	a_bottom = a->y_loc + a->height;
	a_left = a->x_loc - a->width;
	a_right = a->x_loc + a->width;

	b_top = b->y_loc - b->height;
	b_bottom = b->y_loc + b->height;
	b_left = b->x_loc - b->width;
	b_right = b->x_loc + b->width;

	//If a's right is left of b's left
	//If a's left is right of b's right
	//If a's top is below b's bottom
	//If a's bottom is above b's top
	if(a_right < b_left || a_left > b_right || a_bottom < b_top || a_top > b_bottom) 
		return false;
	else return true;
}


void destroy(actor_t *actor) {
	lcd_draw_image(
		actor->x_loc,	 // X Pos
		actor->width,	  // Image Horizontal Width
		actor->y_loc, // Y Pos
		actor->height,	 // Image Vertical Height
		actor->bitmap,	   // Image
		LCD_COLOR_BLACK,  // Foreground Color
		LCD_COLOR_BLACK	// Background Color
	);
}

//Draws all actors.
void draw_actors(void) {
	actor_t *actor = actors;
	while(actor) {
		lcd_draw_image(
			actor->x_loc,		 // X Pos
			actor->width,		 // Image Horizontal Width
			actor->y_loc,		 // Y Pos
			actor->height,		 // Image Vertical Height
			actor->bitmap, // Image
			LCD_COLOR_WHITE,	 // Foreground Color
			LCD_COLOR_BLACK		 // Background Color
		);
		actor = actor->next;
	}
}

void hero_init(void){
	actor_t *hero;
	actors = malloc(sizeof(actor_t));
	hero = actors;
	hero->bitmap = (uint8_t*)&heroBitmap;
	//hero->clear_bitmap = (uint8_t*)&heroErase;
	hero->count = 0;
	hero->health = PLAYER_HEALTH;
	hero->height = STEVE_HEIGHT;
	hero->width = STEVE_WIDTH;
	hero->lr = IDLE_lr;
	hero->ud = IDLE_ud;
	hero->move_count = 0;
	hero->type = HERO;
	hero->x_loc = COLS / 2;
	hero->y_loc = ROWS /2;
	hero->next = NULL;
}

