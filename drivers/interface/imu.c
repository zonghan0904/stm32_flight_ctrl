#include <stdint.h>
#include <stdbool.h>
#include "mpu6500.h"

void config_imu_accel_scale_calib_setting(float x_scale, float y_scale, float z_scale)
{
	mpu6500_config_scale_calib_setting(x_scale, y_scale, z_scale);
}

void config_imu_accel_offset_calib_setting(float x_offset, float y_offset, float z_offset)
{
	mpu6500_config_offset_calib_setting(x_offset, y_offset, z_offset);
}

void get_imu_raw_accel(float *accel)
{
	mpu6500_get_raw_accel(accel);
}

void get_imu_filtered_accel(float *accel)
{
	mpu6500_get_filtered_accel(accel);
}

void get_imu_raw_gyro(float *gyro)
{
	mpu6500_get_raw_gyro(gyro);
}

void get_imu_filtered_gyro(float *gyro)
{
	mpu6500_get_filtered_gyro(gyro);
}

float get_imu_accel_update_freq(void)
{
	return 0.0f;
}

float get_imu_gyro_update_freq(void)
{
	return 0.0f;
}

