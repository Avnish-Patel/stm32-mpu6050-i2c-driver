/*
 * mpu6050.c
 *
 *  Created on: Jul 19, 2026
 *      Author: dev
 */

#include "mpu6050.h"
#include "main.h"



static MPU6050_Status_t MPU6050_WriteByte(mpu6050_t *hmpu6050,
		                              uint8_t reg_addr , uint8_t value){

  if(HAL_I2C_Mem_Write(hmpu6050->hi2c, hmpu6050->Address, reg_addr,
		                     I2C_MEMADD_SIZE_8BIT, &value, 1, MPU6050_I2C_TIMEOUT) != HAL_OK){
	  return MPU6050_ERROR;
  }

  return MPU6050_OK;
}

static MPU6050_Status_t MPU6050_WriteBytes(mpu6050_t *hmpu6050 ,uint8_t reg_addr,
		                                      uint8_t len, uint8_t* data){

	 if(HAL_I2C_Mem_Write(hmpu6050->hi2c, hmpu6050->Address, reg_addr,
             I2C_MEMADD_SIZE_8BIT, data, len , MPU6050_I2C_TIMEOUT) != HAL_OK){
		  return MPU6050_ERROR;
	  }

	  return MPU6050_OK;

}

static MPU6050_Status_t MPU6050_ReadByte(mpu6050_t *hmpu6050,
		                                  uint8_t reg_addr ,uint8_t* data ){

      if(HAL_I2C_Mem_Read(hmpu6050->hi2c, hmpu6050->Address,reg_addr,
		                   I2C_MEMADD_SIZE_8BIT, data, 1, MPU6050_I2C_TIMEOUT)!=HAL_OK){
    	  return MPU6050_ERROR;
     }
      return MPU6050_OK;
}

static MPU6050_Status_t MPU6050_ReadBytes(mpu6050_t *hmpu6050, uint8_t reg_addr,
		                                    uint8_t len ,uint8_t* data ){

            if( HAL_I2C_Mem_Read(hmpu6050->hi2c,hmpu6050->Address ,reg_addr,
		                     I2C_MEMADD_SIZE_8BIT,data,len, MPU6050_I2C_TIMEOUT)!=HAL_OK){
	            return MPU6050_ERROR;
         }
           return MPU6050_OK;
}

static MPU6050_Status_t MPU6050_SetDLPF(mpu6050_t *hmpu6050, MPU6050_DLPF_t dlpf){

	uint8_t reg;

	if (MPU6050_ReadByte(hmpu6050, MPU6050_CONFIG, &reg) != MPU6050_OK)
	    return MPU6050_ERROR;

	reg &= ~0x07;          // Clear DLPF bits
	reg |= (uint8_t)dlpf;  // Set only DLPF bits

	if (MPU6050_WriteByte(hmpu6050, MPU6050_CONFIG, reg) != MPU6050_OK)
	    return MPU6050_ERROR;

        if (dlpf == MPU6050_DLPF_260HZ || dlpf == MPU6050_DLPF_RESERVED)
        {
            hmpu6050->gyro_output_rate_hz = 8000;
            return MPU6050_OK;
        }
        else
        {
            hmpu6050->gyro_output_rate_hz = 1000;
            return MPU6050_OK;
        }
}


static MPU6050_Status_t MPU6050_SetSampleRate(mpu6050_t *hmpu6050, uint16_t sample_rate){

    uint8_t divider;
    if ((sample_rate == 0) ||
        (sample_rate > hmpu6050->gyro_output_rate_hz))
    {
        return MPU6050_INVALID_PARAM;
    }

    // Sample Rate = Gyro Output Rate / (1 + SMPLRT_DIV)
    divider = (hmpu6050->gyro_output_rate_hz / sample_rate) - 1;

    if(MPU6050_WriteByte(hmpu6050, MPU6050_SMPLRT_DIV, divider)!=MPU6050_OK){
    	return MPU6050_ERROR;
    }
    return MPU6050_OK;
}


