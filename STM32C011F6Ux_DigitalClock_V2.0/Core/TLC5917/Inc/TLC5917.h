/**
  ******************************************************************************
  * @file           : TLC5917.h
  * @brief          : Header for TLC5917 8-bit constant-current LED sink driver.
  ******************************************************************************
  */

#ifndef INC_TLC5917_H_
#define INC_TLC5917_H_

#include "main.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/**
  * @brief TLC5917 Device Handle structure definition.
  */
typedef struct
{
  SPI_HandleTypeDef *spiHandle; /**< Pointer to SPI peripheral handle */
  GPIO_TypeDef      *csPort;    /**< GPIO port connected to Latch Enable (LE) pin */
  uint16_t           csPin;     /**< GPIO pin connected to Latch Enable (LE) pin */
} TLC5917_HandleTypeDef;

/**
  * @brief  Initializes the TLC5917 driver instance with peripheral bindings.
  * @param  tlc5917:   Pointer to TLC5917 handle structure.
  * @param  spiHandle: Pointer to SPI handle used for serial data communication.
  * @param  csPort:    GPIO port connected to Latch Enable (LE) pin.
  * @param  csPin:     GPIO pin connected to Latch Enable (LE) pin.
  * @retval uint8_t:   1 on success, 0 on error.
  */
uint8_t TLC5917_Init(TLC5917_HandleTypeDef *tlc5917, SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin);

/**
  * @brief  Transmits an 8-bit segment pattern via SPI and pulses the LE pin to latch outputs.
  * @param  tlc5917: Pointer to TLC5917 handle structure.
  * @param  data:    8-bit segment output pattern.
  * @retval uint8_t: 1 if transmission and latch succeeded, 0 otherwise.
  */
uint8_t TLC5917_WriteRegister(TLC5917_HandleTypeDef *tlc5917, uint8_t data);

#endif /* INC_TLC5917_H_ */