/**
    ******************************************************************************
    * @file    assignment2.c
    * @author  liu.c
    * @version V1.0
    * @date    2015��8��10��10:05:39
    * @brief   led���á���������
    ******************************************************************************
    * @attention
    *
    * ʵ��ƽ̨:STM32BOARDV1.0
    * ��ַ��www.aiiage.com
    *
    ******************************************************************************
    */
    
#include "assignment2.h"  
 /**
    * @brief  ��ʼ������LED��IO
    * @param  ��
    * @retval ��
    */
void LED_GPIO_Config(void)
{       
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����LED������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE ); 
    
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
            
    /*ѡ��Ҫ���Ƶ�����*/                                                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    
    GPIO_Init(GPIOC, &GPIO_InitStructure);
        
    /*ѡ��Ҫ���Ƶ�����*/    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
            
    /*�ر�LED��*/
    GPIO_SetBits(GPIOC, GPIO_Pin_12|GPIO_Pin_13);    
        
/////////////////////////////�����ô���/////////////////////////////////////////////////////        
//      /*����LED������ʱ��*/
//      RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 

//      /*ѡ��Ҫ���Ƶ�GPIOB����*/                                                              
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  

//      /*��������ģʽΪͨ���������*/
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

//      /*������������Ϊ50MHz */   
//      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//      
//      /*���ÿ⺯������ʼ��GPIOB0*/
//      GPIO_Init(GPIOA, &GPIO_InitStructure);  
//      
//              /*ѡ��Ҫ���Ƶ�GPIOB����*/                                                              
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   

//      /*��������ģʽΪͨ���������*/
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

//      /*������������Ϊ50MHz */   
//      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

//      /*���ÿ⺯������ʼ��GPIOA9\10*/
//      GPIO_Init(GPIOA, &GPIO_InitStructure);
//      
//      GPIO_SetBits(GPIOA, GPIO_Pin_9|GPIO_Pin_10);    
//      GPIO_ResetBits(GPIOA, GPIO_Pin_9|GPIO_Pin_10);  
}

 /**
    * @brief  ��ʼ��KEY1��KEY2
    * @param  ��
    * @retval ��
    */
void KEY_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*���������˿ڣ�PD����ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            
        /*ѡ��Ҫ���Ƶ�����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   
    
    GPIO_Init(GPIOD, &GPIO_InitStructure);          
        /*ѡ��Ҫ���Ƶ�����*/    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;   
    
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}   

/**
    * @brief   ����Ƿ��а�������
    * @param   ����Ķ˿ںͶ˿�λ
    *     @arg GPIOx: x�����ǣ�A...G�� 
    *     @arg GPIO_PIN ������GPIO_PIN_x��x������1...16��
    *   @arg Down_state:��������ʱ�ĵ�ƽ��1Ϊ�ߵ�ƽ��0Ϊ�͵�ƽ
    * @retval  ������״̬
    *     @arg KEY_ON:��������
    *     @arg KEY_OFF:����û����
    */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state)
{           
    /*����Ƿ��а������� */
    if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state ) 
    {      
        /*��ʱ����*/
        Delay(10000);       
        if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state )  
        {    
            /*�ȴ������ͷ� */
            while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == Down_state);   
            return  KEY_ON;  
        }
        else
            return KEY_OFF;
    }
    else
        return KEY_OFF;
}

/*-----------------------------------------------------------------*/
uchar Key1Count=0;
void Key_Test(void)
{
        /*config key*/
//      LED1( ON ); 
    if( Key_Scan(GPIOD,GPIO_Pin_0,0) == KEY_ON ) // KEY1
    {
        /*LED1��ת*/
        LED2_TOGGLE;
        Key1Count++;
        if(Key1Count%2==0)//Ϊż����ʱ��˯��
            DCMOTOR_NSLEEP(OFF);
        else
            DCMOTOR_NSLEEP(ON);
    } 
    else if( Key_Scan(GPIOD,GPIO_Pin_1,0) == KEY_ON ) // KEY1
    {
        /*LED1��ת*/
        LED2_TOGGLE;
    } 

}

/**
    * @brief  ����TIM3�������PWMʱ�õ���I/O
    * @param  ��
    * @retval ��
    */