static MPU6050_Status_t MPU6050_SetGyroSensitivity(mpu6050_t *hmpu6050 ,
		                                       MPU6050_GyroRange_t gyro_sens){

	if(MPU6050_WriteByte( hmpu6050, MPU6050_GYRO_CONFIG, gyro_sens)== MPU6050_OK){

		switch(gyro_sens)
			{
			case MPU6050_GYRO_250DPS:
			    hmpu6050->gyro_scale_factor = 131.0f;
			    break;

			case MPU6050_GYRO_500DPS:
			    hmpu6050->gyro_scale_factor = 65.5f;
			    break;

			case MPU6050_GYRO_1000DPS:
			    hmpu6050->gyro_scale_factor = 32.8f;
			    break;

			case MPU6050_GYRO_2000DPS:
			    hmpu6050->gyro_scale_factor = 16.4f;
			    break;

			default:
			    return MPU6050_INVALID_PARAM;
			}
		return MPU6050_OK;
	}
	return MPU6050_ERROR;
}


static MPU6050_Status_t MPU6050_SetAccelSensitivity(mpu6050_t *hmpu6050 ,
		                                   MPU6050_AccelRange_t accel_sens){

	if(MPU6050_WriteByte(hmpu6050, MPU6050_ACCEL_CONFIG, accel_sens)== MPU6050_OK){

		switch(accel_sens)
				{
				case MPU6050_ACCEL_2G:
				    hmpu6050->accel_scale_factor = 16384.0f;
				    break;

				case MPU6050_ACCEL_4G:
				    hmpu6050->accel_scale_factor = 8192.0f;
				    break;

				case MPU6050_ACCEL_8G:
				    hmpu6050->accel_scale_factor = 4096.0f;
				    break;

				case MPU6050_ACCEL_16G:
				    hmpu6050->accel_scale_factor = 2048.0f;
				    break;

				default:
				    return MPU6050_INVALID_PARAM;
				}
		return MPU6050_OK;
	}
	return MPU6050_ERROR;
}

 MPU6050_Status_t MPU6050_WakeUp(mpu6050_t *hmpu6050){

	uint8_t reg;

	if(MPU6050_ReadByte(hmpu6050, MPU6050_PWR_MGMT_1, &reg)!=MPU6050_OK)
		return MPU6050_ERROR;

	reg &= ~MPU6050_CLKSEL_MASK;
	reg &= ~(1U << MPU6050_SLEEP_BIT);       //wake up
	reg |= MPU6050_CLOCK_PLL_XGYRO;

	if(MPU6050_WriteByte(hmpu6050, MPU6050_PWR_MGMT_1, reg)!=MPU6050_OK)
		return MPU6050_ERROR;

	return MPU6050_OK;
}


 MPU6050_Status_t MPU6050_Sleep(mpu6050_t *hmpu6050){

	uint8_t reg;

	if(MPU6050_ReadByte(hmpu6050, MPU6050_PWR_MGMT_1, &reg)!=MPU6050_OK)
			return MPU6050_ERROR;

	reg |= (1U << MPU6050_SLEEP_BIT);      //sleep

	if(MPU6050_WriteByte(hmpu6050, MPU6050_PWR_MGMT_1, reg)!=MPU6050_OK)
			return MPU6050_ERROR;

	return MPU6050_OK;
}

MPU6050_Status_t MPU6050_Reset(mpu6050_t *hmpu6050){
    uint8_t reg;

    if (MPU6050_ReadByte(hmpu6050, MPU6050_PWR_MGMT_1, &reg) != MPU6050_OK)
    {
        return MPU6050_ERROR;
    }

    reg |= (1U << MPU6050_DEVICE_RESET_BIT);

    if (MPU6050_WriteByte(hmpu6050, MPU6050_PWR_MGMT_1, reg) != MPU6050_OK)
    {
        return MPU6050_ERROR;
    }

    return MPU6050_OK;
}

