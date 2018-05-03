#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <stdint.h>
#include "timers.h"

//Defines notes by frequencies
#define C4 		261
#define C4_s	277
#define	D4		293
#define	E4_f	311
#define	E4		330
#define	F4		350
#define	F4_s	370
#define G4		392
#define G4_s	415
#define	A4		440
#define B4_f	466
#define B4		493
#define C5 		523
#define C5_s	554
#define	D5		587
#define	E5_f	622
#define	E5		659
#define	F5		699
#define	F5_s	740
#define G5		784
#define G5_s	831
#define	A5		880
#define B5_f	932
#define B5		988

//Sound effect selection
#define NOTHING 		0
#define TEAR_SOUND	1
#define ENEMY_SOUND	2
#define HURT_SOUND 	3
#define SONG				4

//Sound effect durations
#define NOTHING_DURATION	1
#define TEAR_DURATION			4
#define ENEMY_DURATION		5
#define HURT_DURATION			6
#define SONG_DURATION			8


//Picks which song should be played by next in sequenc
//Parameters: None
//Return: None
void next_in_sequence(void);
//Called by handler plays next note in list.
//Parameters: None
//Return: None
void play_sequence(uint8_t sequence);

#endif
