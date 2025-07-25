#ifndef STMCOMANDS_H
#define STMCOMANDS_H

#include "types.h"
#include "SLABCP2112.h"
#include "SMBusConfig.h"

UINT16 ReadWord(HID_SMBUS_DEVICE device, INT *data, INT battery_register,INT slave_address);
UINT16 ReadTextBlock(HID_SMBUS_DEVICE device, char *block, INT *block_length, INT battery_register);
UINT16 ReadDataBlock(HID_SMBUS_DEVICE device, BYTE *block, INT *block_length, INT battery_register);
UINT16 ReadByte(HID_SMBUS_DEVICE device, BYTE *data, INT battery_register,INT slave_address);

#endif // STMCOMANDS_H
