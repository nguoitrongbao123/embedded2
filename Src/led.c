#include "led.h"

void LedDo_Update(float Distance)
{
	int a=0;
    if (Distance <= 2)
    {
    	a = 99;
    }
    else if (Distance > 2 && Distance <= 4)
    {
    	a = 110;
    }
    else if (Distance > 4 && Distance <= 6)
    {
    	a = 124;
    }
    else if (Distance > 6 && Distance <= 8)
    {
    	a = 141;
    }
    else if (Distance > 8 && Distance <= 10)
    {
    	a = 166;
    }
    else if (Distance > 10 && Distance <= 12)
    {
    	a = 199;
    }
    else if (Distance > 12 && Distance <= 14)
    {
    	a = 249;
    }
    else if (Distance > 14 && Distance <= 16)
    {
    	a = 332;
    }
    else if (Distance > 16 && Distance <= 18)
    {
    	a = 499;
    }
    else if (Distance > 18 && Distance <= 25)
    {
    	a = 999;
    }


    // Cập nhật giá trị ARR và khởi động lại TIM4 nếu cần
    if (Distance < 25)
    {
    	TIM4->ARR = a;
    }

}
void Ledxanh(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Bật clock cho GPIOA
	GPIOA->MODER &= ~(GPIO_MODER_MODER0); // Reset PA0 mode
	GPIOA->MODER |= (1 << GPIO_MODER_MODER0_Pos);
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; // Bật clock cho TIM5
	 TIM5->PSC = 16000 - 1; // Prescaler: Chia clock xuống còn 1ms
		    TIM5->ARR = 499;  // Auto-reload value: 1 giây (1Hz)
		    TIM5->CR1 |= TIM_CR1_CEN; // Bật Timer 5
}
void Leddo(void) {

    // 1. Cấu hình clock cho GPIOA và GPIOB
	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Bật clock cho GPIOB
	    GPIOB->MODER &= ~(GPIO_MODER_MODER7); // Reset PB7 mode
	    GPIOB->MODER |= (1 << GPIO_MODER_MODER7_Pos);
	    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Bật clock cho TIM4
	    TIM4->PSC = 16000 - 1; // Prescaler: Chia clock xuống còn 1ms
	    TIM4->CR1 |= TIM_CR1_CEN; // Bật Timer 4
}

void nhayledxanh(){
        if (TIM5->SR & TIM_SR_UIF) { // Kiểm tra update interrupt flag của TIM2
            TIM5->SR &= ~TIM_SR_UIF; // Xóa cờ
            GPIOA->ODR ^= GPIO_ODR_OD0; // Đảo trạng thái PA0
        }
}
void nhayleddo(){
        // 7. Kiểm tra cờ TIM3 và nháy LED PB7
        if (TIM4->SR & TIM_SR_UIF) { // Kiểm tra update interrupt flag của TIM3
            TIM4->SR &= ~TIM_SR_UIF; // Xóa cờ
            GPIOB->ODR ^= GPIO_ODR_OD7; // Đảo trạng thái PB7

    }
}
