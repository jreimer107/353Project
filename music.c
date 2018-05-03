#include "music.h"

//These are needed to preserve/update state of sound as sound is updated by interrupts
uint8_t curr_sound = 0;			//Current note of sequence to play
uint8_t sequence_size = 0;	//Size of seqeunce that is playing
uint8_t currplaying = 0;		//Current sequence that is playing
uint8_t song_place = 0;			//Needed to preserve place in song when other tones are playing


//The following arrays are lists of frequencies and durations for
//sound effects through our game

//Plays nothing used to prevent garbage noise in beginning
const uint32_t nothing[][2] = {
	{0,1}
};

//Plays when a tear is fired
const uint32_t tear_sound[][2] = {
  {1000, 1},
  {500, 1},
	{250, 1},
	{0, 1}
};

//Plays when an enemy dies
const uint32_t enemy_sound[][2] = {
  {750, 1},
  {250, 1},
	{250, 1},
	{200, 1},
	{0, 1}
};

//Plays when the player is hurt
const uint32_t hurt_sound[][2] = {
	{D5,2},
	{C5_s,2},
	{D5,2},
	{C5_s,2},
	{D5,2},
	{C5_s,2}
};

//Constant background sound
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

//Picks which song should be played by next in sequenc
//Parameters: None
//Return: None
void play_sequence(uint8_t sequence) {
	//sets global variable to variable passed in
	currplaying = sequence;
	//starts from the beginning of the sound effect
	curr_sound = 0;
	//sets proper size based on sound effect
	if (sequence == NOTHING) sequence_size = NOTHING_DURATION;
	else if (sequence == TEAR_SOUND) sequence_size = TEAR_DURATION;
	else if (sequence == ENEMY_SOUND) sequence_size = ENEMY_DURATION;
	else if (sequence == HURT_SOUND) sequence_size = HURT_DURATION;
	else if (sequence == SONG) sequence_size = SONG_DURATION;
}

//Called by handler plays next note in list.
//Parameters: None
//Return: None
void next_in_sequence(void) {
	//plays effect only if there is a note to be played
	//currsound is incremented to get to next note.
	//currsound is modulated in the song to create a loop
	if(curr_sound <= sequence_size - 1) {
		if (currplaying == TEAR_SOUND) {
			play_freq(TIMER1_BASE, tear_sound[curr_sound][0], tear_sound[curr_sound][1]);
			curr_sound++;
		}
		else if (currplaying == NOTHING) {
			play_freq(TIMER1_BASE, nothing[curr_sound][0], nothing[curr_sound][1]);
			curr_sound++;
		}
		else if (currplaying == ENEMY_SOUND) {
			play_freq(TIMER1_BASE, enemy_sound[curr_sound][0], enemy_sound[curr_sound][1]);
			curr_sound++;
		}
		else if (currplaying == SONG){
			play_freq(TIMER1_BASE, song[curr_sound][0],song[curr_sound][1]);
			song_place = (song_place + 1) % 8;	//Song loops
			curr_sound = (curr_sound + 1) % 8;
		}
		else if (currplaying == HURT_SOUND) {
			play_freq(TIMER1_BASE, hurt_sound[curr_sound][0], hurt_sound[curr_sound][1]);
			curr_sound++;
		}		
	}
	//if an effect has ended the song is requeued
	else {
		currplaying = 3;
		//song_place = (song_place + 1) % 8;	//UNCOMMENT FOR POSSIBLE SPACE INVADERS
		curr_sound = song_place;
		sequence_size = 8;
	}
	
}
