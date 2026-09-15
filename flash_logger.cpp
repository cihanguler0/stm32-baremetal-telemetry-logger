#include "flash_logger.h"
#include "crc16.h"
#include "bsp_uart.h"
#include <stdio.h>

/* Flash Interface Base Address.*/
#define FLASH_R_BASE            (0x40023C00UL)
/* Protects Flash from writing/ deleting actions. Needs a key to unlock. */
#define FLASH_KEYR              (*(volatile uint32_t *)(FLASH_R_BASE + 0x04UL))
/* Holds Flash's current flag. */
#define FLASH_SR                (*(volatile uint32_t *)(FLASH_R_BASE + 0x0CUL))
/* Starts Flash's processes */
#define FLASH_CR                (*(volatile uint32_t *)(FLASH_R_BASE + 0x10UL))
/* Keys to open FLASH_KEYR. */
#define FLASH_KEY1              (0x45670123UL)
#define FLASH_KEY2              (0xCDEF89ABUL)

/*  These are static points in RAM that are tracked to indicate the next packet to be 
    written to in persistent memory.*/
static uint32_t s_current_write_addr = FLASH_LOG_START_ADDR;

static void Flash_Unlock(void) {
    if (FLASH_CR & (1UL << 31)) {
        FLASH_KEYR = FLASH_KEY1;
        FLASH_KEYR = FLASH_KEY2;
    }
}

/* After using writing and erasing function automatically locks again. */
static void Flash_Lock(void) {
    FLASH_CR |= (1UL << 31);
}

/* Bit 16 = 1 (BUSY) 
   When charge is transferred to flash cells or sectors are charged, the hardware sets this bit to 1. 
   The processor waits here until it finishes the Flash erase or write operation and sets the BSY bit to 0. */
static void Flash_WaitForLastOperation(void) {
    while (FLASH_SR & (1UL << 16));
}

void Flash_Init(void) {
    s_current_write_addr = FLASH_LOG_START_ADDR;
    while (s_current_write_addr < (FLASH_LOG_START_ADDR + (FLASH_LOG_MAX_PACKETS * sizeof(TelemetryLogPacket_t)))) {
        uint32_t *ptr = (uint32_t *)s_current_write_addr;
        if (*ptr == 0xFFFFFFFF) {
            break;
        }
        s_current_write_addr += sizeof(TelemetryLogPacket_t);
    }
}

bool Flash_EraseLogSector(void) {
    Flash_Unlock();
    Flash_WaitForLastOperation();

    FLASH_CR &= ~((0x1FUL << 3) | (3UL << 8));
    FLASH_CR |= (FLASH_LOG_SECTOR << 3);
    FLASH_CR |= (1UL << 1);
    FLASH_CR |= (1UL << 16);

    Flash_WaitForLastOperation();
    FLASH_CR &= ~(1UL << 1);
    Flash_Lock();

    s_current_write_addr = FLASH_LOG_START_ADDR;
    return true;
}

bool Flash_WritePacket(const TelemetryLogPacket_t *packet) {
    if (!packet) return false;
    if (s_current_write_addr >= (FLASH_LOG_START_ADDR + (FLASH_LOG_MAX_PACKETS * sizeof(TelemetryLogPacket_t)))) {
        return false;
    }

    TelemetryLogPacket_t log_to_write = *packet;
    log_to_write.crc = CRC16_Calculate((const uint8_t *)&log_to_write, sizeof(TelemetryLogPacket_t) - sizeof(uint16_t));

    Flash_Unlock();
    Flash_WaitForLastOperation();

    FLASH_CR &= ~(3UL << 8);

    const uint8_t *byte_ptr = (const uint8_t *)&log_to_write;
    for (size_t i = 0; i < sizeof(TelemetryLogPacket_t); i++) {
        FLASH_CR |= (1UL << 0);
        *(volatile uint8_t *)(s_current_write_addr + i) = byte_ptr[i];
        Flash_WaitForLastOperation();
        FLASH_CR &= ~(1UL << 0);
    }

    Flash_Lock();
    s_current_write_addr += sizeof(TelemetryLogPacket_t);
    return true;
}

void Flash_DumpLogs(void) {
    uint32_t read_addr = FLASH_LOG_START_ADDR;
    uint32_t count = 0;

    printf("\n--- FLASH LOG DUMP BASLANGICI ---\n");
    while (read_addr < s_current_write_addr) {
        const TelemetryLogPacket_t *pkt = (const TelemetryLogPacket_t *)read_addr;
        if (*(const uint32_t *)read_addr == 0xFFFFFFFF) {
            break;
        }

        uint16_t calc_crc = CRC16_Calculate((const uint8_t *)pkt, sizeof(TelemetryLogPacket_t) - sizeof(uint16_t));
        bool valid = (calc_crc == pkt->crc);

        printf("[%u] Zaman: %u ms | Sicaklik: %d.%d C | PWM: %u | LED: %s | CRC: %s (0x%04X)\n",
               count++,
               pkt->timestamp_ms,
               pkt->temperature / 10,
               pkt->temperature % 10,
               pkt->pwm_value,
               pkt->led_status ? "ON" : "OFF",
               valid ? "GECERLI" : "HATALI",
               pkt->crc);

        read_addr += sizeof(TelemetryLogPacket_t);
    }
    printf("--- TOPLAM %u KAYIT DUMP EDILDI ---\n\n", count);
}