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