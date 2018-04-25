#ifndef __MCP23017_H__
#define __MCP23017_H__


#include <stdint.h>
#include "i2c.h"
#include "gpio_port.h"

#define MCP24LC32AT_DEV_ID			0x50
#define EEPROM_TEST_NUM_BYTES    20

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
#define   EEPROM_I2C_SCL_PCTL_M     GPIO_PCTL_PA6_M
#define   EEPROM_I2C_SCL_PIN_PCTL 	GPIO_PCTL_PA6_I2C1SCL
#define   EEPROM_I2C_SDA_PCTL_M     GPIO_PCTL_PA7_M
#define   EEPROM_I2C_SDA_PIN_PCTL  	GPIO_PCTL_PA7_I2C1SDA

#define		IODIRB		0x10
#define		GPINTENB	0x12
#define		DEFVALB		0x13
#define		INTCONB		0x14
#define		IOCON			0x05
#define		GGPUB			0x16
#define		INTFB			0x17

//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       Data written to the EEPROM.
//
// Returns
// I2C_OK if the byte was written to the EEPROM.
//*****************************************************************************
i2c_status_t mcp_byte_write
( 
  uint32_t  i2c_base,
  uint16_t  mcpDataAddress,
  uint8_t   data
);

//*****************************************************************************
// Reads a single byte of data from the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       data read from the EEPROM is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the EEPROM.
//*****************************************************************************
i2c_status_t mcp_byte_read
( 
  uint32_t  i2c_base,
  uint16_t  address,
  uint8_t   *data
);

//*****************************************************************************
// Initialize the EEPROM peripheral
//*****************************************************************************
bool mcp_init(void);


#endif
