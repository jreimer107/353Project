#include "actors.h"

actor_t *actors; //Hero should be head node;


//Main update function. Calls helper functions based on actor type.
//Helper functions must complete four tasks:
//1. Update the position of the actor.
//2. Update the actor's state based on any collisions.
//3. Complete any type-specific actions.
//4. Report the aliveness of the actor.
void update_actors() {
	actor_t *curr_actor = actors;
	actor_t *prev_actor = NULL;
	bool kill;
	while (curr_actor) {
		if (curr_actor->type == HERO) {
			kill = update_hero(curr_actor);
		}
		else if (curr_actor->type == MISSILE) {
			kill = update_missile(curr_actor);
		}
		else if (curr_actor->type == ZOMBIE) {
			kill = update_zombie(curr_actor);
		}
		else if (curr_actor->type == BAT) {
			kill = update_bat(curr_actor);
		}
		else if (curr_actor->type == BLOB) {
			kill = update_blob(curr_actor);
		}
		else if (curr_actor->type == MIMIC) {
			kill = update_mimic(curr_actor) {
			}
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
		}

		prev_actor = curr_actor;
		curr_actor = curr_actor->next;
	}
}

//Hero has an invincibility counter that prevents it from being damaged continuously.
//It also fires missiles.
bool update_hero(actor_t *hero) {
	static uint8_t button_count = 0;
	lr_t edge_lr = at_edge_lr(hero);
	ud_t edge_ud = at_edge_ud(hero);

	//Update direction facing (needed for missiles) and position
	//TODO: face where shooting
	if (ps2_x > LEFT_THRESHOLD) {
		hero->lf = LEFT;
		if (edge_lr != LEFT) hero->x_loc--;
	}
	else if (ps2_x < RIGHT_THRESHOLD) {
		hero->lf = RIGHT;
		if (edge_lr != RIGHT) hero->x_loc++;
	}
	else hero->lf = IDLE;
	if (ps2_y > UP_THRESHOLD) {
		 hero->ud = UP;
		 if (edge_ud != UP) hero->y_loc--;
	}
	else if (ps2_y < DOWN_THRESHOLD) {
		hero->ud = DOWN;
		if (edge_ud != DOWN) hero->y_loc++;
	}
	else hero->ud = DOWN;	//Want to default to facing down.


	//Update collisions
	if (!hero->count) { 
		actor_t *enemy = actors->next;	//First actor is hero
		while(enemy) {
			if (enemy->type != MISSILE && detect_collision(hero, enemy)) {
				hero->count = HERO_INVINCIBILITY;
				hero->health--;
			}
		}
	}
	//Invincibility count is active, do not check collisions
	else count--;


	//Check for button presses for new missile
	//Button needs to be held for 40ms (4 interrupts) before missile fires
	if (lp_io_read_pin(SW1_BIT)) {
		//Make sure missile only fires once per button press, gets stuck at 5.
		if (button_count <= 4) button_count++;
		//Fire missile
		if (button_count == 4) create_actor(MISSILE, hero->x_loc, hero->y_loc, hero->lr, hero->ud);
	}
	else button_count = 0;


	//Report aliveness (false for dead)
	if (hero->health) return false;
	else return true;
}

//Missile dies on contact with enemy, travels in straight line, and hurts enemies.
bool update_missile(actor_t *missile) {
	actor_t enemy = actors->next;

	//Check enemy collision
	while(enemy) {
		if (enemy->type != HERO && enemy->type != MISSILE && detect_collision(missile, enemy)) {
			enemy->health -= MISSILE_DAMAGE;
			return true;
		}
		enemy = enemy->next;
	}

	//Check if off screen
	if (at_edge_lr(missile) != IDLE || at_edge_ud(missile) != IDLE) {
		return true;
	}
	
	//Update position
	if (missile->lr == LEFT) missile->x_loc--;
	else missile->x_loc++;
	if (missile->ud == UP) missile->y_loc--;
	else missile->y_loc++;

	//If we get here missile is still alive
	return false;
}

