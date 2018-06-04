#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

int set_interface_attribs(int fd, int speed);
void set_mincount(int fd, int mcount);
void set_passive(int fd);
void wakeup_sensor(int fd);
int sendReadCmd(int fd);
int sync_read(int fd, unsigned char *frame);
int check_frame(unsigned char *frame);
void sleep_sensor(int fd);
void robust_sensor_main(int fd, unsigned short int *dust_val);
int sensor_init();