static void TIM3_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ����TIM3CLK Ϊ 72MHZ */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

    /* GPIOA and GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

    /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*GPIOB Configuration: TIM3 channel 3 and 4 as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
    * @brief  ����TIM3�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
    * @param  ��
    * @retval ��
    */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT ���¼���
 *                    TIMx_CCR(��ƽ�����仯)
 * �ź�����=(TIMx_ARR +1 ) * ʱ������
 * ռ�ձ�=TIMx_CCR/(TIMx_ARR +1)
 */

    /* PWM�źŵ�ƽ����ֵ */
//  u16 CCR1_Val = 500;        
    u16 CCR2_Val = 0;
    u16 CCR3_Val = 0;
//  u16 CCR4_Val = 125;
static void TIM3_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;


/* ----------------------------------------------------------------------- 
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR+1)* 100% = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR+1)* 100% = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR+1)* 100% = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR+1)* 100% = 12.5%
    ----------------------------------------------------------------------- */

    /* Time base configuration */      
    TIM_TimeBaseStructure.TIM_Period = 999;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Prescaler =3 ;      //����Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;  //����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

//  /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //����ΪPWMģʽ1 
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   
//  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;      //��������ֵ�������������������ֵʱ����ƽ��������
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
//  TIM_OC1Init(TIM3, &TIM_OCInitStructure);     //ʹ��ͨ��1
//  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    /* PWM1 Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR2_Val;   //����ͨ��2�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);    //ʹ��ͨ��2
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val; //����ͨ��3�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);   //ʹ��ͨ��3
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

//  /* PWM1 Mode configuration: Channel4 */
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;   //����ͨ��4�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM
//  TIM_OC4Init(TIM3, &TIM_OCInitStructure);    //ʹ��ͨ��4
//  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
//  TIM_ARRPreloadConfig(TIM3, ENABLE);          // ʹ��TIM3���ؼĴ���ARR

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);                   //ʹ�ܶ�ʱ��3    
}

/**
    * @brief  TIM3 ���PWM�źų�ʼ����ֻҪ�����������
    *         TIM3���ĸ�ͨ���ͻ���PWM�ź����
    * @param  ��
    * @retval ��
    */
void TIM3_PWM_Init(void)
{
    TIM3_GPIO_Config();
    TIM3_Mode_Config(); 
}
/**
    * @brief  drv8829���ƽų�ʼ��
    * @param  ��
    * @retval ��
    */
void DCMOTORCON_GPIO_Config(void)
{       
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*��������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE ); 
    
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
            
    /*ѡ��Ҫ���Ƶ�����*/                                                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    /*ѡ��Ҫ���Ƶ�����*/    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    
    GPIO_Init( GPIOC, &GPIO_InitStructure); 
            
    /*��ʼ���״̬*/
    DCMOTOR1_DERCTION(FORWARD);
    DCMOTOR2_DERCTION(FORWARD);
    DCMOTOR_NSLEEP(OFF);
}

/**
    * @brief  drv8829���ƽų�ʼ��
    * @param  ��
    * @retval ��
    */
void STEPMOTORCON_GPIO_Config(void)
{       
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����LED������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE ); 
    
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//          
//      /*STEP1Ҫ���õ�����*/                                                              
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
//  
//      GPIO_Init(GPIOE, &GPIO_InitStructure);
//  
    /*STEP1��2��3Ҫ���õ�����*/                                                            
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_11 ;
    
    GPIO_Init(GPIOE, &GPIO_InitStructure);

            
    /*��ʼ���״̬*/
    STEPMOTOR1_DIRCTION(LEFT);
    STEPMOTOR1_NSLEEP(OFF);
        
    STEPMOTOR2_DIRCTION(LEFT);
    STEPMOTOR2_NSLEEP(OFF);
        
    STEPMOTOR3_DIRCTION(LEFT);
    STEPMOTOR3_NSLEEP(OFF);

}

/**
    * @brief  �������Ʋ�������Ŀɵ��Ķ�ʱ��
    * @param  ��
    * @retval ��
    */
void TIM6_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
//  TIM6->CR1&=0xFE; //�رն�ʱ��6����Ҫ����ʱ�ٴ򿪣�PWM��Ƶ��=72M/((ARR+1)*(PSC+1))

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 
    TIM6->PSC = 72 - 1; // ��Ƶ���ʱ��Ϊ 1000KHz
    TIM6->ARR = 200-1; // 10000 Hz  
    TIM6->CR1 = 0x0004 +0x0001; // URS = 1 //CEN = 1
    TIM6->DIER = 0x0001; // ʹ�ܸ����ж�
}
/**
    * @brief  �����ٶȵ��Ժ���
    * @param  ��
    * @retval ��
    */
