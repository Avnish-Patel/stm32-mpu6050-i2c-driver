# STM32 MPU6050 I2C Driver

A reusable **MPU6050 (GY-521)** driver for STM32 microcontrollers using the **STM32 HAL** library.
The driver provides APIs for initialization, sensor configuration, accelerometer, gyroscope, temperature measurement, burst reading, interrupt configuration, 
and gyroscope calibration.

---

## Features

- Device initialization
- WHO_AM_I device verification
- Accelerometer data acquisition
- Gyroscope data acquisition
- Temperature sensor reading
- Read accelerometer, gyroscope and temperature separately
- Burst read of all sensor data (14-byte transaction)
- Configurable accelerometer full-scale range
- Configurable gyroscope full-scale range
- Configurable Digital Low Pass Filter (DLPF)
- Configurable sample rate
- Sleep mode
- Wake-up mode
- Software reset
- Interrupt enable/disable (Data Ready)
- Gyroscope offset calibration
- Error handling using status codes

---

## Hardware Used

- STM32F429ZI Discovery Board
- MPU6050 (GY-521 Module)

---

## Software

- STM32CubeIDE
- STM32 HAL Driver
- Embedded C

---

## Communication Interface

- I2C

---

## Connections

| MPU6050 | STM32 |
|---------|--------|
| VCC | 3.3V |
| GND | GND |
| SDA | I2C SDA |
| SCL | I2C SCL |
| INT | Optional (Interrupt Pin) |

---

## Driver APIs

### Initialization

```c
MPU6050_Init();
```

### Sensor Reading

```c
MPU6050_ReadAccel();
MPU6050_ReadGyro();
MPU6050_ReadTemperature();
MPU6050_ReadAll();
MPU6050_ReadAll_AtOnce();
```

### Configuration

```c
MPU6050_SetDLPF();
MPU6050_SetSampleRate();
MPU6050_Set_GyroSensitivity();
MPU6050_Set_AccelSensitivity();
```

### Power Management

```c
MPU6050_WakeUp();
MPU6050_Sleep();
MPU6050_Reset();
```

### Interrupts

```c
MPU6050_InterruptEnable();
MPU6050_DisableInterrupt();
```

### Calibration

```c
MPU6050_Calibrate_Gyro();
```

---

## Example

```c
MPU6050_Init(&hmpu6050, &hi2c1, &config);

MPU6050_Calibrate_Gyro(&hmpu6050, 500);

while (1)
{
    MPU6050_ReadAll(&hmpu6050);

    printf("Gyro : X=%.3f Y=%.3f Z=%.3f\r\n",
            hmpu6050.gyro_x,
            hmpu6050.gyro_y,
            hmpu6050.gyro_z);

    printf("Accel: X=%.3f Y=%.3f Z=%.3f\r\n",
            hmpu6050.accel_x,
            hmpu6050.accel_y,
            hmpu6050.accel_z);

    printf("Temperature = %.2f °C\r\n",
            hmpu6050.temperature);

    HAL_Delay(500);
}
```

---

## Sample Output

```
Gyro : X=0.040  Y=-0.060  Z=0.025
Accel: X=0.053  Y=0.017  Z=0.803
Temperature = 26.49 °C
```

---

## Project Structure

```
Core
├── Inc
│   └── mpu6050.h
└── Src
    └── mpu6050.c
```

---

## Future Improvements

- Motion Detection
- Zero Motion Detection
- Free Fall Detection
- FIFO Support
- Self-Test
- Digital Motion Processor (DMP) Support
- DMA-based I2C transfers
- Interrupt-driven data acquisition

The driver currently focuses on the core MPU6050 functionality commonly used in embedded applications.
Advanced features such as FIFO, DMP, and motion detection are planned for future releases.
---

## License

This project is licensed under the MIT License.
