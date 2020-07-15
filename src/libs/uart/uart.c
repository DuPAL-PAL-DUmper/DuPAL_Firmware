#include <avr/io.h>
#include <stdio.h>

#ifndef BAUD
#define BAUD 1200
#endif

#include <util/setbaud.h>

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

#include "common/defines.h"

#if defined(__SECOND_UART__)
#define UART_NUMBER 1
#else
#define UART_NUMBER 0
#endif

#if defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__) | defined(__AVR_ATtiny4313__)
#define UART_UBRRH UBRRH
#define UART_UBRRL UBRRL

#define UART_UCSRA UCSRA
#define UART_UCSRC UCSRC
#define UART_UCSRB UCSRB

#define UART_RXEN RXEN
#define UART_TXEN TXEN

#define UART_UCSZ0 UCSZ0
#define UART_UCSZ1 UCSZ1

#define UART_UDR UDR

#define UART_UDRE UDRE

#define UART_RXC RXC

#define UART_U2X U2X

#elif defined (__AVR_ATmega8A__)

#define UART_UDR                UDR
#define UART_UCSRA              token_paste2(UCSR, A)
#define UART_UCSRB              token_paste2(UCSR, B)
#define UART_UCSRC              token_paste2(UCSR, C)
#define UART_UBRR               UBRR
#define UART_UBRRL              token_paste2(UBRR, L)
#define UART_UBRRH              token_paste2(UBRR, H)
#define UART_U2X                U2X

#define UART_UDRE               UDRE

#define UART_RXC		RXC

#define UART_RXEN		RXEN
#define UART_TXEN		TXEN

#define UART_UCSZ0              token_paste2(UCSZ, 0)
#define UART_UCSZ1              token_paste2(UCSZ, 1)

#else // Not an ATTiny

#define UART_UDR                token_paste2(UDR, UART_NUMBER)

#undef UCSR
#define UART_UCSRA              token_paste3(UCSR, UART_NUMBER, A)
#define UART_UCSRB              token_paste3(UCSR, UART_NUMBER, B)
#define UART_UCSRC              token_paste3(UCSR, UART_NUMBER, C)
#define UART_UBRR               token_paste2(UBRR, UART_NUMBER)
#define UART_UBRRL              token_paste3(UBRR, UART_NUMBER, L)
#define UART_UBRRH              token_paste3(UBRR, UART_NUMBER, H)

#define UART_U2X				token_paste2(U2X, UART_NUMBER)

#undef UDRE
#define UART_UDRE				token_paste2(UDRE, UART_NUMBER)

#undef RXC
#define UART_RXC				token_paste2(RXC, UART_NUMBER)

#undef RXEN
#undef TXEN
#define UART_RXEN				token_paste2(RXEN, UART_NUMBER)
#define UART_TXEN				token_paste2(TXEN, UART_NUMBER)

#undef UCSZ
#define UART_UCSZ0              token_paste3(UCSZ, UART_NUMBER, 0)
#define UART_UCSZ1              token_paste3(UCSZ, UART_NUMBER, 1)

#endif

void uart_init(void) {
    UART_UBRRH = UBRRH_VALUE;
    UART_UBRRL = UBRRL_VALUE;

#if USE_2X
    UART_UCSRA |= (1 << UART_U2X);
#else
    UART_UCSRA &= ~(1 << UART_U2X);
#endif

#if defined (__AVR_ATmega8A__)
    // ATMega8A requires the msb to be set to 1, otherwise UBRRH is selected
    UART_UCSRC = 0x80 | (1 << UART_UCSZ1) | (1 << UART_UCSZ0); /* 8-bit data */
#else
    UART_UCSRC = (1 << UART_UCSZ1) | (1 << UART_UCSZ0); /* 8-bit data */
#endif
    UART_UCSRB = (1 << UART_RXEN) | (1 << UART_TXEN);   /* Enable RX and TX */
}

int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UART_UCSRA, UART_UDRE);
    UART_UDR = c;

    return 0;
}

int uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UART_UCSRA, UART_RXC);

    return UART_UDR;
}