MPU6050_Status_t MPU6050_EnableDataReadyInterrupt(mpu6050_t *hmpu6050){

	uint8_t irq_pin_conf=0;
	uint8_t enable_irq=0;

	if(MPU6050_ReadByte(hmpu6050, MPU6050_INT_ENABLE, &enable_irq)!=MPU6050_OK){
			return MPU6050_ERROR;
		}
	enable_irq |=0x01;      // DATA_RDY_EN

	if(MPU6050_WriteByte(hmpu6050, MPU6050_INT_ENABLE, enable_irq)!=MPU6050_OK){
		return MPU6050_ERROR;
	}

	if(MPU6050_ReadByte(hmpu6050, MPU6050_INT_PIN_CFG, &irq_pin_conf)!=MPU6050_OK){
		return MPU6050_ERROR;
	}

	irq_pin_conf |= 0x30;         // LATCH_INT_EN | INT_RD_CLEAR
	if(MPU6050_WriteByte(hmpu6050, MPU6050_INT_PIN_CFG, irq_pin_conf)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	return MPU6050_OK;
}

MPU6050_Status_t MPU6050_DisableInterrupt(mpu6050_t *hmpu6050){

	uint8_t irq_enable_reg =0;

	if(MPU6050_ReadByte(hmpu6050, MPU6050_INT_ENABLE, &irq_enable_reg)!=MPU6050_OK){
				return MPU6050_ERROR;
			}
		irq_enable_reg &=~(0x01);      // DATA_RDY_EN

		if(MPU6050_WriteByte(hmpu6050, MPU6050_INT_ENABLE, irq_enable_reg)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	return MPU6050_OK;
}


MPU6050_Status_t MPU6050_CalibrateGyro(mpu6050_t *hmpu6050 , uint16_t Samples) {
    int32_t raw_gx = 0, raw_gy = 0, raw_gz = 0;
    uint8_t rec_data[6];

    if (Samples == 0) {
        return MPU6050_INVALID_PARAM;
    }

    for (uint16_t i = 0; i < Samples; i++) {
        if(MPU6050_ReadBytes(hmpu6050, MPU6050_GYRO_XOUT_H, 6, rec_data)!=MPU6050_OK){
        	return MPU6050_ERROR;
        }

        raw_gx += (int16_t)((rec_data[0] << 8 )| rec_data[1]);
        raw_gy += (int16_t)((rec_data[2] << 8 )| rec_data[3]);
        raw_gz += (int16_t)((rec_data[4] << 8 )| rec_data[5]);

        HAL_Delay(2);
    }

    hmpu6050->Gyro_X_Offset = (raw_gx / (float)Samples) ;
    hmpu6050->Gyro_Y_Offset = (raw_gy / (float)Samples) ;
    hmpu6050->Gyro_Z_Offset = (raw_gz / (float)Samples) ;

    return MPU6050_OK;
}


MPU6050_Status_t MPU6050_Init(mpu6050_t *hmpu6050,
		                      I2C_HandleTypeDef *hi2c, MPU6050_Config_t *config){


	if (hmpu6050 == NULL || hi2c == NULL || config == NULL){
	    return MPU6050_INVALID_PARAM;
	}


	hmpu6050->hi2c = hi2c;
	hmpu6050->Address=MPU6050_ADDR;
	uint8_t who_am_i;

	if (MPU6050_ReadByte(hmpu6050, MPU6050_WHO_AM_I, &who_am_i) != MPU6050_OK){
	    return MPU6050_ERROR;
	}

	if (who_am_i != MPU6050_WHO_AM_I_VALUE){

	    return MPU6050_DEVICE_NOT_FOUND;
	}

	if (MPU6050_WakeUp(hmpu6050) != MPU6050_OK){
	    return MPU6050_ERROR;
	}

	if(MPU6050_SetDLPF(hmpu6050, config->dlpf)!=MPU6050_OK){
		return MPU6050_ERROR;
	}

	if(MPU6050_SetSampleRate(hmpu6050, config->sample_rate)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	if(MPU6050_SetGyroSensitivity(hmpu6050, config->gyro_range)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	if(MPU6050_SetAccelSensitivity(hmpu6050, config->accel_range)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	return MPU6050_OK;
}


 MPU6050_Status_t MPU6050_ReadGyro(mpu6050_t *hmpu6050){

	uint8_t gyro_raw_data[6];


	if(MPU6050_ReadBytes(hmpu6050, MPU6050_GYRO_XOUT_H, 6, gyro_raw_data)!=MPU6050_OK){
		return MPU6050_ERROR;
	}



	hmpu6050->gyro_x_raw=((int16_t)gyro_raw_data[0] << 8)|(int16_t)gyro_raw_data[1];
	hmpu6050->gyro_y_raw=((int16_t)gyro_raw_data[2] << 8)|(int16_t)gyro_raw_data[3];
	hmpu6050->gyro_z_raw=((int16_t)gyro_raw_data[4] << 8)|(int16_t)gyro_raw_data[5];

	hmpu6050->gyro_x =((float)hmpu6050->gyro_x_raw-hmpu6050->Gyro_X_Offset)/hmpu6050->gyro_scale_factor;
	hmpu6050->gyro_y =((float)hmpu6050->gyro_y_raw-hmpu6050->Gyro_Y_Offset)/hmpu6050->gyro_scale_factor;
	hmpu6050->gyro_z =((float)hmpu6050->gyro_z_raw-hmpu6050->Gyro_Z_Offset)/hmpu6050->gyro_scale_factor;

	return MPU6050_OK;
}


 MPU6050_Status_t MPU6050_ReadAccel(mpu6050_t *hmpu6050){

	uint8_t accel_raw_data[6];


	if(MPU6050_ReadBytes(hmpu6050, MPU6050_ACCEL_XOUT_H, 6, accel_raw_data)!=MPU6050_OK){
		return MPU6050_ERROR;
	}

	hmpu6050->accel_x_raw=((int16_t)accel_raw_data[0] << 8)|(int16_t)accel_raw_data[1];
	hmpu6050->accel_y_raw=((int16_t)accel_raw_data[2] << 8)|(int16_t)accel_raw_data[3];
	hmpu6050->accel_z_raw=((int16_t)accel_raw_data[4] << 8)|(int16_t)accel_raw_data[5];

	hmpu6050->accel_x = (float)hmpu6050->accel_x_raw /hmpu6050->accel_scale_factor;
	hmpu6050->accel_y = (float)hmpu6050->accel_y_raw /hmpu6050->accel_scale_factor;
	hmpu6050->accel_z = (float)hmpu6050->accel_z_raw /hmpu6050->accel_scale_factor;

	return MPU6050_OK;
}


 MPU6050_Status_t MPU6050_ReadTemperature(mpu6050_t *hmpu6050){

	uint8_t temp_raw_data[2]={0,0};
	int16_t temp_raw;

	if(MPU6050_ReadBytes(hmpu6050, MPU6050_TEMP_OUT_H, 2, temp_raw_data)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	temp_raw = (((int16_t)temp_raw_data[0])<<8)|(int16_t)temp_raw_data[1];

	hmpu6050->temperature= ((float)temp_raw/340.0f)+36.53f;

	return MPU6050_OK;
}


MPU6050_Status_t MPU6050_ReadAll(mpu6050_t *hmpu6050){
    if (MPU6050_ReadAccel(hmpu6050) != MPU6050_OK)
        return MPU6050_ERROR;

    if (MPU6050_ReadTemperature(hmpu6050) != MPU6050_OK)
        return MPU6050_ERROR;

    if (MPU6050_ReadGyro(hmpu6050) != MPU6050_OK)
        return MPU6050_ERROR;

    return MPU6050_OK;
}

MPU6050_Status_t MPU6050_ReadAllBrust(mpu6050_t *hmpu6050){

	uint8_t raw_data[14];
	int16_t temp_raw;

	if(MPU6050_ReadBytes(hmpu6050, MPU6050_ACCEL_XOUT_H, 14,raw_data)!=MPU6050_OK){
			return MPU6050_ERROR;
		}

	    temp_raw = (((int16_t)raw_data[6])<<8)|(int16_t)raw_data[7];

		hmpu6050->temperature= ((float)temp_raw/340.0f)+36.53f;

		hmpu6050->accel_x_raw=((int16_t)raw_data[0] << 8)|(int16_t)raw_data[1];
		hmpu6050->accel_y_raw=((int16_t)raw_data[2] << 8)|(int16_t)raw_data[3];
		hmpu6050->accel_z_raw=((int16_t)raw_data[4] << 8)|(int16_t)raw_data[5];

		hmpu6050->accel_x = (float)hmpu6050->accel_x_raw /hmpu6050->accel_scale_factor;
		hmpu6050->accel_y = (float)hmpu6050->accel_y_raw /hmpu6050->accel_scale_factor;
		hmpu6050->accel_z = (float)hmpu6050->accel_z_raw /hmpu6050->accel_scale_factor;

		hmpu6050->gyro_x_raw=((int16_t)raw_data[8] << 8)|(int16_t)raw_data[9];
		hmpu6050->gyro_y_raw=((int16_t)raw_data[10] << 8)|(int16_t)raw_data[11];
		hmpu6050->gyro_z_raw=((int16_t)raw_data[12] << 8)|(int16_t)raw_data[13];

		hmpu6050->gyro_x = ((float)hmpu6050->gyro_x_raw - hmpu6050->Gyro_X_Offset) /hmpu6050->gyro_scale_factor;
		hmpu6050->gyro_y = ((float)hmpu6050->gyro_y_raw - hmpu6050->Gyro_Y_Offset)/hmpu6050->gyro_scale_factor;
		hmpu6050->gyro_z = ((float)hmpu6050->gyro_z_raw - hmpu6050->Gyro_Z_Offset) /hmpu6050->gyro_scale_factor;

		return MPU6050_OK;
}
