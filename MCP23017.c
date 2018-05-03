#include "MCP23017.h"
  
  
//*****************************************************************************
// Writes a single byte of data out to the MCP23017.  
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
  uint8_t  address,
  uint8_t   data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  // Set the I2C address to be the MCP23017
		status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
  
  // Send the Lower byte of the address
  status = i2cSendByte(i2c_base, (uint8_t)address, I2C_MCS_START | I2C_MCS_RUN);

  // Send the Byte of data to write

	status = i2cSendByte(i2c_base, data, I2C_MCS_RUN| I2C_MCS_STOP);

  return status;
}

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
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};


  // Set the I2C slave address to be the MCP23017 and in Write Mode

	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);

  // Send the Lower byte of the address
	status = i2cSendByte(i2c_base, address, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
		
  // Set the I2C slave address to be the EEPROM and in Read Mode
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_READ);
		
  // Read the data returned by the EEPROM
  status = i2cGetByte(i2c_base, data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
  return status;
}

//*****************************************************************************
// Initialize the I2C peripheral
// Parameters: None
// Returns true if the writes were completed successfully
//*****************************************************************************
bool mcp_init(void)
{	
	//configures mcp pins to input
	if(mcp_byte_write(MCP_I2C_BASE, IODIRB, 0xFF) != I2C_OK)
	{
		return false;
	}
	//configures mcp interrupt enable
	if(mcp_byte_write(MCP_I2C_BASE, GPINTENB, 0x0F) != I2C_OK)
	{
		return false;
	}
	//configures the mcp to interrupt when pins change
	if(mcp_byte_write(MCP_I2C_BASE, INTCONB, 0x00) != I2C_OK){
		return false;
	}
	//Enables pull up resistors
	if(mcp_byte_write(MCP_I2C_BASE, GGPUB, 0xFF) != I2C_OK){
		return false;
	}
	
	return true;
}

