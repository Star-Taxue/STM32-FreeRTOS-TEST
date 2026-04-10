/**
 * @file    bsp_imu.c
 * @brief   IMU姿态解算实现 - 互补滤波算法
 * @author  STM32 Team
 * @date    2026-04-09
 * @version V1.0
 */

#include "Inc/bsp_imu.h"
#include <math.h>
#include "stm32f4xx_hal.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static complementary_filter_t imu_filter;

void BSP_IMU_Init(void)
{
    BSP_ComplementaryFilter_Init(&imu_filter, 0.98f);
}

void BSP_ComplementaryFilter_Init(complementary_filter_t *filter, float alpha)
{
    filter->angle.roll = 0.0f;
    filter->angle.pitch = 0.0f;
    filter->angle.yaw = 0.0f;
    filter->alpha = alpha;
    filter->last_update = HAL_GetTick();
}

void BSP_IMU_Update(float *accel, float *gyro, float dt)
{
    BSP_ComplementaryFilter_Update(&imu_filter, accel, gyro, dt);
}

void BSP_ComplementaryFilter_Update(complementary_filter_t *filter,
                                    float *accel, float *gyro, float dt)
{
    static float accel_filtered[3] = {0};
    float alpha = 0.8f;
    
    for (int i = 0; i < 3; i++) {
        accel_filtered[i] = alpha * accel_filtered[i] + (1 - alpha) * accel[i];
    }
    
    float accel_magnitude = sqrtf(accel_filtered[0] * accel_filtered[0] +
                                  accel_filtered[1] * accel_filtered[1] +
                                  accel_filtered[2] * accel_filtered[2]);
    
    float accel_roll, accel_pitch;
    
    if (fabsf(accel_magnitude - 1.0f) < 0.3f) {
        accel_roll = atan2f(accel_filtered[1], accel_filtered[2]) * 180.0f / M_PI;
        accel_pitch = atan2f(-accel_filtered[0],
                             sqrtf(accel_filtered[1] * accel_filtered[1] +
                                   accel_filtered[2] * accel_filtered[2])) * 180.0f / M_PI;
    } else {
        accel_roll = filter->angle.roll;
        accel_pitch = filter->angle.pitch;
    }
    
    float gyro_roll = filter->angle.roll + gyro[0] * dt;
    float gyro_pitch = filter->angle.pitch + gyro[1] * dt;
    float gyro_yaw = filter->angle.yaw + gyro[2] * dt;
    
    float effective_alpha = (fabsf(accel_magnitude - 1.0f) < 0.3f) ? filter->alpha : 0.99f;
    
    filter->angle.roll = effective_alpha * gyro_roll + (1.0f - effective_alpha) * accel_roll;
    filter->angle.pitch = effective_alpha * gyro_pitch + (1.0f - effective_alpha) * accel_pitch;
    filter->angle.yaw = gyro_yaw;
    
    if (filter->angle.roll > 180.0f) filter->angle.roll -= 360.0f;
    if (filter->angle.roll < -180.0f) filter->angle.roll += 360.0f;
    if (filter->angle.pitch > 180.0f) filter->angle.pitch -= 360.0f;
    if (filter->angle.pitch < -180.0f) filter->angle.pitch += 360.0f;
    if (filter->angle.yaw > 180.0f) filter->angle.yaw -= 360.0f;
    if (filter->angle.yaw < -180.0f) filter->angle.yaw += 360.0f;
}

void BSP_IMU_GetAngle(euler_angle_t *angle)
{
    angle->roll = imu_filter.angle.roll;
    angle->pitch = imu_filter.angle.pitch;
    angle->yaw = imu_filter.angle.yaw;
}
