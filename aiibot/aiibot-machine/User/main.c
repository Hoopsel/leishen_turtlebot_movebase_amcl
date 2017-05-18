/**
  ******************************************************************************
  * @file    main.c
  * @author  liu.c
  * @version V1.0
  * @date    2015��8��10��10:05:39
  * @brief   main.c
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:STM32BOARDV1.0
	* ��ַ��www.aiiage.com
  *
  ******************************************************************************
  */
  
#include "main.h"
/**
  * @brief  ������
  * @param  ��
  * @retval ��
*/


int main(void)
{	
	int hehehe=1;	
	LED_GPIO_Config();	
	KEY_GPIO_Config();	
	Fdc_Init();   //������������ʼ��	
	//StepInfr_Gpio_Config();//�������λ�ü��	
	InfradDisInit(); //�������ʼ��	
	I2C_MPU6050_Init();//�����������ӿڳ�ʼ��			
	InitMPU6050();  //��ʼ��MPU6050 
	
	TIM3_PWM_Init();						//DC PWM
	DCMOTORCON_GPIO_Config();
	
	Encoder_Init_TIM2();
	Encoder_Init_TIM5();
	
	STEPMOTORCON_GPIO_Config();
	TIM6_Init();               //STEP1 ��2 ��3PWM
	
	USART1_Config();//���� ���� 
	NVIC_Configuration_UART1();
	
	UART4_Config(); //rk3288��ͨ��
	NVIC_Configuration_UART4();
	
	N5110_Init();//������ʾ����
	
	//UITRASONIC_Config();//����������
	
	PIDParaSet(2);//1:���ֳ� 2�����ֳ� 3��Knet  
	
	SysTick_Init(); //5ms�����ж�һ��
	
	//STEP2_Angle(360);//����
	//STEP1_Angle(360);//����
	
	
	//RotFlag = 1;
	//g_iRotaAngle =180;		 
	//g_fCarSpeedSet_temp = 10 ;
	
	//CCR2_Val = 500;
	//CCR3_Val = 500;
	while(1)
	{

#if 0  //�������
		STEP1_Angle( g_iStep1AngleCount ) ;

#elif 0
		scopetransport();				
// 			printf("%f \n",distance1);

#elif 0		//��λ��Э��				
		LCN5110shownum5(0,0,USART1_RX_BUF[0]);
		LCN5110shownum5(0,1,USART1_RX_BUF[1]);
		LCN5110shownum5(0,2,USART1_RX_BUF[2]);
		LCN5110shownum5(0,3,USART1_RX_BUF[3]);
		LCN5110shownum5(0,4,USART1_RX_BUF[4]);
		LCN5110shownum5(0,5,g_liTemp[3]);

#elif 1				
		Other_features();//����̧ͷ�����������⡢ˤ��
	  	Trouble_printf();//��ӡϵͳ���й�����Ϣ
		
#elif 0
		upload(1,1); 
//          STEP2_Angle(45);//����
// 		    STEP1_Angle(45);//����
		Delay(6000000);				 
		Delay(6000000);
		Delay(6000000);
		//Delay(6000000);				 
		//Delay(6000000);
		//Delay(6000000);
		
#elif 0
		g_fCarSpeedSet_temp=5;
		Delay(6000000);				 
		Delay(6000000);			 
		//g_iRotaAngle = -45;
			
		g_fCarSpeedSet_temp=0;
		Delay(6000000);				 
		Delay(6000000);
		
#elif 0

	 	Delay(6000000);				 
		Delay(6000000);			 
		Delay(6000000);
	    if(hehehe==0)
		{
			hehehe=1;
			RotFlag = 1;
			g_iRotaAngle = 180;
		}
	
		//Delay(6000000);				 
		//Delay(6000000);			 
		//Delay(6000000);				 
		//Delay(6000000);
		//Delay(6000000);
#endif	
	};	  
}

/*********************************************END OF FILE**********************/

