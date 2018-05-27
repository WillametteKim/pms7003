#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define BUFLEN 32
#define PREAMBLE "BM"

int main()
{
	struct termios options;
	int uart0_filestream = -1;
	//unsigned char rx_buffer[BUFLEN];
	char preamble[8];
	int rx_buffer[BUFLEN];
	int rx_length;
	int count;


	uart0_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (uart0_filestream == -1) {     // ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART. Ensure it is not in use by another application\n");
		return -1;
	}

	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

	/*
	while(1)
	{
	// Read up to 255 characters from the port if they are there
		if (uart0_filestream != -1) 
		{
			rx_length = read(uart0_filestream, (void*)rx_buffer, BUFLEN); 
			//Filestream, buffer to store in, number of bytes to read (max)
			if (rx_length < 0) {
				//printf("err no read");
				continue;
			} 

			else if (rx_length == 0) {
				//printf("err no read");
				continue;
			} 

			else { //Bytes received
				rx_buffer[rx_length] = '\0';
				printf("%i bytes read : %s\n", rx_length, rx_buffer);
			}
		}//if
	}//whlie
	*/

	while(1)
	{
		rx_length = read(uart0_filestream, (void*)preamble, 8);
		printf("%s\n", preamble);
		continue;

		if(strcmp(PREAMBLE, preamble) == 0)	//if we got sync with frame header
			{
				printf("hello!\n");
				rx_length = read(uart0_filestream, (void*)rx_buffer, BUFLEN);
				printf("%s\n", rx_buffer);
			}

		if(rx_length <= 0) 
			;//{printf("die with err\n"); continue;}
		else
			;//printf("%d: %s\n", rx_length, rx_buffer);
	}





	/*
	int readByte, count;
	char buf[32] = {NULL};
	int sfd;

	//OPEN serial port
	sfd = open("/dev/serial0", O_RDWR | O_NOCTTY);
	if(sfd == -1)
	{
		printf("Err: %d\n", errno);
		//printf("Err: %d\n", errno);	change to stderr
		return -1;
	}

	while(1)
	{
	count = read(sfd, buf, 32);
	//buf[32] = 0;
	printf("%x\n", buf);
	}
	*/



return 0;
}//main