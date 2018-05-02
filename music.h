#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <stdint.h>
#include "timers.h"

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




extern const uint32_t nothing[][2];
extern const uint32_t tear_sound[][2];
extern const uint32_t enemy_sound[][2];

void next_in_sequence(void);
void play_sequence(uint8_t sequence);

#endif
