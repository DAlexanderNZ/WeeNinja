#include "input.h"

#define POS_UNCERTAINTY 1e-4
#define ACCEL_UNCERTAINTY 0.1
/*
 * FOV values from NIH documentation
 * https://pmc.ncbi.nlm.nih.gov/articles/PMC7218719/#:~:text=With%20the%20filter%20applied%20and%20by%20default%2C,33%20degrees%20horizontally%20and%2023%20degrees%20vertically.
 */
const float G = 9.81f;
const float WIDTH = 768.0f;
const float HEIGHT = 1024.0f;
const float CX = 384.0f;
const float CY = 512.0f;
const float FOV_X = 33.0f * M_PI / 180.0f;
const float FOV_Y = 23.0f * M_PI / 180.0f;

const float RAD_PER_PIXEL_X = FOV_X / WIDTH;
const float RAD_PER_PIXEL_Y = FOV_Y / HEIGHT;
const float IR_SEP = 0.2f;

Kalman3D kalman = {0};
struct acc_cal wm_cal;
cwiid_wiimote_t *wiimote;
float last_accel_event_timestamp_sec = 0.0f;

float timespec_to_float_seconds(const struct timespec *ts) {
  return (float)ts->tv_sec + (float)ts->tv_nsec / 1e9f;
}

void poll_position(float pos[]) {
  pos[0] = kalman.x[0];
  pos[1] = kalman.x[1];
  pos[2] = kalman.x[2];
}

void poll_velocity(float vel[]) {
  vel[0] = kalman.x[3];
  vel[1] = kalman.x[4];
  vel[2] = kalman.x[5];
}

int ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                     float pos[]) {
  float angle_x1 = (px1 - CX) * RAD_PER_PIXEL_X;
  float angle_x2 = (px2 - CX) * RAD_PER_PIXEL_X;
  float angle_y1 = (py1 - CY) * RAD_PER_PIXEL_Y;
  float angle_y2 = (py2 - CY) * RAD_PER_PIXEL_Y;

  float delta_angle_x = angle_x1 - angle_x2;
  if (fabs(delta_angle_x) < 1e-6) {
    // Invalid angles, so we'll ignore this event.
    return 0;
  }

  float z = IR_SEP / (2.0f * tan(0.5f * delta_angle_x));
  float x = z * tan(0.5f * (angle_x1 + angle_x2));
  float y = z * tan(0.5f * (angle_y1 + angle_y2));
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
  return 1;
}

void track_ir_event(struct cwiid_ir_src srcs[]) {
  uint16_t px1 = 0;
  uint16_t px2 = 0;
  uint16_t py1 = 0;
  uint16_t py2 = 0;
  int blob_count = 0;

  for (int i = 0; i < CWIID_IR_SRC_COUNT; i++) {
    if (srcs[i].valid) {
      if (blob_count == 0) {
        px1 = srcs[i].pos[CWIID_X];
        py1 = srcs[i].pos[CWIID_Y];
        blob_count++;
      } else if (blob_count == 1) {
        px2 = srcs[i].pos[CWIID_X];
        py2 = srcs[i].pos[CWIID_Y];
        blob_count++;
      } else {
        break;
      }
    }
  }
  if (blob_count == 2) {
    float position[3] = {0};
    int ir_updated = ir_to_real_space(px1, py1, px2, py2, position);
    if (ir_updated) {
      kalman3d_update(&kalman, position, POS_UNCERTAINTY);
    }
  }
}

// Modified to accept event_time_sec from the cwiid message header
void handle_accel_event(struct cwiid_acc_mesg msg,
                        float current_event_time_sec) {
  float norm_ax = ((float)msg.acc[CWIID_X] - wm_cal.zero[CWIID_X]) /
                  (wm_cal.one[CWIID_X] - wm_cal.zero[CWIID_X]);
  float norm_ay = ((float)msg.acc[CWIID_Y] - wm_cal.zero[CWIID_Y]) /
                  (wm_cal.one[CWIID_Y] - wm_cal.zero[CWIID_Y]);
  float norm_az = ((float)msg.acc[CWIID_Z] - wm_cal.zero[CWIID_Z]) /
                  (wm_cal.one[CWIID_Z] - wm_cal.zero[CWIID_Z]);

  float dt;
  if (last_accel_event_timestamp_sec == 0.0f) {
    dt = 1e-2f; // I think it reports on a 1/100Hz frequency
  } else {
    dt = current_event_time_sec - last_accel_event_timestamp_sec;
  }

  if (dt < 0)
    dt = 0;

  float accel_m_s2[3] = {norm_ax * G, norm_ay * G, norm_az * G};

  kalman3d_predict(&kalman, accel_m_s2, dt);

  last_accel_event_timestamp_sec = current_event_time_sec;
}

void cwiid_callback(cwiid_wiimote_t *UNUSED_wiimote_arg, int mesg_count,
                    union cwiid_mesg mesg_array[],
                    struct timespec *callback_invoke_timestamp) {
  (void)UNUSED_wiimote_arg;
  (void)callback_invoke_timestamp;

  for (int i = 0; i < mesg_count; i++) {
    float current_message_time_sec =
        timespec_to_float_seconds(&mesg_array[i].hdr.timestamp);

    switch (mesg_array[i].type) {
    case CWIID_MESG_BTN:
      break;
    case CWIID_MESG_IR:
      track_ir_event(mesg_array[i].ir_mesg.src);
      break;
    case CWIID_MESG_ACC:
      handle_accel_event(mesg_array[i].acc_mesg, current_message_time_sec);
      break;
    default:
      break;
    }
  }
}

int init_input() {
  kalman3d_init(&kalman, POS_UNCERTAINTY, ACCEL_UNCERTAINTY);

  last_accel_event_timestamp_sec = 0.0f;

  bdaddr_t bdaddr = *BDADDR_ANY;

  wiimote = cwiid_open(&bdaddr, CWIID_FLAG_MESG_IFC);
  if (!wiimote) {
    fprintf(stderr, "Unable to connect to Wiimote. Please ensure it is "
                    "discoverable (press 1+2).\n");
    return 1;
  }
  fprintf(stdout, "Wiimote connected successfully!\n");

  if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
    fprintf(stderr, "Unable to set Wiimote message callback\n");
    cwiid_close(wiimote);
    return 1;
  }

  if (cwiid_set_rpt_mode(wiimote,
                         CWIID_RPT_BTN | CWIID_RPT_IR | CWIID_RPT_ACC)) {
    fprintf(stderr, "Error setting Wiimote report mode\n");
    cwiid_close(wiimote);
    return 1;
  }
  fprintf(stdout, "Wiimote report mode set.\n");

  if (cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &wm_cal)) {
    fprintf(stderr, "Error getting Wiimote accelerometer calibration data\n");
    cwiid_close(wiimote);
    return 1;
  }
  fprintf(stdout, "Accelerometer calibration data received.\n");

  cwiid_set_led(wiimote, CWIID_LED1_ON);

  return 0;
}

void free_input() {
  if (wiimote) {
    cwiid_set_led(wiimote, 0);
    cwiid_close(wiimote);
    wiimote = NULL;
    fprintf(stdout, "Wiimote connection closed.\n");
  }
}
