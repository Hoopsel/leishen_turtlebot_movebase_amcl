/**
  ******************************************************************************
  * @file    interrupt.c
  * @author  liu.c
  * @version V1.0
  * @date    2015��8��12��11:33:43
  * @brief   �жϴ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:STM32BOARDV1.0
	* ��ַ��www.aiiage.com
  *
  ******************************************************************************
  */
#include "interrupt.h"
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
#if 0
u16 USART1_RX_STA		=	0;
uchar USART1_RX_BUF[5] 	=	{0};
uchar USART1_REC_LEN 	=	5;
struct a
{
	int data;
	uchar flag; 
} * RxBuff;

long int g_liTemp[4]={0};
void USART1_IRQHandler(void)
{
	uchar ch=0;
	long int data=0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{ 
		//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//�رմ���1�жϣ��������ж������ش�
		ch =USART_ReceiveData(USART1);//(USART1->DR);//
		if((USART1_RX_STA&0x8000) == 0)//
		{
			if(USART1_RX_STA & 0x4000)//
			{
				if(ch!=0x0a) USART1_RX_STA = 0;//
				else USART1_RX_STA |= 0x8000;	//
			}
			else //
			{	
				if(ch == 0x0d) 
					USART1_RX_STA |= 0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF] = ch ;
					USART1_RX_STA++;
					if(USART1_RX_STA > (USART1_REC_LEN-1)) 
						USART1_RX_STA = 0;//
				}		 
			}
		}   		 
	}
		
	if( USART1_RX_STA & 0x8000 )
	{   
		USART1_RX_STA=0;
		data = USART1_RX_BUF[0] + (USART1_RX_BUF[1]<<8) + (USART1_RX_BUF[2]<<16) + (USART1_RX_BUF[3]<<24) ;
		//g_liTemp[3] = data ;
		switch (USART1_RX_BUF[4])
		{
		    case 1: MotorDeadValue=data;//STEP1_SPEED(data); break; 
		    case 2: STEP1_Angle(data); 			break;//g_fCarSpeedSet_temp=data;    break;תͷ
			case 9: STEP2_Angle(data);  		break;//��ͷ
		#ifdef BALANCE 
			case 3: g_fBlance_P=data/10.;       break;
			case 4: g_fBlance_D=data/100.;      break;
			case 5: g_fSpeed_P=data/10.;        break;
		    case 6: g_fSpeed_I=data/1000.;      break;
		#else
		    case 5: g_fSpeed_PL=data/10.;       break;
		    case 6: g_fSpeed_IL=data/1000.;     break;
		#endif
		    case 7: if(data>=10)data=10;else if(data<=-10)data=-10; g_fCarSpeedSet_temp = data; break;
		    //case 8: g_fDirectionPwm =data ;    break;
			case 8: if(data>=360)data=360;else if(data<=-360)data=-360;g_iRotaAngle = data; RotFlag = 1;RotFlag_init=1; break;//����ת��Ƕ� 
					  
			//case 97:send2scopeorserialflag = SERIAL ; break;
			//case 98:send2scopeorserialflag = SCOPE  ; break;
		    default: break;//for(i=0;i<4;i++)
			//uart_putchar (UART1, i);		    
		}
	}	
}

#elif 1

u16 BUF_NUM = 0;
u16 USART1_RX_STA		=	0;
uchar USART1_RX_BUF[8]	=	{0};
uchar USART1_REC_LEN	=	8;

float vLeft = 0.0;
float vRight = 0.0;

int HerdWore_vx 		= 	0;
int HerdWore_vth		=   0;
float vx			= 	0.0;
float vth			=  	0.0;

#define wheel_width 0.098		//����ֱ�� (m)
#define ticks_per_circle 486	//ÿȦ��������ֵ		= 18(������) * 27(���ٱ�)
#define ticks_meter 1720.397 	//ÿ�ױ�������ֵ 		1578.55719 = ticks_per_circle / (PI * wheel_width)
#define base_width 0.476f		//��� (m)			  0.455 / 0.9552
#define robot_timer 0.020f		//�������� 20 ms
#define VALUE_MARRE 10			//���������˲�����

