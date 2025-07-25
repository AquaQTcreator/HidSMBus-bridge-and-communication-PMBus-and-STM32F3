#ifndef SMBUSCONFIG_H
#define SMBUSCONFIG_H

#include "types.h"

#define NUMBER_OF_SBS_ADDRESSES 6

// Режим работы шины SMBUS
#define BITRATE_HZ                  100000 //Скорость бит
#define ACK_ADDRESS                 0x02 // адрес мастера
#define AUTO_RESPOND                FALSE //автоответ
#define WRITE_TIMEOUT_MS            1000 // Время на запись
#define READ_TIMEOUT_MS             1000 //Время на чтение
#define TRANSFER_RETRIES            0 //кол-во попыток на отправку
#define SCL_LOW_TIMEOUT             TRUE //тайм-аут, который сбрасывает шину SMBus, если линия SCL удерживается на низком уровне более 25мс.
#define RESPONSE_TIMEOUT_MS         1000 //время на ответ

#define SLAVE_ADDRESS               0x40//0x20 для stm32f3
#define MAX_SMBUS_BLOCK_SIZE        32 //макс размер буфера

#endif // SMBUSCONFIG_H
