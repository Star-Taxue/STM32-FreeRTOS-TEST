#ifndef DRV_SPI_H
#define DRV_SPI_H

#include "main.h"
#include "spi.h"

typedef enum {
    DRV_SPI_OK = 0,
    DRV_SPI_ERROR,
    DRV_SPI_TIMEOUT
} DRV_SPI_Status_t;

#define DRV_SPI_TIMEOUT_MS  100

void DRV_SPI1_ACC_CS_Ctrl(uint8_t state);
void DRV_SPI1_GYRO_CS_Ctrl(uint8_t state);
uint8_t DRV_SPI1_ReadByte(uint8_t reg);
DRV_SPI_Status_t DRV_SPI1_WriteByte(uint8_t reg, uint8_t data);
DRV_SPI_Status_t DRV_SPI1_ReadBuffer(uint8_t reg, uint8_t *buffer, uint16_t len);
DRV_SPI_Status_t DRV_SPI1_WriteBuffer(uint8_t reg, uint8_t *buffer, uint16_t len);

#endif /* DRV_SPI_H */