void USART1_IRQHandler(void)
{
	uchar ch=0;
	long int data = 0;
	float leftWheelSpeed =  0.0;
	float rightWheelSpeed = 0.0;

	//���������ַ��� 0xff 0x00 0x00 0x00 0x00 0xfe
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{ 
		ch = USART_ReceiveData(USART1);//(USART1->DR);//	
		if((USART1_RX_STA & 0x8000) == 0)//	��δ�յ����� 0xff .... 0xfe �ַ���
		{
			if((USART1_RX_STA & 0x4000) == 0)// ��δ���յ� 0xff ��ʼλ
			{
				if(ch == 0xff) // ���յ���ʼλ 0xff 
				{
					USART1_RX_STA |= 0x4000;
					BUF_NUM = 0;
				}
			}
			else //�Ѿ����յ� 0xff ��ʼλ���ȴ����� 0xfe
			{
			 	if(BUF_NUM < USART1_REC_LEN)
				{
				 	USART1_RX_BUF[BUF_NUM & 0X3FFF] = ch ;
					BUF_NUM++;
				}
				else if(ch != 0xfe)	   //��������̫�࣬���½���
				{
					BUF_NUM = 0;
					USART1_RX_STA = 0;
				}
				else
				{//���յ����� 0xff .... 0xfe �ַ���
				 	USART1_RX_STA |= 0x8000;
				}			
			}
		}
	}   		 

	//���յ����� 0xff .... 0xfe �ַ���	
	if( USART1_RX_STA & 0x8000) 
	{   
		USART1_RX_STA=0;
		HerdWore_vx = (USART1_RX_BUF[3] << 24 | USART1_RX_BUF[2] << 16 | USART1_RX_BUF[1] << 8 | USART1_RX_BUF[0]);
		HerdWore_vth = (USART1_RX_BUF[7] << 24 | USART1_RX_BUF[6] << 16 | USART1_RX_BUF[5] << 8 | USART1_RX_BUF[4]);
		vx = *((float *) &HerdWore_vx);
		vth = *((float *) &HerdWore_vth);	

		if(vth == 0.0){
			vLeft = vRight = vx;
		}else if(vx == 0.0){
			vRight = vth * base_width / 2.0;
			vLeft = (-1) * vRight;
		}else
		{
			vRight = vx + vth * base_width / 2.0;
			vLeft = vx - vth * base_width / 2.0;
		}
		
		// (m/s) ת���� PID ����
		vRight = vRight * ticks_meter * robot_timer; // 31.571144 = (17 * 28 * 20 / (PI * 0.098 * 1000))		
		vLeft = vLeft * ticks_meter * robot_timer;

		g_fCarSpeedSetR = vRight;
		g_fCarSpeedSetL = vLeft;
	}
}
#endif


u16 UART4_RX_STA=0;
uchar UART4_RX_BUF[5]={0};
uchar UART4_REC_LEN=5;
uchar UART4ReiFlag=0;
void UART4_IRQHandler(void)
{//rk3288ͨ��Э��
	uchar ch=0;
	long int data=0;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
	{
		LED2(ON);
		ch =USART_ReceiveData(UART4);//(UART4->DR);//
		
		if((UART4_RX_STA&0x8000)==0)//
		{
			if(UART4_RX_STA&0x4000)//
			{

				if(ch!=11) 
					UART4_RX_STA=0;//
				else 
					UART4_RX_STA|=0x8000;	//
			}
			else //
			{	
				if(ch==14) 
					UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=ch ;
					UART4_RX_STA++;
					if( UART4_RX_STA> (UART4_REC_LEN-1) ) 
						UART4_RX_STA = 0;//
				}		 
			}
		}   		 
    }
		
	if( UART4_RX_STA&0x8000 )
    {   
		UART4_RX_STA=0;
        data = UART4_RX_BUF[0]+(UART4_RX_BUF[1]<<8) + (UART4_RX_BUF[2]<<16)+  (UART4_RX_BUF[3]<<24) ;
	  	if(data>100||data<-100)
		  	return;
	  	UART4ReiFlag=1;
		//g_liTemp[3] = data ;
        switch (UART4_RX_BUF[4])
        {
            case 1: if(data>=360)data=360;else if(data<=-360)data=-360; g_iRotaAngle = data;  RotFlag = 1;RotFlag_init=1; break;//����ת��Ƕ� 
			//case 1: g_fDirectionPwm = data ; break; 
            case 2: if(data>=10)data=10;else if(data<=-10)data=-10; g_fCarSpeedSet_temp = data;   break;
            case 3:                 ;        break;
            case 4: STEP2_Angle(data);       break;
            case 5: STEP1_Angle(data);       break;
            default: break;
            
        }
    }
}



/*����ϵͳ�δ�ʱ�� SysTick */
/*��stm32f10x.h �ļ� #define __NVIC_PRIO_BITS          1  */
void SysTick_Init (void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	SysTick_Config(SystemCoreClock /200);//200hzϵͳƵ��	
}


