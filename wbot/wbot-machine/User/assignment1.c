/**
    ******************************************************************************
    * @file    
    * @author  liu.c
    * @version V1.0
    * @date    2015��8��12��14:53:39
    * @brief   
    ******************************************************************************
    * @attention
    *
    * ʵ��ƽ̨:STM32BOARDV1.0
    * ��ַ��www.aiiage.com
    *
    ******************************************************************************
    */
#include "assignment1.h"

void Kalman_Filter(float Accel,float Gyro); 

/*����ȫ�ֱ���*/
short int Accel_z;       //z����ٶ�ֵ�ݴ�
short int Accel_x;       //X����ٶ�ֵ�ݴ�
short int Accel_y;       //y����ٶ�ֵ�ݴ�

short int Gyro_y;        //Y�������������ݴ�
short int Gyro_x;        //X�������������ݴ�

float Angle_gy;      //�ɽ��ٶȼ������б�Ƕ�
float Angle_az;      //�ɼ��ٶȼ������б�Ƕ�
float Angle_ay;      //�ɼ��ٶȼ������б�Ƕ�
float Angle_qingxie;      //��������б��

float Angle;         //С��������б�Ƕ�

float x;
float y;
float z;

/***************************************************��1�����ɼ�mpu6050����******************************************************/

/*��ʼ��I2C1�������*/
static void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd  (RCC_APB2Periph_GPIOB, ENABLE );//ʹ���� I2C1 �йص�ʱ�� 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//PB6-I2C1_SCL��PB7-I2C1_SDA
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    GPIO_ResetBits(GPIOB,GPIO_Pin_6);
}

/*Ӳ������I2C1*/
static void I2C_Mode_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure;  
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ; //I2C ���� 
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; 
    I2C_InitStructure.I2C_OwnAddress1 = MPU_ADRESS; 
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;     
    I2C_Cmd(I2C1,ENABLE);//ʹ�� I2C1 
    I2C_Init(I2C1, &I2C_InitStructure);//I2C1 ��ʼ��    
    I2C_AcknowledgeConfig(I2C1, ENABLE);//����Ӧ��ģʽ  
}

/*
 * ��������I2C_MPU6050_ByteWrite
 * ����  ��дһ���ֽڵ�I2C MPU6050�Ĵ�����
 * ����  ��-pBuffer ������ָ��
 *         -WriteAddr �������ݵ�MPU6050�Ĵ����ĵ�ַ 
 * ���  ����
 * ����  ����
 * ����  ���ڲ�����
 */ 
static void I2C_MPU_ByteWrite(u8 pBuffer, u8 WriteAddr)
{
    while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));/*wait until I2C bus is not busy*///�ȴ�iic�����Ƿ�æµ
    I2C_GenerateSTART(I2C1, ENABLE);/* Send START condition *///���Ϳ�ʼ����
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/* Test on EV5 and clear it *///�¼� 
    I2C_Send7bitAddress(I2C1, MPU_ADRESS, I2C_Direction_Transmitter);/* Send MMA address for write *///�����豸��
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));/* Test on EV6 and clear it *///�¼�    
    I2C_SendData(I2C1, WriteAddr);/* Send the MMA's Register address to write to *///�����豸�Ĵ�����ַ 
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/* Test on EV8 and clear it *///�¼�   
    I2C_SendData(I2C1, pBuffer);/* Send the byte to be written *///����д���豸�Ĵ��������� 
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/* Test on EV8 and clear it *///�¼�    
    I2C_GenerateSTOP(I2C1, ENABLE);/* Send STOP condition *///ֹͣ�ź�
}


/*
 * ��������I2C_MMA_ByteRead
 * ����  ����MMA�Ĵ��������ȡһ�����ݡ� 
 * ����  �� -ReadAddr �������ݵ�MMA�Ĵ����ĵ�ַ��        
 * ���  ����
 * ����  ����ȡ���ļĴ�������
 * ����  ���ڲ�����
 */ 

void I2C_MPU_ByteRead(uchar* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)//��һ��������ָ�����������һ����ַ���1��
                                                                                     //�ڶ��������ǼĴ�����ַ��ֻ��Ҫ�����һ����ַ����                                         //�ڶ��ڶ�����ַ�������������Ƕ��ٸ�8λ��ָ����ۼ�
{   
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));//���æµ   
    I2C_GenerateSTART(I2C1, ENABLE);//��ʼ�ź�  
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//Ӧ��
    I2C_Send7bitAddress(I2C1, MPU_ADRESS, I2C_Direction_Transmitter);//�����豸��ַ
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//Ӧ�� 
    I2C_Cmd(I2C1, ENABLE);//ʹ��iic
    I2C_SendData(I2C1, ReadAddr);//�����豸�Ĵ�����ַ  
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//Ӧ��    
    I2C_GenerateSTART(I2C1, ENABLE);//��ʼ�ź�  
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//Ӧ��  
    I2C_Send7bitAddress(I2C1, MPU_ADRESS, I2C_Direction_Receiver);//�����豸��ַ��+1�����ź�  
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));//Ӧ��  
    while(NumByteToRead)  
    {
        if(NumByteToRead == 1)//����Ͷ�һ��һ��
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);//��Ӧ��      
            I2C_GenerateSTOP(I2C1, ENABLE);//ֹͣ�ź�
        }
        if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))//Ӧ��  
        {      
            *pBuffer = I2C_ReceiveData(I2C1);//�������ݴ���ָ�����
            pBuffer++;//ָ�������ַ+1       
            NumByteToRead--;//Ҫ���ĵ�ַ��������ݸ���-1        
        }   
    }
    I2C_AcknowledgeConfig(I2C1, ENABLE);//��Ӧ��
}

