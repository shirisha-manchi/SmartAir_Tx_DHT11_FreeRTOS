/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define TX_COUNT_TLS_INDEX  0
#include <stdio.h>
#include "dht11.h"  //DHT11 Sensor data

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

//osThreadId defaultTaskHandle;
//osTimerId myTimer01Handle;
//osMutexId myMutex01Handle;
//osSemaphoreId myBinarySem01Handle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
//void StartDefaultTask(void const *argument);
//void Callback01(void const *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*================ TASK PROTOTYPES ======================*/
void Sender_Task(void *arg);
void Receiver_Task(void *arg);
void Timer_Task(TimerHandle_t xTimer);
void Status_Task(TimerHandle_t xTimer);
void Status(void * arg);

//uint8_t status_flag;


//To use printf()
int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart1, (uint8_t*) ptr, (uint16_t) len, HAL_MAX_DELAY);
	return len;
}

/*=============== HANDLES ================================*/
QueueHandle_t QueueHandle_sensor;
SemaphoreHandle_t UART_Mutex;
SemaphoreHandle_t Timer_Sem, Status_sem;
TimerHandle_t TimerHandle, Status_TimerHandle;

/*================= SENSOR VARIABLE ======================*/
DHT11_Data_t temp; //Temp struct variable

//Buffers to store temp and humidity
uint8_t temp_buf;
uint8_t hum_buf;
uint32_t txCount;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	DWT_Delay_Init();

	QueueHandle_sensor = xQueueCreate(4, sizeof(uint8_t)); //creating queue
	Timer_Sem = xSemaphoreCreateBinary();	//Semaphore to notify sender to write in queue
	Status_sem = xSemaphoreCreateBinary();
	UART_Mutex = xSemaphoreCreateMutex();	//Mutex to use UART

	TimerHandle = xTimerCreate("Timerpc", pdMS_TO_TICKS(3000), pdTRUE, 0, Timer_Task); //FreeRTOS Timer to generate 3sec
	Status_TimerHandle = xTimerCreate("Statuspc", pdMS_TO_TICKS(10000), pdTRUE, 0, Status_Task); //To print no of transmissions happend for every 10 sec

	xTimerStart(TimerHandle, 0); //To start FreeRTOS Timer
	//xTimerStart(Status_TimerHandle, 0);

	TaskHandle_t Sender_Task_Handle, Receiver_Task_Handle, Status_Handle;
	xTaskCreate(Sender_Task, "Sender_pc", 256, NULL, 1, &Sender_Task_Handle);
	xTaskCreate(Receiver_Task, "Receiver_pc", 256, NULL, 1, &Receiver_Task_Handle);
	xTaskCreate(Status, "Statuspc", 256, NULL, 1, &Status_Handle);

	vTaskStartScheduler(); //Start the scheduler

	/* USER CODE END 2 */

	/* Create the mutex(es) */
	/* definition and creation of myMutex01 */
//  osMutexDef(myMutex01);
//  myMutex01Handle = osMutexCreate(osMutex(myMutex01));
	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* Create the semaphores(s) */
	/* definition and creation of myBinarySem01 */
//  osSemaphoreDef(myBinarySem01);
//  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);
	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* Create the timer(s) */
	/* definition and creation of myTimer01 */
//  osTimerDef(myTimer01, Callback01);
//  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);
	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* Start scheduler */
	//osKernelStart();
	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);

	/*Configure GPIO pin : PA0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PA4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PG13 PG14 */
	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//Call back function
//When Interrupt is generated goes to ISR in ISR it calls Callback()
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	//status_flag = 1;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	//BaseType_t xHigherPriorityTaskWoken2 = pdFALSE;
	//xSemaphoreGiveFromISR(Status_sem, xHigherPriorityTaskWoken2);
	xSemaphoreGiveFromISR(Timer_Sem, xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //To give CPU back to high priority task


	//portYIELD_FROM_ISR(xHigherPriorityTaskWoken2);
}

//Sender Task
void Sender_Task(void *arg) {
	 vTaskSetThreadLocalStoragePointer(NULL, TX_COUNT_TLS_INDEX,(void*) txCount);
	for (;;) {
		Read_Temp_Hum(&temp);
		xSemaphoreTake(Timer_Sem, portMAX_DELAY);
		xQueueSendToBack(QueueHandle_sensor, &temp.temperature, portMAX_DELAY);
		xQueueSendToBack(QueueHandle_sensor, &temp.humidity, portMAX_DELAY);

		txCount = (uint32_t) pvTaskGetThreadLocalStoragePointer(NULL,TX_COUNT_TLS_INDEX);
		txCount++;
		vTaskSetThreadLocalStoragePointer(NULL, TX_COUNT_TLS_INDEX,(void*) txCount);
	}
}

//Receiver Task
void Receiver_Task(void *arg) {
	for (;;) {
		xQueueReceive(QueueHandle_sensor, &temp_buf, portMAX_DELAY);
		xQueueReceive(QueueHandle_sensor, &hum_buf, portMAX_DELAY);

		xSemaphoreTake(UART_Mutex, portMAX_DELAY);
		printf("Temperature -> %d\tHumidity -> %d\r\n", temp_buf, hum_buf);
		xSemaphoreGive(UART_Mutex);

	}
}
//Status task
void Status(void * arg)
{
	for(;;)
	{
	xSemaphoreTake(Status_sem, portMAX_DELAY);
	xSemaphoreTake(UART_Mutex, portMAX_DELAY);
	printf("No of Transmissions so far: %lu\n", txCount);
	xSemaphoreGive(UART_Mutex);
	}
}
//to give status for every 10 sec
void Status_Task(TimerHandle_t xTimer)
 {
	xSemaphoreTake(UART_Mutex, portMAX_DELAY);
	printf("No of Transmissions so far: %lu\r\n", txCount);
	xSemaphoreGive(UART_Mutex);
}



//Timer Task
void Timer_Task(TimerHandle_t xTimer) {
	xSemaphoreGive(Timer_Sem);
}




/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
////////  /* Infinite loop */
////////  for(;;)
////////  {
////////    osDelay(1);
////////  }
//  /* USER CODE END 5 */
//}
/* Callback01 function */
//void Callback01(void const * argument)
//{
//  /* USER CODE BEGIN Callback01 */
//
//  /* USER CODE END Callback01 */
//}
/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
