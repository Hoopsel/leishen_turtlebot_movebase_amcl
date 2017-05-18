/**
  ******************************************************************************
  * @file    assignment5.c
  * @author  liu.c
  * @version V1.0
  * @date    2015��10��20��10:25:05
  * @brief   ģ��IIC���ú������������ݺ��� ��Ӳ��iicDMA���á��ϴ�������λ��������ת�ַ�����������б�Ƕ�ʱ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:STM32BOARDV1.0
	* ��ַ��www.aiiage.com
  *
  ******************************************************************************
  */
	
#include "assignment5.h"




uint32_t a=0,b=0,c=0;
uint32_t g_uiFdc1Value = 0;   
uint32_t g_uiFdc2Value = 0; 
uint32_t g_uiFdc3Value = 0;   
uint32_t g_uiFdc4Value = 0; 
uint32_t k;
uchar g_ucTch1Flag = 0;
uchar g_ucTch2Flag = 0;
uchar g_ucTch3Flag = 0;
uchar g_ucTch4Flag = 0;
uchar g_ucTch1Flag_EN=0;
uchar g_ucTch2Flag_EN=0;
uchar g_ucTch3Flag_EN=0;
uchar g_ucTch4Flag_EN=0;
//////////////////////////////////////////////////////////////////////////////////
//�趨��ʱ

void delay_uss(u16 n)
{
	u8 k=0;
  for(k=0;k<n*5;k++);
}

