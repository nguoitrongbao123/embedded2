#include "main.h"
#include "motor.h"
#include "i2c-lcd.h"
#include "string.h"
#include "stdio.h"
#include "led.h"
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < time);
}

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;

uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  // is the first value captured ?
float Distance  = 0;
#define TRIG_PIN GPIO_PIN_9
#define TRIG_PORT GPIOA
char buff[16];
// Let's write the callback function

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
	{
		if (Is_First_Captured==0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured==1)   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			Distance = (float)Difference * .034/2.0f;
			Is_First_Captured = 0; // set it back to false

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
		}
	}
}
void HCSR04_Read (void)
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}
void display_lcd_and_leddo(float distance)
{
  HAL_Delay(50);
  lcd_clear();
  lcd_send_cmd(0xC2); // Set cursor to second line, first column
  snprintf(buff, sizeof(buff), "%.2f cm", distance); // Convert float to string with 2 decimal places
  lcd_send_string(buff); // Display the distance value

  // Update VaCa on first line
  lcd_send_cmd(0x80); // Set cursor to first line, first column
  if (distance > 25)
  {
    lcd_send_string("VaCa: 0");

    TIM4->ARR = 2000;
  }
  else if (distance >= 0 && distance < 2)
  {
    lcd_send_string("VaCa: 1");
    if (TIM4->CNT > 99 ) TIM4->CNT = 0;
    TIM4->ARR = 99;
  }
  else if (distance >= 2 && distance < 4)
    {
      lcd_send_string("VaCa: 1");
      if (TIM4->CNT > 110 ) TIM4->CNT = 0;
      TIM4->ARR = 110;
    }
  else if (distance >= 4 && distance < 6)
      {
        lcd_send_string("VaCa: 1");
        if (TIM4->CNT > 124 ) TIM4->CNT = 0;
        TIM4->ARR = 124;
      }
  else if (distance >= 6 && distance < 8)
      {
        lcd_send_string("VaCa: 1");
        if (TIM4->CNT > 141 ) TIM4->CNT = 0;
        TIM4->ARR = 141;
      }
  else if (Distance > 8 && Distance <= 10)
      {
	  lcd_send_string("VaCa: 1");
	  if (TIM4->CNT > 166 ) TIM4->CNT = 0;
	          TIM4->ARR = 166;
      }
      else if (Distance > 10 && Distance <= 12)
      {
    	  lcd_send_string("VaCa: 1");
    	  if (TIM4->CNT > 199 ) TIM4->CNT = 0;
    	          TIM4->ARR = 199;
      }
      else if (Distance > 12 && Distance <= 14)
      {
    	  lcd_send_string("VaCa: 1");
    	  if (TIM4->CNT > 249 ) TIM4->CNT = 0;
    	          TIM4->ARR = 249;
      }
      else if (Distance > 14 && Distance <= 16)
      {
    	  lcd_send_string("VaCa: 1");
    	  if (TIM4->CNT > 332 ) TIM4->CNT = 0;
    	          TIM4->ARR = 332;
      }
      else if (Distance > 16 && Distance <= 18)
      {
    	  lcd_send_string("VaCa: 1");
    	  if (TIM4->CNT > 499 ) TIM4->CNT = 0;
    	          TIM4->ARR = 499;
      }
      else if (Distance > 18 && Distance <= 25)
      {
    	  lcd_send_string("VaCa: 1");
    	  if (TIM4->CNT > 999 ) TIM4->CNT = 0;
    	          TIM4->ARR = 999;
      }

}


int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  motor_Init();
  TIM3_PWM_Init();
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
  lcd_init();
  lcd_send_cmd(0x01); // Clear the screen initially
  Ledxanh();
  Leddo();
  while (1)
  {
	  HCSR04_Read();
	  display_lcd_and_leddo(Distance);
	  nhayledxanh();
      nhayleddo();
  }
}


static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 16-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */

static void MX_GPIO_Init(void)
{
  /* 1. Kích hoạt Clock cho GPIOA và GPIOB */
  RCC->AHB1ENR |= (1 << 0); // Enable GPIOA Clock (Bit 0)
  RCC->AHB1ENR |= (1 << 1); // Enable GPIOB Clock (Bit 1)
  // Đặt MODE (GPIOx_MODER) thành 01 cho Output mode
  GPIOA->MODER &= ~(0x3 << (9 * 2)); // Xóa 2 bit của Pin 9
  GPIOA->MODER |=  (0x1 << (9 * 2)); // Output mode cho Pin 9
  // Đặt OTYPER (GPIOx_OTYPER) thành 0 cho Push-Pull
  GPIOA->OTYPER &= ~(1 << 9); // Push-Pull cho Pin 9


  // Đặt PUPDR (GPIOx_PUPDR) thành 00 cho No Pull-up, Pull-down
  GPIOA->PUPDR &= ~(0x3 << (9 * 2)); // No Pull-up, Pull-down cho Pin 9
  GPIOA->OSPEEDR &= ~(0x3 << (9 * 2)); // Low speed cho Pin 9
  GPIOA->BSRR = (1 << (9 + 16)); // Reset Pin 9 (đặt mức thấp)

}



/* USER CODE BEGIN 4 */

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
