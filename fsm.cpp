#include "fsm.h"
#include "sensor.h"

void FSM_Init(SystemContext_t *ctx) {

    /* *We use pointer, because we wantto change the original data. */

    /* *Null pointer guard. */
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

void FSM_Update(SystemContext_t *ctx, uint32_t current_tick) {

    if (ctx == nullptr) {
        return;
    }

    /* *Get the data from the temperature sensor. */

    ctx->current_temperature = Sensor_ReadTemperature();

    /* *Checks if the current temperature is critical and changes the state.
       *We use different temperatures for changing states, because we  want to block "Chattering".
       *We use a system called "Hysteresis" for this.
       *For a brief example, We use 50C to enter CRITICAL_TEMP state, but we wait to decrease 45C to NORMAL mode.
       because if we had a fluctuating temperature around 50C and used 50C as the limit for both situations, the 
       system would constantly switch states. As a result of this logs would become unnecessarily bloated, and our
       card would be damaged. */

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

            /* *We check the last log time with a formula of "Current Tick - Last Log Time Stamp" 
               If it equals to our Log_Interval_MS, it makes a log.*/
            if ((current_tick - ctx->last_log_timestamp) >= ctx->log_interval_ms) {
                ctx->last_log_timestamp = current_tick;
            }
            break;

        case STATE_DUMPING:

            FSM_HandleEvent(ctx, EVENT_OP_COMPLETE);
            break;

        case STATE_ERASING:
            FSM_HandleEvent(ctx, EVENT_OP_COMPLETE);
            break;

        case STATE_CRITICAL_TEMP:

            /* In critical temperatures, we reset PWM to 0 for caution. */
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