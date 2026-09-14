#include "cli.h"
#include <string.h>
#include <stdio.h>

static RingBuffer_t *s_rx_buffer = nullptr;
static SystemContext_t *s_ctx = nullptr;

/* *CLI copies it's own statick variables, so everytime it gets called it can reach these datas without 
   dealing with the parameters. */
void CLI_Init(RingBuffer_t *rb, SystemContext_t *ctx) {
    s_rx_buffer = rb;
    s_ctx = ctx;
}

void CLI_Process(void) {

    if (s_rx_buffer == nullptr || s_ctx == nullptr) {
        return;
    }

    static char cmd_line[32];
    static uint8_t cmd_index = 0;
    static bool overflow_flag = false;
    uint8_t rx_byte = 0;

    /* *Takes a byte from ring buffer and writes it to rx_byte. Thisl loop goes on until every data in the buffer
       gets checked. */
    while (RingBuffer_Pop(s_rx_buffer, &rx_byte)) {

        if (rx_byte == '\n') {
            if (overflow_flag) {
                overflow_flag = false;
                cmd_index = 0;
                continue;
            }

            cmd_line[cmd_index] = '\0';

            if (cmd_index > 0) {
                char cmd[16];
                int value = 0;
                int matched = sscanf(cmd_line, "%15s %d", cmd, &value);

                if (matched > 0) {

                    if (strcmp(cmd, "SET_PWM") == 0) {

                        if (s_ctx->current_state == STATE_CRITICAL_TEMP) {
                            printf("ERROR: CANNOT CHANGE PWM IN CRITICAL TEMP\n");

                        } else if (matched == 2 && value >= 0 && value <= 255) {
                            s_ctx->pwm_value = (uint8_t)value;
                            printf("PWM HAS SET: %d\n", s_ctx->pwm_value);

                        } else {
                            printf("UNDEFINED PWM VALUE (0-255)\n");
                        }
                    }
                    else if (strcmp(cmd, "LOG_INTERVAL") == 0) {

                        if (matched == 2 && value > 0) {
                            s_ctx->log_interval_ms = (uint32_t)value;
                            printf("LOG INTERVAL HAS UPDATED TO %u ms\n", s_ctx->log_interval_ms);

                        } else {
                            printf("TIME HAS TO BE GREATER THAN 0\n");
                        }
                    }
                    else if (strcmp(cmd, "START_LOG") == 0) {
                        FSM_HandleEvent(s_ctx, EVENT_CMD_START_LOG);
                    }
                    else if (strcmp(cmd, "STOP_LOG") == 0) {
                        FSM_HandleEvent(s_ctx, EVENT_CMD_STOP_LOG);
                    }
                    else if (strcmp(cmd, "DUMP_LOGS") == 0) {
                        FSM_HandleEvent(s_ctx, EVENT_CMD_DUMP);
                    }
                    else if (strcmp(cmd, "ERASE") == 0) {
                        FSM_HandleEvent(s_ctx, EVENT_CMD_ERASE);
                    }
                    else if (strcmp(cmd, "LED_ON") == 0) {
                        s_ctx->led_status = true;
                        printf("LED IS ON\n");
                    }
                    else if (strcmp(cmd, "LED_OFF") == 0) {
                        s_ctx->led_status = false;
                        printf("LED IS OFF\n");
                    }
                    else {
                        printf("UNKNOWN COMMAND: %s\n", cmd);
                    }
                }
            }
            cmd_index = 0;
        }
        else if (rx_byte != '\r') {

            if (cmd_index < (sizeof(cmd_line) - 1)) {
                cmd_line[cmd_index++] = rx_byte;
                
            } else {
                printf("OVERFLOW!\n");
                overflow_flag = true;
                cmd_index = 0;
            }
        }
    }
}