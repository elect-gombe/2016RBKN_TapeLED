/* 
 * File:   main.h
 * Author: oishi
 *
 * Created on 2015/05/08, 12:48
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>


#define LEDs 20
#define BufSize 200

#define NoneData 0x7FFA

#define ErrorOut


extern unsigned char R[LEDs];
extern unsigned char B[LEDs];
extern unsigned char G[LEDs];

extern volatile int RxWptr;
extern volatile unsigned char UARTData[BufSize];


#define CH(){LATBbits.LATB3=1;}
#define CL(){LATBbits.LATB3=0;}

void BarShows(unsigned char);
void SendData(void);
void DataAnalysis(void);

void Clear(void);
void putch(char dat);
void PORTInit(void);
void PLLEnable(void);
void UARTInit(void);
void init(void);
void BarCmdRsv(unsigned char arg1,unsigned char arg2,unsigned char arg3);
void BarShow(unsigned char point,unsigned char Col);

void fallen(int Po,int lng,int speed,int dire,unsigned char _R,unsigned char _G,unsigned char _B,int continue_flag);
void wait(int msec);
/*typedef struct LEDChanges{
    unsigned char Po;
    unsigned char G;
    unsigned char B;
    unsigned char R;
};*/

#endif	/* MAIN_H */
