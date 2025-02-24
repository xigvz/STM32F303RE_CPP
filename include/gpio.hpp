#ifndef GPIO_HPP
#define GPIO_HPP

#include <stdint.h>

// src: stm32f303xe

typedef struct {
    volatile uint32_t MODER; // offset 0x00: mode register
    volatile uint32_t OTYPER; // offset 0x04: output type register
    volatile uint32_t OSPEEDR; // offset 0x08: output speed register
    volatile uint32_t PUPDR; // offset 0x0C: pull-up/pull-down register
    volatile uint32_t IDR; // offset 0x10: input data register
    volatile uint32_t ODR; // offset 0x14: output data register
    volatile uint32_t BSRR; // offset 0x1A: bit set/reset register
    volatile uint32_t LCKR; // offset 0x1C: configuration lock register
    volatile uint32_t AFR[2]; // offset 0x20-0x24: alternate function registers
    volatile uint32_t BRR; // offset 0x28: bit reset register (F303RE)
} MY_GPIO_TypeDef;

// table 3: boundary addresses
#define MY_GPIOA ((MY_GPIO_TypeDef *)0x48000000)
#define MY_GPIOB ((MY_GPIO_TypeDef *)0x48000400)
#define MY_GPIOC ((MY_GPIO_TypeDef *)0x48000800)
#define MY_GPIOD ((MY_GPIO_TypeDef *)0x48000C00)
#define MY_GPIOE ((MY_GPIO_TypeDef *)0x48001000)
#define MY_GPIOF ((MY_GPIO_TypeDef *)0x48001400)

// base: 0x40021000, AHBENR offset: 0x14 = 0x40021014
#define MY_RCC_AHBENR (*((volatile uint32_t *)0x40021014))

typedef enum {
    GPIO_MODE_INPUT = 0x00,
    GPIO_MODE_OUTPUT = 0x01,
    GPIO_MODE_AF = 0x02,
    GPIO_MODE_ANALOG = 0x03
} GPIOMode_TypeDef;

typedef enum {
    GPIO_OTYPE_PP = 0x00,
    GPIO_OTYPE_OD = 0x01
} GPIOOutputType_TypeDef;

typedef enum {
    GPIO_OSPEED_LOW = 0x00,
    GPIO_OSPEED_MEDIUM = 0x01,
    GPIO_OSPEED_HIGH = 0x02
} GPIOSpeed_TypeDef;

typedef enum {
    GPIO_PUPD_NONE = 0x00,
    GPIO_PUPD_PU = 0x01,
    GPIO_PUPD_PD = 0x02
} GPIOPull_TypeDef;

class GPIO {
public:
    GPIO(MY_GPIO_TypeDef *port, uint8_t pin);
    GPIO(MY_GPIO_TypeDef *port, uint8_t pin, GPIOMode_TypeDef mode,
         GPIOPull_TypeDef pull, GPIOOutputType_TypeDef output_type);
    GPIO(MY_GPIO_TypeDef *port, uint8_t pin, GPIOMode_TypeDef mode,
         GPIOPull_TypeDef pull, GPIOOutputType_TypeDef output_type,
         GPIOSpeed_TypeDef speed, uint8_t alternate_function);

    void setMode(GPIOMode_TypeDef mode);
    void setOutputType(GPIOOutputType_TypeDef otype);
    void setOutputSpeed(GPIOSpeed_TypeDef ospeed);
    void setPull(GPIOPull_TypeDef pull);
    void writePin(bool value);
    bool readPin();
    void togglePin();
    void setAlternateFunction(uint8_t alternate_function);

private:
    MY_GPIO_TypeDef *port_;
    uint8_t pin_;

    void enableClock();
    void resetGPIO();
    void configure(GPIOMode_TypeDef mode, GPIOPull_TypeDef pull,
                   GPIOOutputType_TypeDef output_type, GPIOSpeed_TypeDef speed,
                   uint8_t alternate_function);
};
#endif
