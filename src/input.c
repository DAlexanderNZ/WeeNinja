#if ENABLE_CWIID

#include "input.h"
#include "cwiid.h"

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

struct acc_cal wm_cal;
cwiid_wiimote_t *wiimote;
void (*btn_callback)(uint16_t);

SF1eFilter *filter[2] = {NULL, NULL};

float last_position[2] = {0.0f, 0.0f};

float timespec_to_float_seconds(const struct timespec *ts) {
  return (float)ts->tv_sec + (float)ts->tv_nsec / 1e9f;
}

void poll_position(float* position) {
  position[0] = last_position[0];
  position[1] = last_position[1];
}

int ir_to_real_space(uint16_t px1, uint16_t py1, uint16_t px2, uint16_t py2,
                     float pos[]) {
  float dx = 0.5 * (float)(px1 + px2)  - CX;
  float dy = 0.5 * (float)(py1 + py2)  - CY;

  float offset_y  = dy / HEIGHT;
  float offset_x = -dx / WIDTH;
  pos[0] = offset_x;
  pos[1] = offset_y;
  return 1;
}

void one_euro_filter(float position[], const float message_time) {
  if (message_time - filter[0]->lastTime > 0.5) {
    reset_filter();
  }
  for (int i = 0; i < 2; i++) {
    last_position[i] = SF1eFilterDo(filter[i], position[i]);
  }
}
void configure_filter(float minCutoffFrequency, float cutoffSlope) {
  for (int i = 0; i < 2; i++) {
    filter[i]->config.cutoffSlope = cutoffSlope;
    filter[i]->config.minCutoffFrequency = minCutoffFrequency;
  }
  reset_filter();
}

void track_ir_event(struct cwiid_ir_src srcs[], const float message_time) {
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
    if (ir_updated && isfinite(position[0]) && isfinite(position[1])) {
      one_euro_filter(position, message_time);
    }
  }
}

void cwiid_callback(cwiid_wiimote_t *UNUSED_wiimote_arg, int mesg_count,
                    union cwiid_mesg mesg_array[],
                    struct timespec *callback_invoke_timestamp) {
  (void)UNUSED_wiimote_arg;

  for (int i = 0; i < mesg_count; i++) {
    float current_message_time_sec =
        timespec_to_float_seconds(callback_invoke_timestamp);

    switch (mesg_array[i].type) {
    case CWIID_MESG_BTN:
      btn_callback(mesg_array[i].btn_mesg.buttons);
      break;
    case CWIID_MESG_IR:
      track_ir_event(mesg_array[i].ir_mesg.src, current_message_time_sec);
      break;
    default:
      break;
    }
  }
}

/** (Re)set the 1-euro filter
 */
void reset_filter() {
  for (int i = 0; i < 2; i++) {
    if (!filter[i]) {
      filter[i] = SF1eFilterCreate(1e-2f, 1.0f, 0.1f, 1.0f);
    } else {
      SF1eFilterInit(filter[i]);
    }
  }
}

cwiid_wiimote_t* init_input(void (*user_btn_callback)(uint16_t)){
  bdaddr_t bdaddr = *BDADDR_ANY;
  btn_callback = user_btn_callback;
  wiimote = cwiid_open(&bdaddr, CWIID_FLAG_MESG_IFC);
  if (!wiimote) {
    fprintf(stderr, "Unable to connect to Wiimote. Please ensure it is "
                    "discoverable (press 1+2).\n");
    return NULL;
  }
  fprintf(stdout, "Wiimote connected successfully!\n");

  if (cwiid_set_mesg_callback(wiimote, &cwiid_callback)) {
    fprintf(stderr, "Unable to set Wiimote message callback\n");
    cwiid_close(wiimote);
    return NULL;
  }

  if (cwiid_set_rpt_mode(wiimote,
                         CWIID_RPT_BTN | CWIID_RPT_IR | CWIID_RPT_ACC)) {
    fprintf(stderr, "Error setting Wiimote report mode\n");
    cwiid_close(wiimote);
    return NULL;
  }
  fprintf(stdout, "Wiimote report mode set.\n");

  if (cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &wm_cal)) {
    fprintf(stderr, "Error getting Wiimote accelerometer calibration data\n");
    cwiid_close(wiimote);
    return NULL;
  }
  fprintf(stdout, "Accelerometer calibration data received.\n");

  cwiid_set_led(wiimote, CWIID_LED1_ON);
  reset_filter();
  return wiimote;
}

void free_input() {
  if (wiimote) {
    cwiid_set_led(wiimote, 0);
    cwiid_close(wiimote);
    wiimote = NULL;
    fprintf(stdout, "Wiimote connection closed.\n");
    for (int i = 0; i < 2; i++) {
      filter[i] = SF1eFilterDestroy(filter[i]);
    }
  }
}

#endif
