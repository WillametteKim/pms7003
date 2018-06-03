#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
 
int main ()
{
  int fd ; //시리얼 통신 확인 변수
  int count ;
  unsigned int nextTime ;
 
  if ((fd = serialOpen ("/dev/ttyS0", 9600)) < 0)  // 두번째 인자값이 보레이트 설정
  {   // fd가 -1이면 아래 처럼 unable 나온다.
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;                   
    return 1 ;
  }
 
  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }
 
  nextTime = millis () + 300 ;   // millis() 함수는 값이 일정하게 증가하는 함수
 
  for (count = 0 ; count < 256 ; )   //
  {
    if (millis () > nextTime)
    {
      printf ("\nOut: %d: ", count) ;
      fflush (stdout) ;                //뭐하는 변수인지... fflush
      serialPutchar (fd, count) ; // 데이터 전송해주는 함수, count 값이 TX로 가나보다.
      nextTime += 300 ; //delay 역할?
      ++count ;
    }
 
    delay (3) ;
 
    while (serialDataAvail (fd))  // DataAvail은 Rx에 신호가 있을 때 실행됨.
    {
      printf (" -> %3d", serialGetchar (fd)) ;  // 데이터 받는 함수
      fflush (stdout) ;
    }
  }
 
  printf ("\n") ;
  return 0 ;
}
