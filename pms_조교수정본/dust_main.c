#include <stdio.h>
#include <unistd.h> //for sleep
#include "dust.h"

int main()
{
	unsigned short int dust_val[3];
	int fd = sensor_init();

	while(1) {
		robust_sensor_main(fd, dust_val);
		printf("PM2_5:%d\tPM10: %d\tstat:%c\n", dust_val[1], dust_val[2], dust_val[0]);
		sleep(3);
	}

	return 0;
}//main