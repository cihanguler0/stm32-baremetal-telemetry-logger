#include "crc16.h"


/* *This one is for validation/verification. 
   *We check the written data is corrupted or not.
   *This function takes our 8 byte long data and performs polynomial division. Takes the remaining and makes it
   our verification code. */
uint16_t CRC16_Calculate(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    if (!data) return 0;

    /* *Checks every byte in our data. */
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)((uint8_t)data[i] << 8);
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}