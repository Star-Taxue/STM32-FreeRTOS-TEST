#include "Inc/drv_spi.h"

void DRV_SPI1_ACC_CS_Ctrl(uint8_t state)
{
    HAL_GPIO_WritePin(ACC_CS_GPIO_Port, ACC_CS_Pin, state);
}

void DRV_SPI1_GYRO_CS_Ctrl(uint8_t state)
{
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, state);
}

/**
 * @brief  SPI1单字节读取
 */
uint8_t DRV_SPI1_ReadByte(uint8_t reg)
{
    uint8_t tx_data[2] = {(uint8_t)(reg | 0x80), 0x00};  /* 读命令 + 哑字节 */
    uint8_t rx_data[2] = {0};
    
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, DRV_SPI_TIMEOUT_MS);
    
    return rx_data[1];  /* 第二个字节是有效数据 */
}

/**
 * @brief  SPI1单字节写入
 */
DRV_SPI_Status_t DRV_SPI1_WriteByte(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[2] = {(uint8_t)(reg & 0x7F), data};  /* 写命令 + 数据 */
    
    if (HAL_SPI_Transmit(&hspi1, tx_data, 2, DRV_SPI_TIMEOUT_MS) != HAL_OK) {
        return DRV_SPI_ERROR;
    }
    
    return DRV_SPI_OK;
}

/**
 * @brief  SPI1多字节读取
 */
DRV_SPI_Status_t DRV_SPI1_ReadBuffer(uint8_t reg, uint8_t *buffer, uint16_t len)
{
    uint8_t tx_cmd = reg | 0x80;  /* 读命令 */
    
    /* 先发送读命令 */
    if (HAL_SPI_Transmit(&hspi1, &tx_cmd, 1, DRV_SPI_TIMEOUT_MS) != HAL_OK) {
        return DRV_SPI_ERROR;
    }
    
    /* 再接收数据 */
    if (HAL_SPI_Receive(&hspi1, buffer, len, DRV_SPI_TIMEOUT_MS) != HAL_OK) {
        return DRV_SPI_ERROR;
    }
    
    return DRV_SPI_OK;
}

/**
 * @brief  SPI1多字节写入
 */
DRV_SPI_Status_t DRV_SPI1_WriteBuffer(uint8_t reg, uint8_t *buffer, uint16_t len)
{
    uint8_t tx_cmd = reg & 0x7F;  /* 写命令 */
    
    /* 先发送写命令 */
    if (HAL_SPI_Transmit(&hspi1, &tx_cmd, 1, DRV_SPI_TIMEOUT_MS) != HAL_OK) {
        return DRV_SPI_ERROR;
    }
    
    /* 再发送数据 */
    if (HAL_SPI_Transmit(&hspi1, buffer, len, DRV_SPI_TIMEOUT_MS) != HAL_OK) {
        return DRV_SPI_ERROR;
    }
    
    return DRV_SPI_OK;
}


