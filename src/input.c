#include "input.h"

#define CY 384.0
#define CX 512.0
#define POS_UNCERTAINTY 1e-4
#define ACCEL_UNCERTAINTY 0.1
const float fov_x = M_PI_4;
const float fov_y = 0.6108652382;
const float deg_per_x = fov_x / CX;
const float deg_per_y = fov_y / CY;
const float ir_sep = 0.2;

Kalman3D kalman = {0};
struct acc_cal wm_cal;
cwiid_wiimote_t *wiimote;
float timestamp = 0;

float ms_since_epoch() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  return (float)ms / 1000.0f;
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

void ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                      float pos[]) {

  float mid_y = ((float)(py1 + py2)) / 2.0;
  float mid_x = ((float)(px1 + px2)) / 2.0;

  float angle_x1 = (px1 - CX) * deg_per_x;
  float angle_x2 = (px2 - CX) * deg_per_x;
  float angle_y1 = (py1 - CY) * deg_per_y;
  float angle_y2 = (py2 - CY) * deg_per_y;

  float z = ir_sep / (2.0f * tan(angle_x1 - angle_x2));
  float x = z * tan(0.5 * (angle_x1 + angle_x2));
  float y = z * tan(0.5 * (angle_y1 + angle_y2));
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
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
    ir_to_real_space(px1, py1, px2, py2, position);
    kalman3d_update(&kalman, position, 1e-4);
  }
}

void handle_accel_event(struct cwiid_acc_mesg msg) {
  float ax = ((float)msg.acc[CWIID_X] - wm_cal.zero[CWIID_X]) /
             (wm_cal.one[CWIID_X] - wm_cal.zero[CWIID_X]);
  float ay = ((float)msg.acc[CWIID_Y] - wm_cal.zero[CWIID_Y]) /
             (wm_cal.one[CWIID_Y] - wm_cal.zero[CWIID_Y]);
  float az = ((float)msg.acc[CWIID_Z] - wm_cal.zero[CWIID_Z]) /
             (wm_cal.one[CWIID_Z] - wm_cal.zero[CWIID_Z]);
  az -= 1.0f; // correct for gravity
  float accel[3] = {ax, ay, az};
  if (ax * ax + ay * ay + az * az > 9.0f) {
    // Probably the user is not accelerating at 3g
    return;
  }
  float new_t = ms_since_epoch();

  kalman3d_predict(&kalman, accel, new_t - timestamp);
  new_t = timestamp;
}

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg_array[], struct timespec *timestamp) {
  for (int i = 0; i < mesg_count; i++) {
    union cwiid_mesg msg = mesg_array[i];
    switch (msg.type) {
    case CWIID_MESG_BTN:
      break;
    case CWIID_MESG_IR:
      track_ir_event(msg.ir_mesg.src);
      break;
    case CWIID_MESG_ACC:
      handle_accel_event(msg.acc_mesg);
      break;
    default:
      break;
    }
  }
}

int init_input() {
  kalman3d_init(&kalman, 1e-4, 0.1);
  timestamp = ms_since_epoch();
  bdaddr_t bdaddr = *BDADDR_ANY;

  wiimote = cwiid_open(&bdaddr, CWIID_FLAG_MESG_IFC);
  if (!wiimote) {
    fprintf(stderr, "Unable to connect\n");
    return 1; // Exit on failure to connect
  }

  if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
    fprintf(stderr, "Unable to set callback\n");
    cwiid_close(wiimote);
    return 1;
  }

  if (cwiid_set_rpt_mode(wiimote,
                         CWIID_RPT_BTN | CWIID_RPT_IR | CWIID_RPT_ACC)) {
    fprintf(stderr, "Unable to set report mode\n");
    cwiid_close(wiimote);
    return 1;
  }
  cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &wm_cal);
}

void free_input() { cwiid_close(wiimote); }
