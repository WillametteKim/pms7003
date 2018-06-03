/*
2018 Spring - System Programming
Code written by Hyunsoo Kim

SBC: Raspberry Pi 3B
Sensor: PMS 7003 Dust Sensor

SET baud rate to 9600bps
SET
SET

TODD using multiple buffer will work well?
TODO make freealldev() which checks wheter sensor is connected.
TODO what is high and low?
*/
#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>


#define RX 16 
#define TX 15
#define Preamble1 0x42
#define Preamble2 0x4d

/*
struct buffer{
	uint8_t start1;
	uint8_t start2;
	uint8_t	frameLen_high;
	uint8_t	frameLen_low;
	
	uint8_t pm1_cf1_high;	//use cf1 only FACTORY env.
	uint8_t pm1_cf1_low;	//use cf1 only FACTORY env.
	uint8_t pm2_cf1_high;	//use cf1 only FACTORY env.
	uint8_t pm2_cf1_low;	//use cf1 only FACTORY env.
	uint8_t pm10_cf1_high;	//use cf1 only FACTORY env.
	uint8_t pm10_cf1_low;	//use cf1 only FACTORY env.
	
	uint8_t pm10;

};
*/

int main()
{
	unsigned char buffer[32];
	int pm1, pm2, pm10;
	int fd;
	int count;
	char temp;

	if(wiringPiSetup() == -1)
		{printf("wiringPiSetup FAIL\n"); return 1;}

	if(fd == serialOpen("/dev/ttyS0", 9600) < 0)
		{printf("serialOpen FAIL\n"); return 1;}

	pinMode(TX, OUTPUT);
	digitalWrite(TX, HIGH);
	pinMode(RX, INPUT);

	while(1)
	{
		printf("%x ", serialGetchar(fd));
		continue;

		//if(serialGetchar(fd) == Preamble1 && serialGetchar(fd) == Preamble2)
		if(serialGetchar(fd) == Preamble1)
		{
			printf("ssibal2\n");
			for(count = 0; count < 32; count++)
				buffer[count] = serialGetchar(fd);

			pm1  = (buffer[10]<<8 | buffer[11]);
			pm2  = (buffer[12]<<8 | buffer[13]);
			pm10 = (buffer[14]<<8 | buffer[15]);
			printf("PM1.0: %d PM2.5: %d PM10: %d\n", pm1, pm2, pm10);
			continue;
		}

		else
		{
			printf("ssibal3\n");
			printf("wait until sync\n"); 
			continue;
		}

		printf("ssibal4\n");
	}//while 


	return 0;
}

