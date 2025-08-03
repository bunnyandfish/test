#ifndef __UART_H
#define __UART_H

#include"main.h"
#include "stdio.h"

#define UART1_RxDATA_SIZE		8			//接收数据长度

/* 串口接收数据双缓冲结构体 */
typedef struct UartData_RxBuffer
{
	volatile uint8_t Uart_Rxflag;			//接收完成数据旗帜
	char Rx_buffer1[100];		//缓冲区1
	char Rx_buffer2[100];		//缓冲区2
	char * pRxbuf;					//接收缓冲区指针
	char * pReadBuf;				//读取缓冲区指针
}URxbuffer;

/* 接收标志 */
enum Rx_flag{
	Rxflag_False=0,
	Rxflag_Ture
};


extern UART_HandleTypeDef Uart1_Handle;		//串口1句柄
extern char Uart1_TxData[20];				//发送数据缓冲区
extern URxbuffer UART1_RxData;				//数据接收缓冲区

void Uart1_Init(uint32_t Uart1_BaudRate);


#endif
