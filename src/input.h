#ifndef INPUT_H
#define INPUT_H
#include "SF1eFilter.h"
#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void poll_position(float pos[]);
void poll_velocity(float pos[]);
cwiid_wiimote_t* init_input(void (*user_btn_callback)(uint16_t));
void reset_filter();
void configure_fiter(float minCutoffFrequency, float cutoffSlope);
void free_input();
void position_to_screen_space(const float position[], int width, int height,
                              float screen[]);
#endif