//Zombies randomly sway towards player.
bool update_zombie(actor_t *zombie) {
	uint8_t direction_preference = rand() % PREFERENCE_MAX;
	actor_t *hero = actors; //Hero is head of actors list.

	//If dead, return dead.
	if (zombie->health <= 0) return true;

	if (direction_preference > PREFERENCE_CUTOFF) {
		if (zombie->x_loc > hero->x_loc) zombie->x_loc--;
		else if (zombie->x_loc < hero->x_loc) zombie->x_loc++;
	}
	else {
		if (zombie->y_loc > hero->y_loc) zombie->y_loc--;
		else if (zombie->y_loc < hero->y_loc) zombie->y_loc++;
	}
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
	if (bat->lr = LEFT) {
		bat->x_loc--;
		if (edge_lr == LEFT)
			bat->lr = RIGHT;
	}
	else if (bat->lr = RIGHT) {
		bat->x_loc++;
		if (edge_lr == RIGHT)
			bat->lr = LEFT;
	}

	if (bat->ud = UP) {
		bat->y_loc--;
		if (edge_ud == UP)
			bat->ud = DOWN;
	}
	else if (bat->ud = DOWN) {
		bat->y_loc++;
		if (edge_ud == RIGHT)
			bat->ud = UP;
	}
	return false;
}

//Blobs only move every so often. They have a counter that tells them when to move and when not to.
bool update_blob(actor_t *blob) {

	if (blob->health <= 0) return false;

	//Count reached, switch between moving and idle.
	if (blob->count == BLOB_COUNT) {
		if (blob->lr != IDLE || blob->ud != IDLE) {
			blob->lr = IDLE;
			blob->ud == IDLE;
		}
		else { //We are idle, need to pick a direction to hop;
			uint8_t hop_dir = rand();
			//If we are odd, hop horizontal. Else hop vertical
			if (hop_dir % 2) {
				//If we are greater or equal to threshold, hop right. Else hop left.
				if (hop_dir > HOP_THRESHOLD) blob->lr = RIGHT;
				else blob->lr = RIGHT;
			}
			else {
				//If we are greater or equal to threshold, hop down. Else hop up.
				if (hop_dir > HOP_THRESHOLD) blob->ud = DOWN;
				else blob->ud = UP;
			}
		}
		blob->count = 0;
	}
	else blob->count++;

	//Actually move.
	lr_t edge_lr = at_edge_lr(blob);
	ud_t edge_ud = at_edge_ud(blob);
	if (blob->lr == LEFT && edge_lr != LEFT) blob->x_loc--;
	else if (blob->lr == RIGHT && edge_lr != RIGHT) blob->x_loc++;
	else if (blob->ud == UP && edge_lr != UP) blob->y_loc--;
	else if (blob->ud == DOWN && edge_lr != DOWN) blob->y_loc++;
	return true;
}

//Mimics, surprisingly, mimic the movement of the player, but slower (same speed, faster? variants?)
bool update_mimic(actor_t *mimic) {

	if (mimic->health <= 0) return true;

	lr_t edge_lr = at_edge_lr(mimic);
	ud_t edge_ud = at_edge_ud(mimic);
	if (ps2_x > LEFT_THRESHOLD && edge_lr != LEFT) mimic->x_loc--;
	else if (ps2_x < RIGHT_THRESHOLD && edge_lr != RIGHT) mimic->x_loc++;
	if (ps2_y > UP_THRESHOLD && edge_ud != UP) mimic->y_loc--;
	else if (ps2_y < DOWN_THRESHOLD && edge_ud != DOWN) mimic->y_loc++;
	return false;
}

