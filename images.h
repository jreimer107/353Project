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
#define		BLOB_HEIGHT		16
#define		BLOB_WIDTH		16
#define		MIMIC_HEIGHT	24
#define		MIMIC_WIDTH		16


extern const uint8_t heroBitmap[];
extern const uint8_t missleBitmap[];
extern const uint8_t missleErase[];
extern const uint8_t zombieBitmap[];
extern const uint8_t zombieErase[];
extern const uint8_t batBitmap[];
extern const uint8_t batErase[];
extern const uint8_t blobBitmap[];
extern const uint8_t blobErase[];
extern const uint8_t mimicBitmap[];
extern const uint8_t mimicErase[];

#endif