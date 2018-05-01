#include "actors.h"

actor_t *actors; //Hero should be head node;
extern uint16_t ps2_x, ps2_y;

//Reorganizing stuff for speed
hero_t *hero;
enemy_t *enemy;
tear_t *tear;

uint8_t num_enemies;
uint8_t num_tears;

void actors_init() {
	num_enemies = 0;
	num_tears = 0;
	hero = malloc(sizeof(hero_t));
    hero->count = 0;
    hero->health = PLAYER_HEALTH;
    hero->lr = IDLE_lr;
    hero->ud = IDLE_ud;
    hero->move_count = 0;
    hero->x_loc = COLS / 2;
    hero->y_loc = ROWS / 2;
    enemy = calloc(sizeof(enemy_t) * MAX_ENEMIES);
	tear = calloc (sizeof(tear_t) * MAX_TEARS);
}

uint8_t update_actors(void) {
	update_hero();
	update_tears();
	return update_enemies();
}


uint8_t update_enemies() {
	uint8_t killed, i, j;
	//For all enemies that exist
	for (i = 0; i < num_enemies; i++){
		if (enemy[i].update(enemy[i])) {	//If enemy should be killed
			//Cycle enemies forward one in array
			for (j = i; j < num_enemies - 1; j++) {
				enemy[j] = enemy[j + 1];
			}
			//Decrement num enemies
			num_enemies--;
			killed++;	
		}
	}
	return killed;	//Return how many were killed to update rounds
}

void update_tears() {
	uint8_t i, j;
	for (i = 0; i < num_tears; i++) {
		if (update_tear(tear[i])) {
			for (j = i; j < num_tears - 1; j++) {
				tear[j] = tear[j + 1];
			}
			num_tears--;
		}
	}
}

