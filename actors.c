#include "actors.h"

actor_t *actors; //Hero should be head node;
extern uint16_t ps2_x, ps2_y;

//Main update function. Calls helper functions based on actor type.
//Helper functions must complete four tasks:
//1. Update the position of the actor.
//2. Update the actor's state based on any collisions if need be.
//3. Complete any type-specific actions.
//4. Report the aliveness of the actor.
//Parameters: None
//Returns:
//Number of enemies killed in this update
uint8_t update_actors() {
	actor_t *curr_actor = actors;
	actor_t *prev_actor = NULL;
	bool kill;
	uint8_t killed = 0;
	//updates the charachter based on their own update function.
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
			if (prev_actor) {
				prev_actor->next = curr_actor->next;
			}
			//If first actor(Steve) is gone, need to signal game over
			else return GAME_OVER; //Entire list freeing done in main
			
			//Free and disconnect dead actor
			if (curr_actor->type != TEAR) killed++;
			free(curr_actor);
			curr_actor = prev_actor;
		}

		//Next actor
		prev_actor = curr_actor;
		curr_actor = curr_actor->next;
	}
	return killed;
}

//Controls hero collision and movement
//Parameters:
//hero (*actor_t)				Pointer to main character (Steve!)
//Returns:
//true if hero has died
bool update_hero(actor_t *hero) {
	lr_t edge_lr = at_edge_lr(hero);
	ud_t edge_ud = at_edge_ud(hero);

	//Check if dead, unrender
	if (hero->health <= 0){
		lcd_draw_image(hero->x_loc, hero->width, hero->y_loc, hero->height, hero->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		return true;
	}

	//Update collisions
	if (!hero->count) { 
		actor_t *enemy = actors->next;	//First actor is hero
		while(enemy) {
			if (enemy->type != TEAR && !hero->count && detect_collision(hero, enemy)) {
				hero->count = HERO_INVINCIBILITY;
				hero->health--;
				play_sequence(HURT_SOUND);
			}
			enemy = enemy->next;
		}
	}
	//Invincibility count is active, do not check collisions
	else{
		hero->count--;
		if(hero->color == STEVE_COLOR_HURT && hero->count%16 == 0){
			hero->color = STEVE_COLOR;
		}
		else if(hero->count%16 == 0) hero->color = STEVE_COLOR_HURT;
	}

	//Move and draw on speed interval
	if (hero->move_count == PLAYER_SPEED) {
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
		lcd_draw_image(hero->x_loc, hero->width, hero->y_loc, hero->height, hero->bitmap, hero->color, LCD_COLOR_BLACK);
	}
	else hero->move_count++;

	//Report deadness
	return false;
}

//Missile dies on contact with enemy, travels in straight line(including diagonal), and hurts enemies.
//Parameters:
//tear (*actor_t)				Pointer to tear to be updated
//Returns:
//True if tear has "died"
bool update_tear(actor_t *tear) {
	actor_t *enemy = actors->next;

	//Check enemy collision
	while(enemy) {
		if (enemy->type != HERO && enemy->type != TEAR && detect_collision(tear, enemy)) {
			enemy->health -= TEAR_DAMAGE;
			lcd_draw_image(tear->x_loc, tear->width, tear->y_loc, tear->height, tear->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
			return true;
		}
		enemy = enemy->next;
	}

	//Check if off screen, kill + unrender if so
	if (at_edge_lr(tear) != IDLE_lr || at_edge_ud(tear) != IDLE_ud) {
		lcd_draw_image(tear->x_loc, tear->width, tear->y_loc, tear->height, tear->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		return true;
	}
	
	//Update position and render
	if(tear->move_count == TEAR_SPEED) {
		if (tear->lr == LEFT_d) tear->x_loc--;
		else if (tear->lr == RIGHT_d) tear->x_loc++;
		if (tear->ud == UP_d) tear->y_loc--;
		else if (tear->ud == DOWN_d) tear->y_loc++;
		tear->move_count = 0;
		lcd_draw_image(tear->x_loc, tear->width, tear->y_loc, tear->height, tear->bitmap, tear->color, LCD_COLOR_BLACK);
	}
	else tear->move_count++;

	//If we get here tear is still alive
	return false;
}

//Zombies randomly sway towards player.
//Parameters:
//zombie (*actor_t)				Pointer to zombie to be updated
//Returns:
//True if zombie has died
bool update_zombie(actor_t *zombie) {
	uint8_t direction_preference = rand() % PREFERENCE_MAX;
	actor_t *hero = actors; //Hero is head of actors list.

	//If dead, return dead.
	if (zombie->health <= 0) {
		lcd_draw_image(zombie->x_loc, zombie->width, zombie->y_loc, zombie->height, zombie->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		play_sequence(ENEMY_SOUND);
		return true;
	}
	
	//Move and render on speed interval
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
		lcd_draw_image(zombie->x_loc, zombie->width, zombie->y_loc, zombie->height, zombie->bitmap, zombie->color, LCD_COLOR_BLACK);
	}
	else zombie->move_count++;

	return false;
}

//Bats bounce around the screen.
//Parameters:
//bat (*actor_t)				Pointer to bat to be updated
//Returns:
//True if bat has died
bool update_bat(actor_t *bat) {
	//If given only one direction they will repeatedy go back and forth
	//If given two directions they will bounce around like tv screensavers
	lr_t edge_lr = at_edge_lr(bat);
	ud_t edge_ud = at_edge_ud(bat);

	//Death sequence
	if (bat->health <= 0) {
		lcd_draw_image(bat->x_loc, bat->width, bat->y_loc, bat->height, bat->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		play_sequence(ENEMY_SOUND);
		return true;
	}

	//Detect if at edge of screen, switch movement direction
	if (edge_lr == LEFT_d) bat->lr = RIGHT_d;
	if (edge_lr == RIGHT_d) bat->lr = LEFT_d;
	if (edge_ud == UP_d) bat->ud = DOWN_d;
	if (edge_ud == DOWN_d) bat->ud = UP_d;

	//Move at speed inteval
	if (bat->move_count == BAT_SPEED){
		if (bat->lr == LEFT_d) bat->x_loc--;
		else if (bat->lr == RIGHT_d) bat->x_loc++;
		if (bat->ud == UP_d) bat->y_loc--;
		else if (bat->ud == DOWN_d) bat->y_loc++;
		bat->move_count = 0;
        lcd_draw_image(bat->x_loc, bat->width, bat->y_loc, bat->height, bat->bitmap, bat->color, LCD_COLOR_BLACK);
    }
	else bat->move_count++;

	return false;
}

//Slimes only move every so often. They have a counter that tells them when to move and when not to.
//Parameters:
//slime (*actor_t)				Pointer to slime to be updated
//Returns:
//True if slime has died
bool update_slime(actor_t *slime) {
	lr_t edge_lr = at_edge_lr(slime);
	ud_t edge_ud = at_edge_ud(slime);

	if (slime->health <= 0) {
		lcd_draw_image(slime->x_loc, slime->width, slime->y_loc, slime->height, slime->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		play_sequence(ENEMY_SOUND);
		return true;
	}

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
				else slime->lr = LEFT_d;
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
		else if (slime->ud == UP_d && edge_ud != UP_d) slime->y_loc--;
		else if (slime->ud == DOWN_d && edge_ud != DOWN_d) slime->y_loc++;
		slime->move_count = 0;
		lcd_draw_image(slime->x_loc, slime->width, slime->y_loc, slime->height, slime->bitmap, slime->color, LCD_COLOR_BLACK);
		
	}
	else slime->move_count++;

	return false;
}

//Mimics, surprisingly, mimic the movement of the player, but faster
//Parameters:
//mimic (*actor_t)				Pointer to mimic to be updated
//Returns:
//True if mimic has died
bool update_mimic(actor_t *mimic) {
	lr_t edge_lr = at_edge_lr(mimic);
	ud_t edge_ud = at_edge_ud(mimic);
	
	//Death sequence
	if (mimic->health <= 0) {
		lcd_draw_image(mimic->x_loc, mimic->width, mimic->y_loc, mimic->height, mimic->bitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		play_sequence(ENEMY_SOUND);
		return true;
	}
	
	//move on speed interval
	if (mimic->move_count >= MIMIC_SPEED) {
		if (ps2_x > LEFT_THRESHOLD && edge_lr != LEFT_d) mimic->x_loc--;
		else if (ps2_x < RIGHT_THRESHOLD && edge_lr != RIGHT_d) mimic->x_loc++;
		if (ps2_y > UP_THRESHOLD && edge_ud != UP_d) mimic->y_loc--;
		else if (ps2_y < DOWN_THRESHOLD && edge_ud != DOWN_d) mimic->y_loc++;
		mimic->move_count = 0;
		lcd_draw_image(mimic->x_loc, mimic->width, mimic->y_loc, mimic->height, mimic->bitmap, mimic->color, LCD_COLOR_BLACK);
	}
	else mimic->move_count++;
	
	return false;
}

//Creates new montsters and tears by setting their initial values
//Parameters:
//type (uint8_t)		Decides which actor will be spawned	
//x (uint16_t)			X location of actor
//y (uint16_t)			Y location of actor
//lr (lr_t)					Direction the actor is facing, left/right
//ud (ud_t)					Direction the actor is facing, up/down
//Returns:
//Pointer to the actor that was created
actor_t* create_actor(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud) {
	actor_t *actor = malloc(sizeof(actor_t));
	if (type == TEAR) {
		actor->bitmap = (uint8_t*)tearBitmap;
		actor->height = TEAR_HEIGHT;
		actor->width = TEAR_WIDTH;
		actor->color = TEAR_COLOR;
	}
	else if (type == ZOMBIE) {
		actor->bitmap = (uint8_t*)zombieBitmap;
		actor->height = ZOMBIE_HEIGHT;
		actor->width = ZOMBIE_WIDTH;
		actor->health = ZOMBIE_HEALTH;
		actor->color = ZOMBIE_COLOR;
	}
	else if (type == BAT) {
		if (lr == IDLE_lr && lr == IDLE_ud) ud = UP_d;
		actor->bitmap = (uint8_t*)batBitmap;
		actor->height = BAT_HEIGHT;
		actor->width = BAT_WIDTH;
		actor->health = BAT_HEALTH;
		actor->color = BAT_COLOR;	
	}
	else if (type == SLIME) {
		actor->bitmap = (uint8_t*)slimeBitmap;
		actor->height = SLIME_HEIGHT;
		actor->width = SLIME_WIDTH;
		actor->health = SLIME_HEALTH;
		actor->color = SLIME_COLOR;
	}
	else if (type == MIMIC) {
		actor->bitmap = (uint8_t*)mimicBitmap;
		actor->height = MIMIC_HEIGHT;
		actor->width = MIMIC_WIDTH;
		actor->health = MIMIC_HEALTH;
		actor->color = MIMIC_COLOR;
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
//Parameters:
//actor (*actor_t)			Pointer to current actor
//Returns:
//LEFT if the actor is at the left edge of the screen
//RIGHT if the actor is at the right edge of the screen
//else IDLE
lr_t at_edge_lr(actor_t *actor) {
	if (actor->x_loc < actor->width / 2 + 2) return LEFT_d;
	if (actor->x_loc > COLS - actor->width / 2 - 2) return RIGHT_d;
	return IDLE_lr;
}

//Detects if an actor is at an edge.
//Parameters:
//actor (*actor_t)			Pointer to current actor
//Returns:
//UP if the actor is at the top edge of the screen
//DOWN if the actor is at the bottom edge of the screen
//returns IDLE.
ud_t at_edge_ud(actor_t *actor) {
	if (actor->y_loc < actor->height / 2  + 20) return UP_d;
	if (actor->y_loc > ROWS - actor->height / 2 - 1) return DOWN_d;
	return IDLE_ud;
}

//Detects if two actors are colliding
//Parameters:
//a (*actor_t)			First actor for collision test
//b (*actor_t)			Second actor for collision test
//Returns:
//True if colliding
bool detect_collision(actor_t *a, actor_t *b) {
	uint16_t a_top, a_bottom, a_right, a_left;
	uint16_t b_top, b_bottom, b_right, b_left;
	a_top = a->y_loc - a->height/2;
	a_bottom = a->y_loc + a->height/2;
	a_left = a->x_loc - a->width/2;
	a_right = a->x_loc + a->width/2;

	b_top = b->y_loc - b->height/2;
	b_bottom = b->y_loc + b->height/2;
	b_left = b->x_loc - b->width/2;
	b_right = b->x_loc + b->width/2;

	//If a's right is left of b's left
	//If a's left is right of b's right
	//If a's top is below b's bottom
	//If a's bottom is above b's top
	if(a_right < b_left || a_left > b_right || a_bottom < b_top || a_top > b_bottom) 
		return false;
	else return true;
}

//Creates the main character at the start of the game
//Parameters: None
//Returns: None
void hero_init(void){
	actor_t *hero;
	actors = malloc(sizeof(actor_t));
	hero = actors;
	hero->bitmap = (uint8_t*)&heroBitmap;
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
	hero->color = STEVE_COLOR;
	hero->next = NULL;
}

