#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>

#include "libi2c/twi/twi_master.h"
#include "libuart/uart.h"

#define tw_transmit(a, b, c) tw_master_transmit(a, b, c, false)

// start config
#define D_ADDR 0x70
#define D_FLIP 1
#define TZ_H -8
#define TZ_M 0
// month-day
#define DS_S 312
#define DS_E 1105
// 24-hour
#define F24 1
// trim leading zero
#define FTL 0
// end config

#define TZ_HW (TZ_H % 24)
#if TZ_M != 0
#define TZ_MW (TZ_M % 60)
#define TZ_MU (60 - TZ_MW)
#endif

// digits: middle top-left bottom-left bottom bottom-right top-right top
// punch: n n dot right-top right-bottom colon n
#if D_FLIP
uint8_t digit_lut[10] = {
	0x3F, // 0011 1111
	0x30, // 0011 0000
	0x5B, // 0101 1011
	0x79, // 0111 1001
	0x74, // 0111 0100
	0x6D, // 0110 1101
	0x6F, // 0110 1111
	0x38, // 0011 1000
	0x7F, // 0111 1111
	0x7D, // 0111 1101
};
#else
uint8_t digit_lut[10] = {
	0x3F, // 011 1111
	0x06, // 000 0110
	0x5B, // 101 1011
	0x4F, // 100 1111
	0x66, // 110 0110
	0x6D, // 110 1101
	0x7D, // 111 1101
	0x07, // 000 0111
	0x7F, // 111 1111
	0x6F, // 110 1111
};
#endif
#if D_FLIP
uint8_t colon_lut[4] = {0, 4, 12, 8};
#else
uint8_t colon_lut[4] = {0, 8, 12, 4};
#endif

int main() {
	uart_init();
	tw_init(TW_FREQ_400K, true);
	uint8_t buf[10];
	buf[0] = 0x21; // osc on
	tw_transmit(D_ADDR, buf, 1);
	buf[0] = 0x81; // display on
	tw_transmit(D_ADDR, buf, 1);
	buf[0] = 0xEF; // dimming MAX
	tw_transmit(D_ADDR, buf, 1);
	buf[0] = 0;
	buf[1] = 0x40;
	buf[3] = 0x40;
	buf[5] = 0;
	buf[7] = 0x40;
	buf[9] = 0x40;
	tw_transmit(D_ADDR, buf, 10);

	// message parsing
	for (;;) {
		if (uart_getc() == '$') {
			while (uart_getc() != ',');
			#define GET_D (uart_getc() - '0')
			#define GET_D16 ((uint16_t)(uart_getc() - '0'))
			char hour = GET_D * 10 + GET_D;
			char minute = GET_D * 10 + GET_D;
			char second = GET_D * 10 + GET_D;
			for (int i = 0; i < 8; ++i)
				while (uart_getc() != ',');
			uint16_t month_day = GET_D16 * 10 + GET_D16 + GET_D16 * 1000 + GET_D16 * 100;
			hour += TZ_HW;
			#ifdef TZ_MW
			if (minute < TZ_MW)
				hour--;
			if (minute >= TZ_MU)
				hour++;
			#endif
			#if DS_S
			// technically breaks for two hours around dst, but eh it doesn't matter
			if (month_day >= DS_S && month_day < DS_E)
				hour += 1;
			#endif
			hour %= 24;
			#ifdef TZ_MW
			minute += TZ_MW;
			minute %= 60;
			#endif
			// write display buffer
			#define DIGIT(n) digit_lut[n]
			#if D_FLIP
			#define N(n) 10-n
			#else
			#define N(n) n
			#endif
			printf("\r%02d:%02d:%02d", hour, minute, second);
			buf[N(9)] = digit_lut[minute % 10]; // d4
			buf[N(7)] = digit_lut[minute / 10]; // d3
			// punct
			buf[5] = colon_lut[second / 15] | (second % 2 == 0 ? 2 : 0)
			#if !F24
			| (hour >= 12 ? 16 : 0);
			hour = ((hour - 1) % 12) + 1;
			#endif
			;
			buf[N(3)] = digit_lut[hour % 10]; // d1
			#if FTL
			buf[N(1)] = hour < 10 ? 0 : digit_lut[hour / 10]; // d0
			#else
			buf[N(1)] = digit_lut[hour / 10]; // d0
			#endif
			tw_transmit(D_ADDR, buf, 10);
		}
	}
}