/*
 * ��������I2C_MPU6050_Init
 * ����  ��I2C ����(MPU6050)��ʼ��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void I2C_MPU6050_Init(void)
{    
    I2C_GPIO_Config();
    I2C_Mode_Config();
}

/*д���ʼ��MPU6050*/
void InitMPU6050()
{
    I2C_MPU_ByteWrite(0x00,PWR_MGMT_1); //�������״̬
    I2C_MPU_ByteWrite(0x07,SMPLRT_DIV);//
    I2C_MPU_ByteWrite(0x06,CONFIG);//��ͨ�˲�Ƶ��
    I2C_MPU_ByteWrite(0x18,GYRO_CONFIG);//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
    I2C_MPU_ByteWrite(0x01,ACCEL_CONFIG);//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
}

uchar I2C_MPU_ACCE_Read[2];//���ٶȼ���ʱ����
uchar I2C_MPU_GYRO_Read[2];//��������ʱ����

/*��ȡ���ݣ��ϳ�����*/
u16 ACCE_Data_Z(uint8_t ZOUT_H)//z�� ���ٶ�
{
    uint8_t ACCEL_H;
    uint8_t ACCEL_L;
    I2C_MPU_ByteRead(I2C_MPU_ACCE_Read,ZOUT_H,2);
    
    ACCEL_H=I2C_MPU_ACCE_Read[0];
    ACCEL_L=I2C_MPU_ACCE_Read[1];
    
    return (ACCEL_H<<8)+ACCEL_L; //�ϳ�����
}

u16 ACCE_Data_X(uint8_t XOUT_H)//x�� ���ٶ�
{
    uint8_t ACCEL_H;
    uint8_t ACCEL_L;
    I2C_MPU_ByteRead(I2C_MPU_ACCE_Read,XOUT_H,2);
    
    ACCEL_H=I2C_MPU_ACCE_Read[0];
    ACCEL_L=I2C_MPU_ACCE_Read[1];
    
    return (ACCEL_H<<8)+ACCEL_L; //�ϳ�����
}

u16 ACCE_Data_Y(uint8_t YOUT_H)//y�� ���ٶ�
{
    uint8_t ACCEL_H;
    uint8_t ACCEL_L;
    I2C_MPU_ByteRead(I2C_MPU_ACCE_Read,YOUT_H,2);
    
    ACCEL_H=I2C_MPU_ACCE_Read[0];
    ACCEL_L=I2C_MPU_ACCE_Read[1];
    
    return (ACCEL_H<<8)+ACCEL_L; //�ϳ�����
}

u16 GYRO_Data_Y(uint8_t YOUT_H)//y�� ������
{
    uint8_t GYRO_H;
    uint8_t GYRO_L;

    I2C_MPU_ByteRead(I2C_MPU_GYRO_Read,YOUT_H,2);

    GYRO_H=I2C_MPU_GYRO_Read[0];//Y�������Ǹ�λ
    GYRO_L=I2C_MPU_GYRO_Read[1];

    return (GYRO_H<<8)+GYRO_L;   //�ϳ�����
}

u16 GYRO_Data_X(uint8_t XOUT_H)//y�� ������
{
    uint8_t GYRO_H;
    uint8_t GYRO_L;

    I2C_MPU_ByteRead(I2C_MPU_GYRO_Read,XOUT_H,2);

    GYRO_H=I2C_MPU_GYRO_Read[0];//Y�������Ǹ�λ
    GYRO_L=I2C_MPU_GYRO_Read[1];

    return (GYRO_H<<8)+GYRO_L;   //�ϳ�����
}

/*********************************************************��2��������Ƕȣ����ٶ�**********************************************/