//��ʼ��IIC
void IIC_Init(void)
{					     
 	RCC->APB2ENR|=1<<6;//��ʹ������IO PORTEʱ�� 							 
	GPIOE->CRL&=0XFFFF00FF;//PE2/3 �������
	GPIOE->CRL|=0X00003300;	   
	GPIOE->ODR|=3<<2;     //PE2,3 �����
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_uss(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_uss(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;
 	delay_uss(4); 
	IIC_SCL=1;//STOP:when CLK is high DATA change form low to high
 	delay_uss(4); 
	IIC_SDA=1;//����I2C���߽����ź� 						   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_uss(1);	   
	IIC_SCL=1;delay_uss(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_uss(2);
	IIC_SCL=1;
	delay_uss(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_uss(2);
	IIC_SCL=1;
	delay_uss(2);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_uss(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_uss(2); 
		IIC_SCL=0;	
		delay_uss(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��      ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_uss(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_uss(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
//�������ݵ��Ĵ���
void Signal_Send(unsigned char addr,unsigned char data1,unsigned char data2)
{
	  IIC_Start();
	
	  IIC_Send_Byte(SLAVE_ADDRESS);    //д����
	  IIC_Wait_Ack();
	
	
	  IIC_Send_Byte(addr);       //���ͼĴ�����ַ
		IIC_Wait_Ack();
	
	  IIC_Send_Byte(data1);      //���͸�λ��ַ
		IIC_Wait_Ack();
	
	  IIC_Send_Byte(data2);      //���͵�λ��ַ
		IIC_Wait_Ack();
	
	  IIC_Stop();
}
//�ӼĴ����ж�ȡ����
u16 Signal_Read(unsigned char readaddr)
{   
	  u16 reg_data,reg_data1,reg_data2;
	
    IIC_Start();
    IIC_Send_Byte(SLAVE_ADDRESS);        //д����
	  IIC_Wait_Ack();
	  IIC_Send_Byte(readaddr);
	  IIC_Wait_Ack();
	
	  IIC_Start();
	  IIC_Send_Byte(SLAVE_ADDRESS+1);      //������
	  IIC_Wait_Ack();
	
	  reg_data1=IIC_Read_Byte(1);          //��ȡ��λ����
	
	  reg_data2=IIC_Read_Byte(0);          //��ȡ��λ����

	  IIC_Stop();
	  reg_data=(reg_data1<<8)+reg_data2;
	 
	  return reg_data;  //���شӼĴ����ж�ȡ������   
}
void FDC1004_Init1(void)   //FDC1004��ʼ������
{
      Signal_Send(CONF_MEAS1,0x1C,0x00);
			Signal_Send(FDC_CONF,0x04,0x80);
}

u32 FDC1004_Read1(void)  //FDC1004 ��ȡ����ֵ����
{
      a=Signal_Read(MEAS1_MSB);
			b=Signal_Read(MEAS1_LSB);
			c=(a<<8)+(b>>8);
      return c;
}
void FDC1004_Init2(void)   //FDC1004��ʼ������
{
      Signal_Send(CONF_MEAS2,0x3C,0x00);
			Signal_Send(FDC_CONF,0x04,0x40);
}
u32 FDC1004_Read2(void)  //FDC1004 ��ȡ����ֵ����
{
      a=Signal_Read(MEAS2_MSB);
			b=Signal_Read(MEAS2_LSB);
			c=(a<<8)+(b>>8);
      return c;
}
void FDC1004_Init3(void)   //FDC1004��ʼ������
{
      Signal_Send(CONF_MEAS3,0x5C,0x00);
		  Signal_Send(FDC_CONF,0x04,0x20);
}
u32 FDC1004_Read3(void)  //FDC1004 ��ȡ����ֵ����
{
      a=Signal_Read(MEAS3_MSB);
			b=Signal_Read(MEAS3_LSB);
			c=(a<<8)+(b>>8);
      return c;
}
void FDC1004_Init4(void)   //FDC1004��ʼ������
{
      Signal_Send(CONF_MEAS4,0x7C,0x00);
		  Signal_Send(FDC_CONF,0x04,0x10);
}
u32 FDC1004_Read4(void)  //FDC1004 ��ȡ����ֵ����
{
      a=Signal_Read(MEAS4_MSB);
			b=Signal_Read(MEAS4_LSB);
			c=(a<<8)+(b>>8);
      return c;
}



void Fdc_Init(void)
{
		IIC_Init();
}

uint32_t Fdc_ReadCh1(void)
{
			//��ʱһ��ʱ��
	    FDC1004_Init1(); 
      return FDC1004_Read1();   //��ȡ����ֵ			
}
uint32_t Fdc_ReadCh2(void)
{
			//��ʱһ��ʱ��
	    FDC1004_Init2(); 
      return FDC1004_Read2();   //��ȡ����ֵ			
}
uint32_t Fdc_ReadCh3(void)
{
			//��ʱһ��ʱ��
	    FDC1004_Init3(); 
      return FDC1004_Read3();   //��ȡ����ֵ			
}
uint32_t Fdc_ReadCh4(void)
{
			//��ʱһ��ʱ��
	    FDC1004_Init4(); 
      return FDC1004_Read4();   //��ȡ����ֵ			
}

unsigned long int g_ulFdcTime = 0;
/*
 * ��������adc_init
 * ����  ��adc ��ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
void ADC1_Init(void)
{		
	//PA4 ,ͨ��4��ADC1
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStrucutre;
	
	/* 1 */
  //GPIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//PA.4���ó�ģ����������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* 2*/
	//����ADC1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );
	//6��Ƶ
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	
	//��λADC1��������ADC1��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_DeInit(ADC1); 
	
	ADC_InitStrucutre.ADC_ContinuousConvMode=DISABLE;//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStrucutre.ADC_DataAlign=ADC_DataAlign_Right;//ADC�����Ҷ���
	ADC_InitStrucutre.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//ת��������������ⲿ����
	ADC_InitStrucutre.ADC_Mode=ADC_Mode_Independent;//ADC�����ڶ���ģʽ��
	ADC_InitStrucutre.ADC_NbrOfChannel=1;//˳�����ת���Ĺ���ģʽ
	ADC_InitStrucutre.ADC_ScanConvMode=DISABLE;//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_Init(ADC1,&ADC_InitStrucutre);
	
	//ʹ��ָ����ADC1
	ADC_Cmd(ADC1, ENABLE);	
	
	//У׼
	ADC_ResetCalibration(ADC1); //ʹ�ܸ�λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	//ʹ��ָ����ADC1�����ת����������	
	
}	

/*
 * ��������Get_Adc           
 * ����  : ���ԭʼAdcֵ  
 * ����  ����  
 * ���  ������adc  
 * ����  ���ڲ�����
 */	
//���ADC��ֵ��chΪͨ�� PA4Ϊͨ��4
  u16 Get_Adc(void)
	{
		//����ָ����ADC�Ĺ���ͨ���飬һ�����У�����ʱ��
		ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1,ADC_SampleTime_239Cycles5 );		    
		//ʹ���ƶ���ADC1�����ת����������
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
		//�ȴ�ת������
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
		//�������һ�ε�ADC1�������ת�����
		return ADC_GetConversionValue(ADC1);
	}

/*
 * ��������Get_adc_average        
 * ����  : ���ƽ��������adcֵ
 * ����  ������ƽ���Ĵ���
 * ���  ��ƽ����adcֵ
 * ����  ���ڲ�����
 */
	
	u16 Get_adc_average(u8 times)
	{
		u32 temp_val=0;
		u8 t;
		for(t=0;t<times;t++)
		{
			temp_val+=Get_Adc();
			//Delay(50000);
			Delay(59000);//10ms
		}
		return temp_val/times;
	} 	 
	
	void  InfradDisInit(void)
	{//�����๦�� ��ʼ��
			ADC1_Init();
	}
/*
 * ��������Get_distance        
 * ����  : ��ȡ������ֵ
 * ����  ����
 * ���  ��float
 */
	float g_fInfrDis =0;
	float GetInfrDis(void)
	{
		u16 value;
	  float voltage;
	  float distance;
	  //�õ������ADCֵ
		value=Get_adc_average(4);
		//ADCֵת��Ϊ��ѹֵ
		voltage=value*3.3/4096;
		//����datasheet�ϵ�ѹֵ�;����Ӧ�����ߣ����н���������ϵõ��ĵ�ѹֵ�;���Ĺ�ϵ
		distance=1/((voltage-0.179922)/22.081450);
		
		return distance;
	}
	
	void Send2RK3288_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED������ʱ��*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE ); 
	
		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		
		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
			
		/*ѡ��Ҫ���Ƶ�����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	
		GPIO_Init(GPIOE, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOE, GPIO_Pin_15);	
}
		
	void Send2RK3288(int num)
	{	
				static uchar i;
				i = sizeof(num);
	}
	
	
void StepInfr_Gpio_Config(void)
{		

		GPIO_InitTypeDef GPIO_InitStructure;
		
		/*���������˿ڣ�PD����ʱ��*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
				
			/*ѡ��Ҫ���Ƶ�����*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_15; 	
		
		GPIO_Init(GPIOE, &GPIO_InitStructure);			
			/*ѡ��Ҫ���Ƶ�����*/	

////////////////////////////////////////////////////////			
			/*���������˿ڣ�PA����ʱ��*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
				
			/*ѡ��Ҫ���Ƶ�����*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12; 	
		
		GPIO_Init(GPIOA, &GPIO_InitStructure);			

}
//PA12 ���Ҳ��� �ϲ� 
// PA11 ���Ҳ��� �²�
// 0: ���� 
// 1: ���� 
void Step1_2Zero(void)
{
		char i = 0;
	
		if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12) == 0 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 1 )
				  STEP1_Angle(i++) ;	
		else
		if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12) == 1 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0 )
			    STEP1_Angle(i--) ;
		else
		if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12) == 1 && GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 1)
					g_iStep1PresentPulseCount = 0;
		
		
}
void Step2_2Zero(void)
{
//		if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15) == 0 && GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7) == 1 )
				 				
}

void EvadeBarrier(void)
{
		static float fInfrDis_Speed = 0;
		static uchar fInfrDis_Flag = 0;
	  static uchar fInfrDis_Flag2 = 0;
	  static long int fInfrDis_Flag_time=0;
	
		g_fInfrDis   =  GetInfrDis();
	
		if(g_fInfrDis <= 10)
		{			
			 if(fInfrDis_Flag == 0)
			 {
					fInfrDis_Speed = g_fCarSpeedSet_temp;
			 }			
			 g_fCarSpeedSet_temp = -4;
			 fInfrDis_Flag = 1;				 
		}
		else if( fInfrDis_Flag == 1 )
		{
			 if(g_fInfrDis > 10)
			 {
				 g_fCarSpeedSet_temp = fInfrDis_Speed ;
				 fInfrDis_Flag = 0;
			 }			
		}
}


void TchResp(void)
{
	//Fdc1ȡƽ��ֵ
	uint32_t Fdc1_count=0;
	uint32_t Fdc1Value_count=0;
	uint32_t Fdc1Value=0;
	uint32_t Fdc1_time=0;
	
	//Fdc2ȡƽ��ֵ
	uint32_t Fdc2_count=0;
	uint32_t Fdc2Value_count=0;
	uint32_t Fdc2Value=0;
	uint32_t Fdc2_time=0;
	
	//Fdc3ȡƽ��ֵ
	uint32_t Fdc3_count=0;
	uint32_t Fdc3Value_count=0;
	uint32_t Fdc3Value=0;
	uint32_t Fdc3_time=0;	
	
// 		g_uiFdc1Value = Fdc_ReadCh1();
// 		Delay(50000);
	
	 // g_uiFdc2Value = Fdc_ReadCh2();
	//  Delay(50000);
	
// 		g_uiFdc3Value = Fdc_ReadCh3();
// 		Delay(50000);
// 		g_uiFdc4Value = Fdc_ReadCh4();
// 		Delay(50000);
	/*
		if(g_uiFdc1Value>500000 || g_uiFdc3Value>500000)//�����߶���
		{
			
				int temp = Step1PresAngle();//��ǰ�Ƕȶ�Ӧ��������
			
				if(  temp <= -28 )//����ǰ��������ת��ת���Ƕȴ���28��
						STEP1_Angle(30);//�����ң�תͷ30��
				else
				if( (temp >= 28) || ((!g_ucTch1Flag)&&(!g_ucTch3Flag)) ) 	
						STEP1_Angle(-30);
				
				g_ucTch1Flag = 1;
				g_ucTch3Flag = 1;
		}
		else//û��������
		if(g_ucTch1Flag == 1||g_ucTch3Flag == 1)
		{
				g_ucTch1Flag = 0;
				g_ucTch3Flag = 0;		
				STEP1_Angle(0);
			
		}
		*/
		
#if 0
		//Fdc1ȡƽ��ֵ 2016.4.6
		for(Fdc1_count=0;Fdc1_count<5;Fdc1_count++)
		{
			Fdc1Value = Fdc_ReadCh1();
			Fdc1Value_count=Fdc1Value_count+Fdc1Value;
		  Delay(295000);	//��ʱ50ms ������̫С��̫СƵ������IICȥ���������׳���		
    }
		g_uiFdc1Value=(int)(Fdc1Value_count/5);
		
		//Fdc2ȡƽ��ֵ 2016.4.6
		for(Fdc2_count=0;Fdc2_count<5;Fdc2_count++)
		{
			Fdc2Value = Fdc_ReadCh2();
			Fdc2Value_count=Fdc2Value_count+Fdc2Value;
		  Delay(295000);	//��ʱ50ms ������̫С��̫СƵ������IICȥ���������׳���		
    }
		g_uiFdc2Value=(int)(Fdc2Value_count/5);
		
	  //Fdc3ȡƽ��ֵ 2016.4.6
		for(Fdc3_count=0;Fdc3_count<5;Fdc3_count++)
		{
			Fdc3Value = Fdc_ReadCh3();
			Fdc3Value_count=Fdc3Value_count+Fdc3Value;
		  Delay(295000);	//��ʱ50ms ������̫С��̫СƵ������IICȥ���������׳���		
    }
		g_uiFdc3Value=(int)(Fdc3Value_count/5);
					
#endif
		
#if 1
		//һ��ȡƽ��ֵ 2016.4.6
		for(Fdc1_count=0;Fdc1_count<3;Fdc1_count++)
		{
			Fdc1Value = Fdc_ReadCh1();
			Fdc1Value_count=Fdc1Value_count+Fdc1Value;
		  Delay(295000);	//��ʱ50ms ������̫С��̫СƵ������IICȥ���������׳���	
			
			Fdc2Value = Fdc_ReadCh2();
			Fdc2Value_count=Fdc2Value_count+Fdc2Value;
		  Delay(295000);	//��ʱ50ms ������̫С��̫СƵ������IICȥ���������׳���
			
			Fdc3Value = Fdc_ReadCh3();
			Fdc3Value_count=Fdc3Value_count+Fdc3Value;
		  Delay(295000);	//��ʱ50ms ������̫С��̫СƵ������IICȥ���������׳���
    }
		g_uiFdc1Value=(int)(Fdc1Value_count/3);
		g_uiFdc2Value=(int)(Fdc2Value_count/3);
		g_uiFdc3Value=(int)(Fdc3Value_count/3);		
					
#endif	
/**************��ͷ���������*************/
	  if( (g_uiFdc2Value>1000000) && (g_ucTch2Flag_EN==0) )//��ͷ��,g_ucTch2Flag_EN������ֻ�ý���һ��
	  {
				g_ulFdcTime = SYSTEMTIME;
			
			  if( (Step2PresAngle()) >= (-10) )//��ǰͷ�ĽǶȴ�������-10��״̬���� ��Ȼ�ٵ�ͷ���ܻῨ����
				{
					 upload(1,1);//������ͷ�ϴ����ݵ���λ��
					 Delay(600000);//100ms
					
					 g_ucTch2Flag = 1;
					 g_ucTch2Flag_EN=1;	
				   STEP2_Angle( (Step2PresAngle()-20) );//�ڵ�ǰ�ĽǶ��µ�ͷ20��				     
				}
		}
		else if( (g_ucTch2Flag==1) && ((SYSTEMTIME-g_ulFdcTime)>500) && (g_uiFdc2Value<1000000) )//g_uiFdc2Value<500000 �ȴ��ַſ�
		{
			  upload(2,1);//��̧��ͷ�ϴ����ݵ���λ��
			  Delay(600000);//100ms
			
				g_ucTch2Flag = 0;
			  g_ucTch2Flag_EN=0;	
				STEP2_Angle( (Step2PresAngle()+20) );//�ڵ�ǰ�ĽǶ���̧ͷ20��			  
		}
		
/**************���Ҷ�����Ի����ˣ��������******������Ϊ����̧ͷ*************/
	  //if( (g_uiFdc1Value>1000000) && (g_ucTch1Flag_EN==0) )//��ͷ��,g_ucTch1Flag_EN������ֻ�ý���һ��
		
		if( (g_uiFdc1Value>1000000) )//���Ҷ�
	  {
				//g_ulFdcTime = SYSTEMTIME;
			
			  //if( (Step2PresAngle()) <= 20 )//��ǰͷ�ĽǶȴ�������45��(�����Ѿ�̧��25�� ���Ժ���ֻ��̧20��)״̬���� ��Ȼ��̧ͷ���ܻῨ����
				{
					 //upload(1,1);//������ͷ�ϴ����ݵ���λ��
					// Delay(600000);//100ms
					
					 g_ucTch1Flag = 1;
					 g_ucTch1Flag_EN=1;
					
				   g_iStep2PlanThres=0;// Ĭ��500
           g_uiStep2LowSpeed=400;//Ĭ��15000
           g_uiStep2HighSpeed=800;//Ĭ��800
					
				   STEP2_Angle( (Step2PresAngle()+5) );//�ڵ�ǰ�ĽǶ�̧ͷ5��	
					
				}
		}
		//else if( (g_ucTch1Flag==1) && ((SYSTEMTIME-g_ulFdcTime)>700) && (g_uiFdc1Value<1000000) )//g_uiFdc1Value<500000 �ȴ��ַſ�		
		else if( (g_ucTch1Flag==1) && (g_uiFdc1Value<1000000) )//g_uiFdc1Value<500000 �ȴ��ַſ�
		{
			  //upload(2,1);//��̧��ͷ�ϴ����ݵ���λ��
			  //Delay(600000);//100ms
			
				g_ucTch1Flag = 0;
			  g_ucTch1Flag_EN=0;	
				//STEP2_Angle( (Step2PresAngle()+20) );//�ڵ�ǰ�ĽǶ���̧ͷ20��		

				g_iStep2PlanThres=500;// Ĭ��500
        g_uiStep2LowSpeed=15000;//Ĭ��15000
        g_uiStep2HighSpeed=500;//Ĭ��800
		}
		
/**************���������Ի����ˣ��������******������Ϊ������ͷ*************/
		
		if( (g_uiFdc3Value>1000000) )//�����
	  {
				//g_ulFdcTime = SYSTEMTIME;
			
			  //if( (Step3PresAngle()) <= 20 )//��ǰͷ�ĽǶȴ�������45��(�����Ѿ�̧��25�� ���Ժ���ֻ��̧20��)״̬���� ��Ȼ��̧ͷ���ܻῨ����
				{
					 //upload(1,1);//������ͷ�ϴ����ݵ���λ��
					// Delay(600000);//100ms
					
					 g_ucTch3Flag = 1;
					 g_ucTch3Flag_EN=1;
					
				   g_iStep2PlanThres=0;// Ĭ��500
           g_uiStep2LowSpeed=400;//Ĭ��15000
           g_uiStep2HighSpeed=800;//Ĭ��800
					
				   STEP2_Angle( (Step2PresAngle()-5) );//�ڵ�ǰ�ĽǶ�̧ͷ5��	
					
				}
		}
		//else if( (g_ucTch3Flag==1) && ((SYSTEMTIME-g_ulFdcTime)>700) && (g_uiFdc3Value<1000000) )//g_uiFdc1Value<500000 �ȴ��ַſ�		
		else if( (g_ucTch3Flag==1) && (g_uiFdc3Value<1000000) )//g_uiFdc1Value<500000 �ȴ��ַſ�
		{
			  //upload(2,1);//��̧��ͷ�ϴ����ݵ���λ��
			  //Delay(600000);//100ms
			
				g_ucTch3Flag = 0;
			  g_ucTch3Flag_EN=0;	
				//STEP2_Angle( (Step2PresAngle()-20) );//�ڵ�ǰ�ĽǶ���̧ͷ20��		

				g_iStep2PlanThres=500;// Ĭ��500
        g_uiStep2LowSpeed=15000;//Ĭ��15000
        g_uiStep2HighSpeed=500;//Ĭ��800
		}
	

}


int g_iRotaAngle = 0;
uchar RotFlag = 0;
uchar RotFlag_init = 0;
uchar Rot_PIDFlag = 0;
int s_iMileage1 =0 ,s_iMileage2 = 0;

#if 0 //Ҫ�л�PID
void RotaAngle(int Spanangle)
{   

	
	  if(Rot_PIDFlag!=0)//�л�PID
	  {
			Rot_PIDFlag++;
			if(Rot_PIDFlag==10)//10*20ms
			{
	  	  g_fSpeed_PL=5;  g_fSpeed_IL=0.8; g_fSpeed_DL = 0.55 ;
				g_fSpeed_PR=5;  g_fSpeed_IR=0.8; g_fSpeed_DR = 0.55;
				Rot_PIDFlag = 0;
			}
    }
		
		if(RotFlag)//ת���Ƕȱ�־λ
		{		
			  						
			 /////////�л�PID ת�Ƕȵ�PID ////////			 
			 g_fSpeed_PL=15;  g_fSpeed_IL=8; g_fSpeed_DL = 0.5 ;
			 g_fSpeed_PR=15;  g_fSpeed_IR=8; g_fSpeed_DR = 0.5;				
        ///////////////////////		
				
			 
				s_iMileage1 += g_iEncoderSigmaL;
				s_iMileage2 += g_iEncoderSigmaR;
				if(Spanangle > 0)  //��ת���Ƕȴ���0
				{
						g_fDirectionPwm = 2;
						if((s_iMileage1-s_iMileage2) >= 5.28 * Spanangle)
						{
								s_iMileage1 = 0;
								s_iMileage2 = 0;
								g_fDirectionPwm = 0;
								RotFlag = 0;
							
							  Rot_PIDFlag = 1;//�л�PID��־												
						}			
				}
				else   //��ת���Ƕ�С��0
				{
					 g_fDirectionPwm = -2;	
					 if((s_iMileage2 - s_iMileage1) >= 5.28 * (-Spanangle))
					 {
								s_iMileage1 = 0;
								s_iMileage2 = 0;
								g_fDirectionPwm = 0;
								RotFlag = 0;
						 
						    Rot_PIDFlag = 1;//�л�PID��־
					 }
				}
		}
}
#endif

#if 0 //���ٶȹ滮
//�����л�PID
void RotaAngle(int Spanangle)
{   
		if(RotFlag)//ת���Ƕȱ�־λ
		{
				s_iMileage1 += g_iEncoderSigmaL;
				s_iMileage2 += g_iEncoderSigmaR;
				if(Spanangle > 0)  //��ת���Ƕȴ���0
				{
						g_fDirectionPwm = 5;
						if((s_iMileage1-s_iMileage2) >= 5.2864 * Spanangle)
						{
								s_iMileage1 = 0;
								s_iMileage2 = 0;
								g_fDirectionPwm = 0;
								RotFlag = 0;
						}			
				}
				else   //��ת���Ƕ�С��0
				{
					 g_fDirectionPwm = -2;	
					 if((s_iMileage2-s_iMileage1) >= 5.2864 * (-Spanangle))
					 {
								s_iMileage1 = 0;
								s_iMileage2 = 0;
								g_fDirectionPwm = 0;
								RotFlag = 0;
					 }
				}
		}
}
#endif 

#if 1 //�����ٶȹ滮
/*
delt = g_iSpeedControlValueNew - g_iSpeedControlValueOld ;
			g_fSpeedPwm = delt*count/20. + g_iSpeedControlValueOld ;
			if(count == 20)
				count=0;
*/


//�����л�PID
void RotaAngle(int Spanangle)
{
  static int RotaAngle_count=0,RotaAngle_count_old=0;
	static float xi_shu=0.;
	
	if(RotFlag_init == 1 )//Ϊ����ת�ǶȵĹ������ٴ������µĽǶ�ֵʱ��ʼ����һ�εĲ���
	{
		RotFlag_init = 0;
		
	  xi_shu=0.;
		s_iMileage1 = 0; 
		s_iMileage2 = 0;
		g_fDirectionPwm = 0;
		g_fDirectionPwm_old=0;		
		RotaAngle_count=0;
		RotaAngle_count_old=0;
	}
	
	if( (RotFlag==1) && (RotFlag_init==0) )//ת���Ƕȱ�־λ
	{			
		s_iMileage1 += g_iEncoderSigmaL;
		s_iMileage2 += g_iEncoderSigmaR;
	
		if( (Spanangle>=10 && Spanangle<=360) || (Spanangle<=-10 && Spanangle>=-360))//ת������10�Ⱦ�Ϊǰ20%�ͺ�20%Ϊ�ٶȹ滮
			xi_shu=0.2;
		else            //����һ�����һ�����
			xi_shu=0.5;
		
		if(Spanangle > 0)  //��ת���Ƕȴ���0
		{				
				if((s_iMileage1-s_iMileage2) <= (5.2864 * Spanangle*xi_shu))//��ʼ20%����
				{							
					 RotaAngle_count++;
					 g_fDirectionPwm=RotaAngle_count*0.1;//y=k*x
					 g_fDirectionPwm_old=g_fDirectionPwm;
					 RotaAngle_count_old=RotaAngle_count;
				}
				if( (s_iMileage1-s_iMileage2) > (5.2864 * Spanangle*(1-xi_shu)) )//80%�Ժ�Ϊ����
				{
					 if((s_iMileage1-s_iMileage2) >= (5.2864 * Spanangle) )//�ж�ת���Ƕ��Ƿ񵽴�
					 {
							s_iMileage1 = 0;
							s_iMileage2 = 0;
							g_fDirectionPwm = 0;
							g_fDirectionPwm_old=0;
							RotFlag = 0;
							RotaAngle_count=0;
							RotaAngle_count_old=0;							
					 }
					 else	//����80%-100%֮��Ϊ����
					 {	
						 RotaAngle_count--;
						 g_fDirectionPwm=g_fDirectionPwm_old-(RotaAngle_count_old-RotaAngle_count)*0.1;
						 if(g_fDirectionPwm<=0)
							 g_fDirectionPwm=0;
					 }							
				}		
		}
		else if(Spanangle < 0)   //��ת���Ƕ�С��0
		{			
			 if((s_iMileage2 - s_iMileage1) <= (5.2864 * (-Spanangle)*xi_shu))//��ʼ20%����
				{					
					 RotaAngle_count--;
					 g_fDirectionPwm=RotaAngle_count*0.1;//y=k*x
					 g_fDirectionPwm_old=g_fDirectionPwm;
					 RotaAngle_count_old=RotaAngle_count;
				}
				if((s_iMileage2 - s_iMileage1) > (5.2864 * (-Spanangle)*(1-xi_shu)))//80%�Ժ�Ϊ����
				{
					 if((s_iMileage2 - s_iMileage1) >= (5.2864 * (-Spanangle)) )//�ж�ת���Ƕ��Ƿ񵽴�
					 {
							s_iMileage1 = 0;
							s_iMileage2 = 0;
							g_fDirectionPwm = 0;
							g_fDirectionPwm_old=0;
							RotFlag = 0;
							RotaAngle_count=0;
							RotaAngle_count_old=0;
					 }
					 else//����80%-100%֮��Ϊ����
					 {						 
						 RotaAngle_count++;
						 g_fDirectionPwm=g_fDirectionPwm_old-(RotaAngle_count_old-RotaAngle_count)*0.1;
						 if(g_fDirectionPwm>=0)
								g_fDirectionPwm=0;
						}
				 }
			}
	}
}
#endif 


//���ø�λsleep�������ת����
uchar DCReset_Count=0;   //��λ����	
uchar DCshutdown_Count=0; //��ֹ������������߽Ӵ��������߶��˵���PID����һֱ�ۼӵ�����ת
void DCStallSolved(void) //�����ת����
{
		static uchar StallFlag = 1;  //��ת��־λ���жϸ����ٶȺͱ����������ٶ��Ƿ�һֱ��һ��
	
		if( (g_fCarSpeedSetL!=0 && g_fCarSpeedL==0)|| (g_fCarSpeedSetR!=0 &&  g_fCarSpeedR==0 ))							
			 StallFlag = 1;
		else
			 StallFlag = 0;	
		
		if(StallFlag)
		{
			 DCReset_Count++;
			 DCshutdown_Count++;
		}
		else
		{
			 DCReset_Count = 0;
			 DCshutdown_Count=0;	
		}		
		 //if(DCReset_Count >= 25)//20*25=500ms
		if(DCReset_Count >= 10)//20*10=200ms 
		 //if(DCReset_Count >= 2)//20*2=40ms
		 {
					DCReset_Count=0;
			 
					DCMOTOR1_DERCTION(BACK);
					DCMOTOR2_DERCTION(BACK);
					DCMOTOR_NSLEEP(ON);
					Delay(10);
					DCMOTOR1_DERCTION(FORWARD);
					DCMOTOR2_DERCTION(FORWARD);
					DCMOTOR_NSLEEP(OFF);
			 
// 					g_fSpeedIntegralR=0;//��PID�ۼӵĻ�����0
// 					g_fSpeedIntegralL=0;
			 
// 					g_iRotaAngle =0;//���ת��ĽǶ�
// 					RotFlag = 0;
// 					RotFlag_init=1;
					
					Debug_printf_flage=1;//�������ϴ�ӡ������Ϣ				    
		 }
		 if(DCshutdown_Count>=150)//3S�ڱ�����һֱû�з�����ֵ��ʹ���ٶȱ�0 ��ֹ������һֱ����ת,��������3S�ڻ��ֻ��ǻ�һֱ�ۼӴӶ�ʹ�õ�����ǻ������ת
		 {
			 DCshutdown_Count=0;
			 
			 g_fCarSpeedSet_temp=0;
			 g_fCarSpeedSet=0; 
			 g_fDirectionPwm=0;
			 g_fSpeedIntegralR=0;//��PID�ۼӵĻ�����0
			 g_fSpeedIntegralL=0;//��PID�ۼӵĻ�����0
			 
			 g_iRotaAngle =0;//���ת��ĽǶ�
			 RotFlag = 0;
			 RotFlag_init=1;
			 
			 Debug_printf_flage=2;//�������ϴ�ӡ������Ϣ				
			 
		 }
		 
}



//��λ���ϴ����ݸ���λ��Э�� :��Ч����λdata��4���ֽڣ�+���ܱ�־λflag��1���ֽڣ�+������������2���ֽڣ�
void upload(int data,int flag)
{
	char data_char[5]="0000";
	char flag_char[5]="0000";
	char end_char[5]="1411";
  char zheng_he[20]={0};
	
	su_zifu(data,data_char);//����ת�ַ�
	su_zifu(flag,flag_char);//����ת�ַ�

#if 1
	zheng_he[0]=data_char[0];
  zheng_he[1]=data_char[1];
	zheng_he[2]=data_char[2];
  zheng_he[3]=data_char[3];

	zheng_he[4]=flag_char[0];
  zheng_he[5]=flag_char[1];
	zheng_he[6]=flag_char[2];
  zheng_he[7]=flag_char[3];

	zheng_he[8]=end_char[0];
  zheng_he[9]=end_char[1];
	zheng_he[10]=end_char[2];
  zheng_he[11]=end_char[3];
	
  zheng_he[12]=0x0d;
  zheng_he[13]=0x0a;
	
	put_string((u8*)zheng_he,UART4);//���ͽ������ַ���
  //zheng_he[20]=0;
	
#endif	


#if 0	
/*����4*/	
	put_string((u8*)data_char,UART4);//���������ַ���
	put_string((u8*)flag_char,UART4);//���ͱ�ʶ���ַ���
	put_string((u8*)end_char,UART4);//���ͽ������ַ���
		
	put_char(0x0d,UART4); 
	put_char(0x0a,UART4);
	
#endif	
	
/*����1*/
//   put_string((u8*)data_char,USART1);//���������ַ���
// 	put_string((u8*)flag_char,USART1);//���ͱ�ʶ���ַ���
// 	put_string((u8*)end_char,USART1);//���ͽ������ַ���
// 	
// 	put_char(0x0d,USART1); 
// 	put_char(0x0a,USART1); 
}

//����ת�ַ�
void su_zifu(int data,char*p)
{
	 int i=0;
	 while(data>0)
	 {
			p[3-(i++)]=(data%10)+48;
		  data=data/10;
		}
		p[3-i]=data+48;	
}

 static uchar fall_down_flag=0;
//����������бˮƽ����20��ʱ
void fall_down(void)
{

	if( (Angle_qingxie>20) && (fall_down_flag==0) )//����������бˮƽ����20��ʱ ����λ��������
	{
		fall_down_flag=1;
		upload(1,2);//����λ��������
  }
	Delay(600000);//100ms
	if( (Angle_qingxie<20) && (fall_down_flag==1) )//�ָ�ֱ��ʱ
	{
		fall_down_flag=0;
		upload(2,2);//����λ��������
  }
	Delay(600000);//100ms

}

//�����˿�����̧ͷһ���Ƕ�
int face_upward(int face_upward_Angle)
{
	static uchar face_upward_flag=0;
	
	if(face_upward_flag==0)
	{
		 STEP2_Angle(face_upward_Angle);
		 face_upward_flag=1;
	}
	if((g_iStep2PresentPulseCount>=g_iStep2TargetPulseNum) && face_upward_flag==1)//��̧ͷ�ĽǶȴﵽһ���Ƕ�ʱ ���¶�λΪ0��
  {
		face_upward_flag=2;
		g_iStep2PresentPulseCount=0;
		g_iStep2TargetPulseNum=	0;
    return 1;		
  }
	return 0;

}



//���ܼ���
static uchar Other_features_flag=0;
void Other_features(void)
{
	
	if( Other_features_flag==1)
	{		
		TchResp(); //����		
		//EvadeBarrier(); //����
		fall_down(); //�жϻ������Ƿ�ˤ��
	}	
	else if( Other_features_flag==0 )//������̧ͷһ���Ƕȣ�̧��ͷ���ܽ�����һ�����ܵ�ʵ��
	{
		face_upward(25);
		Other_features_flag=1;
  }	
	
}

uchar Debug_printf_flage=0;
//ϵͳ���ϼ�Ⲣ��ӡ��Ϣ
void Trouble_printf(void)
{
	
#ifdef Debug_printf //�رջ��ߴ�ϵͳ���е�����Ϣ	
	
	switch(Debug_printf_flage)
	{
		 case 1:
			     put_string((u8*)"DCReset_Count---DC---duzhuan \r\n",USART1);break;//�����ת�����ں���DCStallSolved
		 case 2: 
			     put_string((u8*)"DCshutdown_Count---DC---bianmaqi_broken \r\n",USART1);break;//������������߲������߶Ͽ������ں���DCStallSolved
		 //default:
  }
	
#endif
}






