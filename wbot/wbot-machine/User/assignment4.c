/**
  ******************************************************************************
  * @file    assignment4.c
  * @author  liu.c
  * @version V1.0
  * @date    2015��9��11��10:42:01
  * @brief   ƽ��������ơ����������ơ��ٶȵ���������ơ���������ٶȹ滮
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:STM32BOARDV1.0
    * ��ַ��www.aiiage.com
  *
  ******************************************************************************
  */
    
#include "assignment4.h" 
float g_fBlancePwm=0 ;
float g_fBlance_P = 0.;
float g_fBlance_D = 0.;
//float g_fBlance_P = 50.;
//float g_fBlance_D = 2;
//float g_fBlance_P = 180;//175.;
//float g_fBlance_D = 0.1;//3.16;
void BalanceControlOut(void)
{
    //static int a=0;
    static float P=0;
    //a++;
    //P=a>>5;
    //if(P > g_fBlance_P || a > 65535 )
    //{
    //  P = g_fBlance_P;
    //  a = 65535;
    //}
    if(P<g_fBlance_P)
        P++;
    if(P>g_fBlance_P)
        P=g_fBlance_P;
    g_fBlancePwm = (Angle)*P + Gyro_y*g_fBlance_D ; 
    //g_liTemp[1]=g_fBlancePwm;
}

uchar Encoder1Flag=FORWARD;
uchar Encoder2Flag=FORWARD;
uchar MotorDeadValue=5;
uchar g_ucHugFlag=0;
int pwmleft_temp=0 ;
int pwmright_temp=0 ;

//���pwm���Ƶ����ת
void  MotorControl(void)
{  
    int pwmleft=0 , pwmright=0 ;
    static int pwm_limit=300;//��������
    
#ifdef  BALANCE
    pwmleft  = g_fBlancePwm - g_fSpeedPwm + g_fDirectionPwm ;
    pwmright = g_fBlancePwm - g_fSpeedPwm - g_fDirectionPwm ;
#else
    pwmleft  = g_fSpeedPwmL;// + g_fDirectionPwm ;
    pwmright = g_fSpeedPwmR;// - g_fDirectionPwm ;        
#endif
    
    if(pwmleft >= 900-MotorDeadValue)
        pwmleft = 900-MotorDeadValue;
    else if(pwmleft <- 900+MotorDeadValue)
        pwmleft =- 900+MotorDeadValue;
    
    if(pwmright>=900-MotorDeadValue)
        pwmright=900-MotorDeadValue;
    else if(pwmright<-900+MotorDeadValue)
        pwmright=-900+MotorDeadValue;
    
    
    //�ٴ����� ��ֹͻȻ������
    if(pwmleft > pwm_limit)
        pwmleft=pwm_limit;
    else if(pwmleft < -pwm_limit)
        pwmleft=-pwm_limit;
    
    if(pwmright > pwm_limit)
        pwmright=pwm_limit;
    else if(pwmright < -pwm_limit)
        pwmright=-pwm_limit;
    
    /*pwmleft_temp=pwmleft;//������
    pwmright_temp=pwmright;	*/
    
    if(pwmleft>=0)
    {
        DCMOTOR1_DERCTION(FORWARD);
        MOTOR1_PULSE = pwmleft + MotorDeadValue ;
        Encoder1Flag = FORWARD;
    }
    else
    {
        DCMOTOR1_DERCTION(BACK);
        MOTOR1_PULSE = - pwmleft + MotorDeadValue;
        Encoder1Flag = BACK;
    }
    
    if(pwmright>=0)
    {
        DCMOTOR2_DERCTION(FORWARD);
        MOTOR2_PULSE = pwmright+ MotorDeadValue;
        Encoder2Flag = FORWARD;
    }
    else
    {
        DCMOTOR2_DERCTION(BACK);
        MOTOR2_PULSE = - pwmright+ MotorDeadValue;
        Encoder2Flag = BACK;
    }       
}
#ifdef BALANCE
    float g_fCarSpeedSet=0.;
    float g_fCarSpeed=0;
    float g_fSpeed_P=0.;
    float g_fSpeed_I=0;
    float g_fSpeed_D=0;
    float g_fCarSpeedSetOld = 0.;
    float g_fSpeedIntegral=0;
    int   g_iSpeedControlValueOld=0;
    int   g_iSpeedControlValueNew=0;
    long int g_lDrift=0 ; //С����λ�Ƽ�¼
