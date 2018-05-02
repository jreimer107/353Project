#include "music.h"

//uint32_t playing[][2]; 
uint8_t curr_sound = 0;
uint8_t sequence_size = 0;
uint8_t currplaying = 0;
uint8_t song_place = 0;

const uint32_t nothing[][2] = {
	{0,1}
};

const uint32_t tear_sound[][2] = {
    {1000, 1},
    {500, 1},
	{250, 1},
	{0, 1}
};

const uint32_t enemy_sound[][2] = {
    {750, 1},
    {250, 1},
	{250, 1},
	{200, 1},
	{0, 1}
};

const uint32_t hurt_sound[][2] = {
	{D5,2},
	{C5_s,2},
	{D5,2},
	{C5_s,2},
	{D5,2},
	{C5_s,2}
};


const uint32_t song[][2] = {
	{D5,17},
	{A4,17},
	{B4_f,17},
	{G4, 17},
	{A4,17},
	{F4,17},
	{G4,17},
	{E4,17}
};


void play_sequence(uint8_t sequence) {
	currplaying = sequence;
	curr_sound = 0;
	if (sequence == 0) sequence_size = 1;
	else if (sequence == 1) sequence_size = 4;
	else if (sequence == 2) sequence_size = 5;
	else if (sequence == 3) sequence_size = 8;
	else if (sequence == 4) sequence_size = 6;
}


void next_in_sequence(void) {
	if(curr_sound <= sequence_size - 1) {
		if (currplaying == 1) {
			play_freq(TIMER1_BASE, tear_sound[curr_sound][0], tear_sound[curr_sound][1]);
			curr_sound++;
		}
		else if (currplaying == 0) {
			play_freq(TIMER1_BASE, nothing[curr_sound][0], nothing[curr_sound][1]);
			curr_sound++;
		}
		else if (currplaying == 2) {
			play_freq(TIMER1_BASE, enemy_sound[curr_sound][0], enemy_sound[curr_sound][1]);
			curr_sound++;
		}
		else if (currplaying == 3){
			play_freq(TIMER1_BASE, song[curr_sound][0],song[curr_sound][1]);
			song_place = (song_place + 1) % 8;
			curr_sound = (curr_sound + 1) % 8;
		}
		else if (currplaying == 4) {
			play_freq(TIMER1_BASE, hurt_sound[curr_sound][0], hurt_sound[curr_sound][1]);
			curr_sound++;
		}		
	}
	else {
		currplaying = 3;
		//song_place = (song_place + 1) % 8;	//UNCOMMENT FOR POSSIBLE SPACE INVADERS
		curr_sound = song_place;
		sequence_size = 8;
	}
	
}
