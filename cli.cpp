#include "cli.h"
#include <string.h>
#include <stdio.h>

static RingBuffer_t *s_rx_buffer = nullptr;

void CLI_Init(RingBuffer_t *rb) {
    s_rx_buffer = rb;
}

void CLI_Process(void) {

    if (s_rx_buffer == nullptr) {
        return;
    }

    static char cmd_line[32];
    static uint8_t cmd_index = 0;
    static bool overflow_flag = false;
    uint8_t rx_byte = 0;

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

                        if (matched == 2 && value >= 0 && value <= 255) {
                            printf("PWN HAS SET: %d\n", value);
                        } else {
                            printf("UNDEFINED PWM VALUE\n");
                        }
                    }

                    else if (strcmp(cmd, "LOG_INTERVAL") == 0) {

                        if (matched == 2 && value > 0) {
                            printf("LOG INTERVAL HAS UPDATED TO %d\n", value);
                        } else {
                            printf("TIME HAS TO BE GREATER THAN 0\n");
                        }
                    }

                    else if (strcmp(cmd, "DUMP_LOGS") == 0) {
                        printf("ALL DATA IS BEING TRANSFERRED TO PC\n");
                    }

                    else if (strcmp(cmd, "ERASE") == 0) {
                        printf("ALL PERMANENT DATA IS BEING ERASED\n");
                    }

                    else if (strcmp(cmd, "LED_ON") == 0) {
                        printf("LED IS ON\n");
                    }

                    else if (strcmp(cmd, "LED_OFF") == 0) {
                        printf("LED IS OFF\n");
                    }

                    else {
                        printf("UNKNOWN COMMAND\n");
                    }
                }
            }

            cmd_index = 0;
        }
        else if (rx_byte != '\r') {

            if (cmd_index < (sizeof(cmd_line) - 1)) {
                cmd_line[cmd_index] = rx_byte;
                cmd_index++;
            } 
            else {
                printf("OVERFLOW!\n");
                overflow_flag = true;
                cmd_index = 0;
            }      
        }
    }
}