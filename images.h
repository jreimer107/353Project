#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <stdint.h>
#include <stdio.h>

// Bitmap sizes for bat
>>>>>>> Stashed changes
#define BAT_WIDTH 40
#define BAT_HEIGHT 22
//Bitmap sizes for Steve
#define STEVE_WIDTH 22
#define STEVE_HEIGHT 42
//Bitmap size for tear
#define TEAR_WIDTH 10
#define TEAR_HEIGHT 10
//Bitmap size for zombie
#define ZOMBIE_WIDTH 24
#define ZOMBIE_HEIGHT 46
//Bitmap size for slime
#define SLIME_WIDTH 12
#define SLIME_HEIGHT 12
//Bitmap size for mimic
#define MIMIC_WIDTH 41
#define MIMIC_HEIGHT 41


extern const uint8_t heroBitmap[];
extern const uint8_t batBitmap[];
extern const uint8_t tearBitmap[];
extern const uint8_t zombieBitmap[];
extern const uint8_t slimeBitmap[];

#endif