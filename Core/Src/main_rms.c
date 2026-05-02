/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "stdio.h"
#ifdef RMS

#define DISPLAY
extern void initialise_monitor_handles(void);

#ifdef DISPLAY 
#define PRINT   printf
#else
#define PRINT(f,...)  ((void)0)
#endif 

SemaphoreHandle_t    Sem1;
TaskHandle_t         hTask1;
QueueHandle_t        Queue1;
EventGroupHandle_t   Flag;
void Task1(void *p);
void Task2(void *p);
void Task3(void *p);
void Task4(void *p);

#define LONG_BUTON_FLAG	             ( 1U << 0U )  //BIT0
#define SHORT_BUTON_FLAG             ( 1U << 4U )  //BIT4


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void *argument);
void LoadFunction(uint32_t time_ms, uint16_t io);


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  
  /* Attempt to create the event group. */
  Flag = xEventGroupCreate();
  /*CreateSemaphore */
  Sem1= xSemaphoreCreateBinary();
  /* Create a queue capable of containing 10 unsigned long values. */
  Queue1 = xQueueCreate( 10, sizeof( uint32_t ) );

  /*Task Creation */
  xTaskCreate(Task1,"T1 "  ,128, NULL,2,&hTask1);
  xTaskCreate(Task2,"T2"  ,128,NULL,4,NULL);
  xTaskCreate(Task3,"T3" ,128,NULL,3,NULL);
  xTaskCreate(Task4,"T4"  ,128,NULL,5,NULL);
  
  vTaskStartScheduler();
    
  while (1)
  {

  }

}


void LoadFunction(uint32_t time_ms, uint16_t io)
{
  
  for (uint32_t i=0;i< time_ms/2;i++)
  {
    
    HAL_Delay(1);
     HAL_GPIO_TogglePin(GPIOD,io);
    
  }
  
}

/*Perdidic Task*/
void Task1(void *p) //P1
{
  TickType_t xLastWakeTime;
  const TickType_t xTaskPeriod = 1000;
  xLastWakeTime = xTaskGetTickCount();
    
  while(1)
  {
     /*Ci =200 */  
    LoadFunction(200,GPIO_PIN_12);
   
     vTaskDelayUntil( &xLastWakeTime, xTaskPeriod );
 
  }
}

void Task2(void *p) // P3
{
   TickType_t xLastWakeTime;
  const TickType_t xTaskPeriod = 150;
  xLastWakeTime = xTaskGetTickCount();

  while(1)
  {
     /*Ci =50 */  
    LoadFunction(50,GPIO_PIN_13);
    
    vTaskDelayUntil( &xLastWakeTime, xTaskPeriod );
    //vTaskDelay(150);
  }
}//end task3




void Task3(void *p) // P3
{
 TickType_t xLastWakeTime;
  const TickType_t xTaskPeriod = 350;
  xLastWakeTime = xTaskGetTickCount();
  
 while(1)
 {
     /*Ci =50 */  
    LoadFunction(80,GPIO_PIN_14);
       
    vTaskDelayUntil( &xLastWakeTime, xTaskPeriod );

    //vTaskDelay(350);
 }
}
void Task4(void *p) // P3
{
  TickType_t xLastWakeTime;
  const TickType_t xTaskPeriod = 350;
  xLastWakeTime = xTaskGetTickCount();
  
  while(1)
  {
     /*Ci =50 */  
    LoadFunction(100,GPIO_PIN_15);
    
     vTaskDelayUntil( &xLastWakeTime, xTaskPeriod );

  }
  
}


// pdFALSE / pdTRUE
/*EXTI0 ISR_Handler*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
   BaseType_t xHigherPriority=pdFALSE;
  
   xSemaphoreGiveFromISR(Sem1, &xHigherPriority);
   /*if True force contex switch else wait for next tick*/
   portYIELD_FROM_ISR(xHigherPriority); 
   
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
#endif
