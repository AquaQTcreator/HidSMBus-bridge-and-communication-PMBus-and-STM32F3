#ifndef CP2112_H
#define CP2112_H

#include "types.h"
#include "SLABCP2112.h"
#include "QDebug"
#include "SLABCP2112.h"

#define VID 0x10C4
#define PID 0xEA90

INT CP2112_Find1(); //Поиск сколько подключено
INT CP2112_Open(INT DevNumber, HID_SMBUS_DEVICE *device); //Открыть у-во
INT CP2112_SetConfig(HID_SMBUS_DEVICE device); //Задать конфг
INT CP2112_GetGpioConfig(HID_SMBUS_DEVICE device, BYTE *direction, BYTE *mode, BYTE *function); //ПОлучить конфиг на порты
INT CP2112_SetGpioConfig(HID_SMBUS_DEVICE device); //Задать конфиг на порты


#endif // CP2112_H
