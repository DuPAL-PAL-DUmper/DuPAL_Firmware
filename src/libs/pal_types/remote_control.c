#include "remote_control.h"

#include <avr/wdt.h>

#include <stdint.h>
#include <string.h>

#include <uart/uart.h>
#include <utils/strutils.h>
#include <ioutils/ioutils.h>

#define PKT_BUFFER_SIZE 32
#define PKT_START '>'
#define PKT_END '<'
#define RESP_START '['
#define RESP_END ']'

#define CMD_WRITE 'W'
#define CMD_READ 'R'
#define CMD_EXIT 'X'
#define CMD_RESET 'K'
#define CMD_MODEL 'M'
<<<<<<< HEAD

#define CMD_ERROR "CMD_ERR\n"
#define RESP_MODEL "[M 01]\n"
=======
#define CMD_LED 'L'

#define CMD_ERROR "CMD_ERR\n"
#define RESP_MODEL "[M 02]\n"
>>>>>>> rev2_board

static char pkt_buffer[PKT_BUFFER_SIZE];

static uint8_t receive_pkt(void);

void remote_control_analyze(void) {
    uart_puts("REMOTE_CONTROL_ENABLED\n");

    ioutils_write(0);

    while(1) {
        if(receive_pkt()) {
            ioutils_setLED(ACT_LED, 1);

            switch(pkt_buffer[0]) {
                case CMD_LED: {
                        uint8_t dat = strutils_str_to_u8(&pkt_buffer[2]);
                        uint8_t led = (dat >> 1) & 0x7F, status = dat & 0x01;

                        pkt_buffer[0] = RESP_START;
                        pkt_buffer[1] = CMD_LED;
                        pkt_buffer[2] = ' ';
                        strutils_u8_to_str(&pkt_buffer[3], dat);
                        pkt_buffer[5] = RESP_END;
                        pkt_buffer[6] = '\n';
                        pkt_buffer[7] = 0;
                    
                        uart_puts(pkt_buffer);

                        ioutils_setLED(led, status);
                    }
                    break;
                case CMD_MODEL: {
                        uart_puts(RESP_MODEL);
                    }
                    break;
                case CMD_WRITE: {
                        uint32_t addr = strutils_str_to_u32(&pkt_buffer[2]) & 0xFFFFFF;
                        ioutils_write(addr);
                        pkt_buffer[0] = RESP_START;
                        pkt_buffer[1] = CMD_WRITE;
                        pkt_buffer[2] = ' ';
                        strutils_u32_to_str(&pkt_buffer[3], addr);
                        pkt_buffer[11] = RESP_END;
                        pkt_buffer[12] = '\n';
                        pkt_buffer[13] = 0;

                        uart_puts(pkt_buffer);
                    }
                    break;
                case CMD_READ:
                    pkt_buffer[0] = RESP_START;
                    pkt_buffer[1] = CMD_READ;
                    pkt_buffer[2] = ' ';
                    strutils_u8_to_str(&pkt_buffer[3], ioutils_read());
                    pkt_buffer[5] = RESP_END;
                    pkt_buffer[6] = '\n';
                    pkt_buffer[7] = 0;

                    uart_puts(pkt_buffer);
                    break;
                case CMD_RESET:
                    while(1); // Will reset the program via watchdog
                case CMD_EXIT:
                    return;
                default:
                    uart_puts(CMD_ERROR);
                    break;
            }

            ioutils_setLED(ACT_LED, 0);
        }

        wdt_reset();
    }
}

static uint8_t receive_pkt(void) {
    uint8_t idx = 0, receiving_pkt = 0;
    char c;

    while(1) {
        wdt_reset();

        if(uart_charavail()) {
            c = uart_getchar();
            switch(c) {
                case PKT_START:
                    memset(pkt_buffer, 0, PKT_BUFFER_SIZE);
                    idx = 0;
                    receiving_pkt = 1;                    
                    break;
                case PKT_END:
                    if(receiving_pkt && idx) return 1;
                    else {
                        receiving_pkt = 0;
                        idx = 0;
                    }
                    break;
                default:
                    if(receiving_pkt && (idx < (PKT_BUFFER_SIZE - 1))) { // Leave one empty space for a null
                        pkt_buffer[idx++] = c;
                    }
                    break;
            }
        }
    }
}