# include "motor.h"
void motor_Init(void) {
    // Kích hoạt clock cho GPIOA
    RCC->AHB1ENR |= (1 << 0); // RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Đặt PA6 làm chức năng thay thế (AF mode)
    GPIOA->MODER &= ~(3 << (6 * 2)); // Xóa bit mode của PA6
    GPIOA->MODER |= (2 << (6 * 2));  // Đặt PA6 vào chế độ Alternate Function (AF)

    // Đặt Alternate Function cho PA6 là AF2 (TIM3_CH1)
    GPIOA->AFR[0] &= ~(0xF << (6 * 4)); // Xóa giá trị AF cũ của PA6
    GPIOA->AFR[0] |= (2 << (6 * 4));    // Gán AF2 cho PA6
}

void TIM3_PWM_Init(void) {
    // Kích hoạt clock cho TIM3
    RCC->APB1ENR |= (1 << 1); // RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Cấu hình prescaler để giảm xung clock xuống 1 kHz (1 ms chu kỳ)
    TIM3->PSC = 16 - 1; // 16 MHz / 16 = 1 MHz (tần số timer)

    // Đặt giá trị auto-reload để tạo PWM với chu kỳ 1 ms
    TIM3->ARR = 1000 - 1; // Giá trị ARR = 1000 (1 kHz PWM)

    // Đặt giá trị ban đầu cho compare register (duty cycle = 0%)
    TIM3->CCR1 = 800; // Bắt đầu với duty cycle 0%

    // Cấu hình PWM mode 1 trên TIM3 Channel 1
    TIM3->CCMR1 &= ~(0x7 << 4); // Xóa OC1M bits
    TIM3->CCMR1 |= (6 << 4);    // OC1M: PWM mode 1 (110)
    TIM3->CCMR1 |= (1 << 3);    // Cho phép preload (OC1PE)

    // Kích hoạt kênh TIM3 Channel 1 output
    TIM3->CCER |= (1 << 0); // CC1E: Kích hoạt output channel 1

    // Bật counter của TIM3
    TIM3->CR1 |= (1 << 0); // CEN: Kích hoạt bộ đếm
}

