/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "stm32f4_discovery.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#define LIS_CS_PORT   GPIOE
#define LIS_CS_PIN    GPIO_PIN_3
/* Read/Write command */
#define READWRITE_CMD_THUANNT                     ((uint8_t)0x80)
/* Multiple byte read/write command */
#define MULTIPLEBYTE_CMD_THUANNT                  ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE_THUANNT                        ((uint8_t)0x00)

// Macro điều khiển CS
#define LIS_CS_LOW()   HAL_GPIO_WritePin(LIS_CS_PORT, LIS_CS_PIN, GPIO_PIN_RESET)
#define LIS_CS_HIGH()  HAL_GPIO_WritePin(LIS_CS_PORT, LIS_CS_PIN, GPIO_PIN_SET)
static uint8_t SPIme_WriteRead(uint8_t Byte);
void ACCELERO_me_IO_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
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
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
void spi1_flush_rx(void)
{
    volatile uint8_t tmp;

    // Đọc liên tục khi cờ RXNE = 1 (có dữ liệu trong RX buffer)
    while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE))
    {
        tmp = SPI1->DR;
    }

    // Clear lỗi Overrun nếu có
    __HAL_SPI_CLEAR_OVRFLAG(&hspi1);

    (void)tmp; // tránh warning "unused variable"
}
#if 0
uint8_t lis302dl_read_reg(uint8_t reg)
{
    uint8_t tx = reg | 0x80 ;   // bit7=1 => Read
    uint8_t rx = 0;
#if 0
    spi1_flush_rx();
#else
#endif
    LIS_CS_LOW();
    HAL_SPI_Transmit(&hspi1, &tx, 1, HAL_MAX_DELAY);
    //HAL_Delay(5);
    HAL_SPI_Receive(&hspi1, &rx, 1, HAL_MAX_DELAY);
    HAL_Delay(5);
    LIS_CS_HIGH();

    return rx;
}
#else


