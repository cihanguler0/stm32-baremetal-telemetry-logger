#include "fsm.h"
#include "sensor.h"
#include <stdio.h>

void FSM_Init(SystemContext_t *ctx) {

    if (ctx == nullptr) {
        return;
    }

    Sensor_Init();

    ctx->current_state = STATE_IDLE;
    ctx->pwm_value = 0;
    ctx->log_interval_ms = 1000;
    ctx->led_status = false;
    ctx->current_temperature = Sensor_ReadTemperature();
    ctx->last_log_timestamp = 0;
}

void FSM_HandleEvent(SystemContext_t *ctx, FSM_Event_t event) {

    if (ctx == nullptr || event == EVENT_NONE) {
        return;
    }

    /* This part is in outside of switch block, because critical temperature state always in first priority.*/
    if (event == EVENT_TEMP_CRITICAL) {

        if (ctx->current_state != STATE_CRITICAL_TEMP) {

            ctx->current_state = STATE_CRITICAL_TEMP;
            ctx->pwm_value = 0;
            ctx->led_status = true;
            printf("[ALARM] CRITICAL TEMPERATURE REACHED! OUTPUTS DISABLED.\n");
        }
        return;
    }

    switch (ctx->current_state) {

        case STATE_IDLE:

            if (event == EVENT_CMD_START_LOG) {
                ctx->current_state = STATE_LOGGING;
                printf("[FSM] STATE -> LOGGING\n");

            } else if (event == EVENT_CMD_DUMP) {
                ctx->current_state = STATE_DUMPING;
                printf("[FSM] STATE -> DUMPING\n");

            } else if (event == EVENT_CMD_ERASE) {
                ctx->current_state = STATE_ERASING;
                printf("[FSM] STATE -> ERASING\n");
            }
            break;

        case STATE_LOGGING:

            if (event == EVENT_CMD_STOP_LOG) {
                ctx->current_state = STATE_IDLE;
                printf("[FSM] STATE -> IDLE\n");
            }
            break;

        case STATE_DUMPING:

            if (event == EVENT_OP_COMPLETE) {
                ctx->current_state = STATE_IDLE;
                printf("[FSM] DUMP COMPLETE -> IDLE\n");
            }
            break;

        case STATE_ERASING:

            if (event == EVENT_OP_COMPLETE) {
                ctx->current_state = STATE_IDLE;
                printf("[FSM] ERASE COMPLETE -> IDLE\n");
            }
            break;

        case STATE_CRITICAL_TEMP:

            if (event == EVENT_TEMP_NORMAL) {
                ctx->current_state = STATE_IDLE;
                ctx->led_status = false;
                printf("[FSM] TEMPERATURE BACK TO NORMAL -> IDLE\n");
            }
            break;

        default:
            break;
    }
}

void FSM_Update(SystemContext_t *ctx, uint32_t current_tick) {

    if (ctx == nullptr) {
        return;
    }

    ctx->current_temperature = Sensor_ReadTemperature();

    if (ctx->current_state != STATE_CRITICAL_TEMP) {

        if (Sensor_IsCritical(ctx->current_temperature)) {
            FSM_HandleEvent(ctx, EVENT_TEMP_CRITICAL);
        }

    } else {

        if (ctx->current_temperature < TEMP_HYSTERESIS_C_X10) {
            FSM_HandleEvent(ctx, EVENT_TEMP_NORMAL);
        }
    }

    switch (ctx->current_state) {

        case STATE_LOGGING:

            if ((current_tick - ctx->last_log_timestamp) >= ctx->log_interval_ms) {
                ctx->last_log_timestamp = current_tick;
                printf("[LOG @ %u ms] Temp: %d.%d C | PWM: %u | LED: %s\n",
                       current_tick,
                       ctx->current_temperature / 10,
                       ctx->current_temperature % 10,
                       ctx->pwm_value,
                       ctx->led_status ? "ON" : "OFF");
            }
            break;

        case STATE_DUMPING:
            printf("ALL DATA IS BEING TRANSFERRED TO PC\n");
            FSM_HandleEvent(ctx, EVENT_OP_COMPLETE);
            break;

        case STATE_ERASING:
            printf("ALL PERMANENT DATA IS BEING ERASED\n");
            FSM_HandleEvent(ctx, EVENT_OP_COMPLETE);
            break;

        case STATE_CRITICAL_TEMP:
            ctx->pwm_value = 0;
            break;

        case STATE_IDLE:
        default:
            break;
    }
}

const char* FSM_StateToString(FSM_State_t state) {
    switch (state) {

        case STATE_IDLE:          return "IDLE";
        case STATE_LOGGING:       return "LOGGING";
        case STATE_DUMPING:       return "DUMPING";
        case STATE_ERASING:       return "ERASING";
        case STATE_CRITICAL_TEMP: return "CRITICAL_TEMP";
        default:                  return "UNKNOWN";
    }
}