/*��ʱ���жϷ�����������ϵͳ��������*/
uint  Tick_Count=0;
uchar Control_Count=0;
static uchar Led_Count=0;
unsigned long int SYSTEMTIME = 0; //ϵͳʱ��
void SysTick_Handler()
{	
	static uchar UITRA_Count = 0; 
	Control_Count++;
	Tick_Count++;
	Led_Count++;
	SYSTEMTIME++;

	if( Led_Count%100 == 0)//ϵͳָʾ��
	{
		LED1_TOGGLE;
		Led_Count = 0;
	}
		
#ifdef  BALANCE
	
	Read_Encoder(); 
	if(Tick_Count==20)//100ms ����һ���ٶȿ�����
	{
		Tick_Count = 0;
		SpeedControl();
		Control_Count=0;
		//UITRA_Measure();
		DirControl();
	}
	//DirControlOut();
	SpeedControlOut();
	Angle_Calcu(); //�Ƕȼ���	
	BalanceControlOut();
	MotorControl();//pwm�������

#else

	if(Control_Count==4)//20ms ����һ���ٶȿ�����
	{   
	 	Control_Count=0;
	#if 0
		Angle_Calcu(); //�Ƕȼ��㣬��Ҫ��Ϊ��ʵ�ֻ�����ˤ��ʱ�ϴ����ݵ���λ��	2016.2.20			  
	  	//Speed_down();//��⳵���Ƿ�Ҫͣ������ͣ������������,//2016.4.12 ֹͣ�ٶȹ滮		
		Read_Encoder();
		RotaAngle(g_iRotaAngle);//����һ���Ƕ�ת
		SpeedControl();				
		SpeedControlOut();//�ٶȹ滮					  
		MotorControl();//pwm�������
		DCStallSolved();//�����ת										  					

	#elif 1
		Read_Encoder();
		Updata_Encoder_Data();
		SpeedControl(); 
		MotorControl();//pwm�������
		DCStallSolved();//�����ת
			
	#endif						
	}	 				
#endif		
}


uchar uc_TrigFlag=1;
void TIM6_IRQHandler(void)
{
	int delt=0;
	TIM6->SR=0x0000;//���жϱ�־
	
	//LED2_TOGGLE;

	//if( (g_iStep1TargetPulseNum>g_iStep1PresentPulseCount) && (g_iStep2TargetPulseNum==g_iStep2PresentPulseCount) ) 
	if((g_iStep1TargetPulseNum>g_iStep1PresentPulseCount)) 
	{
		g_iStep1PresentPulseCount++;
		delt = g_iStep1TargetPulseNum - g_iStep1PresentPulseCount;
		Step1SpeedPlan(delt);
		STEPMOTOR1_STEP;
		STEPMOTOR1_DIRCTION(RIGHT);
	}
	//else if( (g_iStep1TargetPulseNum<g_iStep1PresentPulseCount) && (g_iStep2TargetPulseNum==g_iStep2PresentPulseCount))
	else if( (g_iStep1TargetPulseNum<g_iStep1PresentPulseCount))
	{
		g_iStep1PresentPulseCount--;
		delt = g_iStep1TargetPulseNum - g_iStep1PresentPulseCount;
		Step1SpeedPlan(-delt);
		STEPMOTOR1_STEP;
		STEPMOTOR1_DIRCTION(LEFT);
	}

	if(g_iStep2TargetPulseNum > g_iStep2PresentPulseCount)
	{
		g_iStep2PresentPulseCount++;
		delt = g_iStep2TargetPulseNum - g_iStep2PresentPulseCount;
		Step2SpeedPlan(delt);
		STEPMOTOR2_STEP;
		STEPMOTOR2_DIRCTION(RIGHT);
	}
	else if(g_iStep2TargetPulseNum < g_iStep2PresentPulseCount)
	{
		g_iStep2PresentPulseCount--;
		delt = g_iStep2TargetPulseNum - g_iStep2PresentPulseCount;
		Step2SpeedPlan(-delt);
		STEPMOTOR2_STEP;
		STEPMOTOR2_DIRCTION(LEFT);
	}
}	

/**
  * @brief  ��������������
  * @param  ��
  * @retval ��
  */
vu16 vu16_Freq_Value[4]={0};

static u16 this_time_CH1 = 0; 
static u16 last_time_CH1 = 0;
static uchar CaptureEdgeFlag1=Rising;

static u16 this_time_CH2 = 0; 
static u16 last_time_CH2 = 0; 
static uchar CaptureEdgeFlag2=Rising;
//	
static u16 this_time_CH3 = 0; 
static u16 last_time_CH3 = 0; 
static uchar CaptureEdgeFlag3=Rising;
//	
static u16 this_time_CH4 = 0; 
static u16 last_time_CH4 = 0; 
static uchar CaptureEdgeFlag4=Rising;

