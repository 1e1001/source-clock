#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

/* i love magic numbers :) */

void uart_putc(char c) {
	if (c == '\n')
		uart_putc('\r');
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = c;
}

char uart_getc() {
	char c;
	while (!uart_data());
	c = UDR0;
	if (c == '\r')
		c = '\n';
	return c;
}

void uart_puts(const char *c) {
	while (*c)
		uart_putc(*c++);
}

static int ufdev_putc(char c, FILE *this) {
	(void)this;
	uart_putc(c);
	return 0;
}

static int ufdev_getc(FILE *this) {
	(void)this;
	return uart_getc();
}

bool uart_data() {
	return UCSR0A & _BV(RXC0);
}

FILE uart_stdio = FDEV_SETUP_STREAM(ufdev_putc, ufdev_getc, _FDEV_SETUP_RW);

void uart_init() {
	/* originally this was / 16, but for some reason / 8 is what makes it work */
	#define UBRR_VAL (F_CPU / 8 / BAUDRATE - 1)
	UBRR0L = (unsigned char)(UBRR_VAL & 0xff);
	UBRR0H = (unsigned char)(UBRR_VAL >> 0x8);
	/* enable rx and tx */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	/* set frame format: 8data, 1stop bit */
	UCSR0C = (3 << UCSZ00);
	stdin = stdout = stderr = &uart_stdio;
}
