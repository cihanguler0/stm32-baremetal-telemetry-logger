#include "sensor.h"

#define PERIPH_BASE             (0x40000000UL)
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x00010000UL)
/* 0x40010000 */
#define AHB1PERIPH_BASE         (PERIPH_BASE + 0x00020000UL)
/* 0x40020000 */

#define RCC_BASE                (AHB1PERIPH_BASE + 0x3800UL)
/* Base address of RCC Peripheral (Reset and Clock Control): 0x40023800 */
#define RCC_APB2ENR             (*(volatile uint32_t *)(RCC_BASE + 0x44UL))
/* Clock Enable Register of Peripherals of APB2 Bus: 0x40023844 */

#define ADC1_BASE               (APB2PERIPH_BASE + 0x2000UL)
/* 0x40012000 */
#define ADC1_SR                 (*(volatile uint32_t *)(ADC1_BASE + 0x00UL))
/* Status Register: 0x40012000*/
#define ADC1_CR1                (*(volatile uint32_t *)(ADC1_BASE + 0x04UL))
/* Control Register 1 — Resolution, Scan Mode, Interrupt: 0x40012004 */
#define ADC1_CR2                (*(volatile uint32_t *)(ADC1_BASE + 0x08UL))
/* Control Register 2 — ADON - ADC ON/OFF, SOFTWARE START - SWSTART, Trigger Source: 0x40012008 */
#define ADC1_SMPR1              (*(volatile uint32_t *)(ADC1_BASE + 0x0CUL))
/* Sample Time Register 1 (How much time should code wait before getting the first sample?): 0x4001200C */
#define ADC1_SQR1               (*(volatile uint32_t *)(ADC1_BASE + 0x2CUL))
/* Sequence Register 1, between 13-16: 0x4001202C */
#define ADC1_SQR3               (*(volatile uint32_t *)(ADC1_BASE + 0x34UL))
/* Sequence Register 3, starts from 1: 0x40012034 */
#define ADC1_DR                 (*(volatile uint32_t *)(ADC1_BASE + 0x4CUL))
/* Data Register - This is where output transformation has written: 	0x4001204C */
#define ADC_CCR                 (*(volatile uint32_t *)(APB2PERIPH_BASE + 0x2300UL + 0x04UL))
/* Common Control Register — prescaler (For speed control), dual/triple mode, TSVREFE (Tempearature Sensor): 0x40012304 */

void Sensor_Init(void) {

    /* Step 1: Acitvate the ADC1 Peripheral Clock, 8th bit. */
    RCC_APB2ENR |= (1UL << 8);

    /* Step 2: Activate the Temperature Sendor, 23rd bit. */
    ADC_CCR |= (1UL << 23);

    /* Step 3: Arrange Sample Time to 480 cycles for the Temperature Sensor (SMPR1[20:18] = 111) */
    ADC1_SMPR1 |= (7UL << 18);

    /* Step 4: Only read the 16th channel and do not change it. We want to read channel 16, because there is our
    teöperature sensor */
    ADC1_SQR3 = 16;
    ADC1_SQR1 = 0;

    /* Step 5: Activate ADC. (ADON: Bit 0) */
    ADC1_CR2 |= (1UL << 0);
}

int16_t Sensor_ReadTemperature(void) {

    /* Step 6: Software Start. Means "Start Measuring Temperature" (SWSTART: Bit 30) */
    ADC1_CR2 |= (1UL << 30);

    /* Step 7: ADC1_SR holds the flags (EOC: Bit 1)
       *EOC: 1 means result is ready in register. Automatically becomes 1 when conversation has done.
       *& means AND in logic design. We check only the true (1) bits.
       *With this, CPU checksthis line until EOC = 1 */
    while (!(ADC1_SR & (1UL << 1)));

    /* *Step 8: ADC1_DR is the data register address which the result is written
       *Only the first 12-bit is meaningful, that's why we mask it with 0x0FFF
       *Result is only digital right now. */
    uint16_t raw_adc = (uint16_t)(ADC1_DR & 0x0FFF);

    /*
     Step 9: The result we get from the sensor is a meaningless number between 0-4095
     *But there is a linear relationship between the number and 3.3 volts.
     *That's why we multiply it with 3300 and then divide it to 4095, to get result in mV.
     *Temperature (°C) = ((V_SENSE - V25) / Avg_Slope) + 25 formula is given in the datasheet to convert voltage
     to temperature.
     */
    int32_t v_sense_mv = ((int32_t)raw_adc * 3300) / 4095;
    int16_t temp_c_x10 = (int16_t)(((v_sense_mv - 760) * 10) / 25 + 250);

    /* We return it in int16_t fromat, because in this way we can show it in 0.1C sensivity. */
    return temp_c_x10;
}