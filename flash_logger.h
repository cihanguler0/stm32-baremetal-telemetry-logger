#ifndef FLASH_LOGGER_H
#define FLASH_LOGGER_H
#include <stdint.h>
#include <stdbool.h>

#pragma pack(push, 1)

typedef struct {
    uint32_t timestamp_ms;
    int16_t  temperature;
    uint8_t  pwm_value;
    uint8_t  led_status;
    uint16_t crc;
} TelemetryLogPacket_t;

#pragma pack(pop)

#define FLASH_LOG_SECTOR        7
#define FLASH_LOG_START_ADDR    (0x08060000UL)
#define FLASH_LOG_MAX_PACKETS   1024

void Flash_Init(void);
bool Flash_EraseLogSector(void);
bool Flash_WritePacket(const TelemetryLogPacket_t *packet);
void Flash_DumpLogs(void);

#endif