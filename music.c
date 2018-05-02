#include "music.h"

uint32_t (*playing)[2]; 
uint8_t curr_sound = 0;
uint8_t sequence_size = 0;

const uint32_t nothing[][2] = {
	{0,1}
};

const uint32_t tear_sound[][2] = {
    {1000, 1},
    {500, 1},
	{250, 1},
	{0, 1}
};



void play_sequence(uint32_t (*sequence)[2], uint8_t size) {
	playing = sequence;
	curr_sound = 0;
	sequence_size = size;
}


void next_in_sequence(void) {
	if(curr_sound <= sequence_size - 1) {
		play_freq(TIMER1_BASE, playing[curr_sound][0], playing[curr_sound][1]);
		curr_sound++;
	}
}
