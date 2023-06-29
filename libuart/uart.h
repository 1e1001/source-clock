/* uart library, based on https://github.com/arduino-c/uart and https://github.com/arduino-c/uart_stdio */
#ifndef LIB_UART_H
#define LIB_UART_H
#include <stdbool.h>

/* place one character on uart, replaces newlines with crlf */
void uart_putc(char c);
/* read one character in, without any read-line capabilities */
char uart_getc();
/* place an entire string to uart */
void uart_puts(const char *c);
/* returns true if there is data to read */
bool uart_data();
/* initialize uart at BAUDRATE hz, and sets it as the stdio device */
void uart_init();

#endif
