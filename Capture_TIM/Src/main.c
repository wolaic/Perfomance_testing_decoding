/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 使用通用定时器输入捕获功能测量按键从按下到弹开时间
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "GeneralTIM/bsp_GeneralTIM.h"
#include "led/bsp_led.h"
#include "usart/bsp_debug_usart.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
STRUCT_CAPTURE strCapture = {2, 0, 0, 0 };

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;  // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 9倍频，得到72MHz主时钟
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：72MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟：72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;               // APB1时钟：36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;               // APB2时钟：72MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);  // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{  
  uint32_t ulTmrClk, ulTime;
  
  /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
  HAL_Init();
  /* 配置系统时钟 */
  SystemClock_Config();

  /* LED初始化 */
  LED_GPIO_Init();
  
  /* 初始化串口并配置串口中断优先级 */
  MX_DEBUG_USART_Init();
  
  /* 通用定时器初始化并配置输入捕获功能 */
  GENERAL_TIMx_Init();
  
  printf ( "STM32 Test Start\n" );


  /* 获取定时器时钟周期 */	
	ulTmrClk = HAL_RCC_GetHCLKFreq()/GENERAL_TIM_PRESCALER;    
  /* 启动定时器 */
  HAL_TIM_Base_Start_IT(&htimx);  
  /* 启动定时器通道输入捕获并开启中断 */
  HAL_TIM_IC_Start_IT(&htimx,GENERAL_TIM_CHANNELx);
  
  /* 无限循环 */
  while (1)
  {	
//		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);
//		
//		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET);
//		
//		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);
    /* 完成测量高电平脉宽 */
    if(strCapture.ucFinishFlag == 1 )
		{
      /* 计算高电平计数值 */
				if (strCapture .ucMissFlag >3)
		{
			printf("Null\n");
			strCapture .ucFinishFlag = 0;
			strCapture .ucMissFlag = 3;
			
		}
		if(strCapture .ucFinishFlag == 1)
		{
			ulTime = strCapture .usPeriod * GENERAL_TIM_PERIOD + strCapture .usCtr;
			/* 打印高电平脉宽时间 */
			printf ( ">>decoding time:%d.%06d s\n", ulTime / ulTmrClk, ulTime % ulTmrClk ); 
		  
			strCapture .ucFinishFlag = 0;
			strCapture .ucMissFlag = 2;
		}
		}
  }
}

/**
  * 函数功能: 非阻塞模式下定时器的回调函数
  * 输入参数: htim：定时器句柄
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  strCapture .usPeriod ++;
}

/**
  * 函数功能: 定时器输入捕获中断回调函数
  * 输入参数: htim：定时器句柄
  * 返 回 值: 无
  * 说    明: 无
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  TIM_IC_InitTypeDef sConfigIC;
  
  if ( strCapture .ucStartFlag == 0 )
  {
//    LED1_TOGGLE;       
    
    __HAL_TIM_SET_COUNTER(htim,0); // 清零定时器计数
    strCapture .usPeriod = 0;			
    strCapture .usCtr = 0;
    
    // 配置输入捕获参数，主要是修改触发电平
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htimx, &sConfigIC, GENERAL_TIM_CHANNELx);
    // 清除中断标志位
    __HAL_TIM_CLEAR_IT(htim, GENERAL_TIM_IT_CCx);
    // 启动输入捕获并开启中断
    HAL_TIM_IC_Start_IT(&htimx,TIM_CHANNEL_2);  
		HAL_TIM_IC_Start_IT(&htimx,GENERAL_TIM_CHANNELx);
    strCapture .ucStartFlag = 1;
		
  }		
  
  else
  {
//    LED1_TOGGLE;
    
    // 获取定时器计数值
    strCapture .usCtr = HAL_TIM_ReadCapturedValue(&htimx,TIM_CHANNEL_2);
    // 配置输入捕获参数，主要是修改触发电平
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htimx, &sConfigIC, TIM_CHANNEL_2);
    
    // 清除中断标志位
    __HAL_TIM_CLEAR_IT(htim, TIM_IT_CC2); 
    // 启动输入捕获并开启中断
    HAL_TIM_IC_Start_IT(&htimx,GENERAL_TIM_CHANNELx);    
    strCapture .ucStartFlag = 0;			
    strCapture .ucFinishFlag = 1;    
  }
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
