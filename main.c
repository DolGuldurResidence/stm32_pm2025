#include <stdint.h>
#include <stm32f10x.h>

void delay(uint32_t ticks) {
    for (uint32_t i=0; i<ticks; i++) {
        __NOP();
    }
}

int __attribute((noreturn)) main(void) {
    // Включаем тактирование
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // PC13 - выход push-pull
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    // PA0 - вход с pull-up
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // CNF0[1:0]=10 (Input with pull-up/down), MODE0=00 (input)
    GPIOA->ODR |= (1U << 0); // подтяжка к питанию

    uint32_t delays[] = { 50000, 200000, 1000000, 4000000 };
    int freq_count = sizeof(delays)/sizeof(delays[0]);
    int freq_idx = 0;

    int prev_button = 1;

    while (1) {
        // Чтение кнопки (активна при нажатии на GND, то есть LOW)
        int button = (GPIOA->IDR & (1U << 0)) ? 1 : 0;
        if (!button && prev_button) { // обработка фронта нажатия
            freq_idx = (freq_idx + 1) % freq_count; // переключение частоты
            delay(100000); // простая антидребезговая задержка
        }
        prev_button = button;

        // Мигание светодиодом с нужной частотой
        GPIOC->ODR |= (1U << 13);
        delay(delays[freq_idx]);
        GPIOC->ODR &= ~(1U << 13);
        delay(delays[freq_idx]);
    }
}
