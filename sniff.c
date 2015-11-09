#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

static const int pulse_length = 186;
static const int pulse_pad = 74; // 0.4 * 186

// http://wiringpi.com/wp-content/uploads/2013/03/pins.pdf
static const int pin = 2;

static void handle_interrupt();
static int switch_number(int code);

int main(int argc, char *argv[])
{
	wiringPiSetup();
	pinMode(pin, INPUT);

    wiringPiISR(pin, INT_EDGE_BOTH, &handle_interrupt);

	sleep(10);
}

static void handle_interrupt()
{
	static unsigned int last_time;
	static unsigned int first_duration;
	static int code;
	static int processing;
	
	unsigned int time = micros();
	unsigned int duration = time - last_time;

	if (duration > 30 * pulse_length) {
		if (code != 0) {
			printf("raw code: %d, 0x%x\n", code, code);
			printf("group 0x%x\n", code >> 16);
			printf("switch: %d\n", switch_number(code));
			printf("on/off: %s\n", (code & 0xf) == 0x3 ? "on" : "off");
		}
		code = 0;
		processing = 1;
	} else if (processing) {
		if ((duration > pulse_length - pulse_pad && duration < pulse_length + pulse_pad) ||
				(duration > pulse_length * 3 - pulse_pad && duration < pulse_length * 3 + pulse_pad)) {
			if (first_duration == 0) {
				first_duration = duration;
			} else {
				// we have a full bit, so shift what we've collected so far to the left
				code <<= 1;
				if (first_duration > duration) {
					// this is a one, longer pulse was first
					code += 1;
				}
				first_duration = 0;
			}
		} else {
			code = 0;
			processing = 0;
			first_duration = 0;
		}
	}

	last_time = time;
}

static int switch_number(int code)
{
	int tranform = (code >> 4) & 0x3ff;
	if ((tranform & 0x3) == 0x3) return 1;
	if ((tranform & 0xc) == 0xc) return 2;
	if ((tranform & 0x30) == 0x30) return 3;
	if ((tranform & 0xc0) == 0xc0) return 4;
	if ((tranform & 0x300) == 0x300) return 5;
	return -1;
}
