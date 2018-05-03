#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdbool.h>
#include <stdint.h>

#include "driver_defines.h"

//Base period of our timers
#define TICKS 62500

//*****************************************************************************
// Configure a general purpose timer to be a 32-bit timer.  
//
// Paramters
//  base_address          The base address of a general purpose timer
//
//  mode                  bit mask for Periodic, One-Shot, or Capture
//
//  count_up              When true, the timer counts up.  When false, it counts
//                        down
//
//  enable_interrupts     When set to true, the timer generates and interrupt
//                        when the timer expires.  When set to false, the timer
//                        does not generate interrupts.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_config_32(uint32_t base_addr, uint32_t mode, bool count_up, bool enable_interrupts);


//*****************************************************************************
// Waits for 'ticks' number of clock cycles and then returns.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_wait(uint32_t base_addr, uint32_t ticks);

//*****************************************************************************
// Configure a general purpose timer to be a 16-bit timer.  
//
// Paramters
//  base_address          The base address of a general purpose timer
//
//  mode                  bit mask for Periodic, One-Shot, or Capture
//
//  count_up              When true, the timer counts up.  When false, it counts
//                        down
//
//  enable_interrupts     When set to true, the timer generates and interrupt
//                        when the timer expires.  When set to false, the timer
//                        does not generate interrupts.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_config_16(uint32_t base_addr, uint32_t mode, bool count_up, bool enable_interrupts);

//*****************************************************************************
// This function sets the prescalar and Interval Load Register of the 16 bit timers.
//
// Paramters
//	timer
//
//	prescalar_value (uint16_t)
//
//	ILR_value (uint16_t)
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_start_16(uint32_t base_addr, uint8_t prescalerA_value, uint8_t prescalerB_value, uint16_t ILRA_value, uint16_t ILRB_value);

//*****************************************************************************
//Configures a general purpose timer as a PWM
//Parameters:
//base_addr (uint32_t)				Base address of general purpose timer being used
//Returns true if the timer is valid.
//*****************************************************************************
bool pwm_timer_config(uint32_t base_addr);

//*****************************************************************************
//Plays a note with given frequency and duration
//Parameters:
//base_addr (uint32_t)					Base address of the timer being used.
//frequency (uint32_t)					Frequency (used to determine pitch) of the note
//duration (uint32_t)						Multiplier, 1 is a few milliseconds long, 2 is twice as long etc. Max at 17
//Returns true if the timer is valid.
//*****************************************************************************
bool play_freq(uint32_t base_addr, uint32_t frequency, uint32_t duration);

#endif
