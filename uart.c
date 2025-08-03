#include "uart.h"


UART_HandleTypeDef Uart1_Handle;		//串口1句柄
char Uart1_TxData[20]={0};				//数据发送缓冲区
URxbuffer UART1_RxData={0};				//数据接收缓冲区
	

/***************************************************
 * @brief   printf重定向
 * @param   
 * 
 * @return  无
***************************************************/
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&Uart1_Handle, (uint8_t *)&ch, 1, HAL_MAX_DELAY); // 使用轮询方式发送数据
	return ch;
}

void UART1_RxDataStruct_Init(void)
{
	UART1_RxData.pReadBuf=UART1_RxData.Rx_buffer1;
	UART1_RxData.pRxbuf=UART1_RxData.Rx_buffer2;
	UART1_RxData.Uart_Rxflag=Rxflag_False;
}

/***************************************************
 * @brief   串口1初始化
 * @param   Uart1_BaudRate：指定串口1的波特率
 * 
 * @return  无
***************************************************/
void Uart1_Init(uint32_t Uart1_BaudRate)
{
	__HAL_RCC_USART1_CLK_ENABLE();						//时钟使能
	
#ifdef UART1_REMAP_ENABLE
	__HAL_RCC_AFIO_CLK_ENABLE();
	/* 串口1引脚重映射 */
	__HAL_AFIO_REMAP_USART1_ENABLE();  // 部分重映射
#endif
	
	
	Uart1_Handle.Instance=USART1;
	Uart1_Handle.Init.BaudRate=Uart1_BaudRate;
	Uart1_Handle.Init.WordLength=UART_WORDLENGTH_9B;		//数据长度8bit,校验位1bit
	Uart1_Handle.Init.StopBits=UART_STOPBITS_1;				//1位停止位宽度
	Uart1_Handle.Init.Parity=UART_PARITY_EVEN;				//偶校验
	Uart1_Handle.Init.Mode=UART_MODE_TX_RX;					//发送与接收
	Uart1_Handle.Init.HwFlowCtl=UART_HWCONTROL_NONE;		//不使用硬件流控制
	Uart1_Handle.Init.OverSampling=UART_OVERSAMPLING_16;	//每位数据为采样16次
	if(HAL_UART_Init(&Uart1_Handle)!=HAL_OK)
	{
		Error_Handler();
	}
	
	UART1_RxDataStruct_Init();					//初始化接收数据结构体的内容
	
	HAL_NVIC_SetPriority(USART1_IRQn,4,0);		//中断通道选择，中断优先级选择
	HAL_NVIC_EnableIRQ(USART1_IRQn);			//使能中断通道
	HAL_UART_Receive_IT(&Uart1_Handle,(uint8_t*)UART1_RxData.pRxbuf,UART1_RxDATA_SIZE);
	
}

/***************************************************
 * @brief   串口回调函数
 * @param   无
 * 
 * @return  无
***************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef Uart_PinInitStruct;

/* GPIO时钟使能 */
#ifdef	UART1_REMAP_ENABLE	
		__HAL_RCC_GPIOB_CLK_ENABLE();
#else
		__HAL_RCC_GPIOA_CLK_ENABLE();
#endif
	
	if(huart->Instance==USART1)
	{
		/* 串口1接收引脚初始化 */
		Uart_PinInitStruct.Pin=Uart1_Pin_Rx;				//接收引脚
		Uart_PinInitStruct.Mode=GPIO_MODE_INPUT;			//输入模式
		Uart_PinInitStruct.Pull=GPIO_PULLUP;
		Uart_PinInitStruct.Speed=GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(Uart1_Port,&Uart_PinInitStruct);
		
		/* 串口1发送引脚初始化 */
		Uart_PinInitStruct.Pin=Uart1_Pin_Tx;				//发送引脚
		Uart_PinInitStruct.Mode=GPIO_MODE_AF_PP;			//片上外设推挽输出
		Uart_PinInitStruct.Pull=GPIO_NOPULL;
		HAL_GPIO_Init(Uart1_Port,&Uart_PinInitStruct);
	}
}


/***************************************************
 * @brief   串口1中断处理函数
 * @param   无
 * 
 * @return  无
***************************************************/
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Uart1_Handle);
}

/***************************************************
 * @brief   串口回调函数
 * @param   huart指向串口的任务句柄
 * 
 * @return  无
***************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *husart)
{
	char *ptemp;
	if(husart->Instance==USART1)
	{
		
		/* 缓冲区切换 */
		ptemp=UART1_RxData.pRxbuf;							//临时存取已经接收完成的数据缓冲区指针
		UART1_RxData.pRxbuf=UART1_RxData.pReadBuf;			//接收缓冲区指针指向另一个缓冲区
		UART1_RxData.pReadBuf=ptemp;							//读取缓冲区指针指向完成数据接收的缓冲区
		HAL_UART_Receive_IT(&Uart1_Handle,(uint8_t *)UART1_RxData.pRxbuf,UART1_RxDATA_SIZE);
		UART1_RxData.Uart_Rxflag=Rxflag_Ture;		//置标志位
	}
}

