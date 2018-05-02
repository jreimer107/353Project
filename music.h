#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <stdint.h>
#include "timers.h"

extern const uint32_t nothing[][2];
extern const uint32_t tear_sound[][2];

void next_in_sequence(void);
void play_sequence(uint32_t (*sequence)[2], uint8_t size);

#endif
