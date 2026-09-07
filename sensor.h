#ifndef SENSOR_H
#define SENSOR_H
#include <stdint.h>
#include <stdbool.h>
#define TEMP_CRITICAL_THRESHOLD_C_X10   500  
#define TEMP_HYSTERESIS_C_X10           450 

void Sensor_Init(void);
int16_t Sensor_ReadTemperature(void);
bool Sensor_IsCritical(int16_t temp);
void Hardware_Simulate_SetTemperature(int16_t raw_temp);

#endif