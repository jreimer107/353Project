#include "music.h"

//uint32_t playing[][2]; 
uint8_t curr_sound = 0;
uint8_t sequence_size = 0;
uint8_t currplaying = 0;

const uint32_t nothing[][2] = {
	{0,1}
};

const uint32_t tear_sound[][2] = {
    {1000, 100},
    {500, 100},
	{250, 100},
	{0, 1}
};




void play_sequence(uint8_t sequence) {
	currplaying = sequence;
	curr_sound = 0;
	if (sequence == 0) sequence_size = 1;
	else if (sequence == 1) sequence_size = 4;
}


void next_in_sequence(void) {
	if(curr_sound <= sequence_size - 1) {
		if (currplaying == 1) {
			play_freq(TIMER1_BASE, tear_sound[curr_sound][0], tear_sound[curr_sound][1]);
		}
		else if (currplaying == 0) {
			play_freq(TIMER1_BASE, nothing[curr_sound][0], nothing[curr_sound][1]);
		}
		curr_sound++;
	}
	
}