#else
    float g_fCarSpeedSet=0.;
    
    float g_fCarSpeedL=0;
    float g_fSpeed_PL=0.;
    float g_fSpeed_IL=0;
    float g_fSpeed_DL=0;
    float g_fCarSpeedSetL = 0;
    float g_fCarSpeedSetOldL = 0.;
	float g_fCarSpeedSetLTemp = 0.;
    float g_fSpeedIntegralL=0;
    int   g_iSpeedControlValueOldL=0;
    int   g_iSpeedControlValueNewL=0;
    long int g_lDriftL=0 ; //С����λ��
    /////////////////////////////////////////////////////////////
    
    float g_fCarSpeedR=0;
    float g_fSpeed_PR=0.;
    float g_fSpeed_IR=0;
    float g_fSpeed_DR=0;
    float g_fCarSpeedSetR = 0;
    float g_fCarSpeedSetOldR = 0.;
	float g_fCarSpeedSetRTemp = 0.;
    float g_fSpeedIntegralR =0;
    int   g_iSpeedControlValueOldR =0;
    int   g_iSpeedControlValueNewR =0;
    long int g_lDriftR=0 ; //С����λ��
#endif

//�����ٶȵ����Ŀ�����,100ms����һ��
void SpeedControl(void) 
{
#ifdef BALANCE

    float fp=0;
    float fi=0;
    int delt=0; 
    
    if(g_fCarSpeedSet>300)
        g_fCarSpeedSet=300;
    else if(g_fCarSpeedSet<-300)
        g_fCarSpeedSet=-300;
    
    g_fCarSpeed = (g_iEncoderSigmaL + g_iEncoderSigmaR)/2;
    g_iEncoderSigmaL = g_iEncoderSigmaR=0 ;
    
    delt = ( g_fCarSpeedSet - g_fCarSpeed ) ;
    
    fp = delt * g_fSpeed_P ;
    fi = delt * g_fSpeed_I ;
    
    g_fSpeedIntegral += fi ;
    
    if(g_fSpeedIntegral>5000)
        g_fSpeedIntegral=5000;
    if(g_fSpeedIntegral<-5000)
        g_fSpeedIntegral=-5000;  
    
    g_iSpeedControlValueOld = g_iSpeedControlValueNew;
    g_iSpeedControlValueNew = fp + g_fSpeedIntegral ;

#else
     
     float fpL=0,fiL=0,fdL=0,deltL=0;
     static float deltOldL=0;
     
     float fpR = 0,fiR = 0,fdR = 0,deltR = 0;
     static float deltOldR = 0;
	 //put_number(g_fCarSpeedSetR,USART1);
//////////////////////// right motor ///////////////////////////////////////          
     g_fCarSpeedR = g_iEncoderSigmaR;
     g_iEncoderSigmaR = 0;
     //g_fCarSpeedSetR = g_fCarSpeedSet - g_fDirectionPwm ;//�趨��Ŀ�공��
     deltR = ( g_fCarSpeedSetR - g_fCarSpeedR ) ;//Ŀ�공�ټ�ȥ���������ص��ٶ�
    
     fpR = deltR * g_fSpeed_PR ;//PԽ���������g_iSpeedControlValueNewRԽ����Խ��ﵽԤ��ֵ����̫���˾ͻ���ָ����趨�ٶȾͶ�һ�µĸо�����Ӧ�ܿ�ɣ�
     fiR = deltR * g_fSpeed_IR ;
     fdR = (deltR-deltOldR)*g_fSpeed_DR;
     g_fSpeedIntegralR += fiR ;          
     g_iSpeedControlValueOldR = g_iSpeedControlValueNewR ;
     g_iSpeedControlValueNewR = fpR + g_fSpeedIntegralR +fdR ;
     g_fSpeedPwmR = g_iSpeedControlValueNewR;
     
//////////////////////// left motor ///////////////////////////////////////                 
     g_fCarSpeedL = g_iEncoderSigmaL;
     g_iEncoderSigmaL = 0;
     //g_fCarSpeedSetL = g_fCarSpeedSet + g_fDirectionPwm ;//�趨��Ŀ�공��
     deltL = ( g_fCarSpeedSetL - g_fCarSpeedL ) ;
    
     fpL = deltL * g_fSpeed_PL ;
     fiL = deltL * g_fSpeed_IL ;
     fdL = (deltL-deltOldL)*g_fSpeed_DL;
     g_fSpeedIntegralL += fiL ;
     g_iSpeedControlValueOldL = g_iSpeedControlValueNewL ;
     g_iSpeedControlValueNewL = fpL + g_fSpeedIntegralL +fdL ;
     g_fSpeedPwmL = g_iSpeedControlValueNewL;
     
#endif
}

