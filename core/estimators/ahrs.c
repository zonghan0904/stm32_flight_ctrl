#include <math.h>
#include <stdio.h>
#include "arm_math.h"
#include "mpu6500.h"
#include "ahrs.h"
#include "madgwick_ahrs.h"
#include "lpf.h"
#include "uart.h"
#include "matrix.h"
#include "se3_math.h"
#include "quaternion.h"

madgwick_t madgwick_ahrs;

void ahrs_init(void)
{
	madgwick_init(&madgwick_ahrs, 400, 0.3);
}

void ahrs_gyro_integration(float *q, float *gyro, float time)
{
#if 0
	float w[4];
	w[0] = 0.0f;
	w[1] = gyro[0];
	w[2] = gyro[1];
	w[3] = gyro[2];

	float q_dot[4];
	quaternion_mult(q, w, q_dot);
	q[0] += q_dot[0] * 0.5 * time;
	q[1] += q_dot[1] * 0.5 * time;
	q[2] += q_dot[2] * 0.5 * time;
	q[3] += q_dot[3] * 0.5 * time;
#else
	float w[3];
	w[0] = gyro[0] * time;
	w[1] = gyro[1] * time;
	w[2] = gyro[2] * time;

	float w_norm;
	arm_sqrt_f32((w[0]*w[0]) + (w[1]*w[1]) + (w[2]*w[2]), &w_norm);

	/* reciprocal of w_norm is unstable when w_norm is ~0 */
	if(w_norm > 1.0e-12) {
		float q_last[4];
		quaternion_copy(q_last, q);

		float half_w_norm = 0.5f * w_norm;
		float recip_w_norm = 1.0f / w_norm;

		float q_rot[4];
		q_rot[0] = arm_cos_f32(half_w_norm);
		q_rot[1] = arm_sin_f32(half_w_norm) * w[0] * recip_w_norm;
		q_rot[2] = arm_sin_f32(half_w_norm) * w[1] * recip_w_norm;
		q_rot[3] = arm_sin_f32(half_w_norm) * w[2] * recip_w_norm;

		quaternion_mult(q_last, q_rot, q);
		quat_normalize(q);
	}
#endif
}

void reset_quaternion_yaw_angle(float *q)
{
	float q_original[4];
	q_original[0] = q[0];
	q_original[1] = q[1];
	q_original[2] = q[2];
	q_original[3] = q[3];

	euler_t euler;
	quat_to_euler(q, &euler);
	float half_psi = euler.yaw / 2.0f;

	float q_negative_yaw[4];
	q_negative_yaw[0] = arm_cos_f32(-half_psi);
	q_negative_yaw[1] = 0.0f;
	q_negative_yaw[2] = 0.0f;
	q_negative_yaw[3] = arm_sin_f32(-half_psi);

	quaternion_mult(q_negative_yaw, q_original, q);
}

void ahrs_selector(float *q, float *gravity, float *gyro_rad)
{
	madgwick_imu_ahrs(&madgwick_ahrs, gravity, gyro_rad);
	quaternion_copy(q, madgwick_ahrs.q);
}

void ahrs_estimate(ahrs_t *ahrs, float *accel, float *gyro)
{
	/* note that acceleromter senses the negative gravity acceleration (normal force)
	 * a_imu = (R(phi, theta, psi) * a_translation) - (R(phi, theta, psi) * g) */
	float gravity[3];
	gravity[0] = -accel[0];
	gravity[1] = -accel[1];
	gravity[2] = -accel[2];

	float gyro_rad[3];
	gyro_rad[0] = deg_to_rad(gyro[0]);
	gyro_rad[1] = deg_to_rad(gyro[1]);
	gyro_rad[2] = deg_to_rad(gyro[2]);

	madgwick_imu_ahrs(&madgwick_ahrs, gravity, gyro_rad);
	quaternion_copy(ahrs->q, madgwick_ahrs.q);


	euler_t euler;
	quat_to_euler(ahrs->q, &euler);
	ahrs->attitude.roll = rad_to_deg(euler.roll);
	ahrs->attitude.pitch = rad_to_deg(euler.pitch);
	ahrs->attitude.yaw = rad_to_deg(euler.yaw);
}

void debug_print_ahrs_quaternion(ahrs_t *ahrs){
    char s[100] = {0};
    sprintf(s, "[ahrs] q1: %f, q2: %f, q3: %f, q4: %f\n\r", ahrs->q[0], ahrs->q[1], ahrs->q[2], ahrs->q[3]);
    uart3_puts(s);
}