void STEP1_SPEED(unsigned long speed)
{
    if(0<speed<500000)
        TIM6->ARR = speed; //
        //TIM6->ARR = 500000/speed-1; //
}


/**
    * @brief  �����Ƕȵ��Ժ���
    * @param  ��
    * @retval ��
    */
int g_iStep1TargetPulseNum=0; //Ŀ��Ƕȶ�Ӧ��������
int g_iStep1PresentPulseCount=0; //��ǰ�Ƕȶ�Ӧ��������
uchar g_ucStep1TurnFlag=0;
void STEP1_Angle(float angle)
{
//      TIM6->ARR =  150-1; //����Ƶ���趨
    g_ucStep1TurnFlag = 1;
    if(angle>=0)
    {
        if(angle >= 45)
            angle = 45;
        g_iStep1TargetPulseNum=angle*92.088;//92.088=1.8*32*5.18     1��  ������������,32������оƬ��MO��M1���Ž�3.3V�趨��
        //g_iStep1TargetPulseNum=angle*1105.92;//1105.92=1.8*32*19.2    1��  ������������,32������оƬ��MO��M1���Ž�3.3V�趨��
        //STEPMOTOR1_DIRCTION(RIGHT);
    }
    else
    {
        if(angle <= -45)
            angle = -45;
        g_iStep1TargetPulseNum=angle*92.088;//  /1.8*32*5.18;
//              STEPMOTOR1_DIRCTION(LEFT);
    }         
//      TIM6->CR1|=0x01; //�򿪶�ʱ��6����ʼ����
    STEPMOTOR1_NSLEEP(OFF); // ���Ѳ������
}

///**
//  * @brief  �����Ƕȵ��Ժ���
//  * @param  ��
//  * @retval ��
//  */
int g_iStep2TargetPulseNum=0; //Ŀ��Ƕȶ�Ӧ��������
int g_iStep2PresentPulseCount=0; //��ǰ�Ƕȶ�Ӧ��������
uchar g_ucStep2TurnFlag=0;
void STEP2_Angle(float angle)
{
//      TIM6->ARR = 60-1;
    g_ucStep2TurnFlag=1;
        
    if(angle>=0)
    {
        if(angle>=45)
            angle = 45;
        g_iStep2TargetPulseNum = angle*92.088;// /1.8*32*17;
        //STEPMOTOR2_DIRCTION(RIGHT);
    }
    else
    {       
        if(angle<=-20)
            angle = -20;
        g_iStep2TargetPulseNum = angle*92.088; ///1.8*32*17;
        //STEPMOTOR2_DIRCTION(LEFT);
    }
//      TIM6->CR1|=0x01; //�򿪶�ʱ��6����ʼ����
//      STEPMOTOR2_NSLEEP(OFF); // ���Ѳ������
}

//void TIM6_IRQHandler(void)
//{
//  LED2(ON);
//  TIM6->SR=0x0000;//���жϱ�־
//} 

