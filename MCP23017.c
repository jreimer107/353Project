#include "MCP23017.h"
  
  
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
  uint8_t  address,
  uint8_t   data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // Set the I2C address to be the EEPROM
	// ADD CODE
  //==============================================================
		status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
  
		
  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================
  status = i2cSendByte(i2c_base, (uint8_t)address, I2C_MCS_START | I2C_MCS_RUN);
	
  //==============================================================
  // Send the Byte of data to write
	// ADD CODE
  //==============================================================
	status = i2cSendByte(i2c_base, data, I2C_MCS_RUN| I2C_MCS_STOP);

  return status;
}

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
  uint8_t  address,
  uint8_t   *data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};


  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Write Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);

  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================
	status = i2cSendByte(i2c_base, address, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
		
  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Read Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_READ);

  //==============================================================
  // Read the data returned by the EEPROM
	// ADD CODE
  //==============================================================
  status = i2cGetByte(i2c_base, data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
  return status;
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************

bool mcp_init(void)
{
	if(mcp_byte_write(MCP_I2C_BASE, IODIRB, 0xFF) != I2C_OK)
	{
		return false;
	}
	if(mcp_byte_write(MCP_I2C_BASE, GPINTENB, 0x0F) != I2C_OK)
	{
		return false;
	}
	if(mcp_byte_write(MCP_I2C_BASE, INTCONB, 0x00) != I2C_OK){
		return false;
	}
	if(mcp_byte_write(MCP_I2C_BASE, GGPUB, 0xFF) != I2C_OK){
		return false;
	}
	
	return true;
}