float g_fSpeedPwmL=0;
float g_fSpeedPwmR=0;
float g_fSpeedPwm = 0;
float g_fCarSpeedSetOld = 0;
float SpeedDelt = 0;
float g_fCarSpeedSet_temp = 0;
float g_fCarSpeedSet_Last = 0;
uchar PlanFlag = 0;
uchar PlanFlag2 = 0;
uchar PlanFlag4 = 0;
uchar PlanFlag_count=0;

void SpeedControlOut(void) 
{
#ifdef BALANCE

    //��20�����øú�����5msһ�ε����ٶȿ�����
    static float delt=0;
    static uchar count=0;
    count++;
    delt = g_iSpeedControlValueNew - g_iSpeedControlValueOld ;
    g_fSpeedPwm = delt*count/20. + g_iSpeedControlValueOld ;
    if(count == 20)
        count=0;    
#else    
    if(g_fCarSpeedSet_Last != g_fCarSpeedSet_temp)
    {
        PlanFlag4 = 1;
        PlanFlag2 = 1;
        PlanFlag_count = 0;
        PlanFlag = 0;   
        g_fCarSpeedSetOld = g_fCarSpeedSet;
    }
        
    if(PlanFlag2 == 1)  
    {
     
        PlanFlag_count++;
        if(PlanFlag == 0)
        {
            SpeedDelt = g_fCarSpeedSet_temp - g_fCarSpeedSetOld;
            PlanFlag = 1;
        }
        
        //g_fCarSpeedSet = (int)(SpeedDelt*PlanFlag_count/100) + g_fCarSpeedSetOld;
        g_fCarSpeedSet = (SpeedDelt*PlanFlag_count/100) + g_fCarSpeedSetOld;
        
        if(PlanFlag4 == 1)
        {
            g_fCarSpeedSet_Last = g_fCarSpeedSet_temp;
            PlanFlag4 = 0;  
        }
        
        if(PlanFlag_count == 100)
        {
            PlanFlag_count = 0;
            PlanFlag2 = 0;
            g_fCarSpeedSetOld = g_fCarSpeedSet;
            PlanFlag = 0;    
        }    
    }
#endif
}
/*
void MotorSpeedControl(void)
{
	if(g_fCarSpeedSetOldL != g_fCarSpeedSetL)
	{
		 g_fCarSpeedSetL = g_fCarSpeedSetOldL + ;

		 g_fCarSpeedSetOldL =  g_fCarSpeedSetL
	} 	


}
*/
int Spanangle_old;
float g_fDirectionPwm=0.;
float g_fDirectionPwm_old=0.;
float g_fDirectionValueNew =0.;
float g_fDirectionValueOld =0.;
float g_fDir_P=0.;
float g_fDir_D=0.;
float g_fDirAngle=0.;

void DirControl(void)
{
//      g_fDirectionValueOld = g_fDirectionValueNew ; 
//      g_fDirectionValueNew = g_fDir_P*g_fDirAngle+g_fDir_D*Gyro_x;    
}

void DirControlOut(void)
{               
    float delt = 0;
    static uchar count1=0 ; 
    count1++;
    delt = g_fDirectionValueNew - g_fDirectionValueOld; 
    g_fDirectionPwm = delt*count1/10. + g_fDirectionValueOld;
    if(count1 == 10)
        count1=0;
}


int g_iStep1PlanThres=500;// ����ʱ���仯������������������ֵ�������ٶȹ滮������������ٶ�����
uint g_uiStep1LowSpeed=15000;//�趨Ϊ�����������������ٶ�
uint g_uiStep1HighSpeed=800;
// uint g_cuiStep1Psc[1501]=;
// uint g_cuiStep1Arr[1501]=2; 