uint8_t lis302dl_read_reg(uint8_t reg)
{
    uint8_t tx[2] = {0};
    uint8_t tx_init[2] = {0};
    uint8_t rx[2] = {0};
    HAL_StatusTypeDef ret;
    GPIO_PinState pin_state;

    tx[0] = reg | 0x80;  // set bit7 = Read
#if 1
    tx[1] = 0x00;        // dummy byte
    spi1_flush_rx();
#else
    tx[1] = 0xff;
#endif

    LIS_CS_LOW();
    pin_state = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
    if(pin_state != GPIO_PIN_RESET) {
    	HAL_UART_Transmit(&huart2, "Err RESET\r\n", strlen("Err RESET\r\n"), HAL_MAX_DELAY);
    }
    do {
    	//HAL_Delay(500);
    	ret = HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    	if(ret == HAL_OK) {
    		break;
    	}
    	//HAL_Delay(100);
    } while(1);
    LIS_CS_HIGH();
    pin_state = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
    if(pin_state != GPIO_PIN_SET) {
    	HAL_UART_Transmit(&huart2, "Err SET\r\n", strlen("Err SET\r\n"), HAL_MAX_DELAY);
    }
    return rx[1]; // byte thứ 2 là dữ liệu từ LIS302DL
}
#endif
uint8_t lis302dl_whoami_check(void)
{
    return lis302dl_read_reg(0x0F);

}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
#define MSG_ME "Hello back!\n"
static void showled(uint8_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
#if 0
  MX_SPI1_Init();
#else
  SPIx_Init_ext(&hspi1);
#endif
  uint8_t ch[2] = {0};
  uint8_t cmd = 0;
  int count = 0;
  uint8_t str[32];
  HAL_StatusTypeDef err;
  uint8_t ReadAddr = 0x0F;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#if 0
	  LIS_CS_LOW();
	  HAL_GPIO_WritePin(LIS_CS_PORT, LIS_CS_PIN, GPIO_PIN_RESET); // kéo LOW
	  GPIO_PinState stt = HAL_GPIO_ReadPin(LIS_CS_PORT, LIS_CS_PIN);
	  if (stt == GPIO_PIN_RESET) {
	      printf("OK");
	  }
	  HAL_Delay(1000);
	  LIS_CS_HIGH();

	  HAL_GPIO_WritePin(LIS_CS_PORT, LIS_CS_PIN, GPIO_PIN_SET); // kéo HIGH
	  stt = HAL_GPIO_ReadPin(LIS_CS_PORT, LIS_CS_PIN);
	  if (stt == GPIO_PIN_SET) {
		  printf("OK");
	  }
	  HAL_Delay(1000);
#else

	  err = HAL_UART_Receive(&huart2, &cmd, 1, HAL_MAX_DELAY);
	  if(err) {
		  HAL_Delay(1000);
		  continue;
	  }
	  //ch = lis302dl_whoami_check();

#if 0
	  ACCELERO_IO_Init();
	  ACCELERO_me_IO_Read(&ch, ReadAddr, 1);
#else

	  ACCELERO_IO_Init();
	  ch[0] = ch[1] = 0;
	  if(cmd == 'F') {
		  ACCELERO_IO_Read(ch, 0x0F, 1);
	  }
	  if(cmd == 'D') {
		  ACCELERO_IO_Read(ch, 0x0D, 1);
	  }
	  if(cmd == 'E') {
		  ACCELERO_IO_Read(ch, 0x0E, 1);
	  }
	  else {
		  ACCELERO_IO_Read(ch, 0x0F, 1);
	  }
#endif
	  memset(str, 0, sizeof(str));
	  snprintf(str, 32, "SPI1 whoami: 0x%X\r\n", (int)ch[0]);
	  HAL_UART_Transmit(&huart2, str, strlen(str), HAL_MAX_DELAY);
	  //HAL_UART_Transmit(&huart2, MSG_ME, sizeof(MSG_ME), HAL_MAX_DELAY);
	  //HAL_Delay(1000);
	  //HAL_UART_Transmit(&huart2, MSG_ME, sizeof(MSG_ME), HAL_MAX_DELAY);
	  //HAL_Delay(1000);
	  showled(count++);
#endif
  }
  /* USER CODE END 3 */
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

#if 1

  GPIO_InitTypeDef GPIO_InitStruct = {0};
#if 1
  __HAL_RCC_GPIOE_CLK_ENABLE();
  ACCELERO_CS_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // phải là OUTPUT_PP
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); // mặc định HIGH
#endif
  // ---- Cấu hình LED3–6 (PD12,13,14,15) ----
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  // Bật tất cả LED (set HIGH)
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);
#endif

#if 0
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // phải là OUTPUT_PP
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); // mặc định HIGH
#endif

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static uint8_t SPIme_WriteRead(uint8_t Byte)
{
  uint8_t receivedbyte = 0;

  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, 100) != HAL_OK)
  {
    //SPIx_Error();
  }

  return receivedbyte;
}
void ACCELERO_me_IO_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  if(NumByteToRead > 0x01)
  {
    ReadAddr |= (uint8_t)(READWRITE_CMD_THUANNT | MULTIPLEBYTE_CMD_THUANNT);
  }
  else
  {
    ReadAddr |= (uint8_t)READWRITE_CMD_THUANNT;
  }
  /* Set chip select Low at the start of the transmission */
  LIS_CS_LOW();

  /* Send the Address of the indexed register */
  SPIme_WriteRead(ReadAddr);

  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to ACCELEROMETER (Slave device) */
    *pBuffer = SPIme_WriteRead(DUMMY_BYTE_THUANNT);
    NumByteToRead--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  LIS_CS_HIGH();
}
void showled(uint8_t i) {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
	switch (i%4) {
		case 0:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
			break;
		default:
			break;
	}
}
/* USER CODE END 4 */

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
