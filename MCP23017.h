#ifndef __MCP23017_H__
#define __MCP23017_H__


#include <stdint.h>
#include "i2c.h"
#include "gpio_port.h"

//Look at the schematic for reasoning.
#define MCP23017_DEV_ID			0x27

//*****************************************************************************
// Fill out the #defines below to configure which pins are connected to
// the I2C Bus
//*****************************************************************************
#define   MCP_GPIO_BASE         	GPIOF_BASE
#define   MCP_I2C_BASE          	I2C1_BASE
#define   MCP_I2C_SCL_PIN       	PA6
#define  	MCP_I2C_SDA_PIN       	PA7
#define		SW2_IO_EXPANDER_INT			19
#define		UP_BUTTON_PIN						1
#define		DOWN_BUTTON_PIN					2
#define		LEFT_BUTTON_PIN					3
#define		RIGHT_BUTTON_PIN				4

#define		IODIRB		0x01	//IO direction
#define		GPINTENB	0x05	//Interupt enable
#define		INTCONB		0x09	//Interupt on change
#define		GGPUB			0x0D	//Pull Up resistors
#define 	GPIOBMCP	0x13		//GPIO Pin

//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the MCP23017
//
//    data:       Data written to the MCP23017.
//
// Returns
// I2C_OK if the byte was written to the MCP23017.
//*****************************************************************************
i2c_status_t mcp_byte_write
( 
  uint32_t  i2c_base,
  uint8_t  mcpDataAddress,
  uint8_t   data
);

//*****************************************************************************
// Reads a single byte of data from the MCP23017.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the MCP23017
//
//    data:       data read from the MCP23017 is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the MCP23017.
//*****************************************************************************
i2c_status_t mcp_byte_read
( 
  uint32_t  i2c_base,
  uint8_t  address,
  uint8_t   *data
);

//*****************************************************************************
// Initialize the I2C peripheral
// Parameters: None
// Returns true if the writes were completed successfully
//*****************************************************************************
bool mcp_init(void);


#endif
