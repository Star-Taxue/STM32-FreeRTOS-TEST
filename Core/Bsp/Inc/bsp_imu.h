/**
 * @file    bsp_imu.h
 * @brief   IMU姿态解算BSP层
 * @author  STM32 Team
 * @date    2026-04-09
 * @version V1.0
 */

#ifndef BSP_IMU_H
#define BSP_IMU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

typedef struct {
    float roll;
    float pitch;
    float yaw;
} euler_angle_t;

typedef struct {
    euler_angle_t angle;
    float alpha;
    uint32_t last_update;
} complementary_filter_t;

/* Exported functions prototypes ---------------------------------------------*/

void BSP_IMU_Init(void);
void BSP_IMU_Update(float *accel, float *gyro, float dt);
void BSP_IMU_GetAngle(euler_angle_t *angle);

void BSP_ComplementaryFilter_Init(complementary_filter_t *filter, float alpha);
void BSP_ComplementaryFilter_Update(complementary_filter_t *filter,
                                    float *accel, float *gyro, float dt);

#ifdef __cplusplus
}
#endif

#endif /* BSP_IMU_H */