//�������1���ٶȹ滮
void Step1SpeedPlan(int delt)
{
    static uchar StartFlag=0;
    static uint  DELT=0;    //ԭʼת���Ƕȶ�Ӧ������
    if(g_ucStep1TurnFlag)//��һ�ε��ù滮����
    {
        if(delt < (g_iStep1PlanThres <<1) )  
        {
            TIM6->ARR = g_uiStep1LowSpeed-1;
            g_ucStep1TurnFlag=0;
        }
        else
        {
            g_ucStep1TurnFlag=0;
            StartFlag=1;
            DELT=delt;
        }
    }
    
    if(StartFlag==1)
    {
        if( delt <= g_iStep1PlanThres )
        {    
             
            TIM6-> ARR = g_uiStep1HighSpeed + ((g_iStep1PlanThres-delt)<<1 );
        }
        else if (g_iStep1PlanThres < delt && delt < DELT - 200 )
            TIM6->ARR = g_uiStep1HighSpeed-1;
        else if( delt > DELT - 200)
            TIM6->ARR = 1000 - 3*(DELT-delt) ;
    }

    if(delt==0)
    {
        StartFlag=0;
    }           
                 
}

int g_iStep2PlanThres=500;// Ĭ��500 ����ʱ���仯������������������ֵ�������ٶȹ滮������������ٶ�����
uint g_uiStep2LowSpeed=15000;//Ĭ��15000 �趨Ϊ�����������������ٶ�
uint g_uiStep2HighSpeed=500;//Ĭ��800
void Step2SpeedPlan(int delt)
{//�������1���ٶȹ滮
    static uchar StartFlag=0;
    static uint  DELT=0;    //ԭʼת���Ƕȶ�Ӧ������
    if(g_ucStep2TurnFlag)//��һ�ε��ù滮����
    {
        if(delt < (g_iStep2PlanThres <<1 )) ///�������ٶȹ滮��ֱ������������� 
        {
            TIM6->ARR = g_uiStep2LowSpeed-1;
            g_ucStep2TurnFlag=0;
        }
        else
        {
            g_ucStep2TurnFlag=0;
            StartFlag=1;
            DELT=delt;
        }
    }
    
    if(StartFlag==1)
    {
        if( delt <= g_iStep2PlanThres )
        {    
            TIM6-> ARR = g_uiStep2HighSpeed + ((g_iStep2PlanThres-delt)<<1 );
        }
        else if (g_iStep2PlanThres < delt && delt < DELT -  200 )
            TIM6->ARR = g_uiStep2HighSpeed-1;
        else if( delt > DELT - 200)
            TIM6->ARR = 4000 - 2*(DELT-delt) ;
    }
    if(delt==0)
    {
        StartFlag=0;
    }                           
}

int Step1PresAngle(void)
{
    return g_iStep1PresentPulseCount/92.088;
}

int Step2PresAngle(void)
{
    return g_iStep2PresentPulseCount/92.088;
}
//������ѡ��
void PIDParaSet(uchar i)
{
#ifdef BALANCE
    g_fBlance_P = 160. ; g_fBlance_D = 4; g_fSpeed_P=10;
    g_fSpeed_I = 0.2;    g_fDir_P=0.; g_fDir_D=0.;

#else  	
	#if 0
		g_fSpeed_PL=5;  g_fSpeed_IL=0.8; g_fSpeed_DL = 0.55 ; 
	  	g_fSpeed_PR=5;  g_fSpeed_IR=0.8; g_fSpeed_DR = 0.55 ; 
	  	g_fDir_P = 0. ;  g_fDir_D = 0.; g_fCarSpeedSet_temp =0.;
	
	#elif 0
	    g_fSpeed_PL=15;  g_fSpeed_IL=8; g_fSpeed_DL = 0.5 ;     
	    g_fSpeed_PR=15;  g_fSpeed_IR=8; g_fSpeed_DR = 0.5; 
	    g_fDir_P = 0. ;  g_fDir_D = 0.; g_fCarSpeedSet_temp =0.;

	#elif 1
		g_fSpeed_PL=5;  g_fSpeed_IL=2; g_fSpeed_DL = 0.5 ;     
	    g_fSpeed_PR=5;  g_fSpeed_IR=2; g_fSpeed_DR = 0.5; 
	    g_fDir_P = 0.;  g_fDir_D = 0.; g_fCarSpeedSet_temp =0.;
	
	#endif
#endif
}


