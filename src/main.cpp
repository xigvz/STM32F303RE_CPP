#include "gpio.hpp"
#include "stm32f303xe.h"
#include <cstdio>
#include <cstring>

// onboard
#define LED0_PORT MY_GPIOA
#define LED0_PIN 5 // PA5
#define B0_PORT MY_GPIOC
#define B0_PIN 13 // PC13

// external
#define LED1_PORT MY_GPIOA
#define LED1_PIN 6 // PA6
#define B1_PORT MY_GPIOC
#define B1_PIN 0 // PB0

void SystemClock_Config();
void USART2_Init();
void USART2_SendString(const char *str);
void Delay(uint32_t time_ms);
constexpr int BUFLENGTH = 100;
char sndBuf[BUFLENGTH];

GPIO LED0(LED0_PORT, LED0_PIN, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP);
GPIO BTN0(B0_PORT, B0_PIN, GPIO_MODE_INPUT, GPIO_PUPD_PU, GPIO_OTYPE_PP);

volatile bool buttonPressed = false;
volatile bool ledState = false;

int main(void) {
    SystemClock_Config();
    USART2_Init();

    LED0.writePin(ledState);

    snprintf(sndBuf, BUFLENGTH, "Init success\r\n");
    USART2_SendString(sndBuf);

    while (1) {
        bool buttonState = BTN0.readPin();

        if (!buttonState && !buttonPressed) {
            ledState = !ledState;
            LED0.writePin(ledState);
            buttonPressed = true;
            Delay(50);
        } else if (buttonState && buttonPressed) {
            buttonPressed = false;
        }
    }
}


void SystemClock_Config(void) {
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY))
        ;

    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_PREDIV;

    RCC->CFGR &= ~RCC_CFGR_PLLMUL;
    RCC->CFGR |= RCC_CFGR_PLLMUL9;

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;

    SystemCoreClockUpdate();
}

void USART2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    GPIOA->MODER |= (0b10 << GPIO_MODER_MODER2_Pos) | (0b10 << GPIO_MODER_MODER3_Pos);
    GPIOA->AFR[0] |= (0b0111 << GPIO_AFRL_AFRL2_Pos) | (0b0111 << GPIO_AFRL_AFRL3_Pos);
    USART2->BRR = 36000000 / 9600;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void USART2_SendString(const char *str) {
    while (*str) {
        while (!(USART2->ISR & USART_ISR_TXE))
            ;
        USART2->TDR = *str++;
    }
}

void Delay(uint32_t time_ms) {
    for (volatile uint32_t i = 0; i < time_ms; i++)
        for (volatile uint32_t j = 0; j < 3225; j++);
}
