// kalman3d.c
#include "kalman3d.h"
#include <string.h> // for memset

void kalman3d_init(Kalman3D *kf, float pos_noise_var, float accel_noise_var) {
    memset(kf, 0, sizeof(Kalman3D));
    for (int i = 0; i < STATE_DIM; ++i) {
        kf->P[i][i] = (i < 3) ? pos_noise_var : 1.0; // More uncertain velocity
    }
}

void kalman3d_predict(Kalman3D *kf, const float a[3], float dt) {
    // State transition matrix A and control matrix B are implicit
    for (int i = 0; i < 3; ++i) {
        kf->x[i] += kf->x[i + 3] * dt + 0.5 * a[i] * dt * dt;
        kf->x[i + 3] += a[i] * dt;
    }

    // Covariance prediction: P = A * P * Aᵀ + Q
    float Q[STATE_DIM][STATE_DIM] = {0};
    float q_pos = 0.25 * dt * dt * dt * dt;
    float q_vel = dt * dt;
    for (int i = 0; i < 3; ++i) {
        Q[i][i] = q_pos;
        Q[i + 3][i + 3] = q_vel;
    }

    // Simple model: linear motion with process noise (Q)
    for (int i = 0; i < STATE_DIM; ++i) {
        for (int j = 0; j < STATE_DIM; ++j) {
            kf->P[i][j] += Q[i][j];
        }
    }
}

void kalman3d_update(Kalman3D *kf, const float z[3], float meas_noise_var) {
    // Measurement matrix H: extracts position
    // R: measurement noise
    float R[MEAS_DIM][MEAS_DIM] = {
        {meas_noise_var, 0, 0}, {0, meas_noise_var, 0}, {0, 0, meas_noise_var}};

    // Compute innovation y = z - Hx
    float y[3];
    for (int i = 0; i < 3; ++i) {
        y[i] = z[i] - kf->x[i];
    }

    // Compute innovation covariance S = HPHᵀ + R
    float S[3][3] = {{0}};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            S[i][j] = kf->P[i][j] + R[i][j];

    // Compute Kalman gain K = P Hᵀ S⁻¹ (assume S is diagonal)
    float K[STATE_DIM][3] = {{0}};
    for (int i = 0; i < STATE_DIM; ++i)
        for (int j = 0; j < 3; ++j)
            K[i][j] = kf->P[i][j] / S[j][j]; // Simplified for diagonal S

    // Update state x = x + K y
    for (int i = 0; i < STATE_DIM; ++i) {
        for (int j = 0; j < 3; ++j)
            kf->x[i] += K[i][j] * y[j];
    }

    // Update covariance P = (I - KH)P
    float P_new[STATE_DIM][STATE_DIM] = {{0}};
    for (int i = 0; i < STATE_DIM; ++i) {
        for (int j = 0; j < STATE_DIM; ++j) {
            P_new[i][j] = kf->P[i][j];
            for (int k = 0; k < 3; ++k) {
                P_new[i][j] -= K[i][k] * kf->P[k][j];
            }
        }
    }
    memcpy(kf->P, P_new, sizeof(P_new));
}
