#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>

static const int pulse_length = 186;
// This pin is not the first pin on the RPi GPIO header!
// http://wiringpi.com/wp-content/uploads/2013/03/pins.pdf
static const int pin = 0;
// specific to your etekcity switches
static const int switch_group = 0x14;
// another known group would be 0x05, https://github.com/xillwillx/433Mhz/blob/master/send.php

static void send(int code, int bits, int send_count);
static void transmit(int high_pulses, int low_pulses);

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "missing required arguments: %s <switch_num> <on/off>\n", argv[0]);
		return 1;
	}

	int switch_num = atoi(argv[1]);
	if (switch_num < 1 || switch_num > 5)
	{
		fprintf(stderr, "invalid switch number: %d\n", switch_num);
		return 1;
	}

	// 0x5500 is the basic set of bits common to all codes
	// switchgrp ---- ---- ---- on/off
	// switch #    55 4433 2211
	// 0000 0000 0101 0101 0000 0000
	int code = 0x5500 | (switch_group << 16);

	if (strcmp(argv[2], "on") == 0) {
		code |= 0x3;
	} else {
		code |= 0xc;
	}

	// this puts a '11' in the right place
	code |= 0x3 << ((switch_num + 1) * 2);

	wiringPiSetup();
	pinMode(pin, OUTPUT);
	send(code, 24, 5);
	return 0;
}

static void send(int code, int bits, int send_count)
{
	for (int repeat = 0; repeat < send_count; repeat++) {
		for (int i = bits - 1; i >= 0; i--) {
			int bit = (code >> i) & 1;
			switch(bit) {
				case 0:
					// send 1 on, 3 off
					transmit(1, 3);
					break;
				case 1:
					// send 3 on, 1 off
					transmit(3, 1);
					break;
			}
		}
		// send a sync bit
		transmit(1, 31);
	}
}

static void transmit(int high_pulses, int low_pulses)
{
	digitalWrite(pin, HIGH);
	delayMicroseconds(pulse_length * high_pulses);
	digitalWrite(pin, LOW);
	delayMicroseconds(pulse_length * low_pulses);
}