//Hero has an invincibility counter that prevents it from being damaged continuously.
//It also fires tears.
bool update_hero() {
	uint8_t i;
	lr_t edge_lr = at_edge_lr(hero->x_loc, STEVE_WIDTH);
	ud_t edge_ud = at_edge_ud(hero->y_loc, STEVE_HEIGHT);

	if (!hero->health) {
		lcd_draw_image(hero->x_loc, STEVE_WIDTH, hero->y_loc, STEVE_HEIGHT, &steveBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		return true;
	}


	//Invincibility count is active, do not check collisions
	if (hero->count), hero->count--;

	//Move on speed interval
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
	}
	else hero->move_count++;

	//Report aliveness (false for alive)
	lcd_draw_image(hero->x_loc, STEVE_WIDTH, hero->y_loc, STEVE_HEIGHT, &steveBitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	
	return false;
}

//Missile dies on contact with enemy, travels in straight line, and hurts enemies.
bool update_tear(tear_t *tear) {
	//Check if off screen
	if (at_edge_lr(tear->x_loc, TEAR_WIDTH) != IDLE_lr || at_edge_ud(tear->y_loc, TEAR_HEIGHT) != IDLE_ud) {
		lcd_draw_image(tear->x_loc, TEAR_WIDTH, tear->y_loc, TEAR_HEIGHT, &tearBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		return true;
	}
	
	//Update position
	if(tear->move_count == TEAR_SPEED) {
		if (tear->lr == LEFT_d) tear->x_loc--;
		else if (tear->lr == RIGHT_d) tear->x_loc++;
		if (tear->ud == UP_d) tear->y_loc--;
		else if (tear->ud == DOWN_d) tear->y_loc++;
		tear->move_count = 0;
	}
	else tear->move_count++;

	//If we get here tear is still alive
	lcd_draw_image(tear->x_loc, TEAR_WIDTH, tear->y_loc, TEAR_HEIGHT, &tearBitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	return false;
}

//Zombies randomly sway towards player.
bool update_zombie(enemy_t *zombie) {
	uint8_t direction_preference;
	
	//Update collisions
	if (detect_all_collisions(zombie->x_loc, zombie->y_loc, ZOMBIE_HEIGHT, ZOMBIE_WIDTH)) {
		zombie->health--;
	}

	//If dead, return dead.
	if (zombie->health <= 0) {
		lcd_draw_image(zombie->x_loc, ZOMBIE_WIDTH, zombie->y_loc, ZOMBIE_HEIGHT, &zombieBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		return true;
	}

	//Move on speed interval
	direction_preference = rand() % PREFERENCE_MAX;
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

	lcd_draw_image(zombie->x_loc, ZOMBIE_WIDTH, zombie->y_loc, ZOMBIE_HEIGHT, &zombieBitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	return false;
}

//Bats bounce around the screen.
bool update_bat(enemy_t *bat) {
	//If given only one direction they will repeatedy go back and forth
	//If given two directions they will bounce around like tv screensavers
	lr_t edge_lr = at_edge_lr(bat->x_loc, BAT_WIDTH);
	ud_t edge_ud = at_edge_ud(bat->y_loc, BAT_HEIGHT);

	//Update collisions
	if (detect_all_collisions(bat->x_loc, bat->y_loc, BAT_HEIGHT, BAT_WIDTH)) {
		bat->health--;
	}

	//Detect dead
	if (bat->health <= 0) {
		lcd_draw_image(bat->x_loc, BAT_WIDTH, bat->y_loc, BAT_HEIGHT, &batBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
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
	}
	else bat->move_count++;

	lcd_draw_image(bat->x_loc, BAT_WIDTH, bat->y_loc, BAT_HEIGHT, &batBitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	return false;
}

//Slimes only move every so often. They have a counter that tells them when to move and when not to.
bool update_slime(enemy_t *slime) {
	lr_t edge_lr = at_edge_lr(slime->x_loc, SLIME_WIDTH);
	ud_t edge_ud = at_edge_ud(slime->y_loc, SLIME_HEIGHT);

	if (detect_all_collisions(slime->x_loc, slime->y_loc, SLIME_HEIGHT, SLIME_WIDTH)) {
		slime->health--;
	}

	//Detect dead
	if (slime->health <= 0) {
		lcd_draw_image(slime->x_loc, SLIME_WIDTH, slime->y_loc, SLIME_HEIGHT, &slimeBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
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
	}
	else slime->move_count++;

	lcd_draw_image(slime->x_loc, SLIME_WIDTH, slime->y_loc, SLIME_HEIGHT, &slimeBitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	return false;
}

//Mimics, surprisingly, mimic the movement of the player, but slower (same speed, faster? variants?)
bool update_mimic(enemy_t *mimic) {
	lr_t edge_lr = at_edge_lr(mimic->x_loc, MIMIC_WIDTH);
	ud_t edge_ud = at_edge_ud(mimic->y_loc, MIMIC_HEIGHT);

	//Detect collisions
	if (detect_all_collisions(mimic->x_loc, mimic->y_loc, MIMIC_HEIGHT, MIMIC_WIDTH)) {
		mimic->health--;
	}

	//Detect dead, unrender
	if (mimic->health <= 0) {
		lcd_draw_image(mimic->x_loc, MIMIC_WIDTH, mimic->y_loc, MIMIC_HEIGHT, &mimicBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		return true;
	}

	//move on speed interval
	if (mimic->move_count >= MIMIC_SPEED) {
		if (ps2_x > LEFT_THRESHOLD && edge_lr != LEFT_d) mimic->x_loc--;
		else if (ps2_x < RIGHT_THRESHOLD && edge_lr != RIGHT_d) mimic->x_loc++;
		if (ps2_y > UP_THRESHOLD && edge_ud != UP_d) mimic->y_loc--;
		else if (ps2_y < DOWN_THRESHOLD && edge_ud != DOWN_d) mimic->y_loc++;
		mimic->move_count = 0;
	}
	else mimic->move_count++;

	lcd_draw_image(mimic->x_loc, MIMIC_WIDTH, mimic->y_loc, MIMIC_HEIGHT, &mimicBitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	return false;
}

enemy_t* create_enemy(uint8_t type, uint16_t x, uint16_t y, lr_t lr, ud_t ud) {
	enemy_t *new = enemy[num_enemies];
	if (num_enemies == MAX_ENEMIES) return NULL;
	
	if (type == ZOMBIE) {
        new->health = ZOMBIE_HEALTH;
		new->update = update_zombie;
		new->draw = draw_zombie;
	}
	else if (type == BAT) {
		new->health = BAT_HEALTH;
		new->update = update_bat;
		new->draw = draw_bat;
	}
	else if (type == SLIME) {
		new->health = SLIME_HEALTH;
		new->update = update_slime;
		new->draw = draw_slime;
	}
	else if (type == MIMIC) {
		new->health = MIMIC_HEALTH;
		new->update = update_mimic;
		new->draw = draw_mimic;
	}

	new->lr = lr;
	new->ud = ud;
	new->x_loc = x;
	new->y_loc = y;
	new->count = 0;
	new->move_count = 0;

	num_enemies++;

	return new;
}

tear_t* create_tear(void) {
	tear_t *new = tear[num_tears];
	if (num_tears == MAX_TEARS) return NULL;
	new->x_loc = hero->x_loc;
	new->y_loc = hero->y_loc;
	new->lr = hero->lr;
	new->ud = hero->ud;
	new->move_count = 0;
	num_tears++
	return new;
}

//Detects if an actor is at an edge.
//returns LEFT if the actor is at the left edge of the screen
//returns RIGHT if the actor is at the right edge of the screen
//else returns IDLE.
lr_t at_edge_lr(uint16_t x_loc, uint8_t width) {
	if (x_loc < width / 2 + 2) return LEFT_d;
	if (x_loc > COLS - width / 2 - 2) return RIGHT_d;
	return IDLE_lr;
}

//Detects if an actor is at an edge.
//returns UP if the actor is at the top edge of the screen
//returns DOWN if the actor is at the bottom edge of the screen
//Else returns IDLE.
ud_t at_edge_ud(uint16_t y_loc, uint8_t height) {
	if (y_loc < height / 2  + 1) return UP_d;
	if (y_loc > ROWS - height / 2 - 1) return DOWN_d;
	return IDLE_ud;
}


bool detect_all_collisions(uint16_t x_loc, uint16_t y_loc, uint8_t height, uint8_t width) {
	//Hero collision, damage hero
	if (!hero->count && detect_collision(hero->x_loc, hero->y_loc, STEVE_HEIGHT, STEVE_WIDTH, x_loc, y_loc, height, width)) {
		hero->count = HERO_INVINCIBILITY;
		hero->health--;
	}
	//Tear collision, damage enemy
	for (i = 0; i < num_tears; i++) {
		if (detect_collision(tear[i].x_loc, tear[i].y_loc, TEAR_HEIGHT, TEAR_WIDTH, x_loc, y_loc, height, width)) {
			draw_tear(tear[i], LCD_COLOR_BLACK);
			for (j = i; j < num_tears - 1; j++) {
				tear[j] = tear[j + 1];
			}
			num_tears--;
			return true;
		}
	}
	return false;
}


bool detect_collision(uint16_t a_x, uint16_t a_y, uint8_t a_height, uint8_t a_width,
	uint16_t b_x, uint16_t b_y, uint8_t b_height, uint8_t b_width) {

	uint16_t a_top, a_bottom, a_right, a_left;
	uint16_t b_top, b_bottom, b_right, b_left;
	a_top = a_y - a_height/2;
	a_bottom = a_y + a_height/2;
	a_left = a_x - a_width/2;
	a_right = a_x + a_width/2;

	b_top = b_y - b_height/2;
	b_bottom = b_y + b_height/2;
	b_left = b_x - b_width/2;
	b_right = b_x + b_width/2;

	//If a's right is left of b's left
	//If a's left is right of b's right
	//If a's top is below b's bottom
	//If a's bottom is above b's top
	if(a_right < b_left || a_left > b_right || a_bottom < b_top || a_top > b_bottom) 
		return false;
	else return true;
}