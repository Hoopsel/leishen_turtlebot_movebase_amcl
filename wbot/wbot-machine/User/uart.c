/**
  ******************************************************************************
  * @file    uart.c
  * @author  liu.c
  * @version V1.0
  * @date    2015��8��10��14:45:46
  * @brief   ��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:STM32BOARDV1.0
	* ��ַ��www.aiiage.com
  *
  ******************************************************************************
  */
#include "uart.h"
/**
  * @brief  USART1 GPIO ����,����ģʽ���á�9600 8-N-1
  * @param  ��
  * @retval ��
  */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
}

/// ����USART1�����ж�
void NVIC_Configuration_UART1(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/// ����USART4�����ж�
void NVIC_Configuration_UART4(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =14;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������USART4_Config
 * ����  ��USART4 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void UART4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config USART4 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* USART4 GPIO config */
   /* Configure USART4 Tx (PC.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	    
  /* Configure USART4 Rx (PC.11) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	  
	/* USART4 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(UART4, &USART_InitStructure); 
	
	/*	�����ж����ȼ� */
	NVIC_Configuration_UART4();
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

	USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4, USART_FLAG_TC);
}


/// �ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	
		return (ch);
}

/// �ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}


void put_char(unsigned char ch,USART_TypeDef * uart)
{		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(uart, USART_FLAG_TC) == RESET);		
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(uart, (uchar) ch);
}

void put_string(u8 *p,USART_TypeDef * uart)
{
   while(*p)
     put_char (*p++,uart);
   
}

void put_number(long int value,USART_TypeDef * uart)  
{
   u8 w,q,b,s,g,p[7]=" ";
   value=value%100000;
   if(value>=0)
   {    
      w=value/10000+48;
      q=value%10000/1000+48;
      b=value%1000/100+48;
      s=value%100/10+48;
      g=value%10+48;
      p[0]=' ';
      p[1]=w;
      p[2]=q;
      p[3]=b;
      p[4]=s;
      p[5]=g;
   }
   else
   {
      value=-value;
      w=value/10000+48;
      q=value%10000/1000+48;
      b=value%1000/100+48;
      s=value%100/10+48;
      g=value%10+48;
      p[0]='-';
      p[1]=w;
      p[2]=q;
      p[3]=b;
      p[4]=s;
      p[5]=g;
   
   }
   put_string(p,uart);
   
}
/*********************************************END OF FILE**********************/
