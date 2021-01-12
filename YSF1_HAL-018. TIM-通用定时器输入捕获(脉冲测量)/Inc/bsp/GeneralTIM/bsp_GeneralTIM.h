#ifndef __GENERAL_TIM_H__
#define __GENERAL_TIM_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/************************** ���벶���ⲿ���� ********************************/
typedef struct              //�����ߵ�ƽ����
{   
	uint8_t   ucFinishFlag;
	uint8_t   ucStartFlag;
	uint16_t  usCtr;
	uint16_t  usPeriod;
}STRUCT_CAPTURE;

/* �궨�� --------------------------------------------------------------------*/
#define GENERAL_TIMx                        TIM5
#define GENERAL_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM5_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM5_CLK_DISABLE()
#define GENERAL_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE();
#define GENERAL_TIM_CHANNELx                TIM_CHANNEL_1
#define GENERAL_TIM_IT_CCx                  TIM_IT_CC1

#define GENERAL_TIM_IRQn                    TIM5_IRQn
#define GENERAL_TIM_IRQHANDLER              TIM5_IRQHandler

#define KEY1_RCC_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY1_GPIO_PIN                       GPIO_PIN_0
#define KEY1_GPIO                           GPIOA

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��GENERAL_TIMx_PRESCALER+1��
#define GENERAL_TIM_PRESCALER               71  // ʵ��ʱ��Ƶ��Ϊ��36kHz

// ���嶨ʱ�����ڣ�����ʱ����ʼ������GENERAL_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define GENERAL_TIM_PERIOD                  0xFFFF

#define GENERAL_TIM_STRAT_ICPolarity        TIM_INPUTCHANNELPOLARITY_RISING          //��������ʼ����
#define GENERAL_TIM_END_ICPolarity          TIM_INPUTCHANNELPOLARITY_RISING        //�����Ľ�������

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx;
extern uint8_t indexWave[];

/* �������� ------------------------------------------------------------------*/
void GENERAL_TIMx_Init(void);

#endif	/* __GENERAL_TIM_H__ */
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
