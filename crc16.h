#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>
#include <stddef.h>

uint16_t CRC16_Calculate(const uint8_t *data, size_t length);

#endif