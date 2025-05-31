// kalman3d.h
#ifndef KALMAN3D_H
#define KALMAN3D_H

#include <stddef.h>

#define STATE_DIM 6
#define MEAS_DIM 3
#define CTRL_DIM 3

typedef struct {
    float x[STATE_DIM];          // State vector
    float P[STATE_DIM][STATE_DIM]; // Covariance
} Kalman3D;

void kalman3d_init(Kalman3D *kf, float pos_noise_var, float accel_noise_var);

void kalman3d_predict(Kalman3D *kf, const float accel[3], float dt);
void kalman3d_update(Kalman3D *kf, const float pos_meas[3], float meas_noise_var);

#endif
