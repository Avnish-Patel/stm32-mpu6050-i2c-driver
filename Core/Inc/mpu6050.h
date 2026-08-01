/*
 * mpu6050.h
 *
 *  Created on: Jul 8, 2026
 *      Author: dev
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define MPU6050_I2C_ADDR_LOW      0x68
#define MPU6050_I2C_ADDR_HIGH     0x69

/* Default: AD0 connected to GND */
#define MPU6050_ADDR               (MPU6050_I2C_ADDR_LOW << 1)
#define MPU6050_SMPLRT_DIV          0X19
#define MPU6050_CONFIG              0X1A
#define MPU6050_GYRO_CONFIG         0X1B
#define MPU6050_ACCEL_CONFIG        0X1C
#define MPU6050_INT_PIN_CFG         0X37
#define MPU6050_INT_ENABLE          0X38
#define MPU6050_INT_STATUS          0X3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75
#define MPU6050_WHO_AM_I_VALUE      0x68
#define MPU6050_WAKEUP              0x00
#define MPU6050_CLKSEL_MASK         0x07
#define MPU6050_SLEEP_BIT           6
#define MPU6050_DEVICE_RESET_BIT    7
#define MPU6050_INT_PORT 	        GPIOA
#define MPU6050_INT_PIN 	        GPIO_PIN_5
#define MPU6050_I2C_TIMEOUT         100
#define MPU6050_CLOCK_INTERNAL      0x00
#define MPU6050_CLOCK_PLL_XGYRO     0x01
#define MPU6050_CLOCK_PLL_YGYRO     0x02
#define MPU6050_CLOCK_PLL_ZGYRO     0x03

typedef enum{
    MPU6050_GYRO_250DPS  = ((0x00) << 3),
    MPU6050_GYRO_500DPS  = ((0x01) << 3),
    MPU6050_GYRO_1000DPS = ((0x02) << 3),
    MPU6050_GYRO_2000DPS = ((0x03) << 3)
} MPU6050_GyroRange_t;

typedef enum{
    MPU6050_ACCEL_2G  = ((0x00) << 3),
    MPU6050_ACCEL_4G  = ((0x01) << 3),
    MPU6050_ACCEL_8G  = ((0x02) << 3),
    MPU6050_ACCEL_16G = ((0x03) << 3)
} MPU6050_AccelRange_t;


typedef enum{
    MPU6050_OK = 0,
    MPU6050_ERROR,
    MPU6050_TIMEOUT,
    MPU6050_INVALID_PARAM,
    MPU6050_DEVICE_NOT_FOUND,
    MPU6050_I2C_ERROR
} MPU6050_Status_t;


typedef enum{
    MPU6050_DLPF_260HZ = 0,
    MPU6050_DLPF_184HZ,
    MPU6050_DLPF_94HZ,
    MPU6050_DLPF_44HZ,
    MPU6050_DLPF_21HZ,
    MPU6050_DLPF_10HZ,
    MPU6050_DLPF_5HZ,
    MPU6050_DLPF_RESERVED
} MPU6050_DLPF_t;


typedef struct{

	I2C_HandleTypeDef *hi2c;
	uint16_t Address;

	uint16_t gyro_output_rate_hz;
	float gyro_scale_factor;
	float accel_scale_factor;

	float Gyro_X_Offset;
	float Gyro_Y_Offset;
	float Gyro_Z_Offset;

	int16_t accel_x_raw;
	int16_t accel_y_raw;
	int16_t accel_z_raw;

	float accel_x;
	float accel_y;
	float accel_z;

	int16_t gyro_x_raw;
	int16_t gyro_y_raw;
	int16_t gyro_z_raw;

	float gyro_x;
	float gyro_y;
	float gyro_z;

	float temperature;
}mpu6050_t;


typedef struct{
	MPU6050_DLPF_t dlpf;
	uint16_t sample_rate;
	MPU6050_GyroRange_t gyro_range;
	MPU6050_AccelRange_t accel_range;
}MPU6050_Config_t;

MPU6050_Status_t MPU6050_Init(mpu6050_t *hmpu6050,
		                      I2C_HandleTypeDef *hi2c, MPU6050_Config_t *config);

MPU6050_Status_t MPU6050_ReadGyro(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_ReadAccel(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_ReadTemperature(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_EnableDataReadyInterrupt(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_WakeUp(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_Sleep(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_DisableInterrupt(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_CalibrateGyro(mpu6050_t *hmpu6050 ,uint16_t Samples);

MPU6050_Status_t MPU6050_ReadAll(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_Reset(mpu6050_t *hmpu6050);
MPU6050_Status_t MPU6050_ReadAllBrust(mpu6050_t *hmpu6050);

#endif /* INC_MPU6050_H_ */