static void UITRASONIC_GPIO_Config(void)
{       
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*��������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO , ENABLE ); 

    
    //����TIM4����ӳ�书�ܣ�ӳ�䵽PD12-PD15�����������ֲ�p119
    GPIO_PinRemapConfig(GPIO_Remap_TIM4 , ENABLE);
    
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
        
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

            /*TRIG Ҫ���õ�����*/                                                              
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
        
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//�������룬 ����Ϊ��������
        
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    /*�������� Ҫ���õ�����*/                                                              
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

    GPIO_Init(GPIOD, &GPIO_InitStructure);

    
}
/// ����TIM4�ж�
void NVIC_Configuration_TIM4(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
    * @brief �����������Ȳ����жϳ�ʼ��
    * @param  ��
    * @retval ��
    */
static void TIM4_Mode_Configuration(void)
{
    
    
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; // TIM4 ʱ��
    TIM_DeInit(TIM4); //�ͷ�TIM4
    
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4 , ENABLE );
    
    TIM_TimeBaseStructure.TIM_Period = 0xffff;                  //�Զ�Ԥװ��ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;                 //Ԥ��Ƶֵ TIMx_CLK=1MHz     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�Ӳ���Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
                                                             
    //TIM4_TimeBase 
        
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //| TIM_Channel_2 ;       //����ͨ��
    TIM_ICInitStructure.TIM_ICPolarity =    TIM_ICPolarity_Rising;     //bu׽����
    TIM_ICInitStructure.TIM_ICSelection =   TIM_ICSelection_DirectTI;    //
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //��׽����Ƶ
    TIM_ICInitStructure.TIM_ICFilter = 0x0;          //��׽���벻�˲�
    TIM_ICInit(TIM4, &TIM_ICInitStructure); 
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;       //����ͨ��
    TIM_ICInitStructure.TIM_ICPolarity =    TIM_ICPolarity_Rising;     //bu׽����
    TIM_ICInitStructure.TIM_ICSelection =   TIM_ICSelection_DirectTI;    //
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //��׽����Ƶ
    TIM_ICInitStructure.TIM_ICFilter = 0x0;          //��׽���벻�˲�
    TIM_ICInit(TIM4, &TIM_ICInitStructure); 
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;       //����ͨ��
    TIM_ICInitStructure.TIM_ICPolarity =    TIM_ICPolarity_Rising;     //bu׽����
    TIM_ICInitStructure.TIM_ICSelection =   TIM_ICSelection_DirectTI;    //
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //��׽����Ƶ
    TIM_ICInitStructure.TIM_ICFilter = 0x0;          //��׽���벻�˲�
    TIM_ICInit(TIM4, &TIM_ICInitStructure); 
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;       //����ͨ��
    TIM_ICInitStructure.TIM_ICPolarity =    TIM_ICPolarity_Rising;     //bu׽����
    TIM_ICInitStructure.TIM_ICSelection =   TIM_ICSelection_DirectTI;    //
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //��׽����Ƶ
    TIM_ICInitStructure.TIM_ICFilter = 0x0;          //��׽���벻�˲�
    TIM_ICInit(TIM4, &TIM_ICInitStructure); 
    
    
    
    /* Enable the CCx Interrupt Request */
    NVIC_Configuration_TIM4();
    
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
//      TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE); 
//      TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
//      TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
    
    TIM_Cmd(TIM4, ENABLE);
        
}

void UITRASONIC_Config(void)
{
    UITRASONIC_GPIO_Config();
    TIM4_Mode_Configuration();
}

uchar flag_srf=0;
float distance1=0; 
float distance2=0; 
float distance3=0; 
float distance4=0; 
void UITRA_Measure(void)
{
    uint i=0;
    //ÿ�β���ǰ��ʼ��һЩֵ
    flag_srf=START;  //

    TRIG1_HIGH;      //trig SRF05
    Delay(70);/////
    TRIG1_LOW;

    while((flag_srf!=FINISH)&&(i++<40))  //wait transfer over
        Delay(120);
    distance1 = vu16_Freq_Value[0]/5.8; //transfer to real distance cm
    flag_srf = START;  //
    

//       TRIG2_HIGH;      //trig SRF05
//       Delay(70);/////
//       TRIG2_LOW;
//       
//       while( (flag_srf!= FINISH)&&(i++<40))  //wait transfer over
//           Delay(120);
//       distance2 = vu16_Freq_Value[1]/58.; //transfer to real distance cm
//           flag_srf = START;  //
//           

//           TRIG3_HIGH;      //trig SRF05
//       Delay(70);/////
//       TRIG3_LOW;
//       
//       while( (flag_srf!= FINISH)&&(i++<40))  //wait transfer over
//           Delay(120);
//       distance3 = vu16_Freq_Value[2]/58.; //transfer to real distance cm
//           flag_srf = START;  //
//           

//           TRIG4_HIGH;      //trig SRF05
//       Delay(70);/////
//       TRIG4_LOW;
//       
//       while( (flag_srf!= FINISH)&&(i++<40))  //wait transfer over
//           Delay(120);
//       distance4 = vu16_Freq_Value[3]/58.; //transfer to real distance cm
//           flag_srf = START;  //
}