void TIM4_IRQHandler(void)
{                                          
	if( TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET) 
	{ 
		//LED2_TOGGLE;
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		//TRIG1_LOW;
		//uc_TrigFlag=0;
		if(CaptureEdgeFlag1 == Rising) 
		{ 
			CaptureEdgeFlag1 = Falling;
			last_time_CH1 = TIM_GetCapture1(TIM4); 
			TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
		} 
		else 
		if(CaptureEdgeFlag1 == Falling ) 
		{ 
			CaptureEdgeFlag1 = Rising;
			this_time_CH1 = TIM_GetCapture1(TIM4); 
			if(this_time_CH1 > last_time_CH1) 
				vu16_Freq_Value[0] = (this_time_CH1 - last_time_CH1);//�����������صĸߵ�ƽ�����ʱ���¼����
			else 							
				vu16_Freq_Value[0] = ((0xFFFF - last_time_CH1) + this_time_CH1); //���糬�����ĸߵ�ƽ����ʱ�䳬����1�����ڼ���ֵ		 
			flag_srf=FINISH;//have measured
			TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Rising); //����Ϊ�����ز���	
		}
	}
//		else
//		if( TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET) 
//		{ 
//				
//				TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
////				TRIG1_LOW;
////				uc_TrigFlag=0; 
//				if( CaptureEdgeFlag2 == Rising ) 
//				{ 
//						CaptureEdgeFlag2 = Falling;
//						last_time_CH2 = TIM_GetCapture2(TIM4);
//						TIM_OC2PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���					
//				} 
//				else 
//				if(CaptureEdgeFlag2 == Falling ) 
//				{ 
//						CaptureEdgeFlag2 = Rising;
//						this_time_CH2 = TIM_GetCapture2(TIM4); 
//						if(this_time_CH2 > last_time_CH2) 
//								vu16_Freq_Value[1] = (this_time_CH2 - last_time_CH2); 
//						else 							
//								vu16_Freq_Value[1] = ((0xFFFF - last_time_CH2) + this_time_CH2); 		  
//						flag_srf=FINISH;//have measured
//						TIM_OC2PolarityConfig(TIM4, TIM_ICPolarity_Rising); //����Ϊ�����ز���							
//				}
////				LED2_TOGGLE;
//		}
//		else
// 		if( TIM_GetITStatus(TIM4, TIM_IT_CC3) == SET) 
//		{ 
//				
//				TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
////				TRIG1_LOW;
////				uc_TrigFlag=0; 
//				if( CaptureEdgeFlag3 == Rising ) 
//				{ 
//						CaptureEdgeFlag3 = Falling;
//						last_time_CH3 = TIM_GetCapture3(TIM4);
//						TIM_OC3PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���							
//				} 
//				else 
//				if(CaptureEdgeFlag3 == Falling ) 
//				{ 
//						CaptureEdgeFlag3 = Rising;
//						this_time_CH3 = TIM_GetCapture3(TIM4); 
//						if(this_time_CH3 > last_time_CH3) 
//								vu16_Freq_Value[2] = (this_time_CH3 - last_time_CH3); 
//						else 							
//								vu16_Freq_Value[2] = ((0xFFFF - last_time_CH3) + this_time_CH3); 		 
//						flag_srf=FINISH;//have measured
//						TIM_OC3PolarityConfig(TIM4, TIM_ICPolarity_Rising);//����Ϊ�½��ز���				
//				}
////				LED1_TOGGLE;
//		}
//		else
//		if( TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET) 
//		{ 
//				
//				TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
////				TRIG1_LOW;
////				uc_TrigFlag=0; 
//				if( CaptureEdgeFlag4 == Rising ) 
//				{ 
//						CaptureEdgeFlag4 = Falling;
//						last_time_CH4 = TIM_GetCapture4(TIM4); 
//						TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Falling);//����Ϊ�½��ز���				
//				} 
//				else 
//				if(CaptureEdgeFlag4 == Falling ) 
//				{ 
//						CaptureEdgeFlag4 = Rising;
//						this_time_CH4 = TIM_GetCapture4(TIM4); 
//						if(this_time_CH4 > last_time_CH4) 
//								vu16_Freq_Value[3] = (this_time_CH4 - last_time_CH4); 
//						else 							
//								vu16_Freq_Value[3] = ((0xFFFF - last_time_CH4) + this_time_CH4); 		 
//						flag_srf=FINISH;//have measured
//						TIM_OC4PolarityConfig(TIM4, TIM_ICPolarity_Rising);//����Ϊ�½��ز���				
//				}
////				LED1_TOGGLE;
//		}
		
//		LED2_TOGGLE;
//			LED2(ON);
//		LCN5110shownum5(1,1,vu16_Freq_Value[0]);
		
//		 GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)((1-GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13))));
}




