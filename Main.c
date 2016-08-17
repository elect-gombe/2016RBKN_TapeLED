/* 
 * File:   MainSouceFile.c
 * Author: oishi
 *
 * Created on 2015/05/06, 14:44
 */


#include "p33FJ64GP802.h"
#include "Patern.h"

#include <libpic30.h>

#include "main.h"

#include <xc.h>

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

#define FCY 39617500UL

#define TIMEOUT (100/*ms*/  /  5)

volatile int RxWptr;
volatile unsigned char UARTData[BufSize];

volatile uint32_t timer_count1;
volatile uint32_t timer_count2;

typedef enum{
  mode_A,
  mode_B,
  mode_C,
  mode_WT_RC,
  mode_CN_MS,
  mode_ERR,
} mode_t;

volatile mode_t g_mode;

void putch(char dat){
  while(U1STAbits.UTXBF);
  U1TXREG = dat;
}

char getch(void){
  while(!RxWptr);
  return UARTData[--RxWptr];
}

int isUARTRcv(void){
    return RxWptr;
}


/*
 *
 *
 * 
 */

_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRCPLL);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);

unsigned char R[LEDs];
unsigned char G[LEDs];
unsigned char B[LEDs];


void T1SetUp(void);
void Patern_2(uint32_t position,int lng,unsigned char _R,unsigned char _G,unsigned char _B);
void Dimming(void);

void PORTInit(void){
  TRISB=0x0020;
  ADPCFG = 0xFFFF;
}

void PLLEnable(void){
  PLLFBD = 41;
  CLKDIVbits.PLLPOST=0;
  CLKDIVbits.PLLPRE=0;
  while(!OSCCONbits.LOCK);
}

void UARTInit(void){
  U1BRG=21;
  U1MODE = 0x8000;
  U1STA  = 0x0400;

  _PCFG4 = 1;
  _PCFG5 = 1;

  _U1RXR = 5;
  _RP6R  = 3;
    
  _U1RXIP= 4;
  _U1RXIF= 0;
  _U1RXIE= 1;
}


void init(void){
  PORTInit();
  PLLEnable();
  UARTInit();
  T1SetUp();
}

void Clear(void){
  int a;
  for(a=0;a<LEDs;a++){
    R[a]=0;
    G[a]=0;
    B[a]=0;
  }
}

int main(void) {
  uint32_t b = 0;
  init();
   
  while(1) {
        uint32_t a;
        a = (a + 1) % (LEDs * 10);
        Clear();

        Patern_2(LEDs*3-a+10, 3, 30, 30, 125);
        Patern_2(a, 3, 30, 30, 125);

        Dimming();;;
        SendData();
        wait5ms();
        wait5ms();
    }

  return EXIT_FAILURE;
}

void Dimming(void){
  int i;
  for(i=0;i<LEDs;i++){
    R[i]>>=1;
    G[i]>>=1;
    B[i]>>=1;
  }
}

void Patern_2(uint32_t position,int lng,unsigned char _R,unsigned char _G,unsigned char _B){
  unsigned int i;
  int j;
  unsigned int k;

  
  for(i=1;i<lng*3;i++){
    j = ((unsigned int)(position + lng * 3 - i) % (LEDs*3))/3;
    k = (lng * 9);
    R[j]   += _R  * i / k;
    G[j]   += _G  * i / k;
    B[j]   += _B  * i / k;
  }

  for(i=lng*3;i>=1;i--){
    j = ((unsigned int)(position + i - lng * 3) % (LEDs*3))/3;
    k = lng * 9;
    R[j]   += _R * i / k;
    G[j]   += _G * i / k;
    B[j]   += _B * i / k;
  }
}

void wait5ms(void){
  volatile uint32_t time = timer_count2;
  while(timer_count2 == time)  Idle();
}

void T1SetUp(void){
  PR1 = 156*5 - 1; // about 5mS
  T1CON = 0x0030; // PS 1/256
  TMR1 = 0;
    
  IPC0bits.T1IP = 0x1;
  IFS0bits.T1IF = 0;
  IEC0bits.T1IE = 1;
  T1CONbits.TON = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
  /*flag clear*/
  IFS0bits.T1IF = 0;

  timer_count1++;
  if(timer_count1 > TIMEOUT){
    g_mode = mode_CN_MS;
  }
  timer_count2++;
}


void __attribute__ ((interrupt , no_auto_psv)) _U1RXInterrupt(void){
  unsigned char dat;

  /*flag clear*/
  _U1RXIF=0;

  /*is overrun occered?*/
  if(U1STAbits.OERR){
    U1STAbits.OERR=0;
    RxWptr = 0;
  }
  /*rcv charactor*/
  else{
    dat = U1RXREG;
    UARTData[RxWptr++]=dat;

    timer_count1=0;

    if(RxWptr==BufSize){
      RxWptr = 0;
    }
  }
}
