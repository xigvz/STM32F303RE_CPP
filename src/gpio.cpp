#include "gpio.hpp"

// constructors
GPIO::GPIO(MY_GPIO_TypeDef *port, uint8_t pin) : port_(port), pin_(pin) {}

GPIO::GPIO(MY_GPIO_TypeDef *port, uint8_t pin, GPIOMode_TypeDef mode,
           GPIOPull_TypeDef pull, GPIOOutputType_TypeDef output_type)
    : port_(port), pin_(pin) {
  configure(mode, pull, output_type, GPIO_OSPEED_LOW, 0); // default speed, no af
}

GPIO::GPIO(MY_GPIO_TypeDef *port, uint8_t pin, GPIOMode_TypeDef mode,
           GPIOPull_TypeDef pull, GPIOOutputType_TypeDef output_type,
           GPIOSpeed_TypeDef speed, uint8_t alternate_function)
    : port_(port), pin_(pin) {
  configure(mode, pull, output_type, speed, alternate_function);
}

void GPIO::configure(GPIOMode_TypeDef mode, GPIOPull_TypeDef pull,
                      GPIOOutputType_TypeDef output_type, GPIOSpeed_TypeDef speed,
                      uint8_t alternate_function) {
  enableClock();
  resetGPIO();

  port_->MODER |= (mode << (pin_ * 2));
  port_->OSPEEDR |= (speed << (pin_ * 2));
  port_->PUPDR |= (pull << (pin_ * 2));
  port_->OTYPER |= (output_type << pin_);

  setAlternateFunction(alternate_function);
}

void GPIO::setMode(GPIOMode_TypeDef mode) {
  port_->MODER &= ~(0x03UL << (pin_ * 2));
  port_->MODER |= (mode << (pin_ * 2));
}

void GPIO::setOutputType(GPIOOutputType_TypeDef otype) {
  port_->OTYPER &= ~(0x01UL << pin_);
  port_->OTYPER |= (otype << pin_);
}

void GPIO::setOutputSpeed(GPIOSpeed_TypeDef ospeed) {
  port_->OSPEEDR &= ~(0x03UL << (pin_ * 2));
  port_->OSPEEDR |= (ospeed << (pin_ * 2));
}

void GPIO::setPull(GPIOPull_TypeDef pull) {
  port_->PUPDR &= ~(0x03UL << (pin_ * 2));
  port_->PUPDR |= (pull << (pin_ * 2));
}

void GPIO::writePin(bool value) {
  if (value) {
    port_->BSRR = (1 << pin_); // set
  } else {
    port_->BRR = (1 << pin_); // reset
  }
}

bool GPIO::readPin() { return (port_->IDR & (1 << pin_)) != 0; }

void GPIO::togglePin() { port_->ODR ^= (1 << pin_); }

void GPIO::setAlternateFunction(uint8_t alternate_function) {
  uint8_t pos = ((pin_ & 0x7) << 2);
  port_->AFR[pin_ >> 3] &= ~(0xf << pos); // clear
  port_->AFR[pin_ >> 3] |=
      ((alternate_function & 0xf) << pos); // set
}

void GPIO::enableClock() {  //TODO: find a way to fix this scuffed crap
  if (port_ == MY_GPIOA) {
    MY_RCC_AHBENR |= (1 << 17);
    volatile uint32_t temp = MY_RCC_AHBENR;
    uint32_t isuckatdebugging = 0;
  } else if (port_ == MY_GPIOB) {
    MY_RCC_AHBENR |= (1 << 18);
  } else if (port_ == MY_GPIOC) {
    MY_RCC_AHBENR |= (1 << 19);
  } else if (port_ == MY_GPIOD) {
    MY_RCC_AHBENR |= (1 << 20);
  } else if (port_ == MY_GPIOE) {
    MY_RCC_AHBENR |= (1 << 21);
  } else if (port_ == MY_GPIOF) {
    MY_RCC_AHBENR |= (1 << 22);
  }
}

void GPIO::resetGPIO() {
    port_->MODER &= ~(0x03 << (pin_ * 2));
    port_->OTYPER &= ~(0x01 << pin_);
    port_->OSPEEDR &= ~(0x03 << (pin_ * 2));
    port_->PUPDR &= ~(0x03 << (pin_ * 2));
}
