#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>


//total FRAME = PREAMBLE(2) + DATA(30)
#define FRAMELEN 30
#define BUFLEN 2
#define PREAMBLE "BM"


int main()
{
	struct termios options;
	int uart0_filestream = -1;
	unsigned char rx_buffer[BUFLEN +1];
	unsigned char frame[FRAMELEN +1];
	int rx_length;
	int counter = 0;

	uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
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

	
	while(rx_length = read(uart0_filestream, (void*)rx_buffer, BUFLEN))
	{
		if (rx_length < 0) {
			//printf("err retval<0\n");
			continue;
		} 

		else if (rx_length == 0) {
			//printf("err retval=0\n");
			continue;
		} 

		else { //read PREAMBLE
			rx_buffer[rx_length +1] = '\0';
			if(rx_length = strcmp(rx_buffer, PREAMBLE)) 
			{
				printf("PREAMBLE: %x\n", rx_buffer);
				printf("PREAMBLE: %s\n", rx_buffer);

				//read DATA
				rx_length = read(uart0_filestream, (void*)frame, FRAMELEN);
				frame[FRAMELEN +1] = '\0';
				printf("%d FRAME DATA: %s\n", rx_length, frame);
				memset(frame, 0, FRAMELEN +1);
				continue; 
			}
		}
	}//whlie

	close(uart0_filestream);
	return 0;
}//main