#if 1 

#define wheel_width 0.098		//����ֱ�� (m)
#define ticks_per_circle 486	//ÿȦ��������ֵ		= 18(������) * 27(���ٱ�)
#define ticks_meter 1720.397 	//ÿ�ױ�������ֵ 		= ticks_per_circle / (PI * wheel_width)
#define base_width 0.476f		//��� (m)
#define robot_timer 0.022f		//�������� 20 ms
#define VALUE_MARRE 10			//���������˲�����

struct Encoder_{
	float d_left;	//һ�������� �������߾���  	(m) 	= (left - enc_left) / ticks_mester
	float d_right;	//һ�������� �������߾���  	(m) 	= (right - enc_right) / ticks_mester
	float dx;		//С��x�᷽�����ٶ� 		(m/s)  	= ( (d_left + d_right) / 2 ) / robot_timer
	float dr;		//С������ת���ٶ�  		(rad/s)	= ( (d_right - d_left) / base_width ) / robot_timer
	int enc_left;	//��һ���� ���ֱ�����ֵ
	int enc_right;	//��һ���� ���ֱ�����ֵ
	int left;		//���ֱ�����ֵ
	int right;		//���ֱ�����ֵ
}self;

union Max_Value
{
	unsigned char buf[8];
	struct _Float_{
		//unsigned char startBuf;
	 	float _float_vx;   //x�� ǰ�����ٶ�
		float _float_vth;  //z�� ��ת���ٶ�
		//unsigned char endBuf;
	}Float_RAM;
}Send_Data;

//���ֽ�����λ����������
void Float_Char_Send_Serial()
{
 	u8 i;
	put_char(0xff, USART1); //���Ϳ�ʼλ
	for(i=0;i<8;i++){
	 	put_char(Send_Data.buf[i],USART1); //��������λ
	}
	put_char(0xfe, USART1); //���ͽ���λ
}

float Marrer_Data_L[VALUE_MARRE+1];		//���ֻ�������
float Marrer_Data_R[VALUE_MARRE+1];		//���ֻ�������
float SUM_Left_Value;					//���ֻ��������ۼ�ֵ
float SUM_Right_Value;					//���ֻ��������ۼ�ֵ
u8 i = 0;								//����������

void Updata_Encoder_Data()
{
	
	self.right = g_iEncoderSigmaR;	//��ȡ��ǰ���ֱ�������ֵ	
	self.left = g_iEncoderSigmaL;	//��ȡ��ǰ���ֱ�������ֵ

	self.d_left = self.left / ticks_meter;
	self.d_right = self.right / ticks_meter;

#if 0
	/****************************���û��������˲�*************************************/
	Marrer_Data_L[VALUE_MARRE] = self.d_left;
	Marrer_Data_R[VALUE_MARRE] = self.d_right;
	
	for(i=0; i<VALUE_MARRE; i++){
		 Marrer_Data_L[i] = Marrer_Data_L[i+1];
		 Marrer_Data_R[i] = Marrer_Data_R[i+1];

		 SUM_Left_Value += Marrer_Data_L[i];
		 SUM_Right_Value += Marrer_Data_R[i];
	}
	self.d_left = SUM_Left_Value / VALUE_MARRE;
	self.d_right = SUM_Right_Value / VALUE_MARRE;
	SUM_Left_Value = 0;
	SUM_Right_Value = 0;
	/**********************************************************************************/
#endif

	self.dx = ((self.d_left + self.d_right) / 
	2) / robot_timer;				//���� x ��ǰ���ٶ�	(m/s)
	self.dr = ((self.d_right - self.d_left) / base_width) / robot_timer;	//���� z ����ת�ٶ�	(rad/s)	

	//Send_Data.Float_RAM.startBuf = 0xff;
	Send_Data.Float_RAM._float_vx = self.dx;
	Send_Data.Float_RAM._float_vth = self.dr;	
	//Send_Data.Float_RAM.endBuf = 0xfe;

	Float_Char_Send_Serial();	  //�������ݵ���λ��
}

#endif


