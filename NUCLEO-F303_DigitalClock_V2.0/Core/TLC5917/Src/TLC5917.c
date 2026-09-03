/**
  ******************************************************************************
  * @file           : TLC5917.c
  * @brief          : Implementation file for TLC5917 8-bit LED sink driver.
  ******************************************************************************
  */

#include "main.h"
#include "TLC5917.h"

/**
  * @brief  Initializes the TLC5917 driver instance with peripheral bindings.
  * @param  tlc5917:   Pointer to TLC5917 handle structure.
  * @param  spiHandle: Pointer to SPI handle used for serial data communication.
  * @param  csPort:    GPIO port connected to Latch Enable (LE) pin.
  * @param  csPin:     GPIO pin connected to Latch Enable (LE) pin.
  * @retval uint8_t:   1 on success.
  */
uint8_t TLC5917_Init(TLC5917_HandleTypeDef *tlc5917, SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
{
  tlc5917->spiHandle = spiHandle;
  tlc5917->csPort    = csPort;
  tlc5917->csPin     = csPin;
  return 1;
}

/**
  * @brief  Transmits an 8-bit segment pattern via SPI and pulses LE to update outputs.
  * @param  tlc5917: Pointer to TLC5917 handle structure.
  * @param  data:    8-bit segment output pattern.
  * @retval uint8_t: 1 if transmission and latch succeeded, 0 otherwise.
  */
uint8_t TLC5917_WriteRegister(TLC5917_HandleTypeDef *tlc5917, uint8_t data)
{
  uint8_t txBuf[1] = {data};

  // Ensure Latch Enable is LOW while shifting serial data
  HAL_GPIO_WritePin(tlc5917->csPort, tlc5917->csPin, GPIO_PIN_RESET);
  
  // Direct blocking SPI transmission of the segment byte
  uint8_t status = (HAL_SPI_Transmit(tlc5917->spiHandle, txBuf, 1, HAL_MAX_DELAY) == HAL_OK);
  
  // Generate a controlled latch pulse (LOW -> HIGH -> LOW) on LE to apply data to output pins
  HAL_GPIO_WritePin(tlc5917->csPort, tlc5917->csPin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(tlc5917->csPort, tlc5917->csPin, GPIO_PIN_RESET);

  return status;
}