#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <stdint.h>
#include <stdio.h>

<<<<<<< HEAD
// Bitmap sizes for bat
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


#define       PLANE_HEIGHT 	58	// height of plane image in pixels
#define       PLANE_WIDTH  	47	// width of plane image in pixels
#define				MISSLE_HEIGHT	11	// height of missle image in pixels
#define				MISSLE_WIDTH	7		// width of missle image in pixels
=======
//Change these as needed for bitmaps
#define     HERO_HEIGHT 	24	// height of plane image in pixels
#define     HERO_WIDTH  	16	// width of plane image in pixels
#define		MISSLE_HEIGHT	11	// height of missle image in pixels
#define		MISSLE_WIDTH	7		// width of missle image in pixels
#define		BAT_HEIGHT		10
#define		BAT_WIDTH		16
#define		ZOMBIE_HEIGHT	24
#define		ZOMBIE_WIDTH	16
>>>>>>> e2e82bc2c6ebfeaaa292efdd85e8f850ad277a8e


extern const uint8_t heroBitmap[];
extern const uint8_t missleBitmap[];
extern const uint8_t missleErase[];
extern const uint8_t batBitmap[];
extern const uint8_t steveBitmap[];
extern const uint8_t tearBitmap[];
extern const uint8_t zombieBitmap[];

#endif