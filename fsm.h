#ifndef FSM_H
#define FSM_H
#include <stdint.h>
#include <stdbool.h>

typedef enum {

    STATE_IDLE = 0,
    STATE_LOGGING,
    STATE_DUMPING,
    STATE_ERASING,
    STATE_CRITICAL_TEMP

} FSM_State_t;

typedef enum {

    EVENT_NONE = 0,
    EVENT_CMD_START_LOG,
    EVENT_CMD_STOP_LOG,
    EVENT_CMD_DUMP,
    EVENT_CMD_ERASE,
    EVENT_OP_COMPLETE,
    EVENT_TEMP_CRITICAL,
    EVENT_TEMP_NORMAL

} FSM_Event_t;

typedef struct {

    FSM_State_t current_state;
    uint8_t pwm_value;
    uint32_t log_interval_ms;
    bool led_status;
    int16_t current_temperature;
    uint32_t last_log_timestamp;

} SystemContext_t;

void FSM_Init(SystemContext_t *ctx);

void FSM_HandleEvent(SystemContext_t *ctx, FSM_Event_t event);

void FSM_Update(SystemContext_t *ctx, uint32_t current_tick);

const char* FSM_StateToString(FSM_State_t state);

#endif