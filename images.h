#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <stdint.h>
#include <stdio.h>

#define       HERO_HEIGHT 	58	// height of plane image in pixels
#define       HERO_WIDTH  	47	// width of plane image in pixels
#define				MISSLE_HEIGHT	11	// height of missle image in pixels
#define				MISSLE_WIDTH	7		// width of missle image in pixels

extern const uint8_t heroBitmap[];
extern const uint8_t missleBitmap[];
extern const uint8_t missleErase[];

#endif