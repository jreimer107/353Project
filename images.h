#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <stdint.h>
#include <stdio.h>

//Change these as needed for bitmaps
#define     HERO_HEIGHT 	24	// height of plane image in pixels
#define     HERO_WIDTH  	16	// width of plane image in pixels
#define		MISSLE_HEIGHT	11	// height of missle image in pixels
#define		MISSLE_WIDTH	7		// width of missle image in pixels
#define		BAT_HEIGHT		10
#define		BAT_WIDTH		16
#define		ZOMBIE_HEIGHT	24
#define		ZOMBIE_WIDTH	16


extern const uint8_t heroBitmap[];
extern const uint8_t missleBitmap[];
extern const uint8_t missleErase[];

#endif