actor_t* create_actor(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud) {
	actor_t *actor = malloc(sizeof(actor));
	if (type == MISSILE) {
		actor->bitmap = &missleBitmap;
		actor->clear_bitmap = &missileErase;
		actor->height = MISSILE_HEIGHT;
		actor->width = MISSILE_WIDTH;
	}
	else if (type == ZOMBIE) {
		actor->bitmap = &zombieBitmap;
		actor->clear_bitmap = &zombieErase;
		actor->height = ZOMBIE_HEIGHT;
		actor->width = ZOMBIE_WIDTH;
		actor->health = ZOMBIE_HEALTH;
	}
	else if (type == BAT) {
		actor->bitmap = &batBitmap;
		actor->clear_bitmap = &batErase;
		actor->height = BAT_HEIGHT;
		actor->width = BAT_WIDTH;
		actor->health = BAT_HEALTH;
	}
	else if (type == BLOB) {
		actor->bitmap = &blobBitmap;
		actor->clear_bitmap = &blobErase;
		actor->height = BLOB_HEIGHT;
		actor->width = BLOB_WIDTH;
		actor->health = BLOB_HEALTH;
	}
	else if (typ == MIMIC) {
		actor->bitmap = &mimicBitmap;
		actor->clear_bitmap = &mimicErase;
		actor->height = MIMIC_HEIGHT;
		actor->width = MIMIC_WIDTH;
		actor->health = MIMIC_HEALTH;
	}

	actor->lr = lr;
	actor->ud = ud;
	actor->x_loc = x;
	actor->y_loc = y;
	actor->type = type;
	actor->count = 0;

	//Link into list after head
	actor->next = actors->next;
	actors->next = actor;

	return actor;
}




//Detects if an actor is at an edge.
//returns LEFT if the actor is at the left edge of the screen
//returns RIGHT if the actor is at the right edge of the screen
//else returns IDLE.
lr_t at_edge_lr(actor_t actor) {
	if (actor->x_loc < actor->width / 2) return LEFT;
	if (actor->y_loc > COLS - actor->width / 2) return RIGHT;
	return IDLE;
}

//Detects if an actor is at an edge.
//returns UP if the actor is at the top edge of the screen
//returns DOWN if the actor is at the bottom edge of the screen
//Else returns IDLE.
ud_t at_edge_ud(actor_t actor) {
	if (actor->y_loc < actor->height / 2) return UP;
	if (actor->y_loc > ROWS - actor->height / 2) return DOWN;
	return IDLE;
}

bool detect_collision(actor_t *a, actor_t *b) {
	uint16_t a_top, a_bottom, a_right, a_left;
	uint16_t b_top, b_bottom, b_right, b_left;
	a_top = a->y_loc - a->height;
	a_bottom = a->y_loc + a->height;
	a_left = a->x_loc - a->width;
	a_right = a->x_loc + a->width;

	b_tio = b->y_loc - b->height;
	b_bottom = b->y_loc + b->height;
	b_left = b->x_loc - b->width;
	b_right = b->x_loc + b->width;

	//If a's right is left of b's left
	//If a's left is right of b's right
	//If a's top is below b's bottom
	//If a's bottom is above b's top
	if(a_right < b_left || a_left > b_right || a_bottom < b_top || a_top > b_bottom) return false;
	else return true;
}

//Adds new missle object to actors
void fire_missle(void) {
	struct missle *newMissle = malloc(sizeof(struct missle));
	newMissle->x_loc = hero.x_loc;
	newMissle->y_loc = hero.y_loc - HERO_HEIGHT / 2;
	newMissle->nxt = NULL;
	if (m_head == NULL)
		m_head = newMissle;
	else
		m_tail->nxt = newMissle;
	m_tail = newMissle;
}

void destroy(actor_t *actor) {
	lcd_draw_image(
		actor->x_loc,	 // X Pos
		actor_width,	  // Image Horizontal Width
		actor->y_loc, // Y Pos
		actor_height,	 // Image Vertical Height
		actor->clear_bitmap,	   // Image
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
			actor_width,		 // Image Horizontal Width
			actor->y_loc,		 // Y Pos
			actor_height,		 // Image Vertical Height
			actor->clear_bitmap, // Image
			LCD_COLOR_WHITE,	 // Foreground Color
			LCD_COLOR_BLACK		 // Background Color
		);
		actor = actor->next;
	}
}