/*�Ƕȼ���*/
float g_fGyroTemp=0;
void Angle_Calcu(void)   
{

//��ΧΪ2gʱ�������ϵ��16384 LSB/g
//deg = rad*180/3.14
/*����z���ϵ��������ٶȼ������оƬ��ˮƽ��ļн�*/
    static uchar s_ucFlag=1;
    static short int s_siAccXOffset=0;
    static short int s_siAccZOffset=0;
    TIM6->DIER &= ~0x0001 ;
    TIM4->DIER &= ~0x0002 ;
    USART1->CR1 &=~0X0020;
    UART4->CR1 &=~0X0020;
    if(s_ucFlag==1)//������ʼ״ֵ̬
    {
        s_ucFlag=0;
        s_siAccXOffset = -16384 - ((ACCE_Data_X(ACCEL_XOUT_H)+ACCE_Data_X(ACCEL_XOUT_H)+ACCE_Data_X(ACCEL_XOUT_H)+ACCE_Data_X(ACCEL_XOUT_H) )>>2);
        s_siAccZOffset = 0-ACCE_Data_Z(ACCEL_ZOUT_H);
    }
    Accel_x = ACCE_Data_X(ACCEL_XOUT_H)+s_siAccXOffset;//x��
    Accel_y= ACCE_Data_Y(ACCEL_YOUT_H);//y��   /*2016.02.22����*/
    Accel_z = ACCE_Data_Z(ACCEL_ZOUT_H)+s_siAccZOffset;//z��
    
    Gyro_y = GYRO_Data_Y(GYRO_YOUT_H);//GetData�ϳɵ�ԭʼ����
    Gyro_x = GYRO_Data_X(GYRO_XOUT_H);//GetData�ϳɵ�ԭʼ����
    TIM6->DIER |= 0x0001 ;
    TIM4->DIER |= 0x0002 ;
    USART1->CR1 |= 0X0020;
    UART4->CR1 |= 0X0020;

////    z=Accel_z/16384;                                                //           y Y
////  }                                                                 //          /
////  else                                                              //         /  mpu 6050
////  {                                                                 //        |----------->z
////     z=(Accel_z-65535)/16384;                                       //        |
////  }                                                                 //        |
//    z=Accel_z/16384.;                                                 //        v  //       x
    Angle_az = (atan((float)Accel_z/Accel_x))*180/3.14;  //mpu6050����洹ֱ��z��ˮƽ���� ������ǰ��н�
    Gyro_y=Gyro_y/16.4;
    //Angle_gy = Angle_gy + Gyro_y*0.025;  //���ٶȻ��ֵõ���б�Ƕ�.Խ����ֳ����ĽǶ�Խ��  
    
    /*2016.02.22����*/
    Angle_ay = (atan((float)Accel_y/Accel_x))*180/3.14;//���������Ҽн�  
    Angle_qingxie = sqrt(Angle_az*Angle_az+Angle_ay*Angle_ay);//������360����бˮƽ��ĽǶ� ����ǰ��нǵ�ƽ������ǰ��нǵ�ƽ��֮���ڿ����ż��ɣ�
    
    
    //-------�������˲��ں�-----------------------
    g_fGyroTemp=Gyro_y ; 
    Kalman_Filter( Angle_az ,Gyro_y);       //�������˲��������
                                                                
} 



/**********************************************************��3��������������*************************************************/
        
//float Q_angle=0.001;//0.001  
//float Q_gyro=0.003;//0.03
//float R_angle=0.5;//0.5
//float dt=0.01;//0.1                   //dtΪkalman�˲�������ʱ��;
//char  C_0 = 1;
//float Q_angle=0.001;//0.001  
//float Q_gyro=0.003;//0.03
//float R_angle=0.5;//0.5
//float dt=0.005;//0.1                    //dtΪkalman�˲�������ʱ��;
//char  C_0 = 1;
float Q_angle=0.0001;//0.001  0.0001
float Q_gyro=0.003;//0.03
float R_angle=20;//0.5      20
float dt=0.005;//0.1                    //dtΪkalman�˲�������ʱ��;
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };

/*����������*/
void Kalman_Filter(float Accel,float Gyro)    
{
    Angle+=(Gyro - Q_bias) * dt; //�������

    
    Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

    Pdot[1] = - PP[1][1];
    Pdot[2] = - PP[1][1];
    Pdot[3]=Q_gyro;
    
    PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
    PP[0][1] += Pdot[1] * dt;   // =����������Э����
    PP[1][0] += Pdot[2] * dt;
    PP[1][1] += Pdot[3] * dt;
        
    Angle_err = Accel - Angle;  //zk-�������
    
    PCt_0 = C_0 * PP[0][0];
    PCt_1 = C_0 * PP[1][0];
    
    E = R_angle + C_0 * PCt_0;
    
    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;
    
    t_0 = PCt_0;
    t_1 = C_0 * PP[0][1];

    PP[0][0] -= K_0 * t_0;     //����������Э����
    PP[0][1] -= K_0 * t_1;
    PP[1][0] -= K_1 * t_0;
    PP[1][1] -= K_1 * t_1;
        
    Angle += K_0 * Angle_err;  //�������
    Q_bias  += K_1 * Angle_err;  //�������
    Gyro_y   = Gyro - Q_bias;  //���ֵ(�������)��΢��=���ٶ�
}

long int g_lnMmaOffset=0;
//float K1 =0.02; 
//void Yijielvbo(float angle_m, float gyro_m)
//{
//   Angle = K1 * angle_m+ (1-K1) * (Angle + gyro_m * 0.005);
//  Gyro_y = gyro_m;
//}

