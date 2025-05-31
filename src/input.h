#ifndef INPUT_H
#define INPUT_H
#include "kalman3d.h"
#include <bluetooth/bluetooth.h>
#include <cwiid.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

void poll_position(float pos[]);
void poll_velocity(float pos[]);
void init_input();
void free_input();

#endif