/**************************************************************************
�������ܣ���TIM2��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM2(void)
{
    RCC->APB1ENR|=1<<0;     //TIM2ʱ��ʹ��
    RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
    GPIOA->CRL&=0XFFFFFFF0;//PA0
    GPIOA->CRL|=0X00000004;//��������
    /* �Ѷ�ʱ����ʼ��Ϊ������ģʽ */ 
    TIM2->PSC = 0x0;//Ԥ��Ƶ��
    TIM2->ARR = 65535;
    TIM2->SMCR |= 7<<0;
    TIM2->SMCR |= 5<<4;
    TIM2->CCMR1 |= 1<<0;
    TIM2->CCER |= 0<<1;
    TIM2->CR1 |= 0x01;    //CEN=1��ʹ�ܶ�ʱ��   
}
/**************************************************************************
�������ܣ���TIM5��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM5(void)
{
    RCC->APB1ENR|=1<<3;     //TIM5ʱ��ʹ��
    RCC->APB2ENR|=1<<2;    //ʹ��PORTaʱ��
    GPIOA->CRL&=0XFFFFFF0F;//PA1
    GPIOA->CRL|=0X00000040;//��������
    /* �Ѷ�ʱ����ʼ��Ϊ������ģʽ */ 
    TIM5->PSC = 0x0;//Ԥ��Ƶ��
    TIM5->ARR = 65535;
    TIM5->CCMR1 |= 1<<8;
    TIM5->CCER |= 0<<5;
    TIM5->SMCR |= 7<<0;
    TIM5->SMCR |= 6<<4;
    TIM5->CR1 |= 0x01;    //CEN=1��ʹ�ܶ�ʱ��   
}
/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
**************************************************************************/
int g_iEncoderSigmaL=0;
int g_iEncoderSigmaR=0;
u16  EncoderValueL=0;
u16  EncoderValueR=0;
void Read_Encoder(void)
{
    EncoderValueL = TIM5 -> CNT;  
    TIM5 -> CNT=0;      
    EncoderValueR= TIM2 -> CNT;  
    TIM2 -> CNT= 0 ;

//    EncoderValueL = TIM2 -> CNT;  
//    TIM2 -> CNT=0;      
//    EncoderValueR= TIM5 -> CNT;  
//    TIM5 -> CNT= 0 ;

    if(Encoder1Flag==FORWARD)
        g_iEncoderSigmaL += EncoderValueL;
    else
        g_iEncoderSigmaL -= EncoderValueL;

    if(Encoder2Flag == FORWARD)
        g_iEncoderSigmaR += EncoderValueR;
    else
        g_iEncoderSigmaR -= EncoderValueR;
}


/**************************************************************************
�������ܣ�������Ҫͣ������ʱ��ʹ�ó��ӻ���ֹͣ
��ڲ�������
����  ֵ����
**************************************************************************/

u8 Speed_down_flage=0;
float Speed_down_intSpeed=0;
u8 Speed_down_intSpeed_flage=0;
unsigned long int Speed_down_time=0;

//����ٶ�ͣ������ʱ����ͣ��
void Speed_down(void)
{
    if( (g_fCarSpeedSet!=0) && (Speed_down_intSpeed_flage==0) )//��⵽�ٶȲ�Ϊ0ʱ
    {
        Speed_down_flage=1;
        Speed_down_intSpeed=g_fCarSpeedSet;//����ǰ���ٶ�ֵ��ֵ���������
    } 
    
    if( (g_fCarSpeedSet==0) && (Speed_down_flage==1) )//��⵽Ҫ��ֹͣ��ʱ��
    {
        Speed_down_flage=2;
        Speed_down_intSpeed_flage=1;
        Speed_down_time=SYSTEMTIME;
        
        //pid�л���ȥ
        g_fSpeed_PL=5;  g_fSpeed_IL=0.8; g_fSpeed_DL = 0.55 ;
        g_fSpeed_PR=5;  g_fSpeed_IR=0.8; g_fSpeed_DR = 0.55;
        
        //���ת��
        RotFlag = 0;
        g_iRotaAngle =0;
        g_fDirectionPwm = 0;
    } 
    
    if( Speed_down_flage==2 )//��Ҫֹͣ��ʱ��
    {
        if( (SYSTEMTIME-Speed_down_time) >30 )//g_fCarSpeedSet�ٶ�����������
        {
            Speed_down_time=SYSTEMTIME;
            
            if(Speed_down_intSpeed>0)//����Ϊ��
            {
                g_fCarSpeedSet=Speed_down_intSpeed--;
                if(Speed_down_intSpeed <=0 )
                {
                    g_fCarSpeedSet=0;
                    Speed_down_flage=0;
                    Speed_down_intSpeed_flage=0;
                    Speed_down_time=0;
                }
            }
            else if(Speed_down_intSpeed<0)//����Ϊ��
            {
                g_fCarSpeedSet=Speed_down_intSpeed++;
                if(Speed_down_intSpeed >=0 )
                {
                    g_fCarSpeedSet=0;
                    Speed_down_flage=0;
                    Speed_down_intSpeed_flage=0;
                    Speed_down_time=0;
                }
            }      
        }
    }
}








/*********************************************END OF FILE**********************/
