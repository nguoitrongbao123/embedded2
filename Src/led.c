#include "led.h"

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
