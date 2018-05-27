/*
Ajou Univ., Dept of Software, Hyunsoo Kim
2018. 05. 27

TODO O_SYNC|O_DIRECT : http://heart4u.co.kr/tblog/476
TODO KENRNEL MODULE
TODO EXCEPTION HANDLE
TODO THREAD
TODO DATA CLASSIFY
TODO CHANGE TO PASSIVE MODE, do i check ans?
TODO LCD DISPLAY
TODO NMAP
TODO stderr

TODO set_mincount
TODO sync_read retval control

<PMS7003 SENSOR SPEC>
DATA RATE: 9600bps
TRANSMIT INTERVAL: 1sec
FRAME LENGTH: 32Bytes in normal, data4: 0.3~1ug/m^3, data5: 2.5ug/m^3, data6: 10ug/m^3

------------------------------------------------
|PREAMBLE(2) | LENGTH(2) | DATA(26) | CHECK(2) |   (Bytes)
------------------------------------------------
PREAMBLE: "BM"
LENGTH  : 28Bytes = DATA + CHECK
DATA    : 13fields, each is 2Bytes
CHECK   : OK if (CHECK == SUM all the bytes except CHECK bytes)
MOREINFO: http://eleparts.co.kr/goods/view?no=4208690
          https://github.com/teusH/MySense/blob/master/docs/pms7003.md

void set_passive(int fd)                //set sensor to passive mode to reduce power consump
void wakeup_sensor(int fd)              //send msg to sensor to wake up, and wait 30sec to stabilize
int sendReadCmd(int fd)                 //send msg to sensor to read data, return sndMsgBytes
int sync_read(int fd, unsigned char *frame) //sync with sensor and read, return 0 on success
int parse_frame(unsigned char *frame)   //parse and calc checksum, return 0 on success
void sleep_sensor(int fd)               //send msg to sensor to sleep
*/

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

#define BUFLEN 8
#define FRAMELEN 32
#define CMDLEN 7

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}


//SET SENSOR TO PASSIVE MODE
void set_passive(int fd)
{
    unsigned char cmdSetPassive [CMDLEN] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};  //expecting answer 42 4D 00 04 E1 00 01 74
    int retval;
    int tryCount = 0;

    while(1){
        if((retval = write(fd, cmdSetPassive, CMDLEN)) != CMDLEN) {
            tryCount++;
            printf("Err on write(), failed to set Passive mode, tried %d times\n", tryCount); 
        }

        else 
            break;
    }
    return ;    
}


//WAKE UP SENSOR
void wakeup_sensor(int fd)
{
    unsigned char cmdWakeUp[CMDLEN]  = {0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74};   //expecting NO ANS
    int retval;
    int tryCount = 0;

    while(1){
        if((retval = write(fd, cmdWakeUp, CMDLEN)) != CMDLEN) {
            tryCount++;
            printf("Err on write(), failed to wake Up sensro, tried %d times\n", tryCount); 
        }

        else 
            break;
    }
    return ;    
}

int sendReadCmd(int fd)
{
    unsigned char cmdPassiveRead[CMDLEN] = {0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71}; //expecting whole frame
    int retval;

    if((retval = write(fd, cmdPassiveRead, CMDLEN)) != CMDLEN) {
        printf("Err on write(), failed to send READ CMD\n");
    }

    return retval;    
}

//SYNC WITH PREAMBLE, READ DATA
int sync_read(int fd, unsigned char *frame)
{
    const char preamble1 = 'B'; //0x42 in ASCII
    const char preamble2 = 'M'; //0x4d in ASCII

    //SYNC AND READ
    while(1) {
        unsigned char buf[BUFLEN +1];
        int rdlen, wrlen;
        int count; //loop count variable
        unsigned char *p;

        rdlen = read(fd, buf, BUFLEN);

        //IF WE GOT SYNC
        if (rdlen == 8 && buf[0] == preamble1 && buf[1] == preamble2) {
            printf("Read %d:", rdlen);
            for (p = buf; rdlen-- > 0; p++)
                printf("%d\t", *p);
            printf("\n");
            count = 0;

            if(memcpy(&frame[count*BUFLEN],buf,BUFLEN) < 0) {
                printf("Error memcpy at buf blk %i: %s\n", count, strerror(errno));
                return -1;
            }
        }

        //IF WE READ REST OF THE FRAME
        else if(rdlen == 8) {
            count = count %4; count++; 

            printf("Read %d:", rdlen);
            for (p = buf; rdlen-- > 0; p++)
                printf("%d\t", *p);
            printf("count:%d\n", count);

            if(memcpy(&frame[count*BUFLEN],buf,BUFLEN) < 0) {
                printf("Error memcpy at buf blk %i: %s\n", count, strerror(errno));
                return -1;
            }

            //NOW WE READ WHOLE FRAME
            if(count >= FRAMELEN/BUFLEN-1)
                return 0;
        }

        else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
            count = 0;
            return -1;
        }
    }
}


//VALIDATE FRAME AND PARSE
int parse_frame(unsigned char *frame)
{
    for(int i = 0; i < FRAMELEN; i++) printf("%d", frame[i]); 
        printf("\n");

    //CALC CHECKSUM
    int sum = 0;
    for(int i = 0; i<FRAMELEN -2; i++)
        sum += frame[i];
    int checksum = frame[30] * pow(16,2) + frame[31]; //conversion between Hex<->Dec

    if(sum == checksum) {
        int pm1_0;
        int pm2_5;
        int pm10;

        printf("PM 1.0: %d ", frame[10]<<8 | frame[11]);
        printf("PM 2.5: %d ", frame[12]<<8 | frame[13]);
        printf("PM 10.: %d\n", frame[14]<<8 | frame[15]);
        return 0;
    }
    
    else{
        printf("CHECKSUM ERR, FLUSH PREVIOUS DATA\n");
        return -1;
    }
}


//SLEEP SENSOR 
void sleep_sensor(int fd)
{
    unsigned char cmdSleep[CMDLEN]    = {0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73};   //expecting 42 4D 00 04 E4 00 01 77 
    int retval;
    int tryCount = 0;

    while(1){
        if((retval = write(fd, cmdSleep, CMDLEN)) != CMDLEN) {
            tryCount++;
            printf("Err on write(), failed to set Passive mode, tried %d times\n", tryCount); 
        }

        else 
            break;
    }
    return ;    
}


int main()
{
    const char *portname = "/dev/ttyAMA0";
    unsigned char frame[FRAMELEN +1];
    int fd;

    //OPEN DATA STREAM
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }

    set_interface_attribs(fd, B9600);
    //set_mincount(fd, 0);                /* set to pure timed read */

    if(tcdrain(fd) != 0) {  //delay for output
        printf("Error at writing: %s\n", strerror(errno));
        return -1;
    }

    //SET PASSIVE MODE TO HIBERNATE
    set_passive(fd);

    while(1) {
        wakeup_sensor(fd);
        sleep(15);

        while(1) {
            int retval;
            if( (retval = sendReadCmd(fd)) != CMDLEN) 
                continue;
            if( (retval = sync_read(fd, frame)) != 0) 
                continue;
            if( (retval = parse_frame(frame)) != 0) 
                continue;
            else break;
        }

        sleep_sensor(fd);
        printf("GOING TO SLEEP\n\n");
        sleep(15);
    }
    return 0;
}//main
